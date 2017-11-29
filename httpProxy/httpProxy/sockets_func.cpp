//
//  sockets_func.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file sockets_func.cpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a implementacao dos metodos que fazem as interacoes com os sockets
 */

#include "sockets_func.hpp"
#include "utils.hpp"

int createserverSocket(std::string address, std::string port) {
    std::cout << "CREATE SERVER SOCKET" << std::endl;
    struct addrinfo server;
    struct addrinfo *socket_rep;
    
    int socket_server;
    
    /* Getting information about the address for socket on port */
    memset(&server, 0, sizeof(server));
    
    server.ai_family = AF_UNSPEC;
    server.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(&address[0], &port[0], &server, &socket_rep) != 0) {
        std::cout << address << std::endl;
        std::cout << port << std::endl;
        std::cout << "The address is not right. Error in server address format!" << std::endl;
        return -1;
    }
    
    /* Create the socket and connect to it */
    if ((socket_server = socket(socket_rep->ai_family, socket_rep->ai_socktype, socket_rep->ai_protocol)) < 0) {
        std::cout << " Error recieved while creating socket to server!" << std::endl;
        close(socket_server);
        return -1;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;  /* 1 Sec Timeout - Important for reading buffers from server socket */
    setsockopt(socket_server, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    
    if (connect(socket_server, socket_rep->ai_addr, socket_rep->ai_addrlen) < 0) {
        std::cout << "Error recieved while connecting to server!" << std::endl;
        close(socket_server);
        return -1;
    }
    
    /* Free paRes, which was dynamically allocated by getaddrinfo */
    freeaddrinfo(socket_rep);
    
     std::cout << "END OF CREATE SERVER SOCKET" << std::endl;
    return socket_server;
}

void writeToserverSocket(std::string message, int sockfd, int size)
{
    std::cout << "WRITING TO SERVER SOCKET " << sockfd << std::endl;
    int sent = 0;
    int sent_step;
    
    
    while (sent < size) {
        if ((sent_step = send(sockfd, (void *) (&message[0] + sent), size - sent, 0)) < 0) {
            std::cout << "Error recieved while sending message to the server!" <<std::endl;
            exit (1);
        }
        sent += sent_step;
    }
    
    std::cout << "END OF WRITING TO SERVER SOCKET " << sockfd << std::endl;
}

void writeToclientSocket(std::string message, int sockfd, int size)
{
    std::cout << "WRITING TO CLIENT SOCKET " << sockfd << std::endl;
    std::cout << "Message to sendd: " << std::endl;
    std::cout << message << std::endl;
    std::cout << size << std::endl;
   
    
    char * buff_to_server = new char[size+1];
    std::strcpy(buff_to_server, message.c_str());
    
    buff_to_server[size] = '\0';
    int totalsent = 0;
    int senteach;
    
    while (totalsent < size) {
        if ((senteach = send(sockfd, (void *) (buff_to_server + totalsent), size - totalsent, 0)) < 0) {
            fprintf (stderr," Error in sending to server ! \n");
            exit (1);
        }
        totalsent += senteach;
        
    }
    
    std::cout << " END OF WRITING TO CLIENT SOCKET " << sockfd << std::endl;
    close(sockfd);
    
}


void writeToclientSocket(std::vector<Buffer> result, int sockfd)
{
    std::cout << "WRITING TO CLIENT SOCKET - REDIRECT BUFFERS" << sockfd << std::endl;
    std::cout << "INto write" << std::endl;
    for(int i=0; i<result.size(); i++){
        int totalsent = 0;
        int senteach;
        int buff_length = result.at(i).buff_len;
        
        while (totalsent < buff_length) {
            if ((senteach = send(sockfd, (void *) (result.at(i).step + totalsent), buff_length - totalsent, 0)) < 0) {
                fprintf (stderr," Error in sending to server ! \n");
                exit (1);
            }
            std::cout << "Sent each: " << senteach << std::endl;
            totalsent += senteach;
        }
        
        if(totalsent  != buff_length){
            std::cout << "NEM TODOS OS BYTES FORAM ENVIADOS" << std::endl;
        }
    }
    std::cout << "END OF WRITING TO CLIENT SOCKET - REDIRECT BUFFERS" << sockfd << std::endl;
}


std::vector<Buffer> readFromServer (int Serverfd) {
    std::cout << " ======== READING FROM SERVER ===========" << Serverfd << std::endl;
    
    int iRecv;
    int total = MAX_BUF_SIZE;
    char buf[MAX_BUF_SIZE];
    
    struct timeval tv;
    tv.tv_sec = 5;  /* 1 Sec Timeout - Important for reading buffers from server socket */
    setsockopt(Serverfd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    
    std::vector<struct Buffer> result;
    
    while ((iRecv = recv(Serverfd, buf, MAX_BUF_SIZE, 0)) > 0 ) {
        struct Buffer item;
        memcpy(item.step, buf, iRecv);
        item.buff_len = iRecv;
        result.push_back(item);
        memset(buf,0,MAX_BUF_SIZE);
    }
    
    std::cout << "======= END OF READING FROM SERVER -======= " << Serverfd << std::endl;
    std::cout << result.size() << std::endl;
    return result;
}

std::string readFromSocket (int* socketId){
    
    std::cout << "READING FROM SOCKET " << *socketId << std::endl;
    char buf[MAX_BUF_SIZE];
    char *request_message;
    request_message = (char *) malloc(MAX_BUF_SIZE);
    
    if (request_message == NULL) {
        std::cout << "Got an error while allocating memory for message" << std::endl;
        //exit (1);
    }
    
    request_message[0] = '\0';
    int total_recieved_bits = 0;
    
    while (strstr(request_message, "\r\n\r\n") == NULL) {  // determines end of request
        int recvd = recv(*socketId, buf, MAX_BUF_SIZE, 0) ;
        if(recvd < 0 ){
            std::cout << "An error has occurred while receiving message from socket" << std::endl;
            //exit (1);
        }else if(recvd == 0) {
            break;
        } else {
            
            total_recieved_bits += recvd;
            
            /* if total message size greater than our string size,double the string size */
            
            buf[recvd] = '\0';
            if (total_recieved_bits > MAX_BUF_SIZE) {
                int total_buffer = MAX_BUF_SIZE * 2;
                request_message = (char *) realloc(request_message, total_buffer);
                if (request_message == NULL) {
                    std::cout << "An error has occurred while re-allocating memory for Message" << std::endl;
                    //exit (1);
                }
            }
            
            
        }
        
        strcat(request_message, buf);
        
    }
    std::string str = request_message;
    std::cout << "END OF READING FROM SOCKET " << *socketId << std::endl;
    return str;
}

