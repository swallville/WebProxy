//
//  httpRequest.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/15/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

#ifndef httpRequest_hpp
#define httpRequest_hpp

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>
#include "utils.hpp"

class HttpRequest{

private:
    std::string method;
    std::string url;
    std::string version;
    std::vector<std::string> headers;
    std::string entityBody;

public:

    HttpRequest();
    void setMethod(std::string method);
    void setUrl(std::string url);
    void setVersion(std::string version);
    void setHeaders(std::vector<std::string> headers);
    void setEntityBody(std::string entityBody);
    void addHeader(std::string header);

    std::string getMethod();
    std::string getUrl();
    std::string getVersion();
    std::vector<std::string> getHeaders();
    std::string getEntityBody();
    std::string getHost();
    std::string getPort();

    void print();

   // ~HttpRequest();
};
#endif /* httpRequest_hpp */
