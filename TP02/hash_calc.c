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

    // Initialise our hash digest string
    char* hash_digest;


    // Check if a text was given. If not, inform the user that there is no text.
    if (text == NULL) {
        printf("No text given.");
        return "";
    }


    // Check if a hash function was given. If yes, use it. If not, use SHA1 as our hash function.
    if (hash_f == NULL) {
        md = "SHA1";
    }

    else {
        md = EVP_get_digestbyname(hash_f);
    }
    

    // Code taken from the openssl example, modified for the program
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, text, strlen(text));

    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++) {
        hash_digest[i] = md_value[i];
        // printf("%02x", hash_digest[i]);
    }

    return hash_digest;

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
        strcat(buffer, converted_string);
    }

    fclose(fichier);

}
// ------------------------------------------------------------------------------------------------------------------------------------
