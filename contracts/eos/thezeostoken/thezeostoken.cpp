#include "thezeostoken.hpp"

thezeostoken::thezeostoken(name self, name code, datastream<const char *> ds) :
    contract(self, code, ds),
    txb(_self, _self.value),
    ftb(_self, _self.value),
    global(_self, _self.value)
{
}

void thezeostoken::setvk(const name& code, const name& id, const string& vk)
{
    require_auth(code);
    
    vk_t vks(get_self(), code.value);
    auto c = vks.find(id.value);
    
    if(c == vks.end())
    {
        // add new key
        vks.emplace(code, [&](auto& row){
            row.id = id;
            row.vk = vk;
        });
    }
    else
    {
        if(vk.empty())
        {
            // erase existing key to free up RAM
            vks.erase(c);
        }
        else
        {
            // update existing key
            vks.modify(c, code, [&](auto& row){
                row.vk = vk;
            });
        }
    }
}

void thezeostoken::verifyproof(const string& type, const name& code, const name& id, const string& proof, const string& inputs)
{
    vk_t vks(get_self(), code.value);
    auto c = vks.find(id.value);
    check(c != vks.end(), "vk id doesn't exist");
    
    string str = "zeos_verify_proof://";
    str.append(type);
    str.append("/");
    str.append(c->vk);
    str.append("/");
    str.append(proof);
    str.append("/");
    str.append(inputs);

    //uint32_t dsp_count = 0;
    bool valid = getURI(vector<char>(str.begin(), str.end()), [&](auto& results) { 
        uint32_t dsp_threshold = 1;
        // ensure the specified amount of DSPs have responded before a response is accepted
        check(results.size() >= dsp_threshold, "require multiple results for consensus");
        //dsp_count = results.size();
        auto itr = results.begin();
        auto first = itr->result;
        ++itr;
        while(itr != results.end())
        {
            check(itr->result == first, "consensus failed");
            ++itr;
        }
        return first;
    })[0] == '1';
    
    check(valid, "Proof invalid! Data: " + str);
    //print("Proof verified by ", dsp_count, " DSPs\n\r");
}

uint64_t dummy_mem[5] = {0xDEADBEEFDEADBEEF, 0xDEADBEEFDEADBEEF, 0xDEADBEEFDEADBEEF, 0xDEADBEEFDEADBEEF, 0xDEADBEEFDEADBEEF};
inline bool has_exec_zactions(const action* a)
{
    return a->data.size() > (1 + sizeof(zaction::type) + ZI_SIZE + 1) &&    // ensure valid access for following checks 
           0 == memcmp(&a->data[1], dummy_mem, 40) &&                       // 5 times 0xDEADBEEFDEADBEEF check
           a->data[0] > 1 &&                                                // more zactions than just the dummy?
           a->data[1 + sizeof(zaction::type) + ZI_SIZE] == 0;               // memo field == ""?;
}

