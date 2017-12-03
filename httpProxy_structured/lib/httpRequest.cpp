//
//  httpRequest.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/15/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file httpRequest.cpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a implementacao da classe que representa um HttpRequest
 */

#include "httpRequest.hpp"
#include <stdio.h>
#include <iostream>


HttpRequest::HttpRequest(){
    
}

void HttpRequest::setMethod(std::string method){
    this->method = method;
}

void HttpRequest::setUrl(std::string url){
    this->url = url;
}

void HttpRequest::setVersion(std::string version){
    this->version = version;
}

void HttpRequest::setHeaders(std::vector<std::string> headers){
    this->headers = headers;
}

void HttpRequest::setEntityBody(std::string entityBody){
    this->entityBody = entityBody;
}

void HttpRequest::addHeader(std::string header){
    
    if(headers.size() == 0)
        this->headers.push_back(header);
    else{
        std::string val = this->headers.at(headers.size()-1);
        this->headers.pop_back();
        this->headers.push_back(header);
        this->headers.push_back(val);
    }
        
}

std::string HttpRequest::getMethod(){
    return this->method;
}

std::string HttpRequest::getUrl(){
    return this->url;
}

std::string HttpRequest::getVersion(){
    return this->version;
}

std::vector<std::string> HttpRequest::getHeaders(){
    return this->headers;
}

std::string HttpRequest::getEntityBody(){
    return this->entityBody;
}

std::string HttpRequest::print(){
    std::string result;
    result =  this->method + " " + this->url + " " + this->version + "\n";
    
    long size = this->headers.size();
    for(int i=0; i< size; i++)
        result += this->headers.at(i) + "\n";
    
    result += "\n\n" + this->entityBody + "\n";
    return result;
}

std::string HttpRequest::getHost(){
    std::vector<std::string> split_url = split(url, "http://");
    std::cout << url << std::endl;
    std::vector<std::string> host;
    
    if (split_url.size() > 1) {
        long pos_first_slash = split_url.at(1).find_first_of("/");
        
        std::string host_port = split_url.at(1).substr(0, pos_first_slash);
        
        host = split(host_port, ":");
    } else {
        host = split(url, ":");
    }
    
    return host.at(0);
    
}

std::string HttpRequest::getPort(){
    std::vector<std::string> split_url = split(url, "http://");
    std::vector<std::string> host;
    
    if (split_url.size() > 1) {
        long pos_first_slash = split_url.at(1).find_first_of("/");
        
        std::string host_port = split_url.at(1).substr(0, pos_first_slash);
        
        host = split(host_port, ":");
    } else {
        host = split(url, ":");
        return host.at(1);
    }
    
    if(host.size() == 2)
        return host.at(1);
    
    return "80";
}


