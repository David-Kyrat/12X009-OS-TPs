#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>


EVP_MD_CTX* mdctx;
const EVP_MD* md;

unsigned char md_value[EVP_MAX_MD_SIZE];
unsigned int md_len, i;

// ------------------------------------------------------------------------------------------------------------------------------------
// Hashing function, which takes a string and a hash function as arguments, and will return a string, the hash digest

char* hash(char* text, char* hash_f) {

    md = EVP_get_digestbyname(hash_f);
    
    // Code taken from the openssl example, modified for this program
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, text, strlen(text));

    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        printf("%02x", md_value[i]);
    }

    return md_value;

}
// ------------------------------------------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------------------------------------------
// Function to convert a file's data into a string, which can be used as input for the hash function
// a file is inputted and a string is returned

char* convert_f_to_s(char* filename) {

    char* converted_string = malloc(sizeof(char));
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

    while (fgets(buffer, sizeof(buffer), fichier) != NULL)
    {
        strcat(converted_string, buffer);
    }


    fclose(fichier);

    return converted_string;

}
// ------------------------------------------------------------------------------------------------------------------------------------
