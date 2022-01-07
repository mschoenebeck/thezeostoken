#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_VCPU(name) \
    SVC_RESP_X(vcpu,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_VCPU SVC_CONTRACT_NAME_VCPU_undefined 

#else
#define SVC_CONTRACT_NAME_VCPU vcpuservices 

#endif

#include "../dappservices/_vcpu_impl.hpp"



#define VCPU_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(vrun, true, ((std::vector<char>)(uri))((std::vector<char>)(payload)),     \
         ((uint32_t)(size))((std::vector<char>)(uri)), \
         ((uint32_t)(size))((std::vector<char>)(uri))((std::vector<char>)(data)),TONAME(SVC_CONTRACT_NAME_VCPU) ) \
{ \
    _vcpu_vrun(size, uri, data, current_provider); \
    SEND_SVC_SIGNAL(vrun, current_provider, package, size, uri)                         \
}; \
SVC_ACTION(vrunclean, false, ((std::vector<char>)(uri)),     \
         ((uint32_t)(size))((std::vector<char>)(uri)), \
         ((uint32_t)(size))((std::vector<char>)(uri)),TONAME(SVC_CONTRACT_NAME_VCPU) ) \
{ \
    _vcpu_vrunclean(size, uri, current_provider); \
    SEND_SVC_SIGNAL(vrunclean, current_provider, package, size, uri)                         \
};  \
static void svc_vcpu_vrun(std::vector<char> uri, std::vector<char> payload) { \
    SEND_SVC_REQUEST(vrun, uri, payload) \
};\
static void svc_vcpu_vrunclean(std::vector<char> uri) { \
    SEND_SVC_REQUEST(vrunclean, uri) \
};


#ifdef VCPU_DAPPSERVICE_ACTIONS_MORE
#define VCPU_DAPPSERVICE_ACTIONS \
  VCPU_DAPPSERVICE_BASE_ACTIONS \
  VCPU_DAPPSERVICE_ACTIONS_MORE() 


#else
#define VCPU_DAPPSERVICE_ACTIONS \
  VCPU_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef VCPU_SVC_COMMANDS
#define VCPU_SVC_COMMANDS() (xvrun)(xvrunclean)


#ifndef VCPU_DAPPSERVICE_SKIP_HELPER
struct vcpu_svc_helper{
    VCPU_DAPPSERVICE_ACTIONS
};
#endif

#endif