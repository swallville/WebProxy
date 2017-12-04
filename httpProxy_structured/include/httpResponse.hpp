#include <string>
#include "sockets_func.hpp"

//
// Created by duducalil on 03/12/17.
//

#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

struct cache_type{
    time_t storage_moment;
    double expiration_time;
    std::vector<Buffer> response;
};

bool verify_cache(cache_type cache, std::map<std::string, cache_type> &cache_map, std::string url);
cache_type save_cache(std::vector<Buffer> response_from_server, std::string host);

#endif HTTPRESPONSE_HPP
