/**
 * Hashing function, which takes a string and a hash function as arguments,
 * and will return a string, the hash digest
 * 
 * @param text text to hash
 * @param hash_f hash function to use
 * @return unsigned char*, ``digest`` 
 */
unsigned char* hash(char* text, char* hash_f);

/**
 * Function to convert a file's data into a string, which can be used as input for the hash function
 * a file is inputted and a string is returned
 * 
 * @param filename file to extract content from
 * @return char* file content
 */
char* convert_f_to_s(char* filename);
