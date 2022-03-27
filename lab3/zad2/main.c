#include <sys/times.h>
#include <sys/wait.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

clock_t start, end;
struct tms st_cpu, en_cpu;

void startTimer()
{
    start = times(&st_cpu);
}

void endTimer()
{
    end = times(&en_cpu);
}

void writeHeader(FILE *report) {
    fprintf(report, "%30s\t%15s\n",
            "Number of processes",
            "Real time [s]");
    printf("%30s\t%15s\n",
           "Number of processes",
           "Real time [s]");
}

void saveTimer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end - start) / clk_tics;
    fprintf(f, "%30s:\t%15f\n",
            name,
            real_time);
    printf("%30s:\t%15f\n",
           name,
           real_time);
}

double f(double x) {
    return 4 / (x * x + 1);
}

void calculatePartialSolution(double x, double width) {
    double result = f(x + width / 2) * width;
    char filename[100];
    snprintf(filename, 100, "w%d.txt", getpid() - getppid());
    FILE * resFile = fopen(filename, "w+");
    fprintf(resFile, "%f", result);
    fclose(resFile);
}

double sumPartialSolutions(double from, double to, double width) {
    double solution = 0;
    FILE *partialFile;
    char filename[100];
    char line[20];
    int i = 1;
    while (from < to) {
        snprintf(filename, 100, "w%d.txt", i);
        partialFile = fopen(filename, "r+");
        fgets(line, sizeof line, partialFile);
        solution += atof(line);
        fclose(partialFile);

        from += width;
        i++;
    }
    return solution;
}

int main(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Invalid syntax.\n");
        return -1;
    }
    double width = atof(argv[1]);

#ifdef COUNTTIME
    FILE *report = fopen("report2.txt", "w+");
    startTimer();
#endif

    for (double i = 0.0; i < 1.0;) {
        if (fork() == 0) {
            calculatePartialSolution(i, width);
            exit(0);
        }
        i += width;
    }
    wait(0);

    double solution = sumPartialSolutions(0.0, 1.0, width);

#ifdef COUNTTIME
    endTimer();
    double divisions = 1 / atof(argv[1]);
    char str[15];
    sprintf(str, "%f", round(divisions));
    writeHeader(report);
    saveTimer(str, report);
#endif

    printf("result: %f\n", solution);

    return 0;
}