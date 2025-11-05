#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool isValidIP(char *ip) {
  int num, dots = 0;
  char *ptr;

  if (ip == NULL)
    return false;

  ptr = strtok(ip, ".");
  if (ptr == NULL)
    return false;

  while (ptr) {
    // Check if each octet is a number and within 0-255
    if (!sscanf(ptr, "%d", &num))
      return false;
    if (num < 0 || num > 255)
      return false;

    ptr = strtok(NULL, ".");
    if (ptr != NULL)
      dots++;
  }
  if (dots != 3) // There must be exactly 3 dots
    return false;

  return true;
}

char getClass(int firstOctet) {
  if (firstOctet >= 1 && firstOctet <= 126)
    return 'A';
  else if (firstOctet >= 128 && firstOctet <= 191)
    return 'B';
  else if (firstOctet >= 192 && firstOctet <= 223)
    return 'C';
  else if (firstOctet >= 224 && firstOctet <= 239)
    return 'D'; // Multicast
  else if (firstOctet >= 240 && firstOctet <= 254)
    return 'E'; // Experimental
  else
    return 'I'; // Invalid or special address
}

void printIP(int ip[]) { printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]); }

void calculateNetworkBroadcast(int ip[], int mask[], int network[],
                               int broadcast[]) {
  for (int i = 0; i < 4; i++) {
    network[i] = ip[i] & mask[i];
    broadcast[i] = network[i] | (~mask[i] & 0xFF);
  }
}

int countHosts(int mask[]) {
  // Count number of host bits = count zero bits in mask
  int hostBits = 0;
  for (int i = 0; i < 4; i++) {
    int val = mask[i];
    for (int j = 0; j < 8; j++) {
      if ((val & (1 << j)) == 0)
        hostBits++;
    }
  }
  // Number of hosts = 2^hostBits - 2 (network and broadcast)
  if (hostBits == 0)
    return 0;
  return (1 << hostBits) - 2;
}

void getDefaultMask(char class, int mask[]) {
  switch (class) {
  case 'A':
    mask[0] = 255;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
    break;
  case 'B':
    mask[0] = 255;
    mask[1] = 255;
    mask[2] = 0;
    mask[3] = 0;
    break;
  case 'C':
    mask[0] = 255;
    mask[1] = 255;
    mask[2] = 255;
    mask[3] = 0;
    break;
  default:
    mask[0] = 0;
    mask[1] = 0;
    mask[2] = 0;
    mask[3] = 0;
    break;
  }
}

int main() {
  char input[20];
  char ipCopy[20]; // Copy for validation because strtok modifies string
  int ip[4];

  printf("Enter IP address: ");
  scanf("%19s", input);
  strcpy(ipCopy, input);

  if (!isValidIP(ipCopy)) {
    printf("Invalid IP address format.\n");
    return 1;
  }

  // Parse IP address again since strtok modifies string
  sscanf(input, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);

  char class = getClass(ip[0]);

  if (class == 'I') {
    printf("Invalid or special IP address.\n");
    return 1;
  }

  printf("IP Address: %s\n", input);
  printf("Class: %c\n", class);

  int mask[4];
  getDefaultMask(class, mask);

  printf("Default Subnet Mask: ");
  printIP(mask);
  printf("\n");

  int hosts = countHosts(mask);
  printf("Number of valid hosts per subnet: %d\n", hosts);

  int network[4], broadcast[4];
  calculateNetworkBroadcast(ip, mask, network, broadcast);

  printf("Network Address: ");
  printIP(network);
  printf("\n");

  printf("Broadcast Address: ");
  printIP(broadcast);
  printf("\n");

  return 0;
}
