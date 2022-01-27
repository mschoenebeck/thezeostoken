#pragma once

// VRAM and LiquidOracle
#define USE_ADVANCED_IPFS
#include <eosio/eosio.hpp>
#include "../dappservices/ipfs.hpp"
#include "../dappservices/multi_index.hpp"
#include "../dappservices/oracle.hpp"

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

    // zeos private transaction data table
    TABLE tx
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
    typedef dapp::advanced_multi_index<"tx"_n, tx, uint128_t> txd;
    typedef eosio::multi_index<".tx"_n, tx> txd_t_v_abi;
    typedef eosio::multi_index<"tx"_n, shardbucket> txd_t_abi;

    // zeos note commitments merkle tree table
    TABLE mtree
    {
        uint128_t id;
        checksum256 val;

        uint128_t primary_key() const { return id; }
    };
    typedef dapp::advanced_multi_index<"mtree"_n, mtree, uint128_t> mt;
    typedef eosio::multi_index<".mtree"_n, mtree> mt_t_v_abi;
    typedef eosio::multi_index<"mtree"_n, shardbucket> mt_t_abi;

    // zeos nullifier table
    TABLE nullifier
    {
        checksum256 val;

        checksum256 primary_key() const { return val; }
    };
    typedef dapp::advanced_multi_index<"nullifier"_n, nullifier, checksum256> nf;
    typedef eosio::multi_index<".nullifier"_n, nullifier> nf_t_v_abi;
    typedef eosio::multi_index<"nullifier"_n, shardbucket> nf_t_abi;

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
                 const string& inputs);

    // zTransfer
    ACTION ztransfer(const checksum256& epk_s,
                     const vector<uint8_t>& ciphertext_s,
                     const checksum256& epk_r,
                     const vector<uint8_t>& ciphertext_r,
                     const string& proof,
                     const string& inputs);

    // zBurn
    ACTION zburn(const checksum256& epk_s,
                 const vector<uint8_t>& ciphertext_s,
                 const checksum256& epk_r,
                 const vector<uint8_t>& ciphertext_r,
                 const string& proof,
                 const string& inputs);

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
    
CONTRACT_END((setvk)(verifyproof)(create)(issue)(retire)(transfer)(open)(close)(xdcommit))
