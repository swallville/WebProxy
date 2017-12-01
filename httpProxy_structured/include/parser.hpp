//
//  parser.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file parser.hpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a definição dos metodos que fazem as interacoes com os mensagens http trocadas
 */

#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>
#include <string>
#include <cstring>
#include "httpRequest.hpp"

/**
 * @fn HttpRequest parserRequest(std::string)
 * @brief Função que faz o parser de uma string que representa uma mensagem http request
 * @param str String a ser parseada.
 * @return HttpRequest Objeto HttpRequest representando a mensagem de request recebida como parametro
 */
HttpRequest parserRequest(std::string str);

#endif /* parser_hpp */
