#include <string>
#include "sockets_func.hpp"

//
// Created by duducalil on 03/12/17.
//

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

struct cache_type{
    time_t storage_moment;
    time_t freshness_time;
    std::string host;
    std::vector<Buffer> response;
};

int verify_cache(std::vector<cache_type> cache_vector, std::string host);
cache_type save_cache(std::vector<Buffer> response_from_server, std::string host);

#endif HTTPRESPONSE_HPP
