#include <stdio.h>
#include <stdint.h>

typedef union {
	int16_t valueInt;
	struct { //Sur cette platforme un entier cours a 2 octets
		unsigned char msb; //most significant byte
		unsigned char lsb; //least significant byte
	} bytes;
} convert;

int main(void) {
	convert value;
	printf("Entrer un entier: ");
	scanf("%d", &value);
	printf("Valeur entree: %d\n", value.valueInt);
	printf("Valeur entree (hexadecimal): %x\n", value);
	printf("MSB: h%02x, LSB h%02x\n", value.bytes.lsb, value.bytes.msb);
}