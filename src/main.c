#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	//Uncomment this block to pass the first stage
	
	// server_fd is the file descriptor for the socket we're creating.
	// A socket is a endpoint for communication between two devices (computer, phone, etc) in a network.
	// client_addr is a struct that will store the address of the client that connects to our server.
	// client_addr_len is the size of the struct, so we can pass it to functions that need it.
	int server_fd;
	socklen_t client_addr_len;
	struct sockaddr_in client_addr;
	
	// Create a socket using the IPv4 address family and the TCP protocol.
	// The file descriptor for this socket is stored in server_fd.
	// If the socket creation fails, the file descriptor is -1.
	// We check this and print an error message if it fails.
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}
	
	// We fill in the serv_addr struct with the details of our server.
	// serv_addr will be used to bind the socket to a specific address and port.
	// AF_INET is the address family for IPv4.
	// htons(4221) converts the port number from host byte order to network byte order.
	// INADDR_ANY means the server will listen on all available network interfaces.
	// This is useful for testing since we don't know the IP address of the machine
	// that the tester will run your program on.
	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(4221),
		.sin_addr = { htonl(INADDR_ANY) },
	};
	
	// Bind the socket to the address and port specified in serv_addr.
	// The bind function returns 0 on success, and -1 on failure.
	// If it fails, we print an error message and return 1.
	// The error message is obtained with strerror(errno).
	// errno is a global variable that stores the error number of the last failed system call.
	// strerror converts this error number to a string.
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	
	// Set the maximum length to which the queue of pending connections may grow.
    int connection_backlog = 5;
    // Listen for incoming connections on the socket.
    // The second parameter specifies the maximum length of the pending connections queue.
    if (listen(server_fd, connection_backlog) != 0) {
        // If listen fails, print an error message with the reason for the failure.
        printf("Listen failed: %s \n", strerror(errno));
        return 1;
    }
	
	// Print a message to let the user know that the server is waiting for a client to connect.
	printf("Waiting for a client to connect...\n");
	
	// Set the size of the client address struct, which is needed for the accept function.
	// The accept function populates the client_addr struct with the address of the client that connected.
	client_addr_len = sizeof(client_addr);
	
	// The accept function waits for a client to connect to the socket, and
    // returns a new file descriptor that can be used to communicate with the client.
    // The new file descriptor is stored in accept_fd.
    // The client_addr struct is populated with the address of the client that connected.
    // The client_addr_len is a pointer to the size of the client_addr struct.
    // It is updated to the size of the address actually stored in client_addr.
    int accept_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
    // If accept fails, print an error message with the reason for the failure.
	char *reply = "HTTP/1.1 200 OK\r\n\r\n";
	int bytes_send = send(accept_fd, reply, strlen(reply), 0);
    
	if (accept_fd < 0) {
        printf("Accept failed: %s \n", strerror(errno));
        return 1;
    }
	// char* fd =  
	// printf("Client connected\n");
	
	close(server_fd);

	return 0;
}
