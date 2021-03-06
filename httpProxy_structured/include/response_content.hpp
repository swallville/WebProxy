//
//  response_content.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/22/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file response_content.hpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a definição dos metodos que criam mensagens http de response especificas de erros e afins
 */

#ifndef response_content_hpp
#define response_content_hpp

#include <stdio.h>
#include <string>
#include <cstring>

/**
 * @fn std::string getForbidden()
 * @brief Função que cria uma mensagem http response para erros do tipo 403 - Forbidden
 * @return std::string String representando a mensagem http response para erros 403 - Forbidden url
 */
std::string getForbidden();

/**
 * @fn std::string getNotFound()
 * @brief Função que cria uma mensagem http response para erros do tipo 404 - Not Found
 * @return std::string String representando a mensagem http response para erros 404 - Not Found url
 */
std::string getNotFound();

/**
 * @fn std::string getTimeout()
 * @brief Função que cria uma mensagem http response para erros do tipo 408 - Request Timeout
 * @return std::string String representando a mensagem http response para erros 408 - Request Timeout
 */
std::string getTimeout();

#endif /* response_content_hpp */
