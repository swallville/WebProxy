//
// Created by duducalil on 03/12/17.
//
#include <vector>
#include <unordered_map>
#include <map>
#include "../include/httpResponse.hpp"
#include "../include/utils.hpp"

struct cache_type;


bool verify_cache(cache_type cache, std::map<std::string, cache_type> &cache_map, std::string url){
    time_t now;
    time(&now);

    double diff = difftime(now, cache.storage_moment);

    bool valid = cache.expiration_time > diff;

    if(!valid){
        cache_map.erase(url);
    }
    return valid;
}

cache_type save_cache(std::vector<Buffer> response_from_server, std::string host){
    std::string str_response;

    int size_i = response_from_server.size();
    int size_j;
    for(int i=0; i < size_i; i++){
        size_j = response_from_server[i].buff_len;
        for(int j=0; j < size_j; j++) {
            str_response.push_back(response_from_server[i].step[j]);
        }
    }

    to_upper_case_string(str_response);

    std::string cache_control = find_header(str_response, "CACHE-CONTROL:");

    std::string max_age = get_arg_value_header(cache_control, "MAX-AGE=");

    double freshnessTime;
    double expirationTime = 0;

    if(max_age.empty()){
        std::string date = find_header(str_response, "DATE:");
        time_t date_ttime = string_to_time(date);

        if(!date.empty()) {
            std::string expires = find_header(str_response, "EXPIRES:");
            if (!expires.empty()) {
                time_t expires_ttime = string_to_time(expires);
                freshnessTime = expires_ttime - date_ttime;
            } else {
                std::string last_modified = find_header(str_response, "LAST-MODIFIED:");
                if (!last_modified.empty()) {
                    time_t lastM_tttime = string_to_time(last_modified);
                    freshnessTime = date_ttime - lastM_tttime;
                }

            }
        }
    }
    else
        freshnessTime = atoi(max_age.c_str());


    std::string expires = find_header(str_response, "AGE:");
    int current_age = atoi(expires.c_str());

    expirationTime = freshnessTime - current_age;

    time_t now;
    time(&now);

    cache_type new_cache;
    new_cache.response = response_from_server;
    new_cache.expiration_time = expirationTime;
    new_cache.storage_moment = now;

    return new_cache;
}
