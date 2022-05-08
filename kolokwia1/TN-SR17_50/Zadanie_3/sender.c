#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#define PIPE "./squareFIFO"

int main(int argc, char* argv[]) {

 if(argc !=2){
   printf("Not a suitable number of program parameters\n");
   return(1);
 } 

  //utworz potok nazwany pod sciezka reprezentowana przez PIPE
  //zakladajac ze parametrem wywolania programu jest liczba calkowita
  //zapisz te wartosc jako int do potoku i posprzataj
    mkfifo(PIPE, 0666);
    FILE *file = fopen(PIPE, "w");
    int variable = atoi(argv[1]);
    int* val = &variable;
    fwrite(val, sizeof(int), 1, file);
    fclose(file);
    return 0;
}
