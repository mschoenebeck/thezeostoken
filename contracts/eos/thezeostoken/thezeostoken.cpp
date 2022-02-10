#include "thezeostoken.hpp"

thezeostoken::thezeostoken(name self, name code, datastream<const char *> ds) :
    contract(self, code, ds)
{
}

void thezeostoken::setvk(const name& code, const name& id, const string& vk)
{
    require_auth(code);
    
    vks vk_t(get_self(), code.value);
    auto c = vk_t.find(id.value);
    
    if(c == vk_t.end())
    {
        // add new key
        vk_t.emplace(code, [&](auto& row){
            row.id = id;
            row.vk = vk;
        });
    }
    else
    {
        if(vk.empty())
        {
            // erase existing key to free up RAM
            vk_t.erase(c);
        }
        else
        {
            // update existing key
            vk_t.modify(c, code, [&](auto& row){
                row.vk = vk;
            });
        }
    }
}

void thezeostoken::verifyproof(const name& code, const name& id, const string& proof, const string& inputs)
{
    vks vk_t(get_self(), code.value);
    auto c = vk_t.find(id.value);
    check(c != vk_t.end(), "id doesn't exist");

    // TODO: is it okay to pack 'proof' and 'inputs' as JSON strings into URI? It seems to work here...
    string str = "zeos_verify_proof://";
    str.append(code.to_string());
    str.append("/");
    str.append(id.to_string());
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
    
    check(valid, "proof invalid");
    //print("Proof verified by ", dsp_count, " DSPs\n\r");
}

// zMint
void thezeostoken::zmint(const checksum256& epk_s,
                         const vector<uint8_t>& ciphertext_s,
                         const checksum256& epk_r,
                         const vector<uint8_t>& ciphertext_r,
                         const string& proof,
                         //const string& inputs,
                         const asset& a,
                         const checksum256& z_a,
                         const name& user)
{
    require_auth(user);
    check(a.amount >= 0, "a.amount invalid");

    // pack inputs in order: amt_a, sym_a, z_a
    vector<bool> bits;
    append_bits(bits, a.amount);
    append_bits(bits, a.symbol.raw());
    append_bits(bits, z_a);
    string inputs = inputs_json(compute_multipacking(bits));

    // verify proof
    verifyproof(_self, "zeosmintnote"_n, proof, inputs);

    // burn a from user's balance
    sub_balance(user, a);

    // add z_a to tree
    insert_into_merkle_tree(z_a);

    // add tx data
#ifdef USE_VRAM
    // TODO
#else
    
#endif
}

// zTransfer
void thezeostoken::ztransfer(const checksum256& epk_s,
                             const vector<uint8_t>& ciphertext_s,
                             const checksum256& epk_r,
                             const vector<uint8_t>& ciphertext_r,
                             const string& proof,
                             //const string& inputs,
                             const checksum256& nf_a,
                             const checksum256& z_b,
                             const checksum256& z_c,
                             const checksum256& root)
{
    // pack inputs
    // verify proof
    // add nf
    // add z_b and z_c to tree
    // add tx data
}

// zBurn
void thezeostoken::zburn(const checksum256& epk_s,
                         const vector<uint8_t>& ciphertext_s,
                         const checksum256& epk_r,
                         const vector<uint8_t>& ciphertext_r,
                         const string& proof,
                         //const string& inputs,
                         const checksum256& nf_a,
                         const asset& b,
                         const checksum256& z_c,
                         const checksum256& root,
                         const name& user)
{
    // NOTE: uncomment if users should only be able to withdraw to their own EOS account, not to someone else's
    //require_auth(user);

    // pack inputs
    // verify proof
    // mint b to user's balance
    // add nf
    // add z_c to tree
    // add tx data
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

const uint64_t thezeostoken::MT_ARR_OFFSET[] = {
    0,
    1,
    3,
    7,
    15,
    31,
    63,
    127,
    255,
    511,
    1023,
    2047,
    4095,
    8191,
    16383,
    32767,
    65535,
    131071,
    262143,
    524287,
    1048575,
    2097151,
    4194303,
    8388607,
    16777215,
    33554431,
    67108863,
    134217727,
    268435455,
    536870911,
    1073741823,
    2147483647
};

// merkle tree:
//        ()        
//      /    \
//    ()      ()    
//   /  \    /  \
//  []  []  []  []  
//  l_0 l_1 ... l_2^max
void thezeostoken::insert_into_merkle_tree(const checksum256& val)
{
    // fetch merkle tree state
    mtstate mts(_self, _self.value);
#ifdef USE_VRAM
    auto state = mts.find(0);
#else
    auto state = mts.find(1);
#endif
    check(state != mts.end(), "merkle tree state table not initialized");

    // calculate array index of next free leaf
    uint64_t idx = MT_ARR_OFFSET[state->depth] + state->leaf_index;

    // insert val into leaf
    mt tree(_self, _self.value);
    tree.emplace(_self, [&](auto& leaf) {
        leaf.idx = idx;
        leaf.val = val;
    });

    // calculate merkle path up to root
    for(int d = state->depth; d > 0; d--)
    {
        // if array index of node is uneven it is always the left child
        bool is_left_child = 1 == idx % 2;

        // determine sister node
        uint64_t sis_idx = is_left_child ? idx + 1 : idx - 1;

        // get values of both nodes
        checksum256 l = is_left_child ? tree.get(idx).val : tree.get(sis_idx).val;  //   (idx)     (0)
        checksum256 r = is_left_child ? checksum256() /* =0 */ : tree.get(idx).val; // (sis_idx)  (idx)

        // concatenate and digest
        // TODO
        checksum256 parent_val = checksum256();

        // set idx to parent node index:
        // left child's array index divided by two (integer division) equals array index of parent node
        idx = is_left_child ? idx / 2 : sis_idx / 2;

        // check if parent node was already created
        auto it = tree.find(idx);
        // write new parent
        if(it == tree.end())
        {
            tree.emplace(_self, [&](auto& node) {
                node.idx = idx;
                node.val = parent_val;
            });
        }
        else
        {
            tree.modify(it, _self, [&](auto& node) {
                node.val = parent_val;
            });
        }
    }

    // increment leaf index
}
