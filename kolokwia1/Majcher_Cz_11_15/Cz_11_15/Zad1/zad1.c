
/** Program identyfikuje typy plikow podane jako argumenty, rozpoznaje zwykle pliki,
    katalogi oraz linki symboliczne
    Nalezy uzupenic program w oznaczonych wierszach rozpoznajac odpowiednie 
    rodzeje plikow*/

#include <stdio.h>		
#include <stdlib.h>		
#include <sys/stat.h>


int main(int argc, char *argv[])
{
	int			i;
	struct stat	buf;
	char		*tekst;

	for (i = 1; i < argc; i++) {
		printf("%s: ", argv[i]);
		if (lstat(argv[i], &buf) < 0) {
			printf("lstat error");
			continue;
		}
        stat(argv[i], &buf);
		if (S_ISREG(buf.st_mode))
			tekst = "zwykly plik";
		else if (S_ISDIR(buf.st_mode))
			tekst = "katalog";		
		else if (S_ISLNK(buf.st_mode))
			tekst = "link symboliczny";
        else
			tekst = "**** cos innego !!! ****";
		printf("%s\n", tekst);
	}
	exit(0);
}
