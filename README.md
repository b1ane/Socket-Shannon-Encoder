# Socket-Shannon-Encoder

Objective:
This assignment will introduce you to interprocess communication mechanisms in UNIX using sockets.
 
Problem:
You must write two programs to implement a distributed version of the program to determine the Shannon codes you wrote for programming assignment 1. 

These programs are:

The server program:
The user will execute this program using the following syntax:
 
./exec_filename port_no
 
where exec_filename is the name of your executable file, and port_no is the port number to create the socket. The port number will be available to the server program as a command-line argument.
 
The server program does not receive any information from STDIN and does not print any messages to STDOUT.
 
The server program executes the following task:
Receive multiple requests from the client program using sockets. Therefore, the server program creates a child process per request to handle these requests simultaneously. For this reason, the parent process must handle zombie processes by implementing the fireman() function call (unless you can determine the number of requests the server program receives from the client program). 
Each child process executes the following tasks:
Receive the input string with the message to encode.
Create the alphabet based on the received input string.
Determine the Shannon codes for the symbols in the alphabet. 
Generate the encoded message by using the Shannon codes.
Send the alphabet (with the Shannon codes) and the encoded message to the client program.
The client program:
 
The user will execute this program using the following syntax:
 
./exec_filename hostname port_no < input_filename
 
where exec_filename is the name of your executable file, hostname is the address where the server program is located, port_no is the port number used by the server program, and input_filename is the name of the input file. The hostname and the port number will be available to the client as command-line arguments.
The client program receives n lines from STDIN (using input redirection) (where n is the number of input strings). Each line from the input represents an input message to encode.
 
Example Input:

COSC 3360 COSC 1437
COSC 1336 COSC 2436
COSC 3320 COSC 3380
After reading the information from STDIN, the program must create n child threads (where n is the number of strings from the input). Each child thread executes the following tasks: 
Receive the string with the input message to encode from the main thread.
Create a socket to communicate with the server program.
Send the input message to encode to the server program using sockets. 
Wait for the alphabet (with the Shannon codes) and the encoded message from the server program.
Write the received information into a memory location accessible by the main thread.
Finally, after receiving the alphabet and the encoded message, the main thread prints this information to STDOUT. Given the previous input, the expected output is:
 
Message: COSC 3360 COSC 1437

Alphabet: 
Symbol: C, Frequency: 4, Shannon code: 000
Symbol: 3, Frequency: 3, Shannon code: 001
Symbol:  , Frequency: 3, Shannon code: 010
Symbol: S, Frequency: 2, Shannon code: 1000
Symbol: O, Frequency: 2, Shannon code: 1010
Symbol: 7, Frequency: 1, Shannon code: 10111
Symbol: 6, Frequency: 1, Shannon code: 11001
Symbol: 4, Frequency: 1, Shannon code: 11010
Symbol: 1, Frequency: 1, Shannon code: 11100
Symbol: 0, Frequency: 1, Shannon code: 11110

Encoded message: 00010101000000010001001110011111001000010101000000010111001101000110111

Message: COSC 1336 COSC 2436

Alphabet: 
Symbol: C, Frequency: 4, Shannon code: 000
Symbol: 3, Frequency: 3, Shannon code: 001
Symbol:  , Frequency: 3, Shannon code: 010
Symbol: S, Frequency: 2, Shannon code: 1000
Symbol: O, Frequency: 2, Shannon code: 1010
Symbol: 6, Frequency: 2, Shannon code: 1011
Symbol: 4, Frequency: 1, Shannon code: 11010
Symbol: 2, Frequency: 1, Shannon code: 11100
Symbol: 1, Frequency: 1, Shannon code: 11110

Encoded message: 000101010000000101111000100110110100001010100000001011100110100011011

Message: COSC 3320 COSC 3380

Alphabet: 
Symbol: C, Frequency: 4, Shannon code: 000
Symbol: 3, Frequency: 4, Shannon code: 001
Symbol:  , Frequency: 3, Shannon code: 011
Symbol: S, Frequency: 2, Shannon code: 1001
Symbol: O, Frequency: 2, Shannon code: 1010
Symbol: 0, Frequency: 2, Shannon code: 1100
Symbol: 8, Frequency: 1, Shannon code: 11100
Symbol: 2, Frequency: 1, Shannon code: 11110

Encoded message: 0001010100100001100100111110110001100010101001000011001001111001100
