// Hashing function, which takes a string and a hash function as arguments, and will return a string, the hash digest
unsigned char* hash(char* text, char* hash_f);

// Function to convert a file's data into a string, which can be used as input for the hash function a file is inputted and a string is returned
char* convert_f_to_s(char* filename);
