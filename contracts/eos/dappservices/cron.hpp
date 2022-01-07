#pragma once
#include "../dappservices/dappservices.hpp"

#define SVC_RESP_CRON(name) \
    SVC_RESP_X(cron,name)



#ifdef LIQUIDX

#define SVC_CONTRACT_NAME_CRON SVC_CONTRACT_NAME_CRON_undefined 

#else
#define SVC_CONTRACT_NAME_CRON cronservices 

#endif

#include "../dappservices/_cron_impl.hpp"



#define CRON_DAPPSERVICE_BASE_ACTIONS \
SVC_ACTION(schedule, false, ((name)(timer))((std::vector<char>)(payload))((uint32_t)(seconds)),     \
         ((name)(timer))((uint32_t)(seconds)), \
         ((name)(timer))((std::vector<char>)(payload))((uint32_t)(seconds)),TONAME(SVC_CONTRACT_NAME_CRON) ) \
{ \
    _cron_schedule(timer, payload, seconds, current_provider); \
    SEND_SVC_SIGNAL(schedule, current_provider, package, timer, seconds)                         \
}; \
SVC_ACTION(interval, false, ((name)(timer))((std::vector<char>)(payload))((uint32_t)(seconds)),     \
         ((name)(timer))((uint32_t)(seconds)), \
         ((name)(timer))((std::vector<char>)(payload))((uint32_t)(seconds)),TONAME(SVC_CONTRACT_NAME_CRON) ) \
{ \
    _cron_interval(timer, payload, seconds, current_provider); \
    SEND_SVC_SIGNAL(interval, current_provider, package, timer, seconds)                         \
}; \
SVC_ACTION(rminterval, false, ((name)(timer)),     \
         ((name)(timer)), \
         ((name)(timer)),TONAME(SVC_CONTRACT_NAME_CRON) ) \
{ \
    _cron_rminterval(timer, current_provider); \
    SEND_SVC_SIGNAL(rminterval, current_provider, package, timer)                         \
};  \
static void svc_cron_schedule(name timer, std::vector<char> payload, uint32_t seconds) { \
    SEND_SVC_REQUEST(schedule, timer, payload, seconds) \
};\
static void svc_cron_interval(name timer, std::vector<char> payload, uint32_t seconds) { \
    SEND_SVC_REQUEST(interval, timer, payload, seconds) \
};\
static void svc_cron_rminterval(name timer) { \
    SEND_SVC_REQUEST(rminterval, timer) \
};


#ifdef CRON_DAPPSERVICE_ACTIONS_MORE
#define CRON_DAPPSERVICE_ACTIONS \
  CRON_DAPPSERVICE_BASE_ACTIONS \
  CRON_DAPPSERVICE_ACTIONS_MORE() 


#else
#define CRON_DAPPSERVICE_ACTIONS \
  CRON_DAPPSERVICE_BASE_ACTIONS
#endif


#ifndef CRON_SVC_COMMANDS
#define CRON_SVC_COMMANDS() (xschedule)(xinterval)(xrminterval)


#ifndef CRON_DAPPSERVICE_SKIP_HELPER
struct cron_svc_helper{
    CRON_DAPPSERVICE_ACTIONS
};
#endif

#endif