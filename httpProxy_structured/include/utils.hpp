//
//  utils.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
 * @file utils.hpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com definição de protótipos de funcoes gerais que podem ser utilizadas em qualquer tempo do codigo.
 */

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 *   @fn std::vector<std::string> split(std::string, std::string)
 *   @brief Função que subdivide uma string quebrando ela de acordo com o token requerido
 *   @param str string que deve ser quebrada.
 *   @param token token no qual deve ser sudividida a string str.
 *   @return std::vector<std::string> vetor de strings que representa str quando subdividida pelo caracter token
 */
std::vector<std::string> split(std::string str, std::string token);

/**
 *   @fn std::vector<std::string> readFile(std::string, std::string)
 *   @brief Função que le um arquivo e subidivide o seu conteudo utilizando o caracter passado por parametro para quebrar a string
 *   @param file string que representa o endereco do arquivo que deve ser lido.
 *   @param token token no qual deve ser sudividida a string resultante do conteudo do arquivo.
 *   @return std::vector<std::string> vetor de strings que representa o arquivo quando subdividido pelo caracter token
 */
std::vector<std::string> readFile(std::string file, std::string token);

/**
 *   @fn bool findString(std::vector<std::string>, std::string)
 *   @brief Função que busca uma string em um vetor de strings
 *   @param list vetor de strings.
 *   @param str string que deve ser buscada no vetor passado por parametro.
 *   @return bool True se str aparece em algum elemento de list, false case contrario
 */
bool findString(std::vector<std::string> list, std::string str);

/**
 *   @fn std::string getForbiddenResponse()
 *   @brief Função que gera um HTTPResponse com codigo de erro 403
 *   @return std::string String representando o HttpResponse de um erro 403 - Forbidden url
 */
std::string getForbiddenResponse();

/**
 *   @fn std::string formatedTime(const struct tm *)
 *   @brief Função que formata ums struct tm* para o formato utilizado no HttpResponse
 *   @param timeptr ponteiro para uma struct tm com a data a ser formatada .
 *   @return std::string String formatada com o formato de data do HttpResponse
 */
std::string formatedTime(const struct tm *timeptr);
#endif /* utils_hpp */
