#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <errno.h>

EVP_MD_CTX* mdctx;
const EVP_MD* md;

unsigned char md_value[EVP_MAX_MD_SIZE];
unsigned int md_len, i;

// ------------------------------------------------------------------------------------------------------------------------------------
// Hashing function, which takes a string and a hash function as arguments, and will return a string, the hash digest
unsigned char* hash(char* text, char* hash_f) {

    md = EVP_get_digestbyname(hash_f);
    
    // Code taken from the openssl example, modified for this program
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, text, strlen(text));

    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    for (i = 0; i < md_len; i++) printf("%02x", md_value[i]);
    //printf("\n");

    return md_value;
}


// ------------------------------------------------------------------------------------------------------------------------------------
// Function to convert a file's data into a string, which can be used as input for the hash function
// a file is inputted and a string is returned
char* convert_f_to_s(char* filename) {

    // Set up variables and pointers
    FILE* fichier;
    char* ligne;
    char* converted_string;
    size_t c_s_len = 1;
    converted_string = (char*) malloc(sizeof(char*) * c_s_len);
    size_t len = 0;
    ssize_t nread;

    // Check if filename was given
    if (strcmp(filename, "") == 0) {
        fprintf(stderr, "Filename was not entered\n");
        errno = EINVAL;
        return "";
    }

    // Open file given
    fichier = fopen(filename, "r");

    // Check if the file is empty
    if (fichier == NULL) {
        fprintf(stderr, "File could not be opened\n");
        errno = ENOENT;
        return "";
    }

    // Read each line, and concatenate it to the converted_string
    while ((nread = getline(&ligne, &len, fichier)) != -1) {
            converted_string = (char *) realloc(converted_string, c_s_len + len);
            strcat(converted_string, ligne);
           }


    free(ligne);
    fclose(fichier);

    return converted_string;
}

