#ifndef INPUT_H
#define INPUT_H

/**
 * 
 * Reads user input from stdin and returns it as a string (stripped i.e. without left and right whitespace). 
 * 
 * @return User input or null if nothing could be read
 */
const char* readInput();

 
/**
 * Reads user input from stdin, then parses it (if it was correctly read)
 * and stores the argc into the given 'argc' variable.
 * 
 * @param argc ptr to variable in which to store the number of argument
 * @param isForeground ptr to variable in which to store if the line contained a '&' at the end

 * @return Array of argument
 */
const char** readParseIn(int* argc, int* isForeground);

/**
 * @return prompt global variable
 */
const char* getPrompt();


#endif /* INPUT_H */
