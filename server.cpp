#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    int sockfd;                      //socket file descriptor
    int portno;                      // port number
    int n;                           // like int pid from fork()
    std::string buffer;              // holds the string being transfered to and from the server
    struct sockaddr_in serv_addr;
    struct hostent *server;
}