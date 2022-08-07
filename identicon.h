#ifndef IDENTICON_H
#define IDENTICON_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#define ROW_SIZE 54

// THE FOLLOWING DEFINITIONS CAN BE ADJUSTED BY USER
// Arbitrary keys to xor each character by 
#define XOR_KEY1 'X'
#define XOR_KEY2 'b'
// Intensity of red; must be between 1 and 127
#define RED_MULT 127
// Intensity of green; must be between 1 and 28
#define GREEN_MULT 28
// Intensity of blue; must be between 1 and 28
#define BLUE_MULT 28

/* General function for encryption */
void encrypt();
/* General function for decryption */
void decrypt();
/* Get name of input or output file */
char* get_filename();
/* For encrypt function - get all necessary inputs */
char* get_input(char to_encrypt[], int* length);
/* Encrypt the length of input */
void encrypt_length(int lengthcolor[], uint8_t encrypted_length, FILE *fp);
/* Write encrypted data to file */
void write_encrypt(int lengthcolor[], uint8_t encrypted[], FILE *fp);
/* For decrypt function - read input file */
int read_file(FILE *fp, int encrypted[]);

#endif
