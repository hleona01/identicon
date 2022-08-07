/****************************** IDENTICON.C *********************************/
/* This program uses a simple XOR cipher to create an encrypted "identicon" */
/* based on a given string by generating red, green, and blue values from   */
/* ASCII characters. It can then decrypt the identicon if it is run with    */
/* the same XOR_KEY1, XOR_KEY2, and MULT values the identicon was created   */
/* with.                                                                    */
/*                                                                          */
/* Edit the definitions in this file to change the appearance of your       */
/* identicons. The program works best with input longer than 2 characters.  */
/*                                                                          */
/* In its current state, this program does not provide strong encryption    */
/* and shouldn't be used to encrypt important information!                  */
/****************************************************************************/
#include "identicon.h"

int main() {
    char input[50];

    printf("Enter 'e' to encrypt and 'd' to decrypt: ");
    scanf("%50s", input);  
    if (strcmp (input, "d\0\n") == 0) {
        decrypt();
    }
    else if (strcmp (input, "e\0\n") == 0) {
        encrypt();
    }
    else {
        printf("Please enter a valid command.\n");
        exit(1);
    }
    return 0;
}

char * get_filename() {
    char shortname[257];
    printf("Enter the name of a file to encrypt to or decrypt from (without .ppm extension): ");
    scanf("%256s", shortname);
    const int filenamelength = strlen(shortname);
    // Add 5 for null char and '.ppm'
    char *full_file = malloc(filenamelength + 5);
    strcpy(full_file, shortname);
    full_file[filenamelength] = '.';
    full_file[filenamelength + 1] = 'p';
    full_file[filenamelength + 2] = 'p';
    full_file[filenamelength + 3] = 'm';
    full_file[filenamelength + 4] = '\0';
    return full_file;
}

char * get_input(char to_encrypt[], int * length) {
    char input_string[257];
    printf("Enter a string of 256 or fewer characters: ");
    scanf("%256s", input_string);
    *length = strlen(input_string);

    // Copy input string into char array before encrypting to account
    // for inputs shorter than 255 chars

    // j tracks location in input string
    int j = 0;
    for (int i = 0; i < 256; i++) {
        to_encrypt[i] = input_string[j];
        j++;
        // If you've reached the end of the input length,
        // just keep repeating input
        if (j == *length) {
            j = 0;
        }
    }
    to_encrypt[256] = '\0'; 
    
    // Get name of output file
    return get_filename();
}

void encrypt_length(int lengthcolor[], uint8_t encrypted_length, FILE *fp) {
    // Calculate red
    lengthcolor[0] = (encrypted_length/ 100) * RED_MULT;
    // Calculate green
    lengthcolor[1] = ((encrypted_length % 100) / 10) * GREEN_MULT;
    // Calculate blue
    lengthcolor[2] = (encrypted_length % 10) * BLUE_MULT;

    // 3 header rows and 16 content rows
    char *first = "P3\n18 18\n256\n";
    fprintf(fp, "%s", first);
    // Write border to file 
    for (int j = 0; j < ROW_SIZE; j++) {
        fprintf(fp, "%d ", lengthcolor[j % 3]);
    }
    fprintf(fp, "\n");
    return;
}

