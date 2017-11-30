//
//  utils.hpp
//  httpProxy
//
//  Created by Lukas Ferreira on 11/21/17.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

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


std::vector<std::string> split(std::string, std::string);

std::vector<std::string> readFile(std::string, std::string);

bool findString(std::vector<std::string>, std::string);

std::string getForbiddenResponse();

int hostname_to_ip(std::string hostname , std::string& ip);

void remove_tags(std::string& str);
#endif /* utils_hpp */
