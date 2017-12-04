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

std::string getNotFoundResponse(){
    std::string response = "";
    response.append("HTTP/1.1 404 Not Found\n");
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
    
    std::string content = getNotFound();
    response.append(std::to_string(content.size()));
    response.append("\n");
    response.append("Content-Type: text/html\n");
    response.append("\n\r\n\r");
    response.append(content);
    
    return response;
}

std::string getTimeoutResponse(){
    std::string response = "";
    response.append("HTTP/1.1 408 Request Timeout\n");
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
    
    std::string content = getTimeout();
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

std::string getLastModifiedDate(std::vector<Buffer> buffer){
    
    int position = -1;
    
    for(int i=0; i< buffer.size(); i++){
        std::string str = buffer.at(i).step;
        
        if(str.find("Last-Modified:") != -1){
            position = i;
            break;
        }
    }
    
    if(position != -1){
        std::string header = buffer.at(position).step;
        
        std::vector<std::string> linhas = split(header, "\n");
        
        for(int i=0; i<linhas.size(); i++){
            if(linhas.at(i).find("Last-Modified:") != -1){
                std::string data = linhas.at(i).substr(15, linhas.at(i).size());
                
                return data;
            }
        }
    }
    
    return "";
    
}

std::string getETag(std::vector<Buffer> buffer){
    
    int position = -1;
    
    for(int i=0; i< buffer.size(); i++){
        std::string str = buffer.at(i).step;
        
        if(str.find("ETag:") != -1){
            position = i;
            break;
        }
    }
    
    if(position != -1){
        std::string header = buffer.at(position).step;
        
        std::vector<std::string> linhas = split(header, "\n");
        
        for(int i=0; i<linhas.size(); i++){
            if(linhas.at(i).find("ETag:") != -1){
                std::string data = linhas.at(i).substr(5, linhas.at(i).size());
                
                return data;
            }
        }
    }
    
    return "";
    
}

std::string getExpires(std::vector<Buffer> buffer){
    
    int position = -1;
    
    for(int i=0; i< buffer.size(); i++){
        std::string str = buffer.at(i).step;
        
        if(str.find("Expires:") != -1){
            position = i;
            break;
        }
    }
    
    if(position != -1){
        std::string header = buffer.at(position).step;
        
        std::vector<std::string> linhas = split(header, "\n");
        
        for(int i=0; i<linhas.size(); i++){
            if(linhas.at(i).find("Expires:") != -1){
                std::string data = linhas.at(i).substr(8, linhas.at(i).size());
                
                return data;
            }
        }
    }
    
    return "";
    
}

std::string getDate(std::vector<Buffer> buffer){
    
    int position = -1;
    
    for(int i=0; i< buffer.size(); i++){
        std::string str = buffer.at(i).step;
        
        if(str.find("Date:") != -1){
            position = i;
            break;
        }
    }
    
    if(position != -1){
        std::string header = buffer.at(position).step;
        
        std::vector<std::string> linhas = split(header, "\n");
        
        for(int i=0; i<linhas.size(); i++){
            if(linhas.at(i).find("Date:") != -1){
                std::string data = linhas.at(i).substr(5, linhas.at(i).size());
                
                return data;
            }
        }
    }
    
    return "";
    
}

std::string getMaxAge(std::vector<Buffer> buffer){
    
    int position = -1;
    
    for(int i=0; i< buffer.size(); i++){
        std::string str = buffer.at(i).step;
        
        if(str.find("max-age=") != -1){
            position = i;
            break;
        }
    }
    
    if(position != -1){
        std::string header = buffer.at(position).step;
        
        std::vector<std::string> linhas = split(header, "\n");
        
        for(int i=0; i<linhas.size(); i++){
            int pos = linhas.at(i).find("max-age=");
            if(pos != -1){
                std::string data = linhas.at(i).substr(pos + 8, linhas.at(i).size());
                
                int a = data.find_first_of(" ");
                if(a!=-1){
                    std::string retorno = data.substr(0, a);
                    return retorno;
                }
            }
        }
    }
    
    return "";
    
}
