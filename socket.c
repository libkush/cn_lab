#include <arpa/inet.h> // For sockaddr_in, inet_addr()
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // For socket functions
#include <unistd.h>     // For close()

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];
  int bytes_read;

  // Step 1: Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
  if (server_fd == -1) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully.\n");

  // Step 2: Bind socket to IP / port
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Socket bound to %s:%d\n", "127.0.0.1", PORT);

  // Step 3: Listen for connections
  if (listen(server_fd, 3) < 0) {
    perror("listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Listening for incoming connections...\n");

  // Step 4: Accept a client connection
  client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    perror("accept failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr),
         ntohs(client_addr.sin_port));

  // Step 5: Communicate with client
  while ((bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[bytes_read] = '\0'; // Null terminate the string
    printf("Received from client: %s\n", buffer);

    // Echo back to client
    if (send(client_fd, buffer, bytes_read, 0) < 0) {
      perror("send failed");
      break;
    }
  }

  if (bytes_read == 0)
    printf("Client disconnected.\n");
  else if (bytes_read < 0)
    perror("recv failed");

  // Step 6: Close client socket
  close(client_fd);
  printf("Closed connection with client.\n");

  // Step 7: Close server socket
  close(server_fd);
  printf("Server socket closed. Exiting.\n");

  return 0;
}
