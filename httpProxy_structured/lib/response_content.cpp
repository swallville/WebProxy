//
//  response_content.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/22/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file response_content.cpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a implementacao dos metodos que criam mensagens http de response especificas de erros e afins
 */

#include "response_content.hpp"

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
    response.append("<p>Either the requested url is not allowed by the Administrator or the page contains disallowed terms. <br />If you believe this is not right, please contact the Administration.&nbsp;</p>");
    response.append("</body>");
    response.append("</html>");

    return response;
}

std::string getNotFound(){
    std::string response = "";
    response.append("<!DOCTYPE html>");
    response.append("<html>");
    response.append("<head>");
    response.append("<meta charset=\"utf-8\">");
    response.append("<title>Addr Not Found</title>");
    response.append("</head>");
    response.append("<body>");
    response.append("<h1 style=\"color: #5e9ca0;\"> <span style=\"color: #2b2301;\">Website not allowed!</span> </h1>");
    response.append("<h2 style=\"color: #2e6c80;\">Error Code: 404 Not Found</h2>");
    response.append("<p>The requested url was not found on this server. <br />404 File or Directory Not Found.&nbsp;</p>");
    response.append("</body>");
    response.append("</html>");
    
    return response;
}

std::string getTimeout(){
    std::string response = "";
    response.append("<!DOCTYPE html>");
    response.append("<html>");
    response.append("<head>");
    response.append("<meta charset=\"utf-8\">");
    response.append("<title>Request Timeout</title>");
    response.append("</head>");
    response.append("<body>");
    response.append("<h1 style=\"color: #5e9ca0;\"> <span style=\"color: #2b2301;\">Website not allowed!</span> </h1>");
    response.append("<h2 style=\"color: #2e6c80;\">Error Code: 408 Request Timeout</h2>");
    response.append("<p>The requested url took to long to answer. <br />408 Request Timeout.&nbsp;</p>");
    response.append("</body>");
    response.append("</html>");
    
    return response;
}

