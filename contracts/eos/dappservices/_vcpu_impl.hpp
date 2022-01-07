#pragma once
#include <eosio/eosio.hpp>

#if __has_include("vcpu/headers.hpp")
#include "vcpu/headers.hpp"
#endif

SVC_RESP_VCPU(vrun)(uint32_t size, std::vector<char> uri, std::vector<char> data, name current_provider){
#if __has_include("vcpu/cmds/vrun.hpp")
#include "vcpu/cmds/vrun.hpp"
#endif
}
SVC_RESP_VCPU(vrunclean)(uint32_t size, std::vector<char> uri, name current_provider){
#if __has_include("vcpu/cmds/vrunclean.hpp")
#include "vcpu/cmds/vrunclean.hpp"
#endif
}