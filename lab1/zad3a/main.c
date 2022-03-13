#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#ifdef dynamic
#include <dlfcn.h>
#endif

 clock_t start, end;
struct tms st_cpu, en_cpu;

int closeFile(FILE *report) {
	fclose(report);
	free(array);
	return -1;
}

void writeHeader(FILE *report) {
	fprintf(report, "%30s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
}

void saveTimer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(end - start) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;
    fprintf(f, "%30s:\t\t%15f\t%15f\t%15f\n",
            name,
            real_time,
            user_time,
            system_time);
}

int main(int argc, char* argv[]){
	#ifdef dynamic
    void* handle = dlopen("./liblibrary.so", RTLD_NOW);
    if( !handle ) {
        fprintf(stderr, "dlopen() %s\n", dlerror());
        return -1;
    }
    int (*createArray)(int) = dlsym(handle, "createArray");
    int (*wcFiles)(char*) = dlsym(handle, "wcFiles");
    int (*removeBlock)(int) = dlsym(handle, "removeBlock");
    int (*saveTmpIntoArray)(void) = dlsym(handle, "saveTmpIntoArray");
    #endif

	remove("report2.txt");
	FILE *report = fopen("report2.txt", "a");
	writeHeader(report);
	int i = 1;
	while (i < argc)
	{
		char* command = argv[i++];
		if (strcmp(command, "createArray") == 0)
		{
			if (i + 1 > argc)
			{
				fprintf(stderr, "Incorrect createArray syntax\n");
				closeFile(report);
			}
			int size = atoi(argv[i++]);
			createArray(size);
			saveTimer("createArray", report);
			
		}
		else if (strcmp(command, "wcFiles") == 0)
		{
			if (i + 1 > argc)
			{
				fprintf(stderr, "Incorrect wcFiles syntax\n");
				closeFile(report);
			}
			if (wcFiles(argv[i++]) != 0)
			{
				fprintf(stderr, "Problem with running function on this file\n");
				closeFile(report);
			}
			saveTimer("wcFiles", report);
		}
		else if (strcmp(command, "saveTmpIntoArray") == 0)
		{
			saveTmpIntoArray();
			saveTimer("saveTmpIntoArray", report);
		}
		else if (strcmp(command, "removeBlock") == 0)
		{
			if (i + 1 > argc)
			{
				fprintf(stderr, "Incorrect removeBlock syntax\n");
				closeFile(report);
			}
			removeBlock(atoi(argv[i++]));
			saveTimer("removeBlock", report);
		}
		else
		{
			printf("Unknown command\n");
			closeFile(report);
		}
	}
	fclose(report);
	free(array);
	return 0;
}