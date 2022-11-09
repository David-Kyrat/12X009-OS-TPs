
/**
 * Returns true if the given string is either "." or ".."
 * 
 * @param entry_name The name of the file or directory.
 *
 * @return the value of the comparison of the entry_name and the string ".." or the entry_name and the
 * string ".".
 */
int isDot(const char *entry_name);

/**
 * Takes the mode (int) and dtype (int) of an inode and returns a string representing the permissions of the file
 * @param mode the mode of the file,
 * @return Permissions of the file, as a String. (i.e. -rw-r--r-x ...)
 */
//char* computePerm(int mode);

/**
 * Checks if a file exists
 * @param path The filepath to check.
 * @return Positive Non-zero value if exists, 0 if it doesnt, -1 if error
 */
int exists(const char* path);


/**
 * Checks if a directory entry at given path is a regular file.
 * @param path path to entry to check
 * @param lstat if set at 0 (false), will use lstat for the check instead of stat. 
 *              i.e. would return true if path points to a file or a link to file
 * @return int 0 if not a regular file 1 otherwise
 */
int isFile(const char* path, int lstat);

/**
 * Checks if given path is a Directory
 * @param path path to entry to check
 * @return int 0 if not a directory, non-zero otherwise
 */
int isDir(const char* path);

/**
 * Takes a parent path and a current path and concatenates them into a buffer. 
 * 
 * @param buf The buffer to write the path to.
 * @param parent The parent directory
 * @param current the current directory
 * 
 * @return 0 if success or -1 if path got too long (sets errno = ENAMETOOLONG).
 */
int concat_path(char buf[], const char* parent, const char* current);


/**
 * Computes actual name of a file.
 * @param path path to file to relove name from.
 * @return char* Actual resolved name of file located at path. (Use real path to get the absolute path, then extract the name).
 */
const char* getFileName(const char* path);

/**
 * @brief Get the File Size of entry at given path
 * 
 * @param path, path to file to check size of
 * @return size_t size of file return by stat or -1 if an error happened
 */
ssize_t getFileSize(const char* path);

/**
 * Takes a path and returns the absolute path
 * @param path The path to the file or directory.
 * @return The absolute path of the file.
 */
const char* absPath(const char* path);



