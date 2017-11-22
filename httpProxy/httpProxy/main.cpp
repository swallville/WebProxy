//
//  main.cpp
//  socket
//
//  Created by Lukas Ferreira on 12/11/2017.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

//Handle multiple socket connections with select and fd_set on Linux

#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <sstream>

#include "httpRequest.hpp"
#include "Utils.hpp"
#include "sockets_func.hpp"
#include "parser.hpp"


#define TRUE 1
#define FALSE 0
#define PORT 8000
#define MAX_CONNECTIONS 100


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


void redirect(std::string str, int socketClient){
    HttpRequest request = parserRequest(str);
    
    //redirect the request...
    std::string portno = "80";
    char * host;
    
    std::string requestedUrl = request.getUrl();
    std::vector<std::string> address = split(requestedUrl, ":");
    
    if(address.size() == 3)
        portno = address.at(2);
    
    std::vector<std::string> add = split(address.at(1), "//");
    
    std::string url =add.at(1);
    std::vector<std::string> host_split = split(url, "/");
    url = host_split.at(0);
    
    std::cout << "=================================================================" <<std::endl;
    std::cout << "Host to be called: " << url <<std::endl;
    std::cout << request.getMethod() <<std::endl;
    std::cout << request.getUrl() <<std::endl;
    std::cout << request.getVersion() <<std::endl;
    std::cout << "=================================================================" <<std::endl;
    
    host = new char[url.length() + 1];
    std::strcpy(host, url.c_str());
    
    char *port = new char[portno.size()+1];
    std::strcpy(port, portno.c_str());
    
    int socketServer = createserverSocket(host, port);
    
    writeToserverSocket(str, socketServer, str.length());
    writeToClient(socketClient, socketServer);
    
    //Close the sockets
    close(socketClient);
    close(socketServer);
}


int main(int argc , char *argv[])
{
    struct sockaddr_in address;
    std::vector<Client> client_socket;
    
    //Creating the main socket
    int main_socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
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
        //Creating a set of socket descriptors
        fd_set socket_descriptors;
        //Clear the socket descriptor
        FD_ZERO(&socket_descriptors);
        //Add a descriptor to the set of socket descriptors
        FD_SET(main_socket_id, &socket_descriptors);
        
        int max_socket = main_socket_id;
        
        //add child sockets to set
        for (size_t i = 0; i < client_socket.size(); ++i)
        {
            //Adding a descriptor to the set
            FD_SET(client_socket[i].socket, &socket_descriptors);
            max_socket = std::max(max_socket, client_socket[i].socket);
        }
        
        //Creating a type of thread: a select. Wait indefinitely until someone requests something
        int activity = select( max_socket + 1 , &socket_descriptors , NULL , NULL , NULL);
        
        if ((activity < 0) && (errno != EINTR)){
            std::cout << "Error on creating a select process" << std::endl;
        }
      
        for (std::vector<Client>::iterator p = client_socket.begin(); p != client_socket.end(); ){
            char buffer[1024];
            
            //Incoming connection
            if (FD_ISSET(p->socket, &socket_descriptors))
            {
                //Read incoming message
                long valread = recv(p->socket, buffer, sizeof(buffer), 0);
                
                if (valread <= 0){
                    //It is a Disconnected
                    socklen_t addrlen = sizeof(address);
                    getpeername(p->socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    
                    std::cout << "A host disconneted - IP: " << inet_ntoa(address.sin_addr) << "; PORT: " << ntohs(address.sin_port) << std::endl;
                   
                    //Close the socket and allow reuse of it
                    close(p->socket);
                    p = client_socket.erase(p);
                } else {
                    ++p;
                }
            }
            else
            {
                ++p;
            }
        }
        
        //Incomming connection
        if (FD_ISSET(main_socket_id, &socket_descriptors))
        {
            socklen_t addrlen = sizeof(address);
            
            //Accepting the incomming connection
            int new_socket = accept(main_socket_id, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            
            if (new_socket < 0) {
                std::cout << "Error on accepting the incomming message" << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::cout << "New client requesting a service - IP: " << inet_ntoa(address.sin_addr) << " - PORT: " << ntohs(address.sin_port) << " - Socket to client: " << new_socket << std::endl;
        
            std::string output;
            output.resize(MAX_BUFFER_SIZE);
            
            long size;
            
            if((size = recv(new_socket,  &output[0] , MAX_BUFFER_SIZE-1 , 0)) < 0){
                std::cout << "Error on recieving the request from client " << std::endl;
            }
            
            output[size] = 0;
            std::cout << "========== REQUEST: ========" << std::endl;
            std::cout << output.c_str() << std::endl;
            
            redirect(output, new_socket);
            client_socket.push_back(new_socket);
        }
    }
    
    return 0;
}

