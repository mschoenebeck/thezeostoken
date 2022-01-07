#pragma once
#include <eosio/eosio.hpp>

#if __has_include("storage/headers.hpp")
#include "storage/headers.hpp"
#endif

SVC_RESP_STORAGE(sdummy)(uint32_t size, std::string uri, name current_provider){
#if __has_include("storage/cmds/sdummy.hpp")
#include "storage/cmds/sdummy.hpp"
#endif
}