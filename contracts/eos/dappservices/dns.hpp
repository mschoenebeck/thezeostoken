#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_DNS(name) \
    SVC_RESP_X(dns,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_DNS SVC_CONTRACT_NAME_DNS_undefined 

#else
#define SVC_CONTRACT_NAME_DNS dnsservices1 

#endif

#include "../dappservices/_dns_impl.hpp"



#define DNS_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(dnsq, true, ((std::vector<char>)(payload)),     \
         ((std::vector<char>)(payload)), \
         ((std::vector<char>)(payload)),TONAME(SVC_CONTRACT_NAME_DNS) ) \
{ \
    _dns_dnsq(payload, current_provider); \
    SEND_SVC_SIGNAL(dnsq, current_provider, package, payload)                         \
};  \
static void svc_dns_dnsq(std::vector<char> payload) { \
    SEND_SVC_REQUEST(dnsq, payload) \
};


#ifdef DNS_DAPPSERVICE_ACTIONS_MORE
#define DNS_DAPPSERVICE_ACTIONS \
  DNS_DAPPSERVICE_BASE_ACTIONS \
  DNS_DAPPSERVICE_ACTIONS_MORE() 


#else
#define DNS_DAPPSERVICE_ACTIONS \
  DNS_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef DNS_SVC_COMMANDS
#define DNS_SVC_COMMANDS() (xdnsq)


#ifndef DNS_DAPPSERVICE_SKIP_HELPER
struct dns_svc_helper{
    DNS_DAPPSERVICE_ACTIONS
};
#endif

#endif