void write_encrypt(int lengthcolor[], uint8_t encrypted[], FILE *fp) {
   /* Each pixel is made up of 3 consecutive array indices representing */
    /* the colors red, green, and blue.                                  */
    /* Each letter is translated into a pixel by:                        */
    /*    determining red from hundreds place                            */
    /*    determining green from tens place                              */
    /*    determining blue from ones place                               */

    // Column tracker for each row
    int col_tracker = 0;
    // Number of values in each row, including border
    int row[ROW_SIZE];
    // Write row by row
    for (int i = 0; i < 256; i++) {
        // If we're on the left side of the image,
        // create a one-pixel border based on length
        if (i % 16 == 0) {
            row[0] = lengthcolor[0];
            row[1] = lengthcolor[1];
            row[2]  = lengthcolor[2];
            col_tracker = 3;
        }
        // Get red
        int mycurr = (encrypted[i]/ 100) * RED_MULT;
        row[col_tracker] = mycurr;
        col_tracker++;
        // Get green
        mycurr = ((encrypted[i] % 100) / 10) * GREEN_MULT;
        row[col_tracker] = mycurr;
        col_tracker++;
        // Get blue
        mycurr = (encrypted[i] % 10) * BLUE_MULT;
        row[col_tracker] = mycurr;
        col_tracker++;
        // If the row is finished
        if ((i+1) % 16 == 0) {
            // Add border
            row[col_tracker] = lengthcolor[0];
            row[col_tracker + 1] = lengthcolor[1];
            row[col_tracker + 2]  = lengthcolor[2];
            // Write to file 
            for (int j = 0; j < ROW_SIZE; j++) {
                fprintf(fp, "%d ", row[j]);
            }
            fprintf(fp, "\n");
            // Clear array and reset tracker
            col_tracker = 0;
            memset(row, 0, ROW_SIZE);
        }
    }

    // Write border on bottom
    for (int j = 0; j < ROW_SIZE; j++) {
        fprintf(fp, "%d ", lengthcolor[j % 3]);
    }
}

void encrypt() {
    // input_string: stores user input.
    // to_encrypt: stores characters that will be encrypted.
    // output_file: stores the name of the file to print ppm data to.
    // encrypted: stores encrypted characters.
    char to_encrypt[257];
    uint8_t encrypted[257];
    int length;
    char * outfile = get_input(to_encrypt, &length);

    uint8_t location = 0;
    for (int i = 0; i < 256; i++) {
        // Move char to the index of its XORed location
        location = i ^ XOR_KEY1;
        // XOR char's value by XOR_KEY2 
        encrypted[location] = (to_encrypt[i] ^ XOR_KEY2);
    }
    // Encrypt length
    uint8_t encrypted_length = length ^ XOR_KEY2;

    // Make ppm
    FILE *fp;
    fp = fopen(outfile, "w");

    // Translate length to RGB
    int lengthcolor[3];
    encrypt_length(lengthcolor, encrypted_length, fp);
    write_encrypt(lengthcolor, encrypted, fp);
    fclose(fp);
    free(outfile);
    printf("Successfully encrypted to %s.\n", outfile);
}


int read_file(FILE *fp, int encrypted[]) {
    int tracker = 0;
    char trash[5];
    char str1[5];
    // Skip the header
    fscanf(fp, "%s %s %s %s", trash, trash, trash, trash);

    while (!feof(fp)) {
        fscanf(fp, "%s", str1);
        encrypted[tracker] = atoi(str1);
        tracker++;
        if (feof(fp)) {
            break ;
        }
    }

    int length;
    // Get length
    length = (encrypted[0] / RED_MULT) * 100;
    length = length + (encrypted[1] / GREEN_MULT) * 10;
    length = length + (encrypted[2] / BLUE_MULT);
    length = length ^ XOR_KEY2;
    return length;
}

void decrypt_message(char decrypted_message[], int encrypted[], int length) {
    // Decrypt message
    for (int i = 0; i < length; i++) {
        int orig_location = i ^ XOR_KEY1;
        int row = orig_location / 16; // 0-indexed
        int location = (orig_location*3) + ROW_SIZE + (row*6) + 3;
        int decrypted_char = (encrypted[location] / RED_MULT) * 100 +
                        (encrypted[location + 1] / GREEN_MULT) * 10 +
                        (encrypted[location + 2] / BLUE_MULT);
        decrypted_char = decrypted_char ^ XOR_KEY2;
        decrypted_message[i] = decrypted_char;
    }
}

void decrypt() {
    // Account for each character in the file:
    // 18 rows x 18 columns x 3 numbers/pixel
    int encrypted[973];
    // Get name of file
    char * outfile = get_filename();
    FILE *fp = fopen(outfile, "r");

    if (fp == NULL) {
        fprintf(stderr, "File does not exist.\n");
        exit(1);
    }

    int length = read_file(fp, encrypted);
    char decrypted_message[length + 1];
    decrypted_message[length] = '\0';

    decrypt_message(decrypted_message, encrypted, length);
    printf("Decrypted: %s\n", decrypted_message);
    free(outfile);
}
