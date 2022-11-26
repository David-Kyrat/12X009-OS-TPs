# Example of Codes from the enonce

## Objectives

- Créer des processus avec la fonction fork;
- Gérer ces processus, notamment éviter les zombies;
- Gérer les signaux envoyés au shell.  


## Features  

#### User can:  

- Interact with system
- Execute commands


### Shell must

- Create multiple processes (with `fork`)
- Handle those processes (avoid zombies)
- Be able to use builtin commands (`cd <dirname>` and `exit <exit-code>`)
- Be able to execute jobs, i.e. system's programs (e.g. `ls`, `pwd`, `ps`...)

---  


## Parsing  

### Comment lire l'entrée utilisateur ?

- Transformer la chaine de caractère en un tableau de chaine comme
`argv`, `argc` (c.f. fonction `main`)  

- Dans notre cas on considérera que l'espace et la tabulation sont les deux seuls caractères qui séparent les arguments.  
  - Consultez la fonction `strtok` dans le manuel. La commande `realloc` peut être utile aussi.


```C
  char* token = strtok(userinput, ""); 

  while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, "");
  }

```



---

## Jobs

When the users enters a non-builtin command:  

- Shell will create another process to execute the program with the `execve` call  
```C
  include <unistd.h> 
  int execve(const char *filename, char *const argv[], char *const envp[]);
```  


Shell will wait for the child process to end then display its exit code
if it's available. (e.g. "Foreground job exited withcode 0"),
or if it is not: a simple message (e.g. "Foreground job exited")
