#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_IPFS(name) \
    SVC_RESP_X(ipfs,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_IPFS SVC_CONTRACT_NAME_IPFS_undefined 

#else
#define SVC_CONTRACT_NAME_IPFS ipfsservice1 

#endif

#include "../dappservices/_ipfs_impl.hpp"



#define IPFS_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(commit, false, ((std::vector<char>)(data)),     \
         ((uint32_t)(size))((std::string)(uri)), \
         ((uint32_t)(size))((std::string)(uri)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_commit(size, uri, current_provider); \
    SEND_SVC_SIGNAL(commit, current_provider, package, size, uri)                         \
}; \
SVC_ACTION(cleanup, false, ((std::string)(uri)),     \
         ((uint32_t)(size))((std::string)(uri)), \
         ((uint32_t)(size))((std::string)(uri)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_cleanup(size, uri, current_provider); \
    SEND_SVC_SIGNAL(cleanup, current_provider, package, size, uri)                         \
}; \
SVC_ACTION(warmup, true, ((std::string)(uri)),     \
         ((uint32_t)(size))((std::string)(uri)), \
         ((uint32_t)(size))((std::string)(uri))((std::vector<char>)(data)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_warmup(size, uri, data, current_provider); \
    SEND_SVC_SIGNAL(warmup, current_provider, package, size, uri)                         \
}; \
SVC_ACTION(warmupcode, true, ((std::string)(uri))((name)(code)),     \
         ((uint32_t)(size))((std::string)(uri)), \
         ((uint32_t)(size))((std::string)(uri))((std::vector<char>)(data)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_warmupcode(size, uri, data, current_provider); \
    SEND_SVC_SIGNAL(warmupcode, current_provider, package, size, uri)                         \
}; \
SVC_ACTION(warmuprow, true, ((std::string)(uri))((name)(code))((name)(table))((uint64_t)(scope))((uint8_t)(index_position))((checksum256)(key))((uint8_t)(keysize)),     \
         ((uint32_t)(size))((vector<std::string>)(uris)), \
         ((uint32_t)(size))((vector<std::string>)(uris))((vector<vector<char>>)(data)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_warmuprow(size, uris, data, current_provider); \
    SEND_SVC_SIGNAL(warmuprow, current_provider, package, size, uris)                         \
}; \
SVC_ACTION(warmupchain, true, ((uint32_t)(shard))((name)(code))((name)(table))((name)(chain))((uint64_t)(scope))((uint8_t)(index_position))((checksum256)(key))((uint8_t)(keysize)),     \
         ((uint32_t)(shard))((name)(code))((name)(table))((name)(chain))((uint32_t)(size))((vector<std::string>)(uris)), \
         ((uint32_t)(shard))((name)(code))((name)(table))((name)(chain))((uint32_t)(size))((vector<std::string>)(uris))((vector<vector<char>>)(data)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_warmupchain(shard, code, table, chain, size, uris, data, current_provider); \
    SEND_SVC_SIGNAL(warmupchain, current_provider, package, shard, code, table, chain, size, uris)                         \
}; \
SVC_ACTION(cleanuprow, false, ((vector<string>)(uris)),     \
         ((uint32_t)(size))((vector<string>)(uris)), \
         ((uint32_t)(size))((vector<string>)(uris)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_cleanuprow(size, uris, current_provider); \
    SEND_SVC_SIGNAL(cleanuprow, current_provider, package, size, uris)                         \
}; \
SVC_ACTION(cleanchain, false, ((vector<string>)(uris)),     \
         ((uint32_t)(shard))((name)(code))((name)(table))((name)(chain))((uint32_t)(size))((vector<string>)(uris)), \
         ((uint32_t)(shard))((name)(code))((name)(table))((name)(chain))((uint32_t)(size))((vector<string>)(uris)),TONAME(SVC_CONTRACT_NAME_IPFS) ) \
{ \
    _ipfs_cleanchain(shard, code, table, chain, size, uris, current_provider); \
    SEND_SVC_SIGNAL(cleanchain, current_provider, package, shard, code, table, chain, size, uris)                         \
};  \
static void svc_ipfs_commit(std::vector<char> data) { \
    SEND_SVC_REQUEST(commit, data) \
};\
static void svc_ipfs_cleanup(std::string uri) { \
    SEND_SVC_REQUEST(cleanup, uri) \
};\
static void svc_ipfs_warmup(std::string uri) { \
    SEND_SVC_REQUEST(warmup, uri) \
};\
static void svc_ipfs_warmupcode(std::string uri, name code) { \
    SEND_SVC_REQUEST(warmupcode, uri, code) \
};\
static void svc_ipfs_warmuprow(std::string uri, name code, name table, uint64_t scope, uint8_t index_position, checksum256 key, uint8_t keysize) { \
    SEND_SVC_REQUEST(warmuprow, uri, code, table, scope, index_position, key, keysize) \
};\
static void svc_ipfs_warmupchain(uint32_t shard, name code, name table, name chain, uint64_t scope, uint8_t index_position, checksum256 key, uint8_t keysize) { \
    SEND_SVC_REQUEST(warmupchain, shard, code, table, chain, scope, index_position, key, keysize) \
};\
static void svc_ipfs_cleanuprow(vector<string> uris) { \
    SEND_SVC_REQUEST(cleanuprow, uris) \
};\
static void svc_ipfs_cleanchain(vector<string> uris) { \
    SEND_SVC_REQUEST(cleanchain, uris) \
};


#ifdef IPFS_DAPPSERVICE_ACTIONS_MORE
#define IPFS_DAPPSERVICE_ACTIONS \
  IPFS_DAPPSERVICE_BASE_ACTIONS \
  IPFS_DAPPSERVICE_ACTIONS_MORE() 


#else
#define IPFS_DAPPSERVICE_ACTIONS \
  IPFS_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef IPFS_SVC_COMMANDS
#define IPFS_SVC_COMMANDS() (xcommit)(xcleanup)(xwarmup)(xwarmupcode)(xwarmuprow)(xwarmupchain)(xcleanuprow)(xcleanchain)


#ifndef IPFS_DAPPSERVICE_SKIP_HELPER
struct ipfs_svc_helper{
    IPFS_DAPPSERVICE_ACTIONS
};
#endif

#endif