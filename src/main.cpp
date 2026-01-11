#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <chrono>
#include <iomanip>
#include <cerrno> 

#define format1 "%Y-%m-%d %H:%M:%S"

std::tm get_time(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
    localtime_r(&now_time_t,&tm_buf);
    return tm_buf;
}

int main(){
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd == -1) {
        std::cerr << "Error of creation socket: " << strerror(errno) << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }

    sockaddr_in adress;
    memset(&adress,0,sizeof(adress));
    adress.sin_family = AF_INET;
    adress.sin_addr.s_addr = INADDR_ANY;
    adress.sin_port = htons(8080);

    if(bind(server_fd,(sockaddr*)&adress,sizeof(adress))<0){
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    if(listen(server_fd,5)<0){
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }   

    std::ofstream log_file("out.log");

    if(!log_file.is_open()){
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;  
    }

    auto now = get_time();
    log_file <<"[" <<std::put_time(&now,format1)<<"] Server starts\n";
    std::cout<<"Server start successfully !\n";

    while(true){
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr,&client_len);
        if (client_fd <0) {
            int saved_errno = errno; 
            auto now = get_time();
            log_file << "[" 
            << std::put_time(&now, format1) 
            << "] Error accept: " << strerror(saved_errno) << std::endl;
            std::cerr << "Error accept: " << strerror(errno) << std::endl;
            continue; 
        }

        char buffer[4096];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            auto now = get_time();
            log_file << "[" 
            << std::put_time(&now,format1)
            << "] Получен запрос:\n" << buffer << std::endl;
            std::cout << "Получен запрос:\n" << buffer << std::endl;
        }
        const char* resp =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Hello from C++ HTTP server!\n";
        write(client_fd, resp, strlen(resp));

        close(client_fd);
    }

    now = get_time();
    log_file <<"[" <<std::put_time(&now,format1)<<"] Server closed\n";
    close(server_fd);
    
    return 0;
}