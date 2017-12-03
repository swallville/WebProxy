//
// Created by duducalil on 03/12/17.
//
#include <vector>
#include "../include/httpResponse.hpp"
#include "../include/utils.hpp"

struct cache_type;


int verify_cache(std::vector<cache_type> cache_vector, std::string host){
    unsigned long int size = cache_vector.size();
    bool found = false;
    int i, idx = -1;

    for(i = 0; i < size; i++){
        if(cache_vector[i].host == host){
            found = true;
            idx = i;
        }
    }

    if(found){
        time_t now;
        time(&now);

        double diff = difftime(now, cache_vector[i].storage_moment);

        if(diff > cache_vector[i].expiration_time){
            return idx;
        }
    }

    return -1;
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
        std::string expires = find_header(str_response, "EXPIRES:");
        time_t expires_ttime = string_to_time(expires);
        if(!expires.empty()){
            std::string date = find_header(str_response, "DATE:");
            time_t date_ttime = string_to_time(date);

            freshnessTime = expires_ttime - date_ttime;
        }
        else{
            std::string last_modified = find_header(str_response, "LAST-MODIFIED:");
            if(!last_modified.empty()) {
                std::string date = find_header(str_response, "DATE:");
                time_t date_ttime = string_to_time(date);
                time_t lastM_tttime = string_to_time(last_modified);

                freshnessTime = date_ttime - lastM_tttime;
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
    new_cache.host = host;

    return new_cache;
}
