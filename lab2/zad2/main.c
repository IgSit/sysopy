#include "library.h"
#include "stdio.h"
#include "string.h"
#include <sys/times.h>
#include <unistd.h>

clock_t start, end;
struct tms st_cpu, en_cpu;

void writeHeader(FILE *report) {
    fprintf(report, "%30s\t\t%15s\n",
            "Name",
            "Real [s]");
    printf("%30s\t%15s\n",
           "Name",
           "Real [s]");
}

void saveTimer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end - start) / clk_tics;
    fprintf(f, "%30s:\t\t%15f\n",
            name,
            real_time);
    printf("%30s:\t\t%15f\n",
           name,
           real_time);
}

int main(int argc, char *argv[]) {
    remove("pomiar_zad_2.txt");
    FILE *report = fopen("pomiar_zad_2.txt", "a");
    char first[100];
    char second[100];
    int i = 1;
    if (argc < 3) {  // no files given
        printf("Enter sign and filename (separated by space:\n");
        scanf("%[^ ]s",first);
        scanf(" %[^\n]s", second);
    }
    if (argc < 4) {  // one file missing
        printf("Enter filename:\n");
        scanf("%[^\n]s", second);
    }
    writeHeader(report);
    while (i < argc)
    {
        char* command = argv[i++];
        if (strcmp(command, "count") == 0) {
            if (argc < 3) {
                count(first, second);
            }
            else if (argc < 4) {
                count(argv[i], second);
            }
            else {
                count(argv[i], argv[i + 1]);
            }
            saveTimer("copy", report);
        }
        else if (strcmp(command, "fcount") == 0) {
            if (argc < 3) {
                fcount(first, second);
            }
            else if (argc < 4) {
                fcount(argv[i], second);
            }
            else {
                fcount(argv[i], argv[i + 1]);
            }
            saveTimer("fcopy", report);
        }
    }
    return 0;
}
