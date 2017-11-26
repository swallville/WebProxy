//
//  main.cpp
//  socket
//
//  Created by Lukas Ferreira on 12/11/2017.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

//Handle multiple socket connections with select and fd_set on Linux

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
#define PATH_WHITELIST "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/whitelist.txt"
#define PATH_BLACKLIST "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/blacklist.txt"
#define PATH_DENY_TERMS "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/deny_terms.txt"
#define PARSER_TOKEN "\n"


std::vector<std::string> whitelist;
std::vector<std::string> blacklist;
std::vector<std::string> deny_terms;

struct Client
{
    Client() : socket(-1)
    {
    }

    Client(int s) : socket(s)
    {
    }

    int socket;
};


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
        std::string portno = "80";
        char * host;

        std::string requestedUrl = request.getUrl();
        std::vector<std::string> address = split(requestedUrl, ":");

        if(address.size() == 3)
            portno = address.at(2);

        std::vector<std::string> add = split(address.at(1), "//");

        std::string url = "";
        if(add.size() > 1)
            url =add.at(1);
        else
            url = add.at(0);

        std::vector<std::string> host_split = split(url, "/");
        url = host_split.at(0);

        std::cout << "=================================================================" <<std::endl;
        std::cout << "Host to be called: " << url <<std::endl;
        std::cout << request.getMethod() <<std::endl;
        std::cout << request.getUrl() <<std::endl;
        std::cout << request.getVersion() <<std::endl;
        std::cout << "=================================================================" <<std::endl;

        host = new char[url.size() + 1];
        std::strcpy(host, url.c_str());

        char *port = new char[portno.size()+1];
        std::strcpy(port, portno.c_str());

        int socketServer = createserverSocket(host, port);
        
        if(socketServer != -1){
            struct timeval tv;
            tv.tv_sec = 1;  /* 1 Sec Timeout - Important for reading buffers from server socket */
            setsockopt(socketServer, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
            
            writeToserverSocket(str, socketServer, str.size());
            std::vector<Buffer> response_from_server = readFromServer(socketServer);
            
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
            return;
        }

    }else{
        isForbidden = true;
       
    }

    if(isForbidden){
        std::string forbidden = getForbiddenResponse();
        writeToclientSocket(forbidden, socketClient, forbidden.size());
    }

    //Close the Client socket
    close(socketClient);

}

void beginExecution(int* sockid)
{
    std::string str = readFromSocket(sockid);
    
    if(str.length() == 0){
        return;
    }
    
    HttpRequest request = parserRequest(str);
    
    redirectMessage(request, str, *sockid);
    /*final request to be sent*/

}

int main(int argc , char *argv[])
{
    //Read filter files
    whitelist = readFile(PATH_WHITELIST, PARSER_TOKEN);
    blacklist = readFile(PATH_BLACKLIST, PARSER_TOKEN);
    deny_terms = readFile(PATH_DENY_TERMS, PARSER_TOKEN);
    
    struct sockaddr_in address;
    std::vector<Client> client_socket;
    
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
    
    //signal(SIGPIPE, SIG_IGN);
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
        //set of socket descriptors
        fd_set readfds;
        // Clear a fd_set
        FD_ZERO(&readfds);                  //clear the socket set
        // Add a descriptor to an fd_set
        FD_SET(main_socket_id, &readfds);    //add master socket to set
        
        int max_sd = main_socket_id;
        
        //add child sockets to set
        for (size_t i = 0; i < client_socket.size(); ++i)
        {
            // Add a descriptor to an fd_set
            FD_SET(client_socket[i].socket, &readfds);
            max_sd = std::max(max_sd, client_socket[i].socket);
        }
        
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        
        if ((activity < 0) && (errno != EINTR))
        {
            printf("select error");
            //close(max_sd+1);
        }
        signal(SIGPIPE, SIG_IGN);
        
        //else its some IO operation on some other socket :)
        for (std::vector<Client>::iterator p = client_socket.begin(); p != client_socket.end(); )
        {
            char buffer[1024]; //data buffer of 1K
            
            //If something happened on the master socket , then its an incoming connection
            if (FD_ISSET(p->socket, &readfds))
            {
                //Check if it was for closing , and also read the incoming message
                long valread = recv(p->socket, buffer, sizeof(buffer), 0);
                
                if (valread <= 0)
                {
                    //Somebody disconnected , get his details and print
                    socklen_t addrlen = sizeof(address);
                    getpeername(p->socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    
                    //Close the socket and mark as 0 in list for reuse
                    close(p->socket);
                    p = client_socket.erase(p);
                }
                //Echo back the message that came in
                else
                {
                    ++p;
                }
            }
            else
            {
                ++p;
            }
        }
        
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(main_socket_id, &readfds))
        {
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
            
            beginExecution(&new_socket);
            
            //add new socket to array of sockets
            Client client(new_socket);
            client_socket.push_back(client);
        }
    }
    
    return 0;
}
