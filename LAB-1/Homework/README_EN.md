# TCP Socket Programming Homework 1

## Student Information
- **University**: Feng Chia University
- **Class**: IECS 3B (Information Engineering and Computer Science 3B)
- **Student ID**: D1210799
- **Name**: Wang Jian-Wei
- **Course**: Network Programming Design

## File Contents
1. README.md - Plain text documentation file
2. Demo.PNG - Screenshot of program execution
3. Client.c - Client-side program code
4. Server.c - Server-side program code
5. compile.sh - Compilation script (optional)

## Assignment Overview

This assignment implements a client-server communication program based on TCP Socket with the following main features:

1. **Client Side (Client.c)**: 
   - Connects to the server
   - Reads user input from keyboard
   - Converts letters in input string to uppercase
   - Sends data to server and receives processed results

2. **Server Side (Server.c)**:
   - Listens on specified port (5678)
   - Accepts client connections
   - Analyzes received strings, counts letters and numbers
   - Performs Caesar cipher encryption on letters (shift by 1 position)
   - Returns processed results to client

## Program Compilation and Execution

### Compilation Commands
```bash
# Compile server program
gcc -o server Server.c

# Compile client program
gcc -o client Client.c
```

### Execution Steps
1. **Start Server** (execute in terminal):
   ```bash
   ./server
   ```
   The server will start listening on port 5678, waiting for client connections

2. **Start Client** (execute in another terminal):
   ```bash
   ./client
   ```
   The client will connect to the server, then wait for user input

3. **Test Functionality**:
   - Enter any string (containing letters and numbers) in the client terminal
   - Press Enter to send
   - Observe the processing results returned by the server
   - Press Ctrl+D to exit the program

## Detailed Code Explanation

### Client.c Code Analysis

#### Header File Includes
```c
#include <sys/socket.h>   // Socket-related functions
#include <netinet/in.h>   // IPv4 address structure
#include <arpa/inet.h>    // IP/Port conversion functions
#include <unistd.h>       // System calls
#include <stdio.h>        // Standard input/output
#include <string.h>       // String processing
#include <stdlib.h>       // Program control
#include <ctype.h>        // Character processing
```

#### Main Variable Declarations
- `struct sockaddr_in server`: Stores server address information
- `int sock`: Client socket descriptor
- `char buf[256]`: User input buffer
- `char recvbuf[512]`: Buffer for receiving server responses

#### Core Functionality Flow

1. **Socket Creation and Connection**:
   ```c
   sock = socket(AF_INET, SOCK_STREAM, 0);  // Create TCP socket
   connect(sock, (struct sockaddr*)&server, sizeof(server));  // Connect to server
   ```

2. **Data Processing Loop**:
   - Use `fgets()` to read user input
   - Convert letters to uppercase: `toupper(buf[i])`
   - Use `send()` to send data to server
   - Use `recv()` to receive server response
   - Print processing results

### Server.c Code Analysis

#### Main Variable Declarations
- `struct sockaddr_in server, client`: Server and client address structures
- `int sock`: Server socket descriptor
- `int csock`: Client connection socket descriptor
- `char buf[256]`: Buffer for receiving client data

#### Core Functionality Flow

1. **Socket Creation and Binding**:
   ```c
   sock = socket(AF_INET, SOCK_STREAM, 0);  // Create TCP socket
   bind(sock, (struct sockaddr*)&server, sizeof(server));  // Bind to specified port
   listen(sock, 5);  // Start listening, allow up to 5 connections in queue
   ```

2. **Accept Client Connection**:
   ```c
   csock = accept(sock, (struct sockaddr*)&client, &addresssize);
   ```

3. **Data Processing Loop**:
   - Use `recv()` to receive client data
   - Count letters and numbers
   - Perform Caesar cipher encryption on letters (A→B, B→C, ..., Z→A)
   - Use `send()` to return processing results

#### Character Processing Logic
```c
// Count letters and numbers
if (isalpha(ch)) letters++;
else if (isdigit(ch)) numbers++;

// Caesar cipher encryption
char c = toupper(ch) + 1;
if (c > 'Z') c = 'A';  // Circular processing
```

## Network Communication Protocol

### TCP Connection Establishment Process
1. **Three-way Handshake**: Client initiates connection → Server responds → Client confirms
2. **Data Transmission**: Uses reliable TCP protocol to ensure data integrity
3. **Connection Closure**: Client sends EOF or closes socket

### Communication Format
- **Client → Server**: Original string (letters converted to uppercase)
- **Server → Client**: `"letters: X numbers: Y [number list] and [encrypted letter list]"`

## Execution Results

Please refer to the `Demo.PNG` file to view the actual execution result screenshot.

### Example Execution Flow
1. Input: `"Hello123World"`
2. Client processing: `"HELLO123WORLD"`
3. Server response: `"letters: 10 numbers: 3 [1 2 3] and [I F M M P P X P S M E]"`

## Technical Features

### Security Considerations
- Use `bzero()` to initialize structures and avoid memory leaks
- Check return values of all system calls
- Appropriate error handling mechanisms

### Performance Optimization
- Use appropriate buffer sizes
- Avoid unnecessary string copying
- Efficient Caesar cipher implementation

### Programming Patterns
- Modular design with clear separation of Client and Server responsibilities
- Clear error handling flow
- Detailed Chinese comments for explanation

## Important Notes

1. **Port Conflict**: If port 5678 is occupied, please modify the port number in the program
2. **Firewall Settings**: Ensure system firewall allows connections on this port
3. **Compilation Environment**: Requires POSIX-compatible C compiler (such as gcc)
4. **Execution Order**: Must start Server before Client

---

**Development Environment**: Debian 13  
**Compiler**: GCC  
**Protocol**: TCP/IP  
**Port**: 5678
