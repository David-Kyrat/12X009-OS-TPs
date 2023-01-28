#include <stdio.h>

enum typeMedaille { bronze, argent, or };
typedef enum { true, false } booleen;
typedef enum { lundi = 1, mardi, mercredi, jeudi, vendredi, samedi, dimanche } jours;
typedef enum { micro = 2, mineur = 4, leger = 5, modere = 6, major = 7, important = 9, devastateur = 10} richter;


int main(void)
{

	enum typeMedaille maMedaille = or;
	jours aujourdhui;
	
	switch(or) 	//Séparer les different cas que peut prendre une variable
	{
	case bronze:
		printf("Peux mieux faire !\n");
		break; //A ne pas oublier
	case argent:
		printf("Pas mal !\n");
		break;
	case or:
		printf("Le controle est par la...\n");
		break;
	default:
		printf("Inconnu au bataillon\n");
	}

	aujourdhui = lundi;
	if(aujourdhui != lundi)
		printf("Que faites-vous la ?\n");
	else
		printf("Merci d'etre la.\n");
		
	return 0;
	
	
	//Toutes ces affectations sont autorisées !!!
	//Mais à éviter !
	int tremblementFukushima = 9;
	richter tremblement = 3;
	booleen estValide = mardi;
	
}