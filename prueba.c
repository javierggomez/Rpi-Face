#include <stdio.h>

int main (void) {
	int a=256+97;
	unsigned char b=(unsigned char)a; 
//int c=(int)b;
	printf("%c\n",b);
	return 0;
}
