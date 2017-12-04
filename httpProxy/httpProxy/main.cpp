//
//  main.cpp
//  socket
//
//  Created by Lukas Ferreira on 12/11/2017.
//  Copyright © 2017 Lukas Ferreira. All rights reserved.
//

/**
* @file main.cpp
* @author Lukas Ferreira Machado
* @brief Arquivo que contem a funcao inicial do programa e contem a logica de redirecionamento das mensagens.
*/

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include "httpRequest.hpp"
#include "sockets_func.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include <map>
#include <algorithm>
#include <string>

#define TRUE 1
#define FALSE 0
#define PORT 8000
#define MAX_CONNECTIONS 100
#define PATH_WHITELIST "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/whitelist.txt"
#define PATH_BLACKLIST "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/blacklist.txt"
#define PATH_DENY_TERMS "/Users/rosanarogiski/Documents/WebProxy/httpProxy/httpProxy/deny_terms.txt"
#define PARSER_TOKEN "\n"
#define DIE exit(1);

/**
 * @value whitelist Armazena a lista de url's autorizadas pelo administrador do proxy
 */
std::vector<std::string> whitelist;

/**
 * @value blacklist Armazena a lista de url's proibidas pelo administrador do proxy
 */
std::vector<std::string> blacklist;

/**
 * @value whitelist Armazena a lista de termos proibidos pelo administrador do proxy
 */
std::vector<std::string> deny_terms;

/**
 * @value cache Armazena a lista de httpResponse que representa o cache da aplicacao
 */
std::map<std::string, std::vector<Buffer>> cache;

/**
 * @value editarRequests Armazena a escolha entre editar ou nao os requests
 */
bool editarRequests;

/**
 *   @fn int allowRedirection(HttpRequest, std::string)
 *   @brief Função que verifica se uma determinada url pode ser acessada, ou seja se esta na whitelist, blacklist ou se o request possui termos proibidos
 *   @param request Objeto HttpRequest representando o request recebido.
 *   @param message String com a mensagem original do request recebido pelo proximo.
 *   @return int entre 0 e 3, onde os numeros impares representam true e os pares representam o valor false
 */
int allowRedirection(HttpRequest request, std::string message){
    //Is the url in the whitelist?
    if(findString(whitelist, request.getUrl())){
        return 1;//true
    } else if(findString(blacklist, request.getUrl())){
        return 0;//false
    } else{
        if(findString(deny_terms, message)){
            return 2;//false
        }else{
            return 3;//true
        }
    }

    return true;
}

/**
 *   @fn int tipoResponse(std::vector<std::string>)
 *   @brief Função que verifica com que tipo de caching a pagina web trabalha: Last_Modified, ETag, Expires, max-age
 *   @param cache_response Vetor de Objeto Buffer representando a resposta do servidor.
 *   @return int onde: -1: significa que nenhuma tag de caching foi encontrada; 1: significa cachngo por Last_modified; 2: significa caching por ETag; 3: caching pela tag Expires; 4: caching pela tag max-age
 */
int tipoResponse(std::vector<Buffer> cache_response){
    //Find type of caching
    int tipo = -1;
    for(int i= 0; i<cache_response.size(); i++){
        std::string str = cache_response.at(i).step;
        if(str.find("Last-Modified:") != -1){
            tipo = 1;
            break;
        }else if(str.find("ETag:") != -1){
            tipo = 2;
            break;
        }else if(str.find("Expires:") != -1){
            tipo = 3;
            break;
        }else if(str.find("max-age=") != -1){
            tipo = 4;
            break;
        }
    }
    
    return tipo;
}


/**
 *   @fn void redirectMessage(HttpRequest, std::string, int)
 *   @brief Função que processa, redireciona e retorna ao cliente a resposta do request recebido
 *   @param request Objeto HttpRequest representando o request recebido.
 *   @param str String com a mensagem original do request recebido pelo proximo.
 *   @param socketClient Id represenatndo o socket que liga o proxy ao cliente que solicitou o servico.
 */
