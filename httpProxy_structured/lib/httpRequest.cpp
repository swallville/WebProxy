//
//  httpRequest.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/15/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

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
    this->headers.push_back(header);
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

void HttpRequest::print(){
    std::cout << "------- Request Message: -------" << std::endl;
    std::cout << this->method << " " << this->url << " " << this->version << std::endl;
    
    long size = this->headers.size();
    for(int i=0; i< size; i++)
        std::cout << this->headers.at(i) << std::endl;
    
    std::cout << this->entityBody << std::endl;
    std::cout << "------- End of Request Message: -------" << std::endl;
}
