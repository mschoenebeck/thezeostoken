#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_SIGN(name) \
    SVC_RESP_X(sign,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_SIGN SVC_CONTRACT_NAME_SIGN_undefined 

#else
#define SVC_CONTRACT_NAME_SIGN signfndspsvc 

#endif

#include "../dappservices/_sign_impl.hpp"



#define SIGN_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(signtrx, false, ((std::string)(id))((std::string)(destination))((std::string)(trx_data))((std::string)(chain))((std::string)(chain_type))((std::string)(account))((uint16_t)(sigs_required)),     \
         ((std::string)(id))((std::string)(destination))((std::string)(trx_data))((std::string)(chain))((std::string)(chain_type))((std::string)(account)), \
         ((std::string)(id))((std::string)(destination))((std::string)(trx_data))((std::string)(chain))((std::string)(chain_type))((std::string)(account))((std::string)(trx_id)),TONAME(SVC_CONTRACT_NAME_SIGN) ) \
{ \
    _sign_signtrx(id, destination, trx_data, chain, chain_type, account, trx_id, current_provider); \
    SEND_SVC_SIGNAL(signtrx, current_provider, package, id, destination, trx_data, chain, chain_type, account)                         \
}; \
SVC_ACTION(sgcleanup, false, ((uint64_t)(id)),     \
         ((uint64_t)(id)), \
         ((uint64_t)(id)),TONAME(SVC_CONTRACT_NAME_SIGN) ) \
{ \
    _sign_sgcleanup(id, current_provider); \
    SEND_SVC_SIGNAL(sgcleanup, current_provider, package, id)                         \
};  \
static void svc_sign_signtrx(std::string id, std::string destination, std::string trx_data, std::string chain, std::string chain_type, std::string account, uint16_t sigs_required) { \
    SEND_SVC_REQUEST(signtrx, id, destination, trx_data, chain, chain_type, account, sigs_required) \
};\
static void svc_sign_sgcleanup(uint64_t id) { \
    SEND_SVC_REQUEST(sgcleanup, id) \
};


#ifdef SIGN_DAPPSERVICE_ACTIONS_MORE
#define SIGN_DAPPSERVICE_ACTIONS \
  SIGN_DAPPSERVICE_BASE_ACTIONS \
  SIGN_DAPPSERVICE_ACTIONS_MORE() 


#else
#define SIGN_DAPPSERVICE_ACTIONS \
  SIGN_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef SIGN_SVC_COMMANDS
#define SIGN_SVC_COMMANDS() (xsigntrx)(xsgcleanup)


#ifndef SIGN_DAPPSERVICE_SKIP_HELPER
struct sign_svc_helper{
    SIGN_DAPPSERVICE_ACTIONS
};
#endif

#endif