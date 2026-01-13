#include <iostream>
#include <fstream>

int main(){
    std::ifstream file("../public/user.log");
    if(!file.is_open()){
        std::cout<< "HTTP/1.1 404 Not Found\r\n Connection: close\r\n\r\n";
    }
    std::string str;
    while(std::getline(file,str)){
        std::cout<<"<p>"<< str <<"</p>\r\n";
    }
    std::cout<<"\r\n";
    return 0;
}