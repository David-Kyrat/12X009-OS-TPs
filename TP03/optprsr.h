
/**
 * @brief Check if there is at least 1 argument.
 * @param argc same as argc in main
 * @param fileName argv[0]
 * @return 0 if success otherwise code 22 (Invalid argument)
 */
int checkEnoughArgs(int argc, char* file_name);

char** parseArgs(int argc, char* argv[]);