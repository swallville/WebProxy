//
//  sockets_func.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

#ifndef sockets_func_hpp
#define sockets_func_hpp

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <netdb.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#define MAX_BUF_SIZE 1500

struct Buffer{
    char step[MAX_BUF_SIZE];
    int buff_len;
};

int createserverSocket(std::string, std::string);
void writeToserverSocket(std::string, int, int);
void writeToclientSocket(std::string, int, int);
void writeToclientSocket(std::vector<Buffer>, int);
std::vector<Buffer> readFromServer (int);
std::string readFromSocket (int*);


#endif /* sockets_func_hpp */
