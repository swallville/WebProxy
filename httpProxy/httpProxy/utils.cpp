//
//  utils.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file utils.cpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com implementacao de protótipos de funcoes gerais que podem ser utilizadas em qualquer tempo do codigo.
 */

#include "utils.hpp"
#include "response_content.hpp"

std::vector<std::string> split(std::string str, std::string token){
    std::vector<std::string>result;
    while(str.size()){
        long index = str.find(token);
        if(index!=(long)std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

std::vector<std::string> readFile(std::string file, std::string token){

    std::ifstream ifs;
    //Defines the type of exception that could be throw ffrom the read process
    ifs.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        ifs.open(file.c_str());
        std::stringstream ss;
        ss << ifs.rdbuf();
        std::string str = ss.str();

        return split(str, token);
    }
    catch (std::ifstream::failure e) {
        std::cout << "The file " << file << " could not be read. Please verify." << std::endl;
        std::vector<std::string> res;
        return res;
    }
}

bool findString(std::vector<std::string> list, std::string str){

    for(int i=0; i<(int)list.size(); i++){
        if(list.at(i).size() > 0){
            std::size_t found = str.find(list.at(i));
            //string::npos means the token is not present in the string
            if (found!=std::string::npos){
                return true;
            }
        }
    }
    return false;
}

std::string formatedTime(const struct tm *timeptr)
{
    static const char wday_name[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    std::string result = "";
    result.append(wday_name[timeptr->tm_wday]);
    result.append(", ");
    result.append(std::to_string(timeptr->tm_mday));
    result.append(" ");
    result.append(mon_name[timeptr->tm_mon]);
    result.append(" ");
    result.append(std::to_string(1900 + timeptr->tm_year));
    result.append(" ");
    result.append(std::to_string(timeptr->tm_hour));
    result.append(":");
    result.append(std::to_string(timeptr->tm_min));
    result.append(":");
    result.append(std::to_string(timeptr->tm_sec));
    result.append(" GMT");
    return result;
}

std::string getForbiddenResponse(){
    std::string response = "";
    response.append("HTTP/1.1 403 Forbidden\n");
    response.append("Connection: close\n");
    response.append("Date: ");

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    response.append(formatedTime(timeinfo));
    response.append("\n");
    response.append("Content-Length: ");
    response.append("\n");

    std::string content = getForbidden();
    response.append(std::to_string(content.size()));
    response.append("\n");
    response.append("Content-Type: text/html\n");
    response.append("\n\r\n\r");
    response.append(content);

    return response;
}