void thezeostoken::begin(const string& proof, vector<action>& tx)
{
    // check for context-free actions and other stuff
    auto action = better_get_action(0, 0);
    check(!action, "context-free actions are not allowed");
    check(tx.size() > 0, "transaction must contain at least one action");

    // make sure the action pattern (begin)(step)(step)... is given with as many steps as 'tx' requires
    // check out: https://eoscommunity.github.io/clsdk-docs/book/std/cpay/index.html
    uint32_t begin_index = -1;
    uint32_t last_step = -1;
    uint32_t index = 0;
    for(;; ++index)
    {
        auto action = better_get_action(1, index);
        if(!action)
            break;
        if(action->account == "thezeostoken"_n && action->name == "begin"_n)
        {
            check(begin_index == -1, "only one 'begin' per transaction");
            begin_index = index;
            last_step = index + tx.size();
        }
        else if(begin_index >= 0 && index <= last_step)
        {
            check(action->account == "thezeostoken"_n && action->name == "step"_n, "not enough 'step' actions after 'begin'");
        }
    }
    check(index > last_step, "not enough 'step' actions after 'begin'");

    // collect public inputs for proof bundle verification and check for blacklisted transactions
    string inputs = "";
    for(auto a = tx.begin(); a != tx.end(); ++a)
    {
        // blacklist all actions with pattern: *::transfer and eosio::*
        check(a->account != "eosio"_n, "system contract actions are blacklisted!");
        check(a->name != "transfer"_n, "'transfer' actions are blacklisted!");
        // TODO: any more accounts/actions that should be blacklisted?

        // in case of dummy_exec zaction collect public inputs (halo2 instances)
        if(has_exec_zactions(&(*a)))
        {
            char i = 1;
            char* ptr = a->data.data() + 1 + sizeof(zaction::type) + ZI_SIZE + 1 + sizeof(zaction::type);
            do
            {
                // don't collect inputs if zaction type is ZA_MINTAUTH because it does not require a zero knowledge proof
                if(ZA_MINTAUTH != *reinterpret_cast<const uint64_t*>(ptr - sizeof(zaction::type)))
                {
                    inputs.append(byte2str<ZI_SIZE>(reinterpret_cast<const uint8_t*>(ptr)));
                }
                ptr += ZI_SIZE;
                ptr += *reinterpret_cast<const uint8_t*>(ptr) + 1 + sizeof(zaction::type);
                ++i;
            }
            while(i < a->data[0]);

            // calculate the size of the entire vector<zactions> in bytes and write it into the dummy struct (lower 64 bit of field 'nf'). This will
            // make it easy to reduce the size of the 'data' vector of this particular EOSIO 'action' object to pass it as argument to 'exec' in 'step'
            // function.
            *reinterpret_cast<size_t*>(&a->data[1 + sizeof(zaction::type) + 32]) = ptr - sizeof(zaction::type) - a->data.data();
        }
    }

    // verify proof bundle if some public inputs have been found
    if(inputs.length() > 0)
    {
        verifyproof("zeos", "thezeostoken"_n, "zeosorchard1"_n, proof, inputs);
    }

    // TODO: add encrypted notes to tx_data list
    // TODO check num of enc notes is not more than what counted in loop above (TODO)

    // copy tx into singleton buffer where it remains only during its execution. The last 'step' frees the buffer.
    txb.set({0, tx.size()-1, tx}, _self);
}

void thezeostoken::step()
{
    // execute the action (corresponding to the current 'step')
    auto b = txb.get();
    b.tx[b.cur].send();

    // if this action is not thezeostoken::exec execute its zactions (if any)
    if(!(b.tx[b.cur].name.value    == "exec"_n.value && 
         b.tx[b.cur].account.value == "thezeostoken"_n.value) &&
       has_exec_zactions(&b.tx[b.cur]))
    {
        // Transform current action into an 'exec' action. Call resize() on data to keep only the vector
        // of zactions for exec as first parameter and cut off all additional params.
        b.tx[b.cur].name = "exec"_n;
        b.tx[b.cur].account = "thezeostoken"_n;
        b.tx[b.cur].data.resize(*reinterpret_cast<const size_t*>(&b.tx[b.cur].data[1 + sizeof(zaction::type) + 32]));
        b.tx[b.cur].send();
    }

    // clear buffer or increase cur
    if(b.cur == b.last)
    {
        txb.remove();
    }
    else
    {
        b = txb.get();
        ++b.cur;
        txb.set(b, _self);
    }
}

