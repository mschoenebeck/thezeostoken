#pragma once

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

    thezeostoken(name self, name code, datastream<const char *> ds);

    ACTION create(const name& issuer, const asset& maximum_supply);

    ACTION issue(const name& to, const asset& quantity, const string& memo);
    
    ACTION retire(const asset& quantity, const string& memo);

    ACTION transfer(const name& from,
                    const name& to,
                    const asset& quantity,
                    const string& memo);

    ACTION open(const name& owner, const symbol& symbol, const name& ram_payer);

    ACTION close(const name& owner, const symbol& symbol);

    inline asset get_supply(const symbol_code& sym) const;
    inline asset get_balance(const name& owner, const symbol_code& sym) const;
    
CONTRACT_END((create)(issue)(retire)(transfer)(open)(close)(xdcommit))
