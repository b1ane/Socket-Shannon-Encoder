#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct symbol
{
    char name;
    int frequency;
    double probability;
    char code[10];
};

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Checking for the hostname and port number from the command line.
    if (argc < 3)
    {
        std::cerr << "usage " << argv[0] << "hostname port\n";
        exit(0);
    }
    
    // Creating a socket descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    
    // Storing the hostname information
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }

     // Transforming the port number to int
    portno = atoi(argv[2]);

    // Populating the sockaddr_in structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    
    // Connecting to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }

    // Sending a symbol
    symbol s;
    s.name = 'C';
    s.frequency = 100;
    s.probability = 0.5;
    strcpy(s.code,"Carlos\0");
    n = write(sockfd, &s, sizeof(symbol));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
   
    // Closing the socket
    close(sockfd);
    return 0;
}