void redirectMessage(HttpRequest request, std::string str, int socketClient)
{
    int redirection_allowed = allowRedirection(request, str);
    bool isForbidden = false;

    if(redirection_allowed % 2 == 1 ){ //true
        //Is the Response present in the cache?
        
        std::map<std::string, std::vector<Buffer>>::iterator it = cache.find(request.getUrl());
        std::vector<Buffer> cache_response;
        if (it != cache.end())
            cache_response = cache.find(request.getUrl())->second;

        int tipo = tipoResponse(cache_response);
        
//        std::cout << "Tipo:  " << tipo << std::endl;
    
        //redirect the request...
        
        char * host;
        std::string url = request.getHost();
        std::string portno = request.getPort();
        
//        std::cout << "=================================================================" <<std::endl;
//        std::cout << "Host to be called: " << url << " - Port : " << portno <<std::endl;
//        std::cout << request.getMethod() <<std::endl;
//        std::cout << request.getUrl() <<std::endl;
//        std::cout << request.getVersion() <<std::endl;
//        std::cout << "=================================================================" <<std::endl;

        host = new char[url.size() + 1];
        std::strcpy(host, url.c_str());

        char *port = new char[portno.size()];
        std::strcpy(port, portno.c_str());

        int socketServer = -1;
        if(tipo == 1 || tipo == -1 || tipo == 2){
            socketServer = createserverSocket(host, port);
//            std::cout << "Socket server " << socketServer << std::endl;
        }
      
        bool on_cache = false;
        if(socketServer != -1){
            struct timeval tv;
            tv.tv_sec = 5;  /* 1 Sec Timeout - Important for reading buffers from server socket */
            setsockopt(socketServer, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
            
            if(tipo != -1){
                
                std::vector<std::string> req_str = split(str, PARSER_TOKEN);
                
                std::string str_resultante = "";
                str_resultante = req_str.at(0) + "\n";
                str_resultante += req_str.at(1) + "\n";
                
                if(tipo == 1){
                    std::string date = getLastModifiedDate(cache_response);
                    str_resultante += "If-modified-since: "+date + "\n";
                } else if(tipo == 2){
                    std::string etag = getETag(cache_response);
                    str_resultante += "If-None-Match: \""+etag + "\"\n";
                } else if(tipo == 3){
                    struct tm tm;
                    time_t t;
                    
                    std::string expires = getExpires(cache_response);
                    
                    //Thu, 31 Dec 2037 23:55:55 GMT
                    if (strptime(expires.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm) != NULL){
                        t = mktime(&tm);
                        if (t == -1){
                            if(std::difftime(std::time(nullptr), t) > 0)
                                on_cache = true;
                        }
                    }
                } else if(tipo == 4){
                    struct tm tm;
                    time_t t;
                    
                    std::string date = getDate(cache_response);
                    
                    //Thu, 31 Dec 2037 23:55:55 GMT
                    if (strptime(date.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm) != NULL){
                        t = mktime(&tm);
                        if (t == -1){
                            long time_t = (long) t;
                            std::string maxAge = getMaxAge(cache_response);
                            std::string::size_type sz;   // alias of size_t
                            long li_dec = std::stol (maxAge,&sz);
                            time_t += li_dec;
                            
                             std::time_t agora = std::time(nullptr);
                            long time_now = (long) agora;
                            
                            
                            if(agora < time_t)
                                on_cache = true;
                        }
                    }
                }
                
                if(tipo != 3 && tipo != 4){
                    for(int i=2; i<req_str.size(); i++){
                        str_resultante += req_str.at(i) + "\n";
                    }
                    str = str_resultante;
                }
                
//                std::cout << "To be written: " << str  << std::endl;
//                std::cout << "Entrou no if " << std::endl;
            }
            
            std::vector<Buffer> response_from_server;
            if(!on_cache){
                writeToserverSocket(str, socketServer, (int)str.size());
                response_from_server = readFromServer(socketServer);
                
                //Primeira linha do response contem codigo de retorno
               
                if(response_from_server.size() > 0){
                    std::string line1 = response_from_server.at(0).step;
                    if(line1.find("304 Not Modified") != -1){
                        response_from_server = cache_response;
//                        std::cout << "Entrou no 304 Not Modified" << " -- -- Usou CACHE" << std::endl;
                    }
                }
            }else{
                response_from_server = cache_response;
            }
            
            if(response_from_server.size() > 0){
                if(redirection_allowed > 1){ //deny_terms rule
                    for(int i = 0; i < response_from_server.size(); i++){
                        if(findString(deny_terms, response_from_server.at(i).step)){
                            isForbidden = true;
                            break;
                        }
                    }
                }
                
//                std::cout << "Forbidden " << isForbidden << std::endl;
                if(!isForbidden){
                    // writing to client
                    writeToclientSocket(response_from_server, socketClient);
                    
//                    std::cout << "Adding to cache " << std::endl;
                    cache[request.getUrl()] = response_from_server;
                    
//                    std::cout << "cache size: " << cache.size() << std::endl;
                }
                //Close the server socket
                close(socketServer);
            }else{
                std::string timeout = getTimeoutResponse();
                writeToclientSocket(timeout, socketClient, (int)timeout.size());
                
                close(socketServer);
                isForbidden = false;
            }
            
        }else{
            //erro ao criar socket - host nao existe
            std::string notFound = getNotFoundResponse();
            writeToclientSocket(notFound, socketClient, (int)notFound.size());
            
            close(socketServer);
            return;
        }

    }else{
        isForbidden = true;
       
    }

    if(isForbidden){
        std::string forbidden = getForbiddenResponse();
        writeToclientSocket(forbidden, socketClient, (int)forbidden.size());
    }

    std::ofstream output_file;
    output_file.open("log.txt", std::ios::in | std::fstream::out | std::fstream::app);

    char dt[1000];

    time_t t = time(0);
    struct tm * p = localtime(&t);

    strftime(dt, 1000, "[%d/%b/%Y:%H:%M:%S %z]", p);

    std::string http_status_code;
    std::string ip_addr;

    hostname_to_ip(request.getHost(), ip_addr);

    if(redirection_allowed == 0){
        http_status_code = "403";
    }
    else if(redirection_allowed == 1){
        http_status_code = "200";
    }
    else if(redirection_allowed == 2){
        http_status_code = "403";
    }
    else{
        http_status_code = "200";
    }

    std::string version = request.getVersion();
    remove_tags(version);

    output_file << ip_addr << " - " << dt << " '" << request.getMethod()
                << " " << request.getUrl() << " " << version << "' " << http_status_code
                << "\n";

    output_file.close();

    //Close the Client socket
    close(socketClient);

}

/**
 *   @fn static void* beginExecution(void*)
 *   @brief Função que executa a thread processando o request e devolvendo a resposta para o cliente de origem
 *   @param sockfdPtr struct representando o socket que liga o proxy server ao cliente do request.
 *   @return static void* retorna ponteiro para null
 */
static void* beginExecution(void* sockfdPtr)
{
    long sockid = (intptr_t)sockfdPtr;
    int sockid_cast = (int)sockid;
    std::string str = readFromSocket(&sockid_cast);
    
    if(str.length() == 0){
        return (void*)NULL;
    }
    
    HttpRequest request = parserRequest(str);
    
    if(editarRequests){
        std::cout << "=======================================================================================" << std::endl;
        std::cout << "Insira o novo valor alterado da linha ou apenas enter para manter a linha em questao. Para adicionar novas linhas adicione ao final. Para remover a linha digite 'EXCLUIR'" << std::endl;
        
        std::vector<std::string> request_lines = split(str, PARSER_TOKEN);
        std::string result ="";
        
        std::string linha_anterior = "";
        
        for(int i=0; i<request_lines.size(); i++){
            std::string line;
            std::cout << request_lines.at(i) << std::endl;
            if(i==0)
                std::cin.ignore();
            
            std::getline (std::cin, line);
            
            std::locale loc;
            std::string excluir = "EXCLUIR";
            
            std::transform(line.begin(), line.end(),line.begin(), ::toupper);
            
            if(line.empty()){
                result += request_lines.at(i) + "\n";
                if( i != 0 && linha_anterior.length() == 0){//adicao de linhas
                    
                }else{
                    linha_anterior = request_lines.at(i);
                }
                
            }else if (excluir.compare(line) != 0){
                result += line + "\n";
            }else{
                //excluir a linha
                //faz nada
            }
        }
        
        if(linha_anterior.length() == 0){
            result += "\n\n";
        }
        
        str = result;
        
        std::cout << "=======================================================================================" << std::endl;
        std::cout << "Novo Request: " << std::endl;
        
        std::cout << "---------------------------------------------------------------------------------------" << std::endl;
        std::cout << str << std::endl;
        std::cout << "---------------------------------------------------------------------------------------" << std::endl;
        
        
    }
    redirectMessage(request, str, sockid_cast);
    /*final request to be sent*/

//    std::cout << "END OF ROUTINE ..." <<std::endl << std::endl;
    return (void*)NULL;
}

/**
 *   @fn int main(int, char *)
 *   @brief Função que inicializa o programa
 *   @param argc quantidade de parametros passados para o programa
 *   @param argv parametros passados na execucao do programa
 *   @return int 1 se erro; 0 se a execucao do programa termina sem erros
 */
int main(int argc , char *argv[])
{
    //Read filter files
    whitelist = readFile(PATH_WHITELIST, PARSER_TOKEN);
    blacklist = readFile(PATH_BLACKLIST, PARSER_TOKEN);
    deny_terms = readFile(PATH_DENY_TERMS, PARSER_TOKEN);
    
    std::cout << "==========================================================" << std::endl;
    std::cout << "Deseja editar os request's? (S/N)" << std::endl;
    std::string editar;
    std::cin >> editar;
    std::cout << "==========================================================" << std::endl;
    
    if(editar[0] == 'S' || editar[0] == 's')
        editarRequests = true;
    else
        editarRequests = false;
    
    std::mutex _mutex;
    
    struct sockaddr_in address;
    
    //Creating the main socket
    int main_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    
    if (main_socket_id < 0)
    {
        std::cout << "Error on creating the main server socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Allow main socket to accept multiple connections
    int allow_multiple_connections = TRUE;
    
    if (setsockopt(main_socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&allow_multiple_connections, sizeof(allow_multiple_connections)) < 0)
    {
        std::cout << "Error on allowing multiple connections in the socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Setting the characteristics of a socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    //Bind the socket to the defined port
    if (bind(main_socket_id, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cout << "Error on binding the main socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //signal(SIGPIPE, SIG_IGN);
    std::cout << "SERVER IS LISTENING ON PORT " << PORT << std::endl;
    
    //Set maximim requests on the server queue
    if (listen(main_socket_id, MAX_CONNECTIONS) < 0)
    {
        std::cout << "Error while specifying trying to listen the socket" << std:: endl;
        exit(EXIT_FAILURE);
    }
    
    
    std::cout << "SERVER IS WAITING FOR CONNECTIONS..." << std::endl;
    
   
    
    while (TRUE)
    {
       
        //signal(SIGPIPE, SIG_IGN);
        
        //accept the incoming connection
        socklen_t addrlen = sizeof(address);
            
        int new_socket = accept(main_socket_id, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            
        if (new_socket < 0)
        {
            perror("Error on accepting connections");
            exit(EXIT_FAILURE);
        }
            
        //inform user of socket number - used in send and receive commands
        printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
        pthread_t thread;
        std::unique_lock<std::mutex> lock(_mutex);
        if(pthread_create(&thread, NULL, &beginExecution, (void*)(intptr_t)new_socket) < 0) {
            std::cout << "Error creating thread.. exiting" << std::endl;
            DIE
        }
        
        pthread_detach(thread);
        thread = NULL;
    }
    
    return 0;
}
