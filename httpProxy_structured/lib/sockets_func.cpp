//
//  sockets_func.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

#include "sockets_func.hpp"


int createserverSocket(std::string address, std::string port) {
    struct addrinfo server;
    struct addrinfo *socket_rep;
    
    int socket_server;
    
    /* Getting information about the address for socket on port */
    memset(&server, 0, sizeof(server));
    
    server.ai_family = AF_UNSPEC;
    server.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(&address[0], &port[0], &server, &socket_rep) != 0) {
        std::cout << "The address is not right. Error in server address format!" << std::endl;
        //exit (1);
    }
    
    /* Create the socket and connect to it */
    if ((socket_server = socket(socket_rep->ai_family, socket_rep->ai_socktype, socket_rep->ai_protocol)) < 0) {
        std::cout << " Error recieved while creating socket to server!" << std::endl;
        close(socket_server);
        //exit (1);
    }
    if (connect(socket_server, socket_rep->ai_addr, socket_rep->ai_addrlen) < 0) {
        std::cout << "Error recieved while connecting to server!" << std::endl;
        close(socket_server);
        //exit (1);
    }
    
    /* Free paRes, which was dynamically allocated by getaddrinfo */
    freeaddrinfo(socket_rep);
    
    return socket_server;
}

void writeToserverSocket(std::string message, int sockfd, int size)
{
    int sent = 0;
    int sent_step;
    
    message.resize(size+1);
    message[size] = '\0';
    
    while (sent < size) {
        if ((sent_step = send(sockfd, (void *) (&message[0] + sent), size - sent, 0)) < 0) {
            std::cout << "Error recieved while sending message to the server!" <<std::endl;
            //exit (1);
        }
        sent += sent_step;
    }
}

void writeToclientSocket(std::string message, int sockfd, int size)
{
    int sent = 0;
    int sent_step;
    
    message.resize(size+1);
    message[size] = '\0';
    
    while (sent < size) {
        if ((sent_step = send(sockfd, (void *) (&message[0] + sent), size - sent, 0)) < 0) {
            std::cout << "Error recieved while writing message to the client server!" <<std::endl;
            //exit (1);
        }
        sent += sent_step;
    }
    
}

void writeToClient (int Clientfd, int Serverfd) {
    
    int recieved;
    
    std::string output;
    output.resize(MAX_BUFFER_SIZE);
    
    while ((recieved = recv(Serverfd, &output[0], MAX_BUFFER_SIZE, 0)) > 0) {
        writeToclientSocket(&output[0], Clientfd, recieved);         // writing to client
        //memset(buf,0,sizeof buf);
        output.clear();
    }
    
    /* Error: no recieved message */
    if (recieved < 0) {
        std::cout << "Error recieved while recieving message response from server" << std::endl;
        //exit (1);
    }
}

