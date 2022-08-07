# identicon

This program uses a simple XOR cipher to create an encrypted "identicon" based on a given string by generating red, green, and blue values from ASCII characters. It can then decrypt the identicon if it is run with the same XOR_KEY1, XOR_KEY2, and MULT values the identicon was created with.         

Edit the definitions in identicon.h to change the appearance of your identicons. The program works best with input longer than 2 characters. 

In its current state, this program does not provide strong encryption and shouldn't be used to encrypt important information!
