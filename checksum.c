#include <stdio.h>

int main(void) {
    unsigned int seg[8];
    printf("Enter 8 4-bit numbers (0-15) separated by spaces:\n");

    for (int i = 0; i < 8; ++i) {
        if (scanf("%u", &seg[i]) != 1) {
            fprintf(stderr, "Input error\n");
            return 1;
        }
        if (seg[i] > 15) {
            fprintf(stderr, "Value out of range (must be 0..15): %u\n", seg[i]);
            return 1;
        }
    }

    unsigned int sum = 0;
    for (int i = 0; i < 8; ++i) sum += seg[i];

    /* Modulo-16 checksum: keep only the lowest 4 bits */
    unsigned int checksum_mod16 = sum & 0xF;

    /* One's-complement 4-bit checksum (optional) */
    unsigned int checksum_ones = (~checksum_mod16) & 0xF;

    printf("Sum = %u (0x%X)\n", sum, sum);
    printf("4-bit checksum (sum mod 16) = 0x%X (%u)\n", checksum_mod16, checksum_mod16);
    printf("4-bit one's-complement checksum = 0x%X (%u)\n", checksum_ones, checksum_ones);

    return 0;
}
