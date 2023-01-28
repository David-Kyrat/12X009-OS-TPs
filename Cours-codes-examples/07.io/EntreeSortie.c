#include <stdio.h>

/* La fonction main est toujours la première fonction du programme à être
appellée lors que l'execution*/
int main(void)
{
	float inputFloat;
	char inputChar;
	int nbCorrespondance;
	
	//Entrée de l'utilisateur pour un float et affichage du
	//float de différente manières
	printf("Veuiller entrer un float:\t");
	scanf("%f", &inputFloat);
	printf("Affichage de l'entree sous forme de float: %f\n", inputFloat);	
	printf("Idem avec 2 digits apres la virgule: %.2f\n", inputFloat);
	printf("Idem avec au moins 6 caractères: %6.2f\n", inputFloat);
	printf("Notation scientifique: %e\n", inputFloat);
	printf("Affichage de l'entree sous forme d'entier %d\n", inputFloat);
	
	//Boucle tant que l'utilisateur n'apuye pas sur 'enter' uniquement
	do
	{
		//Entrée de l'utilisateur pour un caractère
		//avec effacement du buffer d'entrée clavier
		printf("Veuiller entrer un caractere: ");
		while((inputChar = getchar()) != '\n' && inputChar != EOF); //effacement buf
		inputChar = getchar(); //could also use scanf("%c", &inputChar);

		//Si le caractère est valide l'afficher sous forme d'entier non signé
		if(inputChar != '\n')
			printf("Le code ASCII de %c est %u\n", inputChar, inputChar);
	} while(inputChar != '\n');
	
	return 0;
}