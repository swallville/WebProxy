//
//  httpRequest.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/15/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file httpRequest.hpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a definição da classe que representa um HttpRequest
 */

#ifndef httpRequest_hpp
#define httpRequest_hpp

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>
#include "utils.hpp"

class HttpRequest{

private:
    /**
     * @value Metodo de Http (Get, Put, Post, ...)
     */
    std::string method;
    
    /**
     * @value String representando a url requisitada no request
     */
    std::string url;
    
    /**
     * @value String representando a versao do HTTP utilizada no browser
     */
    std::string version;
    
    /**
     * @value Vetor de String's representando o conteudo do header do HTTP Request
     */
    std::vector<std::string> headers;
    
    /**
     * @value String representando o conteudo do body do HTTP Request
     */
    std::string entityBody;

public:

    /**
     * @fn HttpRequest()
     * @brief Construtor da classe HttpRequest
     */
    HttpRequest();
    
    /**
     * @fn void setMethod(std::string)
     * @brief Setter para o attributo method
     * @param method String representando o method requisitado na mensagem de request.
     */
    void setMethod(std::string method);
    
    /**
     * @fn void setUrl(std::string)
     * @brief Setter para o attributo url
     * @param url String representando o url requisitado na mensagem de request.
     */
    void setUrl(std::string url);
    
    /**
     * @fn void setVersion(std::string)
     * @brief Setter para o attributo version
     * @param version String representando a versao do httpRequest utilizada no request.
     */
    void setVersion(std::string version);
    
    /**
     * @fn void setHeaders(std::vector<std::string>)
     * @brief Setter para o attributo headers
     * @param headers Vetor de string's representanto o conteudo do header da mensagem de request.
     */
    void setHeaders(std::vector<std::string> headers);
    
    /**
     * @fn void setEntityBody(std::string)
     * @brief Setter para o attributo entityBody
     * @param entityBody String representanto o conteudo do entityBody da mensagem de request.
     */
    void setEntityBody(std::string entityBody);
    
    /**
     * @fn void addHeader(std::string)
     * @brief Adiciona uma string ao vetor de string's que representa o header da mensagem de request
     * @param header String a ser adicionada no atributo headers.
     */
    void addHeader(std::string header);

    
    /**
     * @fn std::string getMethod()
     * @brief Getter para o atributo method
     * @return std::string Retorna o metodo solicitado na mensagem de request de http
     */
    std::string getMethod();
    
    /**
     * @fn std::string getUrl()
     * @brief Getter para o atributo url
     * @return std::string Retorna a url solicitada na mensagem de request de http
     */
    std::string getUrl();
    
    /**
     * @fn std::string getVersion()
     * @brief Getter para o atributo version
     * @return std::string Retorna a versao de http utilizada na mensagem de request
     */
    std::string getVersion();
    
    /**
     * @fn std::vector<std::string> getHeaders()
     * @brief Getter para o atributo headers
     * @return std::vector<std::string> Retorna um vetor de string's representando os headers da  mensagem HTTP Request
     */
    std::vector<std::string> getHeaders();
    
    /**
     * @fn std::string getEntityBody()
     * @brief Getter para o atributo entityBody
     * @return std::string Retorna uma string representando o conteudo do entityBody da mensagem HTTP Request
     */
    std::string getEntityBody();
    
    /**
     * @fn std::string getHost()
     * @brief Faz o parser da url solicitada e identifica o host requisitado na mensagem HttpRequest
     * @return std::string Retorna o host requisitado na mensagem HttpRequest
     */
    std::string getHost();
    
    /**
     * @fn std::string getPort()
     * @brief Faz o parser da url solicitada e identifica a porta requisitada na mensagem HttpRequest
     * @return std::string Retorna o porta requisitada na mensagem HttpRequest. e nao ha uma porta especifica solicitada, retorna-se a porta padrao http: 80
     */
    std::string getPort();

    /**
     * @fn void print()
     * @brief Imprime os dados do http request
     */
    std::string print();

   // ~HttpRequest();
};
#endif /* httpRequest_hpp */
