#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <errno.h>
#include <fcntl.h> //read
#include <unistd.h>

#include "OptionParser.h"

EVP_MD_CTX* mdctx;
const EVP_MD* md;

unsigned char md_value[EVP_MAX_MD_SIZE];
unsigned int md_len, i;

// ------------------------------------------------------------------------------------------------------------------------------------

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

char* convert_f_to_s(char* filename) {
    int fd, initSize = 4096;
    char buff[initSize];  // size of a block
    ssize_t readNb;   // number of bytes read
    size_t readTotNb = 0;
    char* bBuff = tryalc(malloc(initSize)); //big buffer which will contain all file and be realloc'd at each iteration


    // Error handling if unable to open source in read-only or the destination in write-only, create or excl (error if create but file already exists)
    if ((fd = open(filename, O_RDONLY)) < 0) {
        int savedErr = errno;
        fprintf(stderr, "convert_f_to_s: cannot open file %s : %s\n", filename, strerror(savedErr));
        return NULL;
    }
    
    //* While there are bytes left to be read, reads them 4096 by 4096 (4096 or current size of 'buff' if its not that)
    //* nb: if readNb is < to what we expected we dont really care because the program will retry until having read everything
    while ((readNb = read(fd, buff, sizeof buff)) > 0) {
        char* crtToCopy = &buff[readTotNb];  // pointer to start of current 'portion of data' to copy
        readTotNb += readNb;
        //update allocated memory to the size of whats been read until now.
        bBuff = realloc(bBuff, (readTotNb+1)*sizeof(char));
        //copy at most readNb bytes fileToRead buff to bBuff i.e. append current content of the small buffer 'buff'.
        strncat(bBuff, crtToCopy, readNb);
    }
    if (readNb < 0) {
        int savedErr = errno;
        fprintf(stderr, "convert_f_to_s: cannot read file %s : %s\n", filename, strerror(savedErr));
        return NULL;
    }

    if (close(fd) < 0) {
        int savedErr = errno;
        fprintf(stderr, "convert_f_to_s: cannot close file %s : %s\n", filename, strerror(savedErr));
        exit(EXIT_FAILURE);
        return NULL;
    }

    return bBuff;
}

