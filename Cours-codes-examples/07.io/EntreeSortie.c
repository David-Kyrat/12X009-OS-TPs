#include <stdio.h>

/* La fonction main est toujours la premi�re fonction du programme � �tre
appell�e lors que l'execution*/
int main(void)
{
	float inputFloat;
	char inputChar;
	int nbCorrespondance;
	
	//Entr�e de l'utilisateur pour un float et affichage du
	//float de diff�rente mani�res
	printf("Veuiller entrer un float:\t");
	scanf("%f", &inputFloat);
	printf("Affichage de l'entree sous forme de float: %f\n", inputFloat);	
	printf("Idem avec 2 digits apres la virgule: %.2f\n", inputFloat);
	printf("Idem avec au moins 6 caract�res: %6.2f\n", inputFloat);
	printf("Notation scientifique: %e\n", inputFloat);
	printf("Affichage de l'entree sous forme d'entier %d\n", inputFloat);
	
	//Boucle tant que l'utilisateur n'apuye pas sur 'enter' uniquement
	do
	{
		//Entr�e de l'utilisateur pour un caract�re
		//avec effacement du buffer d'entr�e clavier
		printf("Veuiller entrer un caractere: ");
		while((inputChar = getchar()) != '\n' && inputChar != EOF); //effacement buf
		inputChar = getchar(); //could also use scanf("%c", &inputChar);

		//Si le caract�re est valide l'afficher sous forme d'entier non sign�
		if(inputChar != '\n')
			printf("Le code ASCII de %c est %u\n", inputChar, inputChar);
	} while(inputChar != '\n');
	
	return 0;
}