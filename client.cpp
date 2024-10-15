#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <cmath>



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

//main thread reads input
int main() {
    std::string line;
    std::vector<Data> lines;

    Data temp;

    while(std::getline(std::cin, line)) {
        if(line.empty()) {
            break;
        }

        temp.line = line; //stores line in the Data structure 

        lines.push_back(temp); //adds struct instance to vector
    }

    //Create n POSIX threads (where n is the number of input strings)
    int numLines = lines.size();
    pthread_t* tid = new pthread_t[numLines];

//Call pthread_create
    for( int i= 0; i < numLines; i++) {
        if(pthread_create(&tid[i], nullptr, freq, (void*) &lines.at(i)) != 0) {
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    }

//Call pthread_join
    for( int i = 0; i < numLines; i++) {
        pthread_join(tid[i],nullptr);
    }

//Prints the information from the child threads into STDOUT
    for( int i = 0; i < numLines; i++ ) {
        std::cout << "Message: " << lines[i].line << std::endl;
        std::cout << std::endl;
        std::cout << "Alphabet: " << std::endl;
        for(int j = 0; j < lines[i].orderedLetters.size(); j++) {
            std::cout << "Symbol: " << lines[i].orderedLetters[j];
            std::cout << ", Frequency: " << lines[i].orderedFrequency[j];
            std::cout << ", Shannon code: " << lines[i].code[j] << std::endl;
        }
        std::cout<<std::endl;
        std::cout << "Encoded message: " << lines[i].encoded << std::endl;
    }

    if(tid!=nullptr){
        delete[] tid;
    }

    return 0;
}