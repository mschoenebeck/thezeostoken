#pragma once
#define THEZEOSTOKEN_HEADER_FILE

// comment out to use EOS RAM for merkle tree, tx data and nullifier tables, uncomment to use VRAM
//#define USE_VRAM

// VRAM and LiquidOracle includes
#define USE_ADVANCED_IPFS
#include <eosio/eosio.hpp>
#include "../dappservices/ipfs.hpp"
#include "../dappservices/multi_index.hpp"
#include "../dappservices/oracle.hpp"
#include "../../../../zeosio/include/zeosio.hpp"
#include <optional>

using namespace zeosio::groth16;
using namespace eosio;
using namespace std;

#define DAPPSERVICES_ACTIONS() \
    XSIGNAL_DAPPSERVICE_ACTION \
    IPFS_DAPPSERVICE_ACTIONS \
    ORACLE_DAPPSERVICE_ACTIONS

#define DAPPSERVICE_ACTIONS_COMMANDS() \
    IPFS_SVC_COMMANDS() \
    ORACLE_SVC_COMMANDS()

#define G_ROOTS_FIFO_SIZE 32

#define CONTRACT_NAME() thezeostoken

// This version of eosio::get_action doesn't abort when index is out of range.
// Thanks Todd Fleming! https://eoscommunity.github.io/clsdk-docs/book/std/cpay/index.html
optional<action> better_get_action(uint32_t type, uint32_t index)
{
    auto size = internal_use_do_not_use::get_action(type, index, nullptr, 0);
    if(size < 0)
        return nullopt;
    vector<char> raw(size);
    auto size2 = internal_use_do_not_use::get_action(type, index, raw.data(), size);
    check(size2 == size, "get_action failed");
    return unpack<action>(raw.data(), size);
}

CONTRACT_START()
    
    // shardbucket table for dapp::multi_index
    TABLE shardbucket
    {
        vector<char> shard_uri;
        uint64_t shard;

        uint64_t primary_key() const { return shard; }
    };

    // verifier keys table
    TABLE vk
    {
        name id;
        string vk;

        uint64_t primary_key() const { return id.value; }
    };
    typedef eosio::multi_index<"vk"_n, vk> vk_t;

    // encrypted notes table
    TABLE enc_note
    {
        uint64_t idx;
        string ciphertext;
        uint64_t block_num;
        
        uint64_t primary_key() const { return idx; }
    };
#ifdef USE_VRAM
    typedef dapp::advanced_multi_index<"notes"_n, enc_note, uint64_t> enc_notes_t;
    typedef eosio::multi_index<".notes"_n, enc_note> notes_t_v_abi;
    typedef eosio::multi_index<"notes"_n, shardbucket> notes_t_abi;
#else
    typedef eosio::multi_index<"noteseosram"_n, enc_note> enc_notes_t;
#endif

    // zeos note commitments merkle tree table
    TABLE merkle_node
    {
        uint64_t idx;
        checksum256 val;

        uint64_t primary_key() const { return idx; }
    };
#ifdef USE_VRAM
    typedef dapp::advanced_multi_index<"mt"_n, merkle_node, uint64_t> mt_t;
    typedef eosio::multi_index<".mt"_n, merkle_node> mt_t_v_abi;
    typedef eosio::multi_index<"mt"_n, shardbucket> mt_t_abi;
#else
    typedef eosio::multi_index<"mteosram"_n, merkle_node> mt_t;
#endif

    // zeos nullifier table
    TABLE nullifier
    {
        checksum256 val;
        uint64_t block_num;
#ifdef USE_VRAM
        checksum256 primary_key() const { return val; }
    };
    typedef dapp::advanced_multi_index<"nf"_n, nullifier, checksum256> nf_t;
    typedef eosio::multi_index<".nf"_n, nullifier> nf_t_v_abi;
    typedef eosio::multi_index<"nf"_n, shardbucket> nf_t_abi;
