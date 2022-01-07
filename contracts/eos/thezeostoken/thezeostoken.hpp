#pragma once

// LiquidStorage and LiquidOracle
#define USE_ADVANCED_IPFS
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
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
    
    // LiquidStorage Config Table
    TABLE storagecfg
    {
        // all measurements in bytes
        uint64_t max_file_size_in_bytes = UINT64_MAX; // max file size in bytes that can be uploaded at a time, default 10mb
        uint64_t global_upload_limit_per_day = UINT64_MAX; // max upload limit in bytes per day for EOS account, default 1 GB
        uint64_t vaccount_upload_limit_per_day = UINT64_MAX; // max upload limit in bytes per day for LiquidAccounts, default 10 MB
    };
    typedef eosio::singleton<"storagecfg"_n, storagecfg> storagecfg_t;
    
    
    // shardbucket table for dapp::multi_index
    TABLE shardbucket
    {
        vector<char> shard_uri;
        uint64_t shard;

        uint64_t primary_key() const { return shard; }
    };
    
    // This table contains the verifier keys for all circuits [OLD]
    /*
    TABLE vk
    {
        uint64_t index;
        string vk_str;

        uint64_t primary_key() const { return index; }
    };
    typedef dapp::multi_index<"vks"_n, vk> vks;
    typedef eosio::multi_index<".vks"_n, vk> vks_t_v_abi;
    typedef eosio::multi_index<"vks"_n, shardbucket> vks_t_abi;
    */
    
    // This table contains the verifier keys for all circuits [NEW]
    TABLE verifierkey
    {
        name id;
        string vk;

        uint64_t primary_key() const { return id.value; }
    };
    typedef dapp::advanced_multi_index<"verifierkey"_n, verifierkey, uint64_t> vks;
    typedef eosio::multi_index<".verifierkey"_n, verifierkey> vks_t_v_abi;
    typedef eosio::multi_index<"verifierkey"_n, shardbucket> vks_t_abi;

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
    
    void sub_balance( const name& owner, const asset& value );
    void add_balance( const name& owner, const asset& value, const name& ram_payer );

    public:

    // constructor
    thezeostoken(name self, name code, datastream<const char *> ds);

    // SET PARAMS FOR storagecfg TABLE //
    ACTION setstoragecfg(const uint64_t &max_file_size_in_bytes,
                         const uint64_t &global_upload_limit_per_day,
                         const uint64_t &vaccount_upload_limit_per_day);

    // set verifier key
    ACTION setvk(const name& user, const name& id, const string& vk);

    // verify proof
    ACTION verifyproof(const name& id, const string& proof, const string& inputs, const string& payload_uri);

    // token contract
    ACTION create(const name& issuer, const asset& maximum_supply);

    ACTION issue(const name& to, const asset& quantity, const string& memo);
    
    ACTION retire(const asset& quantity, const string& memo);

    ACTION test(const string& json);

    ACTION transfer(const name& from,
                    const name& to,
                    const asset& quantity,
                    const string& memo);

    ACTION open(const name& owner, const symbol& symbol, const name& ram_payer);

    ACTION close(const name& owner, const symbol& symbol);

    inline asset get_supply(const symbol_code& sym) const;
      
    inline asset get_balance(const name& owner, const symbol_code& sym) const;
    
CONTRACT_END((setstoragecfg)(setvk)(verifyproof)(create)(issue)(test)(retire)(transfer)(open)(close)(xdcommit))
