//
//  response_content.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/22/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

#include "../include/response_content.hpp"

std::string getForbidden(){
    std::string response = "";
    response.append("<!DOCTYPE html>");
    response.append("<html>");
    response.append("<head>");
    response.append("<meta charset=\"utf-8\">");
    response.append("<title>Addr Not Allowed</title>");
    response.append("</head>");
    response.append("<body>");
    response.append("<h1 style=\"color: #5e9ca0;\"> <span style=\"color: #2b2301;\">Website not allowed!</span> </h1>");
    response.append("<h2 style=\"color: #2e6c80;\">Error Code: 403 Forbidden</h2>");
    response.append("<p>Either the requested url is not allowed by the Administrator or the page contains disallowed terms. <br />If you believe this is not rigth, please contact the Administration.&nbsp;</p>");
    response.append("</body>");
    response.append("</html>");

    return response;
}

