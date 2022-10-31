/**
 * Used when backing up files to check if 'src' has been modified since last backup at 'dest'.
 * 
 * @param src source file (potentially newer)
 * @param dest destination file (file to check against)
 * @return 1 if and only if src is newer, if dest does not exist or if sizes are different. Otherwise it returns 0
 */
int is_modified(const char* src, const char* dest);

/**
 * Copies the files/folders from the source to the destination
 * modifed example taken from the course (Chapter 7: I/O).
 * (Will overwrite destination if exists)
 * 
 * @param src Source file path
 * @param dest Destination file path
 * 
 * @return 0 if file has been copied correctly, -1 if it hasn't
 */
int copy(const char *from, const char *to);

/**
 * If the file `from` is newer than the file `to`, then copy `from` to `to`
 * 
 * @param from the source file
 * @param to the destination file
 * 
 * @return 0 if success, else -1 (check value of errno for more details)
 */
int copy_ifneeded(const char* from, const char* to);

int ult_copy(const char* from, const char* to, int modif_perm, int preserve_link);
