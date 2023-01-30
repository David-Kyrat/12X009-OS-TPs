#include "../common/common.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MALLOC_SIZE 0x10 //en octets
#define STACK_ALLOC_SIZE 256*1024 //en octets

uint32_t nonInitData; //Variable globale non initialisée
uint32_t initData = 0x12; //Variable globale initialisée
char* ptChar = "Where is this pointed string?"; // ???
char tabChar[] = "Where is this char array?"; // ???

void useStack()
{
  //Augmentation de la pile
  char tab[STACK_ALLOC_SIZE];
  
  //Pause pour examiner le processus
  printf("Stack size has been increase and the program paused...\n");
  fgetc(stdin);
}

int main()
{
  //Décalaration de variables locales
  uint32_t localVar1, i;  
  char *pt_malloc; //pointeur sur la zone allouée

  // Just print my pid
  printf("My PID = %d\n\n", getpid());
  
  //Récuperer l'adresse du brk avant et après allocation de mémoire
  pt_malloc = malloc(MALLOC_SIZE);
  
  //Affichage de l'adresse de la fonction main dans le segment de code
  printf("--- code segment ---\n");
  printf("main function adress: %p\n", main);
  printf("use stack function adress: %p\n", useStack);
  printf("Fixed string adress(ptChar): %p\n", ptChar); //La chaine est constante donc dans .bss
  
  
  //Affichage des differente variable presentes dans le segment de donnée (dabord initialisées puis non initialisées)
  printf("--- data segment ---\n");
  printf("initData address: %p\n", &initData);
  printf("Pointer to fixed string address (ptChar adress): %p\n", &ptChar); //Ce pointeur est alloué en donnée globale initialisée
  printf("Character array address (tabChar): %p\n", tabChar); //Le tableau est une variables (avec equivalence de pointeur) -> données initialisées
    printf("--\n");
  printf("nonInitData address: %p\n", &nonInitData);  
  
  //Affichage des donnée du tas (brk avant et apres allocation de mémoire et pointeur sur donnée allouées)
  printf("--- Heap ---\n");  
  printf("Address allocated variable: %p\n", pt_malloc);
  
  //Affichage des données locales affichées dans la piles
  printf("--- Stack ---\n");
  printf("First Local variable address: %p\n", &localVar1);
  printf("Second Local variable address: %p\n", &i);
  printf("pt_malloc variable address: %p\n", &pt_malloc);
  printf("-------------\n");  
  
  //Pause pour examiner le processus
  fgetc(stdin);
  
  //utilisation de la pile et pause
  useStack();
    
  //try to modify the pointed string
  ptChar[0] = 'e';

  return EXIT_SUCCESS;
}

