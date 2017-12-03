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

#include <netdb.h>
#include <arpa/inet.h>
#include "../include/utils.hpp"
#include "../include/response_content.hpp"

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

int hostname_to_ip(std::string hostname , std::string &ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    
    if ( (he = gethostbyname( hostname.c_str() ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
    
    addr_list = (struct in_addr **) he->h_addr_list;
    
    if (addr_list[0] != NULL) {
        ip.append(inet_ntoa(*addr_list[0]));
        return 0;
    }
    
    return 1;
}

void remove_tags(std::string& str){
    std::size_t found;
    found = str.find('\n');
    if(found == std::string::npos)
        found = str.find('\r');
    while ( found != std::string::npos) {
        str.replace(found, 1, " ");
        found = str.find('\n');
        if(found == std::string::npos)
            found = str.find('\r');
    }
}

void to_upper_case_string(std::string &str){
    for (int i=0; i < str.length(); ++i)
        str[i] = std::toupper(str[i]);
}

std::string find_header(std::string response, std::string header){
    std::string args;
    std::size_t found = response.find(header);

    if (found != std::string::npos) {
        found = response.find(':', found);
        if (found != std::string::npos) {
            std::size_t args_end = response.find("\r\n", found);
            found++;
            int size = args_end - found;
            return response.substr(found , size);
        }
    }
    return "";
}


std::string get_arg_value_header(std::string arg, std::string value){
    std::size_t arg_init = arg.find(value);

    if (arg_init != std::string::npos) {
        std::size_t arg_end = arg.find(',', arg_init);
        if (arg_end == std::string::npos) {
            arg_end = arg.size();
        }
        value = arg.substr(arg_init, arg_end - 1);

        std::vector<std::string> splited_arg = split(value, "=");

        return splited_arg[1];
    }
    return "";
}

int month(std::string month){
    if(month == "JAN"){
        return 0;
    }
    if(month == "FEB"){
        return 1;
    }
    if(month == "MAR"){
        return 2;
    }
    if(month == "APR"){
        return 3;
    }
    if(month == "MAY"){
        return 4;
    }
    if(month == "JUN"){
        return 5;
    }
    if(month == "JUL"){
        return 6;
    }
    if(month == "AUG"){
        return 7;
    }
    if(month == "SEP"){
        return 8;
    }
    if(month == "OCT"){
        return 9;
    }
    if(month == "NOV"){
        return 10;
    }
    if(month == "DEC"){
        return 11;
    }
}

time_t string_to_time(std::string time_str){
    std::vector<std::string> time_array = split(time_str, " ");
    time_t timer;
    struct tm timeinfo = {0};

    timeinfo.tm_mday = atoi(time_array[2].c_str());
    timeinfo.tm_mon = month(time_array[3].c_str());

    int ano = atoi(time_array[4].c_str());
    ano = ano - 1900;
    timeinfo.tm_year = ano;

    std::vector<std::string> time_splited = split(time_array[5], ":");

    timeinfo.tm_sec = atoi(time_splited[2].c_str());
    timeinfo.tm_min = atoi(time_splited[1].c_str());
    timeinfo.tm_hour = atoi(time_splited[0].c_str());

    return mktime(&timeinfo);
}