#else
        // on eos just use the lower 64 bits of the hash as primary key since collisions are very unlikely
        uint64_t primary_key() const { return (uint64_t)*((uint32_t*)val.extract_as_byte_array().data()); }
    };
    typedef eosio::multi_index<"nfeosram"_n, nullifier> nf_t;
#endif

    // buffers an entire tx for the time of execution
    TABLE tx_buffer
    {
        size_t cur;
        size_t last;
        vector<action> tx;
    };
    using txb_t = singleton<"txbuffer"_n, tx_buffer>;
    txb_t txb;

    // buffers a quantity of fungible tokens
    TABLE ft_buffer
    {
        extended_asset quantity;
    };
    using ftb_t = singleton<"ftbuffer"_n, ft_buffer>;
    ftb_t ftb;

    TABLE global
    {
        uint64_t note_count;            // number of encrypted notes
        uint64_t mt_leaf_count;         // number of merkle tree leaves
        uint64_t mt_depth;              // merkle tree depth
        deque<checksum256> mt_roots;    // stores the most recent roots defined by MTS_NUM_ROOTS. the current root is always the first element
    };
    using g_t = singleton<"global"_n, global>;
    g_t global;

    // token contract tables
    TABLE account
    {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"accounts"_n, account> accounts;

    TABLE currency_stats
    {
        asset supply;
        asset max_supply;
        name issuer;

        uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };
    typedef eosio::multi_index<"stat"_n, currency_stats> stats;
    
    void sub_balance(const name& owner,
                     const asset& value);
    
    void add_balance(const name& owner,
                     const asset& value,
                     const name& ram_payer);
    
    checksum256 insert_into_merkle_tree(const checksum256& val);
    
    bool is_root_valid(const checksum256& root);

    public:

    thezeostoken(name self,
                 name code, 
                 datastream<const char *> ds);

    // set verifier key
    ACTION setvk(const name& code,
                 const name& id,
                 const string& vk);

    // verify proof
    ACTION verifyproof(const string& type,
                       const name& code,
                       const name& id,
                       const string& proof,
                       const string& inputs);

    // execute transaction
    ACTION begin(const string& proof, vector<action>& tx);
    ACTION step();
    ACTION exec(const vector<zaction>& ztx);
    ACTION test22(const vector<zaction>& ztx, const uint64_t& test22);
    void onfttransfer(name from, name to, asset quantity, string memo);

    // init
    ACTION init(const uint64_t& depth);

    // token contract actions
    ACTION create(const name& issuer,
                  const asset& maximum_supply);

    ACTION issue(const name& to,
                 const asset& quantity,
                 const string& memo);
    
    ACTION retire(const asset& quantity,
                  const string& memo);

    ACTION transfer(const name& from,
                    const name& to,
                    const asset& quantity,
                    const string& memo);

    ACTION open(const name& owner,
                const symbol& symbol,
                const name& ram_payer);

    ACTION close(const name& owner,
                 const symbol& symbol);

    inline asset get_supply(const symbol_code& sym) const;
    
    inline asset get_balance(const name& owner,
                             const symbol_code& sym) const;
    
CONTRACT_END((setvk)(verifyproof)(begin)(step)(exec)(test22)(init)(create)(issue)(retire)(transfer)(open)(close)(xdcommit))
/*
};

extern "C"
{
    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        if(receiver == name("thezeostoken").value && action == name("transfer").value)
        {
            execute_action(name(receiver), name(code), &CONTRACT_NAME()::ontransfer);
        }
        else
        {
            switch(action)
            {
                EOSIO_DISPATCH_HELPER(CONTRACT_NAME(), DAPPSERVICE_ACTIONS_COMMANDS())
                EOSIO_DISPATCH_HELPER(CONTRACT_NAME(), (setvk)(verifyproof)(begin)(step)(exec)(test22)(init)(create)(issue)(retire)(transfer)(open)(close))
                EOSIO_DISPATCH_HELPER(CONTRACT_NAME(), (xsignal))
            }
        }
        eosio_exit(0);
    }
}
*/