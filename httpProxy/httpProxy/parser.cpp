//
//  parser.cpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//


/**
 * @file parser.cpp
 * @author Lukas Ferreira Machado
 * @brief Arquivo com a implementacao dos metodos que fazem as interacoes com os mensagens http trocadas
 */

#include "parser.hpp"
#include "utils.hpp"


HttpRequest parserRequest(std::string str){
    HttpRequest request = *new HttpRequest();
    
    std::vector<std::string> splitResult = split(str, "\n");
    
    long size = splitResult.size();
    
    std::string firstLine = splitResult.at(0);
    
    std::vector<std::string> line1 = split(firstLine, " ");
    request.setMethod(line1.at(0));
    
    //if(line1.size() > 1)
        request.setUrl(line1.at(1));
        
    request.setVersion(line1.at(2));
    
    for(int i = 1; i < size - 1; i++){
        request.addHeader(splitResult.at(i));
    }
    request.setEntityBody(splitResult.at(size-1));
   
    return request;
}
