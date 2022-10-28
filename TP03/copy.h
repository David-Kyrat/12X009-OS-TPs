/**
 * Checks if a file has been modified
 * 
 * @param src Source file path
 * @param dest Destination file path
 * 
 * @return 0 if it hasn't been modified, 1 if it has
 */
int is_modified(char* src, char* dest);

/**
 * Copies the files/folders from the source to the destination
 * modifed example taken from the course (Chapter 7: I/O)
 * 
 * @param src Source file path
 * @param dest Destination file path
 * 
 * @return 0 if file has been copied correctly, -1 if it hasn't
 */
int copy(const char *from, const char *to);
