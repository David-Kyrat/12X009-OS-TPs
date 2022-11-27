#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define str(s) # s
#define DEF 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4

// 0:default,  1:red, 2:Green,  3:Blue, 4:Purple 
const char* colors[] = {"\033[0m", "\033[0;31m", "\033[0;32m", "\033[0;34m", "\033[0;35m"};


void setOutColor(int color) {
	printf("%s", colors[color]);
}

void resetCol() {
	setOutColor(DEF);
}

int main(int argc, char* argv[]) {
	setOutColor(RED);
	printf("Hello World\n");
	resetCol();

	char* inp;
	scanf("%ms", &inp);
	printf("Entered: \'%s\'\n", inp);

	setOutColor(PURPLE);
	puts("test");
	resetCol();
//	free(inp);
	puts("Exited.");
}
