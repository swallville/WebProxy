//
//  sockets_func.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//


/**
 * @file sockets_func.hpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a definição dos metodos que fazem as interacoes com os sockets
 */

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

/**
 * @value Struct que representa um trecho de buffer das mensagens trocadas entre server, client e proxy
 */
struct Buffer{
    /**
     * @value Conteudo do buffer
     */
    char step[MAX_BUF_SIZE];
    
    /**
     * @value Tamanho do buffer em bytes
     */
    int buff_len;
};

/**
 * @fn int createserverSocket(std::string, std::string)
 * @brief Função que cria um client socket para o server cujo host e porta sao recebidos por parametros
 * @param address String que representa o host a ser conectado.
 * @param port String que representa a porta onde o socket deve se conectar.
 * @return int Inteiro representando o socket de comunicacao com o server recem criado
 */
int createserverSocket(std::string address, std::string port);

/**
 * @fn void writeToserverSocket(std::string, int, int)
 * @brief Função que escreve uma string no socket para um server cujo identificador e passado por parametros
 * @param message String que representa a mensagem a ser enviada.
 * @param sockfd Inteiro que representa o socket de conexao do proxy com o servidor de destino.
 * @param size Inteiro que representa o tamanho da mensagem a ser enviada para o servidor de destino.
 */
void writeToserverSocket(std::string message, int sockfd, int size);

/**
 * @fn void writeToclientSocket(std::string, int, int);
 * @brief Função que escreve uma string no socket para um client cujo identificador e passado por parametros
 * @param message String que representa a mensagem a ser enviada.
 * @param sockfd Inteiro que representa o socket de conexao do proxy com o cliente que requsiitou a mensagem.
 * @param size Inteiro que representa o tamanho da mensagem a ser enviada para o cliente que originalmente requisitou uma pagina.
 */
void writeToclientSocket(std::string message, int sockfd, int size);

/**
 * @fn void writeToclientSocket(std::vector<Buffer>, int)
 * @brief Função que escreve um grupo de buffers no socket para um client cujo identificador e passado por parametros
 * @param result Vetor de Buffer representando um grupo de buffers a serem redirecionados paraum socket client.
 * @param sockfd Inteiro que representa o socket de conexao do proxy com o cliente para onde deve ser redirecionada a mensagem.
 */
void writeToclientSocket(std::vector<Buffer> result, int sockfd);

/**
 * @fn std::vector<Buffer> readFromServer (int)
 * @brief Função que le um grupo de buffers do socket cujo identificador e passado por parametro
 * @param Serverfd Inteiro que representa o socket de onde devem ser lidos os bytes.
 * @return std::vector<Buffer> Vetor de Buffer representando o grupo de buffers lidos do socket Serverfd.
 */
std::vector<Buffer> readFromServer (int Serverfd);

/**
 * @fn std::string readFromSocket (int*)
 * @brief Função que le a mensagem enviada a partir do socket cujo identificador e passado por parametro
 * @param socketId Inteiro que representa o socket de onde devem ser lidos os bytes.
 * @return std::string String representando o conteudo de buffers lidos do socket socketId.
 */
std::string readFromSocket (int* socketId);


#endif /* sockets_func_hpp */
