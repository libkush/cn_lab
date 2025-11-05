#include <stdio.h>

#define DATA_LEN 4
#define DIV_LEN 4

void crc_generate(int data[], int divisor[], int crc[]) {
  int temp[DATA_LEN + DIV_LEN - 1];
  int i, j;

  for (i = 0; i < DATA_LEN; i++) {
    temp[i] = data[i];
  }

  for (i = DATA_LEN; i < DATA_LEN + DIV_LEN - 1; i++) {
    temp[i] = 0;
  }

  for (i = 0; i < DATA_LEN; i++) {
    if (temp[i] == 1) {
      for (j = 0; j < DIV_LEN; j++) {
        temp[i + j] = temp[i + j] ^ divisor[j];
      }
    }
  }

  for (i = 0; i < DATA_LEN; i++) {
    crc[i] = temp[DATA_LEN + i];
  }
}

int main() {
  int data[DATA_LEN], divisor[DIV_LEN], crc[DIV_LEN - 1];
  int i;

  printf("Enter 4-bit data word (space-separated): ");
  for (i = 0; i < DATA_LEN; i++) {
    scanf("%d", &data[i]);
    if (data[i] != 0 && data[i] != 1) {
      printf("Invalid data word!\n");
      return 1;
    }
  }

  printf("Enter 4-bit divisor word (space-separated): ");
  for (i = 0; i < DIV_LEN; i++) {
    scanf("%d", &divisor[i]);
    if (divisor[i] != 0 && divisor[i] != 1) {
      printf("Invalid divisor word!\n");
      return 1;
    }
  }

  crc_generate(data, divisor, crc);

  printf("CRC: ");
  for (i = 0; i < DIV_LEN - 1; i++) {
    printf("%d", crc[i]);
  }
  printf("\n");

  return 0;
}
