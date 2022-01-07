#pragma once
#include <eosio/eosio.hpp>

#if __has_include("dns/headers.hpp")
#include "dns/headers.hpp"
#endif

SVC_RESP_DNS(dnsq)(std::vector<char> payload, name current_provider){
#if __has_include("dns/cmds/dnsq.hpp")
#include "dns/cmds/dnsq.hpp"
#endif
}