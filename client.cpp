#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <vector>
#include <string>
#include <cmath>

struct simpleData {
    char Message[100];
    char Symbol[10];
    int Frequency;
    char ShannonCode[20];
    char EncodedMessage[200];
};

struct Data {
    std::vector<char> letters; //stores each letter in message
    std::vector<int> frequency; //stores frequency of each letter in message
    std::vector<int> shannon; //stores shannon code for each letter in message
    std::string line; //message
    std::string encoded;

    std::vector<char> orderedLetters; //stores each letter in message, ordered by frequency 
    std::vector<int> orderedFrequency; //stores frequency of each letter in message, ordered by frequency 
    std::vector<float> probabilities;
    std::vector<float> accumulation;
    std::vector<float> length;

    std::vector<std::string> code;
};

void inOrder(std::vector<char>& orderedL, std::vector<int>& orderedF) {
    for( int i = 0; i < orderedF.size(); i++) {
        for( int j = i + 1; j < orderedF.size(); j++) {
            if(orderedF[j] > orderedF[i] || (orderedF[j] == orderedF[i] && orderedL[j] > orderedL[i])) {
                std::swap(orderedF[i], orderedF[j]);
                std::swap(orderedL[i], orderedL[j]);
            }
        }
    }
}

void dec2bin(std::vector<float>& len, std::vector<float>& acc,  std::vector<std::string>& cod) {
    for( int i = 0; i < acc.size(); i++ ) {
        float fraction = acc[i];
        std::string code = "";
        int originalLength = len[i];
        while(len[i] > 0) {
            fraction = fraction * 2;
            int integer = static_cast<int>(fraction);
            
            code = code + std::to_string(integer);  

            fraction = fraction - integer;
            len[i] = len[i] - 1;    
        }
        while(code.length() < originalLength) {
            code = "0" + code;
        }
        cod.push_back(code);
    }
}


void* freq(void* void_ptr) {
    Data* ptr = (Data*) void_ptr;

    //stores each letter in string into its own vector 
    for(int i = 0; i < ptr->line.size(); i++) {
        ptr->letters.push_back(ptr->line[i]);
    }

    ptr->frequency.resize(ptr->letters.size(),0);

    //count frequency of each letter in the string
    for( int i = 0; i < ptr->letters.size(); i++) {
        char current = ptr->letters[i];
        bool isDupe = false;

        for(int j = 0; j < i; j++) {
            if(ptr->letters[j] == current) {
                isDupe=true;
                break;
            }
        }

        if(!isDupe) {
            int count = 0; 
            for(int j = 0; j < ptr->letters.size(); j++) {
                if(ptr->letters[j] == current) {
                    count++;
                }
            }
            ptr->frequency[i] = count;
        }
    }


//places letters and its frequencies in the ordered vectors, ordered by frequeny 
    for( int i = 0; i < ptr->letters.size(); i++ ) {
        if(ptr->frequency[i] > 0) {
            ptr->orderedFrequency.push_back(ptr->frequency[i]);
            ptr->orderedLetters.push_back(ptr->letters[i]);
        }
    }

    inOrder(ptr->orderedLetters, ptr->orderedFrequency);

//find probabilities
    int sum = 0;
    for( int i = 0; i < ptr->orderedFrequency.size(); i++ ) {
        sum = sum + ptr->orderedFrequency[i];
    }

    ptr->probabilities.resize(ptr->orderedFrequency.size());

    for( int i = 0; i < ptr->orderedFrequency.size(); i++ ) {
        ptr->probabilities[i] = static_cast<float>(ptr->orderedFrequency[i]) / sum;
    }

//accumulation
    ptr->accumulation.resize(ptr->probabilities.size());

    ptr->accumulation[0] = 0.0; //first symbol is always 0
    for( int i = 1; i < ptr->probabilities.size(); i++) {
        ptr->accumulation[i] = ptr->accumulation[i-1] + ptr->probabilities[i-1]; 
    }

    ptr->length.resize(ptr->probabilities.size());

//length of shannon code
    for( int i = 0; i < ptr->probabilities.size(); i++ ) {
        ptr->length[i] = std::ceil(log2(1/ptr->probabilities[i]));
    }

//actual shannon code for each symbol
    dec2bin(ptr->length, ptr->accumulation, ptr->code);

//generate full shannon code for message
    //string line = contains message 
    for( int i = 0; i < ptr->line.size(); i++ ) {
        char current = ptr->line[i];

        //find code for each character
        for( int j = 0; j < ptr->orderedLetters.size(); j++ ) {
            if(ptr->orderedLetters[j] == current) {
                ptr->encoded = ptr->encoded + ptr->code[j];
                break; //break loop after appending code for symbol
            }
        }
    }


    return ptr;
}


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


    //Read input lines
    std::string line;
    std::vector<Data> lines;

    while(std::getline(std::cin,line)) {
        if(line.empty()) {
            break;
        }
        Data temp;
        temp.line = line;

        lines.push_back(temp);
    }

    //Create n child threads (where n is the number of strings from the input)
    n = lines.size();
    pthread_t* tid = new pthread_t[n];

    for( int i = 0; i < n; i++ ) {
        if(pthread_create(&tid[i], nullptr, freq, (void*)&lines.at(i)) !=0) {
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    }

    for( int i = 0; i < n; i++) {
        pthread_join(tid[i],nullptr);
    }

    simpleData Data1;
    for( int i = 0; i < lines.size(); i++) {
        //MESSAGE
        strncpy(Data1.Message, lines[i].line.c_str(), sizeof(Data1.Message)-1);
        Data1.Message[sizeof(Data1.Message) -1] = '\0';

        Data1.Symbol[0] = '\0';
        n = write(sockfd, &Data1, sizeof(simpleData));
        if( n<0 ) {
            std::cerr << "ERROR writing starting message to socket" << std::endl;
            exit(1);
        }

        for( int j = 0; j < lines[i].orderedLetters.size(); j++) {
            //SYMBOL
            Data1.Symbol[0] = lines[i].orderedLetters[j];
            Data1.Symbol[1] = '\0';

            //FREQUENCY
            Data1.Frequency = lines[i].orderedFrequency[j];

            //SHANNON
            strncpy(Data1.ShannonCode, lines[i].code[j].c_str(), sizeof(Data1.ShannonCode) - 1);
            Data1.ShannonCode[sizeof(Data1.ShannonCode) - 1] = '\0';

            // ENCODED

            n = write(sockfd, &Data1, sizeof(simpleData));
            if (n < 0) {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
        }

        Data1.Symbol[0] = '\0';
        strncpy(Data1.EncodedMessage, lines[i].encoded.c_str(), sizeof(Data1.EncodedMessage) - 1);
        Data1.EncodedMessage[sizeof(Data1.EncodedMessage) - 1] = '\0';

        n=write(sockfd, &Data1, sizeof(simpleData));
        if( n<0 ) {
            std::cerr << "ERROR writing encoded message to socket" << std::endl;
            exit(1);
        }
        
    }

    if(tid!=nullptr){
        delete[] tid;
    }

    // Closing the socket
    close(sockfd);
    return 0;
}