void thezeostoken::exec(const vector<zaction>& ztx)
{
    // this action should only be executable by thezeostoken itself and never by third party contracts!
    // executing the same zactions more than once per proof could be abused
    require_auth(_self);
    //check(0, "exec!");

    checksum256 root;
    bool root_dirty = false;
    for(auto za = ztx.begin(); za != ztx.end(); ++za)
    {
        switch(za->type)
        {
            case ZA_DUMMY:
            case ZA_NULL:
            case ZA_MINTAUTH:
            default:
            {
                // no merkle tree/nf operations required
                break;
            }

            case ZA_MINTFT:
            {
                // TODO all the other checks for TRANSFERFT (ZEOS Book)
                auto funds = ftb.get();
                check(za->ins.b_d1 == funds.quantity.quantity.amount, "ZA_MINTFT: wrong amount");
                check(za->ins.b_d2 == funds.quantity.quantity.symbol.raw(), "ZA_MINTFT: wrong symbol");
                check(za->ins.b_sc == funds.quantity.contract.value, "ZA_MINTFT: wrong contract");
                // add new leaves to merkle tree
                root = insert_into_merkle_tree(za->ins.cmb);
                root_dirty = true;
                // clear buffer
                ftb.remove();
                break;
            }
            
            case ZA_MINTNFT:
            {
                // TODO
                break;
            }
            
            case ZA_TRANSFERFT:
            {
                // check if root is valid
                check(is_root_valid(za->ins.anchor), "ZA_TRANSFERFT: root invalid");
                // TODO all the other checks for TRANSFERFT (ZEOS Book)
                // check if nullifier already exists in list, if not add it
                nf_t nf(_self, _self.value);
                auto it = nf.find((uint64_t)*((uint32_t*)za->ins.nf.extract_as_byte_array().data()));
                check(it == nf.end(), "ZA_TRANSFERFT: nullifier exists => note has been spent already");
                nf.emplace(_self, [&](auto& n) {
                    n.val = za->ins.nf;
                });
                // add new leaves to merkle tree
                insert_into_merkle_tree(za->ins.cmb);
                root = insert_into_merkle_tree(za->ins.cmc);
                root_dirty = true;
                break;
            }
            
            case ZA_TRANSFERNFT:
            {
                // TODO
                break;
            }
            
            case ZA_BURNFT:
            {
                // check if root is valid
                check(is_root_valid(za->ins.anchor), "ZA_BURNFT: root invalid");
                // TODO all the other checks for BURNFT (ZEOS Book)
                // check if nullifier already exists in list, if not add it
                nf_t nf(_self, _self.value);
                auto it = nf.find((uint64_t)*((uint32_t*)za->ins.nf.extract_as_byte_array().data()));
                check(it == nf.end(), "ZA_BURNFT: nullifier exists => note has been spent already");
                nf.emplace(_self, [&](auto& n) {
                    n.val = za->ins.nf;
                });
                // payout b to user account
                // TODO
                // add new leaves to merkle tree
                root = insert_into_merkle_tree(za->ins.cmc);
                root_dirty = true;
                break;
            }
            
            case ZA_BURNFT2:
            {
                // check if root is valid
                check(is_root_valid(za->ins.anchor), "ZA_BURNFT: root invalid");
                // TODO all the other checks for BURNFT2 (ZEOS Book)
                // check if nullifier already exists in list, if not add it
                nf_t nf(_self, _self.value);
                auto it = nf.find((uint64_t)*((uint32_t*)za->ins.nf.extract_as_byte_array().data()));
                check(it == nf.end(), "ZA_BURNFT: nullifier exists => note has been spent already");
                nf.emplace(_self, [&](auto& n) {
                    n.val = za->ins.nf;
                });
                // payout b to user account
                // TODO
                // payout c to user account
                // TODO
                break;
            }
            
            case ZA_BURNNFT:
            {
                // TODO
                break;
            }
            
            case ZA_BURNAUTH:
            {
                // TODO
                break;
            }
        }
    }

    // update list of roots if necessary
    if(root_dirty)
    {
        // update global stats
        auto stats = global.get();
        stats.mt_roots.push_front(root);
        // only memorize the most recent x number of root nodes
        if(stats.mt_roots.size() > G_ROOTS_FIFO_SIZE)
        {
            stats.mt_roots.pop_back();
        }
        global.set(stats, _self);
    }

}

void thezeostoken::test22(const vector<zaction>& ztx, const uint64_t& test22)
{
    require_auth("thezeostoken"_n);
    check(test22 == 12345, test22);
}

void thezeostoken::onfttransfer(name from, name to, asset quantity, string memo)
{
    if(to == _self)
    {
        // buffer quantity & owner contract
        ftb.set({extended_asset(quantity, get_first_receiver())}, _self);
    }
}

void thezeostoken::init(const uint64_t& depth)
{
    require_auth(_self);

    // empty all tables (notes, mt, nf)
    enc_notes_t notes(_self, _self.value);
    for(auto it = notes.begin(); it != notes.end(); )
        it = notes.erase(it);
    mt_t mt(_self, _self.value);
    for(auto it = mt.begin(); it != mt.end(); )
        it = mt.erase(it);
    nf_t nf(_self, _self.value);
    for(auto it = nf.begin(); it != nf.end(); )
        it = nf.erase(it);
    
    // reset global stats table
    global.set({0, 0, depth, deque<checksum256>()}, _self);
}

