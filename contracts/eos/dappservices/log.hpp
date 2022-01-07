#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_LOG(name) \
    SVC_RESP_X(log,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_LOG SVC_CONTRACT_NAME_LOG_undefined 

#else
#define SVC_CONTRACT_NAME_LOG logservices1 

#endif

#include "../dappservices/_log_impl.hpp"



#define LOG_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(logevent, false, ((uint64_t)(time))((std::string)(level))((std::string)(filename))((std::string)(line))((std::string)(func))((std::string)(message)),     \
         ((uint64_t)(size)), \
         ((uint64_t)(size))((std::string)(reciept)),TONAME(SVC_CONTRACT_NAME_LOG) ) \
{ \
    _log_logevent(size, reciept, current_provider); \
    SEND_SVC_SIGNAL(logevent, current_provider, package, size)                         \
}; \
SVC_ACTION(logclear, false, ((std::string)(level)),     \
         ((uint64_t)(size)), \
         ((uint64_t)(size))((std::string)(reciept)),TONAME(SVC_CONTRACT_NAME_LOG) ) \
{ \
    _log_logclear(size, reciept, current_provider); \
    SEND_SVC_SIGNAL(logclear, current_provider, package, size)                         \
};  \
static void svc_log_logevent(uint64_t time, std::string level, std::string filename, std::string line, std::string func, std::string message) { \
    SEND_SVC_REQUEST(logevent, time, level, filename, line, func, message) \
};\
static void svc_log_logclear(std::string level) { \
    SEND_SVC_REQUEST(logclear, level) \
};


#ifdef LOG_DAPPSERVICE_ACTIONS_MORE
#define LOG_DAPPSERVICE_ACTIONS \
  LOG_DAPPSERVICE_BASE_ACTIONS \
  LOG_DAPPSERVICE_ACTIONS_MORE() 


#else
#define LOG_DAPPSERVICE_ACTIONS \
  LOG_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef LOG_SVC_COMMANDS
#define LOG_SVC_COMMANDS() (xlogevent)(xlogclear)


#ifndef LOG_DAPPSERVICE_SKIP_HELPER
struct log_svc_helper{
    LOG_DAPPSERVICE_ACTIONS
};
#endif

#endif