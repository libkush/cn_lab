#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

int main() {
  int sock_fd;
  struct sockaddr_in server_addr;
  char buffer[1024];
  int n, bytes_read;

  // Create socket
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Server details
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Connect to server
  if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("connect failed");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to server at 127.0.0.1:%d\n", PORT);

  // Take input from user
  printf("Enter the number of even numbers to sum: ");
  scanf("%d", &n);

  // Send 'n' to server
  snprintf(buffer, sizeof(buffer), "%d", n);
  send(sock_fd, buffer, strlen(buffer), 0);

  // Receive sum from server
  bytes_read = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes_read <= 0) {
    perror("recv failed or server disconnected");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }
  buffer[bytes_read] = '\0';

  printf("Sum received from server: %s\n", buffer);

  // Close socket
  close(sock_fd);

  return 0;
}
