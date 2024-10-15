// Hostname = 172.17.245.143, port = 1234


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <csignal>


struct symbol
{
    char name;
    int frequency;
    double probability;
    char code[10];
};

// Fireman function
void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    // Creating a signal to execute the fireman function when a child process end its execution
    signal(SIGCHLD, fireman); 
    
    // Check for the port number from the command line  
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }

    //Create a socket descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    
    //Populate the sockaddr_in structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    // Bind the socket descriptor with the sockaddr_in structure
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }

    // Set the maximum number of concurrent connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {
        // Accept a request from the client. A new socket descriptor is created to handle the request
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        
        // Create a child process to answer the request.
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            symbol s;
            n = read(newsockfd, &s, sizeof(symbol));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            std::cout << "Name: " << s.name << std::endl;
            std::cout << "Frequency: " << s.frequency << std::endl;
            std::cout << "Probability: " << s.probability << std::endl;
            std::cout << "Code: " << s.code << std::endl;
            
            //Close the newsocket descriptor
            close(newsockfd);

            // Terminate the child process
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}