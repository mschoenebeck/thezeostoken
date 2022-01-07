#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_READFN(name) \
    SVC_RESP_X(readfn,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_READFN SVC_CONTRACT_NAME_READFN_undefined 

#else
#define SVC_CONTRACT_NAME_READFN readfndspsvc 

#endif

#include "../dappservices/_readfn_impl.hpp"



#define READFN_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(rfnuse, false, ,     \
         ((uint64_t)(size)), \
         ((uint64_t)(size)),TONAME(SVC_CONTRACT_NAME_READFN) ) \
{ \
    _readfn_rfnuse(size, current_provider); \
    SEND_SVC_SIGNAL(rfnuse, current_provider, package, size)                         \
};  \
static void svc_readfn_rfnuse() { \
    SEND_SVC_REQUEST(rfnuse, ) \
};


#ifdef READFN_DAPPSERVICE_ACTIONS_MORE
#define READFN_DAPPSERVICE_ACTIONS \
  READFN_DAPPSERVICE_BASE_ACTIONS \
  READFN_DAPPSERVICE_ACTIONS_MORE() 


#else
#define READFN_DAPPSERVICE_ACTIONS \
  READFN_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef READFN_SVC_COMMANDS
#define READFN_SVC_COMMANDS() (xrfnuse)


#ifndef READFN_DAPPSERVICE_SKIP_HELPER
struct readfn_svc_helper{
    READFN_DAPPSERVICE_ACTIONS
};
#endif

#endif