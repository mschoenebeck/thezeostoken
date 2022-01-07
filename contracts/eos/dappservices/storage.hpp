#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_STORAGE(name) \
    SVC_RESP_X(storage,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_STORAGE SVC_CONTRACT_NAME_STORAGE_undefined 

#else
#define SVC_CONTRACT_NAME_STORAGE liquidstorag 

#endif

#include "../dappservices/_storage_impl.hpp"



#define STORAGE_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(sdummy, false, ((std::vector<char>)(data)),     \
         ((uint32_t)(size))((std::string)(uri)), \
         ((uint32_t)(size))((std::string)(uri)),TONAME(SVC_CONTRACT_NAME_STORAGE) ) \
{ \
    _storage_sdummy(size, uri, current_provider); \
    SEND_SVC_SIGNAL(sdummy, current_provider, package, size, uri)                         \
};  \
static void svc_storage_sdummy(std::vector<char> data) { \
    SEND_SVC_REQUEST(sdummy, data) \
};


#ifdef STORAGE_DAPPSERVICE_ACTIONS_MORE
#define STORAGE_DAPPSERVICE_ACTIONS \
  STORAGE_DAPPSERVICE_BASE_ACTIONS \
  STORAGE_DAPPSERVICE_ACTIONS_MORE() 


#else
#define STORAGE_DAPPSERVICE_ACTIONS \
  STORAGE_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef STORAGE_SVC_COMMANDS
#define STORAGE_SVC_COMMANDS() (xsdummy)


#ifndef STORAGE_DAPPSERVICE_SKIP_HELPER
struct storage_svc_helper{
    STORAGE_DAPPSERVICE_ACTIONS
};
#endif

#endif