/**
 * Used when backing up files to check if 'src' has been modified since last backup at 'dest'.
 * 
 * @param src source file (potentially newer)
 * @param dest destination file (file to check against)
 * @return 1 if and only if src is newer, if dest does not exist or if sizes are different. Otherwise it returns 0
 */
int is_modified(const char* src, const char* dest);


/**
 * Takes a path and creates all the subpaths (starting from 'dest') that are not created yet
 * i.e. in 'dest'/foo/bar/to => will create /foo and /foo/bar if they do not exists
 * 
 * @param from the source file or directory
 * @param to the destination path
 * @param dest parent folder of 'to' that must exist.
 * 
 * @return 0 on success, -1 if error
 */
int create_subpaths_ifneeded(const char* from, const char* to, const char* dest);

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

/**
 * Ultra cp for a single file (usually regular file)
 * It copies a file from one location to another, and if the copy is successful, 
 * 
 * @param from the source file
 * @param to the destination file
 * @param modif_perm if set to 1, the permissions of the file will be set to 0777.
 * @param preserve_link if set to 1, the file will be copied as a link, if it is a link.
 * 
 * @return 0 if success, -1 if error.
 */
int ultra_cp_single(const char* from, const char* to, int modif_perm, int preserve_link);

/**
 * Backup files from 'parent/' to 'dest'
 * does not copy empty directory because they contain no files
 * 
 * @param parent parent directory of files to backup
 * @param dir_name current directory to backup
 * @return 0 if success else error
 */
int ultra_cp(const char* parent, const char *dir_name, const char* dest, int modif_perm, int preserve_link);
