#ifndef INP
#define INP


/**
 *  Representation of a user input, i.e. wraps these:
 * 
 * -- 1. Command (1 char) ’g’ (F_GETLK), ’s’ (F_SETLK), or ’w’ (F_SETLKW)
 *
 * -- 2. Lock type (1 char) ’r’ (F_RDLCK), ’w’ (F_WRLCK), or ’u’ (F_UNLCK)
 * 
 * -- 3. start (int)
 * -- 4. length (int)
 * 
 * -- 5. whence (1 char) 's’ (SEEK_SET , default), ’c’ (SEEK_CUR), or ’e’ (SEEK_END)
 * 
 * Use the getters inp_* (inp_cmd...) to access to the command, lock type... (i.e. attribute of this struct.)
 */
typedef struct Inp Inp;

/**
 * "Constructor" (kind of) of 'Inp' structs.
 * Asks user for input and initialize the fields of the struct's instance accordingly.
 * See documentation of 'Inp' struct for more details on what is asked as input.
 * 
 * @return Newly created instance of Inp* corresponding to the user response.
 * Or NULL if some error happened.
 */
Inp* inp_askUser();

/**
 * Getter for the 'cmd' (g, s or w)
 * @param input Inp*, Pointer to Inp struct from which to retrieve the command
 * @return (copy ?) of command
 */
char inp_cmd(Inp* input); 

/**
 * Getter for the 'ltype' (g, s or w)
 * @param input Inp*, Pointer to Inp struct from which to retrieve the lock type
 * @return (copy ?) lock type
 */
char inp_ltp(Inp* input);

/**
 * Getter for the 'whence' (g, s or w)
 * @param input Inp*, Pointer to Inp struct from which to retrieve the whence param
 * @return whence (referential from wich the 'start' and 'end')
*/
char inp_whc(Inp* input);


/**
 * Getter for the 'start' 
 * @param input Inp*, Pointer to Inp struct from which to retrieve the start of lock location
 * @return 'start' attribute
*/
long inp_start(Inp* input);

/**
 * Getter for the 'length' 
 * @param input Inp*, Pointer to Inp struct from which to retrieve the length of lock "location"
 * @return 'length' attribute
*/
long inp_length(Inp* input);

/**
 * Prints attributes of given 'Inp' struct 'inp'.
 * 
 * @param inp The input to be printed.
 */
void inp_print(Inp* inp);

/**
 * Frees the memory allocated for the properties of the input
 * 
 * @param inp The Inp struct to free.
 */
void inp_free(Inp* inp);

/**
 * Prints process id in specific manner
 * @param in if ('in') then prints formatted as waiting output (i.e. PID=%d> ), else as printing ouput (i.e. [PID=%d] )
 */
void printPid(int in);


/**
 * Converts the cmd character in the input to the corresponding cmd value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding cmd value for the flock function
 */
int toFlock_cmd(Inp* inp);


/**
 * Converts the l_type character in the input to the corresponding l_type value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding l_type value for the flock function
 */
int toFlock_ltype(Inp* inp);


/**
 * Converts the whence character in the input to the corresponding whence value for the flock
 * function
 * 
 * @param inp The input struct
 * 
 * @return Corresponding whence value for the flock function
 */
int toFlock_whence(Inp* inp);

#endif /* INP */