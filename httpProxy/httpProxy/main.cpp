//
//  main.cpp
//  socket
//
//  Created by Lukas Ferreira on 12/11/2017.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

//Handle multiple socket connections with select and fd_set on Linux

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include "httpRequest.hpp"
#include "sockets_func.hpp"
#include "parser.hpp"
#include "utils.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8000
#define MAX_CONNECTIONS 100
#define PATH_WHITELIST "whitelist.txt"
#define PATH_BLACKLIST "blacklist.txt"
#define PATH_DENY_TERMS "deny_terms.txt"
#define PARSER_TOKEN "\n"
#define DIE exit(1);


std::vector<std::string> whitelist;
std::vector<std::string> blacklist;
std::vector<std::string> deny_terms;

int allowRedirection(HttpRequest request, std::string message){
    //Is the url in the whitelist?
    if(findString(whitelist, request.getUrl())){
        return 1;//true
    } else if(findString(blacklist, request.getUrl())){
        return 0;//false
    } else{
        if(findString(deny_terms, message)){
            return 2;//false
        }else{
            return 3;//true
        }
    }

    return true;
}

void redirectMessage(HttpRequest request, std::string str, int socketClient)
{
    int redirection_allowed = allowRedirection(request, str);
    bool isForbidden = false;

    if(redirection_allowed % 2 == 1 ){ //true
        //redirect the request...
        
        char * host;
        std::string url = request.getHost();
        std::string portno = request.getPort();
        
        std::cout << "=================================================================" <<std::endl;
        std::cout << "Host to be called: " << url << " - Port : " << portno <<std::endl;
        std::cout << request.getMethod() <<std::endl;
        std::cout << request.getUrl() <<std::endl;
        std::cout << request.getVersion() <<std::endl;
        std::cout << "=================================================================" <<std::endl;

        host = new char[url.size()];
        std::strcpy(host, url.c_str());

        char *port = new char[portno.size()];
        std::strcpy(port, portno.c_str());

        int socketServer = createserverSocket(host, port);
        
        if(socketServer != -1){
            struct timeval tv;
            tv.tv_sec = 5;  /* 1 Sec Timeout - Important for reading buffers from server socket */
            setsockopt(socketServer, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
            
            writeToserverSocket(str, socketServer, str.size());
            std::vector<Buffer> response_from_server = readFromServer(socketServer);
            
            if(response_from_server.size() > 0){
                if(redirection_allowed > 1){ //deny_terms rule
                    for(int i=0; i<response_from_server.size(); i++){
                        if(findString(deny_terms, response_from_server.at(i).step)){
                            isForbidden = true;
                            break;
                        }
                    }
                }
                
                if(!isForbidden){
                    // writing to client
                    writeToclientSocket(response_from_server, socketClient);
                }
                //Close the server socket
                close(socketServer);
            }else{
                close(socketServer);
                isForbidden = false;
            }
            
        }else{
            close(socketServer);
            return;
        }

    }else{
        isForbidden = true;
       
    }

    if(isForbidden){
        std::string forbidden = getForbiddenResponse();
        writeToclientSocket(forbidden, socketClient, forbidden.size());
    }

    std::ofstream output_file;
    output_file.open("log.txt", std::ios::in | std::fstream::out | std::fstream::app);

    char dt[1000];

    time_t t = time(0);
    struct tm * p = localtime(&t);

    strftime(dt, 1000, "%A, %B %d %Y, %H:%M:%S", p);

    std::string log_message;

    if(redirection_allowed == 0){
        log_message.append("Tried to access the following forbidden website." + request.getHost());
    }
    else if(redirection_allowed == 1){
        log_message.append("Access to website " + request.getHost() + " authorized.");
    }
    else if(redirection_allowed == 2){
        log_message.append("Access to website " + request.getHost() + " denied because of deny terms.");
    }
    else{
        log_message.append("Access to website " + request.getHost() + " authorized.");
    }

    output_file << dt << " - " << log_message << "\n";

    output_file.close();

    //Close the Client socket
    close(socketClient);

}

static void* beginExecution(void* sockfdPtr)
{
    int sockid = (intptr_t)sockfdPtr;
    std::string str = readFromSocket(&sockid);
    
    if(str.length() == 0){
        return (void*)NULL;
    }
    
    HttpRequest request = parserRequest(str);
    
    redirectMessage(request, str, sockid);
    /*final request to be sent*/

    std::cout << "END OF ROUTINE ..." <<std::endl << std::endl;
    return (void*)NULL;
}

int main(int argc , char *argv[])
{
    //Read filter files
    whitelist = readFile(PATH_WHITELIST, PARSER_TOKEN);
    blacklist = readFile(PATH_BLACKLIST, PARSER_TOKEN);
    deny_terms = readFile(PATH_DENY_TERMS, PARSER_TOKEN);
    
    struct sockaddr_in address;

    //Creating the main socket
    int main_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    
    if (main_socket_id < 0)
    {
        std::cout << "Error on creating the main server socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Allow main socket to accept multiple connections
    int allow_multiple_connections = TRUE;
    
    if (setsockopt(main_socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&allow_multiple_connections, sizeof(allow_multiple_connections)) < 0)
    {
        std::cout << "Error on allowing multiple connections in the socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Setting the characteristics of a socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    //Bind the socket to the defined port
    if (bind(main_socket_id, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cout << "Error on binding the main socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    signal(SIGPIPE, SIG_IGN);
    std::cout << "SERVER IS LISTENING ON PORT " << PORT << std::endl;
    
    //Set maximim requests on the server queue
    if (listen(main_socket_id, MAX_CONNECTIONS) < 0)
    {
        std::cout << "Error while specifying trying to listen the socket" << std:: endl;
        exit(EXIT_FAILURE);
    }
    
    
    std::cout << "SERVER IS WAITING FOR CONNECTIONS..." << std::endl;
    
    while (TRUE)
    {
       
        //signal(SIGPIPE, SIG_IGN);
        
        //accept the incoming connection
        socklen_t addrlen = sizeof(address);
            
        int new_socket = accept(main_socket_id, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            
        if (new_socket < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
            
        //inform user of socket number - used in send and receive commands
        printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        pthread_t thread;

        if(pthread_create(&thread, NULL, &beginExecution, (void*)(intptr_t)new_socket) < 0) {
            std::cout << "Error creating thread.. exiting" << std::endl;
            DIE
        }

        pthread_detach(thread);
        thread = NULL;
    }
    
    return 0;
}
