#pragma once

// comment out to use EOS RAM for merkle tree, tx data and nullifier tables, uncomment to use VRAM
//#define USE_VRAM

// determine merkle tree stats table index for global mt status data
#ifdef USE_VRAM
#define MT_IDX 0
#else
#define MT_IDX 1
#endif

// VRAM and LiquidOracle includes
#define USE_ADVANCED_IPFS
#include <eosio/eosio.hpp>
#include "../dappservices/ipfs.hpp"
#include "../dappservices/multi_index.hpp"
#include "../dappservices/oracle.hpp"
#include "zeos.hpp"

using namespace eosio;
using namespace std;

#define DAPPSERVICES_ACTIONS() \
    XSIGNAL_DAPPSERVICE_ACTION \
    IPFS_DAPPSERVICE_ACTIONS \
    ORACLE_DAPPSERVICE_ACTIONS

#define DAPPSERVICE_ACTIONS_COMMANDS() \
    IPFS_SVC_COMMANDS() \
    ORACLE_SVC_COMMANDS()


#define CONTRACT_NAME() thezeostoken

CONTRACT_START()
    
    // shardbucket table for dapp::multi_index
    TABLE shardbucket
    {
        vector<char> shard_uri;
        uint64_t shard;

        uint64_t primary_key() const { return shard; }
    };
    
    // verifier keys table
    TABLE verifierkey
    {
        name id;
        string vk;

        uint64_t primary_key() const { return id.value; }
    };
    typedef dapp::advanced_multi_index<"verifierkey"_n, verifierkey, uint64_t> vks;
    typedef eosio::multi_index<".verifierkey"_n, verifierkey> vks_t_v_abi;
    typedef eosio::multi_index<"verifierkey"_n, shardbucket> vks_t_abi;

#ifdef USE_VRAM
    // zeos private transaction data table
    TABLE transaction_data
    {
        // tx counter: auto increment
        uint128_t id;
        // the actual encrypted tx data
        checksum256 epk_s;              // [u8; 32]
        vector<uint8_t> ciphertext_s;   // Vec<u8>,
        checksum256 epk_r;              // [u8; 32]
        vector<uint8_t> ciphertext_r;   // Vec<u8>
        
        uint128_t primary_key() const { return id; }
    };
    typedef dapp::advanced_multi_index<"txd"_n, transaction_data, uint128_t> txd;
    typedef eosio::multi_index<".txd"_n, transaction_data> txd_t_v_abi;
    typedef eosio::multi_index<"txd"_n, shardbucket> txd_t_abi;
#else
    // zeos private transaction data table
    TABLE transaction_data
    {
        // tx counter: auto increment (uint64_t on EOS RAM)
        uint64_t id;
        // the actual encrypted tx data
        checksum256 epk_s;              // [u8; 32]
        vector<uint8_t> ciphertext_s;   // Vec<u8>,
        checksum256 epk_r;              // [u8; 32]
        vector<uint8_t> ciphertext_r;   // Vec<u8>
        
        uint64_t primary_key() const { return id; }
    };
    typedef eosio::multi_index<"txdeosram"_n, transaction_data> txd;
#endif

#ifdef USE_VRAM
    // zeos note commitments merkle tree table
    TABLE merkle_tree
    {
        uint128_t id;
        checksum256 val;

        uint128_t primary_key() const { return id; }
    };
    typedef dapp::advanced_multi_index<"mt"_n, merkle_tree, uint128_t> mt;
    typedef eosio::multi_index<".mt"_n, merkle_tree> mt_t_v_abi;
    typedef eosio::multi_index<"mt"_n, shardbucket> mt_t_abi;
#else
    // zeos note commitments merkle tree table
    TABLE merkle_tree
    {
        uint64_t id;        // uint64_t on EOS RAM
        checksum256 val;

        uint64_t primary_key() const { return id; }
    };
    typedef eosio::multi_index<"mteosram"_n, merkle_tree> mt;
#endif

#ifdef USE_VRAM
    // zeos nullifier table
    TABLE nullifier
    {
        checksum256 val;

        checksum256 primary_key() const { return val; }
    };
    typedef dapp::advanced_multi_index<"nf"_n, nullifier, checksum256> nf;
    typedef eosio::multi_index<".nf"_n, nullifier> nf_t_v_abi;
    typedef eosio::multi_index<"nf"_n, shardbucket> nf_t_abi;
#else
    // zeos nullifier table
    TABLE nullifier
    {
        checksum256 val;

        // on eos just use the lower 64 bits of the hash as primary key since collisions are very unlikely
        uint64_t primary_key() const { return *((uint64_t*)val.extract_as_byte_array().data()); }
    };
    typedef eosio::multi_index<"nfeosram"_n, nullifier> nf;
#endif

    TABLE merkle_tree_status
    {
        uint64_t index; // = 0 for vram, = 1 for eos ram
        uint64_t tree_index;
        uint128_t leaf_index;
        vector<checksum256> roots;

        uint64_t primary_key() const { return index; }
    };
    typedef eosio::multi_index<"mtstat"_n, merkle_tree_status> mtstat;

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

    public:

    thezeostoken(name self,
                 name code, 
                 datastream<const char *> ds);

    // set verifier key
    ACTION setvk(const name& code,
                 const name& id,
                 const string& vk);

    // verify proof
    ACTION verifyproof(const name& code,
                       const name& id,
                       const string& proof,     
                       const string& inputs);

    // zMint
    ACTION zmint(const checksum256& epk_s,
                 const vector<uint8_t>& ciphertext_s,
                 const checksum256& epk_r,
                 const vector<uint8_t>& ciphertext_r,
                 const string& proof,
                 //const string& inputs,
                 const asset& a,
                 const checksum256& z_a,
                 const name& user);

    // zTransfer
    ACTION ztransfer(const checksum256& epk_s,
                     const vector<uint8_t>& ciphertext_s,
                     const checksum256& epk_r,
                     const vector<uint8_t>& ciphertext_r,
                     const string& proof,
                     //const string& inputs,
                     const checksum256& nf_a,
                     const checksum256& z_b,
                     const checksum256& z_c,
                     const checksum256& root);

    // zBurn
    ACTION zburn(const checksum256& epk_s,
                 const vector<uint8_t>& ciphertext_s,
                 const checksum256& epk_r,
                 const vector<uint8_t>& ciphertext_r,
                 const string& proof,
                 //const string& inputs,
                 const checksum256& nf_a,
                 const asset& b,
                 const checksum256& z_c,
                 const checksum256& root,
                 const name& user);

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
    
CONTRACT_END((setvk)(verifyproof)(zmint)(ztransfer)(zburn)(create)(issue)(retire)(transfer)(open)(close)(xdcommit))
