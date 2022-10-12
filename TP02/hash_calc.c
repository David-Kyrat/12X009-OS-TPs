#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>


EVP_MD_CTX* mdctx;
const EVP_MD* md;

char test_mess[] = "Le manuel disait: Nécessite Windows 7 ou mieux. J'ai donc installé Linux\n";
unsigned char md_value[EVP_MAX_MD_SIZE];
unsigned int md_len, i;

// ------------------------------------------------------------------------------------------------------------------------------------
// Hashing function, which takes a string and a hash function as arguments, and will return a string, the hash digest

char* hash() {

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, test_mess, strlen(test_mess));

    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    printf("Digest is: \"");
    for (i = 0; i < md_len; i++) printf("%02x", md_value[i]);
    printf("\"\n");

}
// ------------------------------------------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------------------------------------------
// Function to convert a file's data into a string, which can be used as input for the hash function
// a file is inputted and a string is returned

char* convert_f_to_s(char* filename) {

    char* converted_string;
    char buffer[100];

    if (filename == "") {
        printf("Le nom du fichier n'a pas été rentré.");
        return "";
    }

    FILE *fichier = fopen(filename, "r");

    if (fichier == NULL) {
        printf("Le fichier n'a pas pu etre ouvert.");
        return "";
    }

    // fgets here reads an entire line or 99 characters (+1 for \0) at a time, whichever comes first
    while (fgets(buffer, sizeof(buffer), fichier) != NULL) {
        printf("Line read = %s\n", buffer);
    }

    fclose(fichier);

}
// ------------------------------------------------------------------------------------------------------------------------------------
