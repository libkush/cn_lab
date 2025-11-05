#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

// Function to calculate sum of first n even numbers
int sum_even_numbers(int n) {
  // Sum of first n even numbers = n * (n + 1)
  return n * (n + 1);
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[1024];
  int n, sum;
  int bytes_read;

  // Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Bind socket to localhost and PORT
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Server listening on 127.0.0.1:%d\n", PORT);

  // Accept client connection
  client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    perror("accept failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr),
         ntohs(client_addr.sin_port));

  // Receive 'n' from client
  bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes_read <= 0) {
    perror("recv failed or client disconnected");
    close(client_fd);
    close(server_fd);
    exit(EXIT_FAILURE);
  }
  buffer[bytes_read] = '\0';

  // Convert received string to integer
  n = atoi(buffer);
  printf("Received n = %d from client.\n", n);

  // Compute sum
  sum = sum_even_numbers(n);
  printf("Sum of first %d even numbers = %d\n", n, sum);

  // Send the sum back to client as string (optional)
  snprintf(buffer, sizeof(buffer), "%d", sum);
  send(client_fd, buffer, strlen(buffer), 0);

  // Close sockets
  close(client_fd);
  close(server_fd);

  printf("Server exiting.\n");
  return 0;
}