void thezeostoken::create(const name& issuer, const asset& maximum_supply)
{
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    check(sym.is_valid(), "invalid symbol name");
    check(maximum_supply.is_valid(), "invalid supply");
    check(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    check(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto& s) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}

void thezeostoken::issue(const name& to, const asset& quantity, const string& memo)
{
    auto sym = quantity.symbol;
    check(sym.is_valid(), "invalid symbol name");
    check(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto& st = *existing;

// TODO: ALWAYS CHECK IF THE FAUCET IS TURNED OFF!
//check(quantity.amount <= 1000000, "only 100 ZEOS tokens are allowed per faucet issue");
    require_auth(st.issuer);
    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must issue positive quantity");

    check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    check(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, same_payer, [&](auto& s) {
       s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if(to != st.issuer) {
       SEND_INLINE_ACTION(*this, transfer, {st.issuer,"active"_n}, {st.issuer, to, quantity, memo});
    }
}

void thezeostoken::retire(const asset& quantity, const string& memo)
{
    auto sym = quantity.symbol;
    check(sym.is_valid(), "invalid symbol name");
    check(memo.size() <= 256, "memo has more than 256 bytes");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    check(existing != statstable.end(), "token with symbol does not exist");
    const auto& st = *existing;

    require_auth(st.issuer);
    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must retire positive quantity");

    check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    statstable.modify(st, same_payer, [&](auto& s) {
        s.supply -= quantity;
    });

    sub_balance(st.issuer, quantity);
}

void thezeostoken::transfer(const name& from, const name& to, const asset& quantity, const string& memo)
{
    check(from != to, "cannot transfer to self");
    require_auth(from);
    check(is_account(to), "to account does not exist");
    auto sym = quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto& st = statstable.get(sym);

    require_recipient(from);
    require_recipient(to);

    check(quantity.is_valid(), "invalid quantity");
    check(quantity.amount > 0, "must transfer positive quantity");
    check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    check(memo.size() <= 256, "memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void thezeostoken::open(const name& owner, const symbol& symbol, const name& ram_payer)
{
    require_auth(ram_payer);

    check(is_account(owner), "owner account does not exist");

    auto sym_code_raw = symbol.code().raw();
    stats statstable(get_self(), sym_code_raw);
    const auto& st = statstable.get(sym_code_raw, "symbol does not exist");
    check(st.supply.symbol == symbol, "symbol precision mismatch");

    accounts acnts(get_self(), owner.value);
    auto it = acnts.find(sym_code_raw);
    if(it == acnts.end()) {
        acnts.emplace(ram_payer, [&](auto& a){
            a.balance = asset{0, symbol};
        });
    }
}

void thezeostoken::close(const name& owner, const symbol& symbol)
{
    require_auth(owner);
    accounts acnts(get_self(), owner.value);
    auto it = acnts.find(symbol.code().raw());
    check(it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect.");
    check(it->balance.amount == 0, "Cannot close because the balance is not zero.");
    acnts.erase(it);
}

void thezeostoken::add_balance(const name& owner, const asset& value, const name& ram_payer)
{
    accounts to_acnts(_self, owner.value);
    auto to = to_acnts.find(value.symbol.code().raw());
    if(to == to_acnts.end()) {
        to_acnts.emplace(ram_payer, [&](auto& a){
            a.balance = value;
        });
    } else {
        to_acnts.modify(to, same_payer, [&](auto& a) {
            a.balance += value;
        });
    }
}

void thezeostoken::sub_balance(const name& owner, const asset& value)
{
    accounts from_acnts(_self, owner.value);
    const auto& from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    check(from.balance.amount >= value.amount, "overdrawn balance");
    if(from.balance.amount == value.amount) {
        from_acnts.erase(from);
    } else {
        from_acnts.modify(from, same_payer, [&](auto& a) {
            a.balance -= value;
        });
    }
}

asset thezeostoken::get_supply(const symbol_code& sym) const
{
    stats statstable(_self, sym.raw());
    const auto& st = statstable.get(sym.raw());
    return st.supply;
}

asset thezeostoken::get_balance(const name& owner, const symbol_code& sym) const
{
    accounts accountstable(_self, owner.value);
    const auto& ac = accountstable.get(sym.raw());
    return ac.balance;
}

// empty roots up to depth 32
// d=0 => EMPTY_LEAF,
// d=1 => sinsemilla_combine(0, EMPTY_LEAF, EMPTY_LEAF)
const Fp thezeostoken::EMPTY_ROOT[] = {
    Fp({0xcfc3a984fffffff9, 0x1011d11bbee5303e, 0xffffffffffffffff, 0x3fffffffffffffff}),
    Fp({0x07a9009e0a581029, 0x09e7b91f81e378fc, 0x1a82a264f22bea0a, 0x0c82031f50309c9f}),
    Fp({0xc9ecfc3a09178d32, 0x0b4c9ff2201994bf, 0x22cb9c0a6e41bd2a, 0x1c3b7eb79eeaf10a}),
    Fp({0x39bf490b9c63a6b3, 0x6b047793eebd2ad7, 0x8b6a56cc1bb1d2e6, 0x0e5ada44b5ac900b}),
    Fp({0xdaab63818557da0d, 0x21bb27e5ff238824, 0xbf65270b467ddf84, 0x339da128deb77752}),
    Fp({0xef0f55ff1a7862c0, 0xd3791e6211299a1c, 0x7d9b4c74da669560, 0x23c5bbf35b04091d}),
    Fp({0x95ef1287d7b44f08, 0x2b896f33a8733787, 0x103446c2f6129e50, 0x3948396852c405ed}),
    Fp({0x650cbea2da366868, 0xb4cf379f57fb95ff, 0xb8f866e7fbe3a165, 0x14341745aa0a32e1}),
    Fp({0x30a2f7c76000d014, 0x582926eae7647de9, 0x897539a776ee3c7c, 0x3ec97effbfdc357a}),
    Fp({0x5a3e1d2dfeb2f3f5, 0x508c680f28b566a7, 0xc637225236726af1, 0x318ff0f1acb17d2a}),
    Fp({0x982c92d2a442f828, 0x21f3b869d3b41fd5, 0xefc315441486318c, 0x317b7c06862887d1}),
    Fp({0x8bb91e504ff6674a, 0x4dfdc4b7c448d625, 0x34526da1897fb804, 0x193258690b34d94d}),
    Fp({0x742ec7b7098d5bff, 0xb56ec4a072832980, 0x69a6ea9032682613, 0x1a78651a8a6b5ba7}),
    Fp({0xa8982dcb00e85cc2, 0xd518389f0e87cb1d, 0x5c52f8e62fa88076, 0x14d9efe65120c572}),
    Fp({0x87e0bdcd42993761, 0xdb9343f10eeea647, 0xf4856994b3b2a0b4, 0x0fb917dc0f856b13}),
    Fp({0xd662ffd211e7f368, 0xfc069740671423a8, 0x2bfa72c6a613489d, 0x1f8c47597f4cff15}),
    Fp({0x2b80f62dc6d863ab, 0x6c5045667d2c8119, 0xd910b640fdf0e263, 0x1b64d31d56b12539}),
    Fp({0xd8dbcb104faa6a75, 0x8f1af94d6735e09c, 0x86fbf312a62ccf41, 0x267ed8285dcc11f5}),
    Fp({0xd76f4dabcd4c72ce, 0xa7f38b447eb41365, 0x771bda01c66290bd, 0x30d681f214d6573e}),
    Fp({0x3c7cf91c27440783, 0x0a09276c5efcd6e4, 0x6d712293c41b473f, 0x1fba7925b82b1993}),
    Fp({0xb0e99d8b68f82bab, 0x25b6f82dee24d331, 0xe84b455aab403234, 0x237352d7b9b5eaea}),
    Fp({0x7df9a36a040c75b4, 0xb66bc076da8e8636, 0x4575bcc51669bb77, 0x3214d00ce4062f44}),
    Fp({0x4494961af85b12e3, 0xced65f75e340ec69, 0xbcd1ad06f0d2dd0f, 0x21debbbef0c3f9b7}),
    Fp({0xacf2a3c599dac9d7, 0x7b45cfb6710feabe, 0x72ad422116d1f1c5, 0x39874c815db36323}),
    Fp({0xe9640012b7154a1f, 0x5f0ce18b91343ecc, 0xdcd87930bbe9c0fd, 0x037ce5b5657b1733}),
    Fp({0x64cce7f2a3cf17ec, 0xd1ff3c6376899f7d, 0x760edce8a9e779b2, 0x088d63087580ae23}),
    Fp({0xf79dbf3510ef2b84, 0xbcc919060d4f063c, 0x2e40a265292f3935, 0x274f5f826a3c118c}),
    Fp({0x41720afbfc0df919, 0xbfd210d9d5b68976, 0xe7826aebcb27128f, 0x2ae08e5ce2536718}),
    Fp({0x51cd6de53b41d7b5, 0x04526eb96ccbce4f, 0xc7a4373b4c51947a, 0x1da4928d22c7d20b}),
    Fp({0xaabcde1182e7c715, 0xd1cbb4a7dd212705, 0x0923feb28a9c387c, 0x253f940e57829ebd}),
    Fp({0xe0a73ed971f07cc4, 0x527c02d03a58ed2a, 0x4335ceb77aa63432, 0x0b84f68d78bd7ec8}),
    Fp({0x4f9a61965c06c1bc, 0xd3eed8b6685d6ff7, 0xe44224d7fcb0721c, 0x3e9d3ee01e5ea615}),
};

// merkle tree structure:
// 
//              (0)                 [d = 0] (root)
//         /            \
//       (1)            (2)         [d = 1]
//     /    \        /      \
//   (3)    (4)    (5)      (6)     [d = 2]
//   / \    / \    /  \    /  \
// (7) (8)(9)(10)(11)(12)(13)(14)   [d = 3]
//
#define MT_ARR_LEAF_ROW_OFFSET(d) ((1<<(d)) - 1)
#define MT_ARR_FULL_TREE_OFFSET(d) ((1<<((d)+1)) - 1)
#define MT_NUM_LEAVES(d) (1<<(d))
checksum256 thezeostoken::insert_into_merkle_tree(const checksum256& val)
{
    // fetch global stats
    auto stats = global.get();

    // calculate array index of next free leaf in >local< tree
    uint64_t idx = MT_ARR_LEAF_ROW_OFFSET(stats.mt_depth) + stats.mt_leaf_count % MT_NUM_LEAVES(stats.mt_depth);
    // calculate tree offset to translate array indices of >local< tree to global array indices
    uint64_t tos = stats.mt_leaf_count / MT_NUM_LEAVES(stats.mt_depth) /*=tree_idx*/ * MT_ARR_FULL_TREE_OFFSET(stats.mt_depth);

    // insert val into leaf
    mt_t tree(_self, _self.value);
    tree.emplace(_self, [&](auto& leaf) {
        leaf.idx = tos + idx;
        leaf.val = val;
    });

    // calculate merkle path up to root
    for(int d = stats.mt_depth; d > 0; d--)
    {
        // if array index of node is uneven it is always the left child
        bool is_left_child = 1 == idx % 2;

        // determine sister node
        uint64_t sis_idx = is_left_child ? idx + 1 : idx - 1;

        // get values of both nodes
        //                            (n)              |            (n)
        //                          /     \            |         /      \
        //                       (idx)     (0)         |     (sis_idx)  (idx)
        Fp l = is_left_child ? tree.get(tos + idx).val : tree.get(tos + sis_idx).val;   // implicit conversion
        Fp r = is_left_child ? EMPTY_ROOT[d]           : tree.get(tos + idx).val;       // implicit conversion

        // calculate sinsemilla merkle hash of parent node
        Fp parent_val = sinsemilla_combine(d, l, r);

        // set idx to parent node index:
        // left child's array index divided by two (integer division) equals array index of parent node
        idx = is_left_child ? idx / 2 : sis_idx / 2;

        // check if parent node was already created
        auto it = tree.find(tos + idx);
        // write new parent
        if(it == tree.end())
        {
            tree.emplace(_self, [&](auto& node) {
                node.idx = tos + idx;
                node.val = parent_val.data; // implicit conversion
            });
        }
        else
        {
            tree.modify(it, _self, [&](auto& node) {
                node.val = parent_val.data; // implicit conversion
            });
        }
    }
    
    // update global stats
    ++stats.mt_leaf_count;
    global.set(stats, _self);

    // return root node
    return tree.get(tos).val;
}

bool thezeostoken::is_root_valid(const checksum256& root)
{
    // a root is valid if it is the root of an existing full merkle tree OR in the queue
    // of the most recent roots of the current merkle tree
    
    // check if root is in deque of most recent roots
    auto stats = global.get();

    for(auto r = stats.mt_roots.begin(); r != stats.mt_roots.end(); ++r)
    {
        if(*r == root)
        {
            return true;
        }
    }
    
    // check roots of previous, full merkle trees (tree_index > 0)
    uint64_t tree_idx = stats.mt_leaf_count / MT_NUM_LEAVES(stats.mt_depth);

    mt_t tree(_self, _self.value);
    for(uint64_t t = 0; t < tree_idx; ++t)
    {
        // can use get here because root must exist if this tree has a leaf
        auto it = tree.get(t * MT_ARR_FULL_TREE_OFFSET(stats.mt_depth));
        
        if(it.val == root)
        {
            return true;
        }
    }
    
    // root was not found
    return false;
}
