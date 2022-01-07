#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_AUTH(name) \
    SVC_RESP_X(auth,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_AUTH SVC_CONTRACT_NAME_AUTH_undefined 

#else
#define SVC_CONTRACT_NAME_AUTH authfndspsvc 

#endif

#include "../dappservices/_auth_impl.hpp"



#define AUTH_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(authusage, false, ((name)(account))((name)(permission))((std::string)(client_code))((checksum256)(payload_hash))((std::vector<char>)(signature)),     \
         ((name)(account))((name)(permission))((std::string)(client_code))((checksum256)(payload_hash))((std::vector<char>)(signature)), \
         ((name)(account))((name)(permission))((std::string)(client_code))((checksum256)(payload_hash))((std::vector<char>)(signature)),TONAME(SVC_CONTRACT_NAME_AUTH) ) \
{ \
    _auth_authusage(account, permission, client_code, payload_hash, signature, current_provider); \
    SEND_SVC_SIGNAL(authusage, current_provider, package, account, permission, client_code, payload_hash, signature)                         \
};  \
static void svc_auth_authusage(name account, name permission, std::string client_code, checksum256 payload_hash, std::vector<char> signature) { \
    SEND_SVC_REQUEST(authusage, account, permission, client_code, payload_hash, signature) \
};


#ifdef AUTH_DAPPSERVICE_ACTIONS_MORE
#define AUTH_DAPPSERVICE_ACTIONS \
  AUTH_DAPPSERVICE_BASE_ACTIONS \
  AUTH_DAPPSERVICE_ACTIONS_MORE() 


#else
#define AUTH_DAPPSERVICE_ACTIONS \
  AUTH_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef AUTH_SVC_COMMANDS
#define AUTH_SVC_COMMANDS() (xauthusage)


#ifndef AUTH_DAPPSERVICE_SKIP_HELPER
struct auth_svc_helper{
    AUTH_DAPPSERVICE_ACTIONS
};
#endif

#endif