## Optymalizacja

(https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)

- O – kompilator próbuje optymalizować kod i czas wykonania, bez wykonywania operacji znacząco zwiększających czas kompilacji
- O1 – możliwy dłuższy czas kompilacji, zużywa dużo pamięci dla dużych funkcji
- O2 – używane jeszcze więcej opcji kompilacji, które nie powodują zwiększenia pamięci programu, w porównaniu do –O zwiększa czas kompilacji i wydajność kodu, używa wszystkich flag używanych przez –O a także dodatkowe flagi
- O3 – używa wszystkie flagi używane przez –O2, a także dodatkowe flagi
- O0 –ogranicza czas kompilacji, wartość domyślna
- Os – optymalizacja rozmiaru, używa wszystkie flagi –O2, które nie zwiększają kodu programu, używa też dodatkowych flag zmniejszających rozmiar kodu

## Pomiar czasu
### Funkcje czekania

    #include <unistd.h>
    unsigned int sleep(unsigned int seconds);
    #include <time.h>
    int nanosleep(const struct timespec *req, struct timespec *rem);
    
    
    struct timespec {
    time_t tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
    };
### Zegary POSIX

Typ danych clock_t – reprezentuje takty zegara
Typ danych clockid_r – reprezentuje określony zegar Posix
Są 4 rodzaje zegarów – zalecany to CLOCK_REALTIME – ogólnosystemowy zegar czasu rzeczywistego

    #include <time.h>
    int clock_getres(clockid_t clk_id, struct timespec *res) – odczytuje rozdzielczość zegara wyspecyfikowanego w parametrze clk_id
    
    int clock_gettime(clockid_t clk_id, struct timespec *tp) – pobranie wartości zegara
    
    int clock_settime(clockid_t clk_id, const struct timespec *tp) - ustawienie wartości zegara
    
    
    #include <sys/times.h>
    
    clock_t times(struct tms *buffer);

### Pola struktury tms

- tms_utime – czas cpu wykonywania procesu w trybie użytkownika
- tms_stime – czas cpu wykonywania procesu w trybie jądra
- tms_cutime – suma czasów cpu wykonywania procesu i wszystkich jego potomków w trybie użytkownika
- tms_cstime - suma czasów cpu wykonywania procesu i wszystkich jego potomków w trybie jądra


## Zarządzanie pamięcią

### Alokacja pamięci: (standard ANSI C)

- malloc – alokuje w pamięci wskazaną liczbę bajtów. Wartość początkowa zawartości pamięci nie jest określona
- calloc – alokuje przestrzeń dla określonej liczby obiektów o zadanym obszarze. Cały zarezerwowany obszar jest wypełniony bitami zerowymi
- realloc – zmienia rozmiar poprzednio zaalokowanego obszaru (zwiększa go lub zmniejsza). Jeśli rozmiar rośnie, może to oznaczać przesunięcie wcześniej zaalokowanego obszaru w inne miejsce, aby dodać wolną przestrzeń na jego końcu. W takiej sytuacji nie jest określona wartość początkowa fragmentu pamięci między końcem starego a końcem nowego obszaru.
- free – zwalnia pamięć wskazaną przez ptr

    #include <stdlib.h>
    
    void * malloc(size_t size);
    void * calloc(size_t nobj, size_t size);
    void *realloc(void *ptr, size_t newsize);
    void free(void *ptr);

Funkcje alokujące są na ogół implementowane za pomocą funkcji systemowej sbrk(2), które rozszerza lub zawęża stertę procesu 
Choć wywołanie funkcji sbrk może rozszerzyć lub zawęzić pamięć procesu, to jednak większość wersji funkcji malloc i free nigdy nie zmniejsza rozmiaru pamięci procesu – zwalniana pamięć staje się dostepna dla kolejnych alokacji, ale nie powraca do jądra systemu – jest utrzymywana w puli, którą dysponuje funkcja malloc.
Uwaga: większość implementacji alokuje nieco więcej pamięci, niż jest to wymagane, dodatkowy obszar jest używany do przechowywania specjalnych danych jak: rozmiar alokowanego bloku, wskaźnika do kolejnego bloku do alokacji.

Inne funkcje mechanizmu alokacji: mallinfo

    mallinfo – dostarcza charakterystyki mechanizmu alokacji:

    struct mallinfo mallinfo(void)
    unsigned long arena;//total space
    unsigned long ordblks; //number of ordinary blocks
    unsigned long smblks; //number of small blocks
    unsigned long hblkhd; //space in holding block headres
    unsigned long hblks; //number of holding blocks
    unsigned long usmblks; //space in small blocks in use
    unsigned long fsmblks; //space in free small blocks
    unsigned long uordblks; //space in ordinary blocks in use
    undigned long fordblks; //space in free ordinary blocks
    unsigned lon keepcostl //space penalty if keep option is used
### Użycie obszarów pamięci

Przykładowy program:

    int main(int argc, char * argv[])
    {
    return 0;
    }

Proces zawiera obszary odpowiadające sekcjom tektu, danych i bss
Zakładając, że proces jest dynamicznie zlinkowany z biblioteką C, analogiczne trzy obszary pamięci istnieją także dla libc.so (biblioteka c) oraz dla ld.so (linkera dynamicznego)
Proces posiada także obszar pamięci odpowiadający za stos
Poniższe dane w pliku /proc/<pid>/maps przedstawiają obszary pamięci, mają postać:
początek obszaru-koniec obszaru prawa dpstępu duży:mały iwęzeł plik


Pierwsze trzy wiersza, to sekcja tekstu, danych i bss biblioteki C (libc.so)
Następne dwa wiersze to sekcje kodu i danych programu wykonywalnego
Następne trzy wiersze to sekcja tekstu, danych i bss linkera dynamicznego (ld.so)
Ostatni wiersz to obszar stosu
Cała przestrzeń adresowa zajmuje około 1340KB, ale tylko 40KB są zapisywalne i prywatne
Jeśli obszar pamięci jest współdzielony lub niemodyfikowalny, jądro przechowuje tylko jedną jego kopię w pamięci
Dlatego biblioteka C potrzebuje tylko 1212KB pamięci fizycznej dla wszystkich procesów
Obszary pamięci bez zmapowanego pliku i o i-węźle 0 – są to strony zerowe (zero page): mapowania zawierające tylko zera
Przez zmapowanie strony zerowej na zapisywalne obszary pamięci, obszar jest ”inicjalizowany” zerami, co jest oczekiwane dla bss


### Biblioteki

#### Co to są biblioteki?

Biblioteka jest zbiorem implementacji zachowań, opisanych w języku programowania, która ma dobrze zdefiniowany interfejs, przez który zachowania są wywoływane [Wikipedia] "program library" jest plikiem zawierającym skompilowany kod i dane, które będą włączone potem do programu/programów, umożliwiają modularne programowanie, szybszą rekompilację i łatwiejsze uaktualnienia [The Linux Documentation Project]

Biblioteki można podzielić na trzy rodzaje: : statyczne, współdzielone i dynamicznie łądowane
Statyczne biblioteki są dołączane do programu wykonywalnego przed jego uruchomieniem
Współdzielone biblioteki są ładowane w momencie uruchomienia programu i mogą być współdzielone z innymi programami
Dynamicznie ładowane biblioteki są ładowane, gdy program wykonywalny się wykonuje.

### Biblioteki statyczne (Static Libraries)

Biblioteki statyczne są zbiorami plików obiektowych. Zazwyczaj mają rozszerzenie ".a" .
Biblioteki statyczne pozwalają użytkownikom linkować się do plików obiektowych bez rekomplilacji kodu. Pozwalają także dystrybuować biblioteki bez rozpowszechniania kodu źródłowego.

•Przykłady:

    my_library.h
    #pragma once
    namespace my_library {
    extern "C" void my_library_function();
    }
    
    my_library.c
    ...
    #include "my_library.h"
    void my_library_function() {
    ...
    }
    
    main.c
    #include "my_library.h"
    int main() {
    my_library_function();
    }

Przykład – kompilacja z plikami obiektowymi
    
    $ gcc -c my_library.c
    $ gcc -c main.c
    $ gcc main.o my_library.o –o main
    $ ./main

Przykłady – kompilacja jako biblioteka statyczna

    $ gcc -c my_library.c
    $ ar rcs libmy_library.a my_library.o
    $ gcc -c main.c
    $ gcc main.o libmy_library.a –o main
    $ ./main
    ...
    $ gcc main.o –l my_library –L ./ -o main
    $ ./main
### Biblioteki współdzielone (Shared Libraries)

Biblioteki współdzielone są ładowane gdy program jest ładowany. Wszystkie programy mogą współdzielić dostęp do współdzielonych bibliotek i będzie uaktualniona (upgraded) jeśli nowa wersja bliblioteki zostanie zainstalowana
Może być zainstalowanych wiele wersji bibliotek, by pozwolić progamom ze specyficznymi potrzebami na używanie konkretnej wersji biblioteki
Biblioteki te mają zazwyczaj rozszerzenie .so
Biblioteki współdzielone używają specyficznej reguły nazewnictwa
Każda biblioteka ma "soname" zaczynające się do prefiksu "lib" , po których następuje nazwa (name) biblioteki, po czym rozszerzenie ".so" ta następnie kropkę i wersję biblioteki (version numer).
Każda biblioteka ma także nazwę rzeczywistą "real name" – nazwę pliku z kodem biblioteki. Rzeczywista nazwa "real name" obejmuje "soname", kropkę, version number i opcjonalnie kropkę i release number.
Oba numery (version i release) umożliwiają wybór dokładnej wersji biblioteki.
Dla jednej biblioteki współdzielonej system często ma wiele linków wskazujących na tę samą nazwę

Przykład:

    soname
    /usr/lib/libreadline.so.3
    
    Linkowanie do realname:
    /usr/lib/libreadline.so.3.0
    
    Lub:
    /usr/lib/libreadline.so
    
    Linkowanie do:
    /usr/lib/libreadline.so.3


Przykłady –Kompilowanie biblioteki współdzielonej

    $ gcc -fPIC –c my_library.c
    (-fPIC position independent code, potrzebny do kodu biblioteki współdzielonej)
    
    $ gcc -shared –W1,-soname,libmy_library.so.1 \
    -o libmy_library.so.1.0.1 my_library.o –lc
    
    $ ln –s libmy_library.so.1.0.1 libmy_library.so.1
    $ ln –s libmy_library.so.1 libmy_library.so

Przykład – użycie biblioteki współdzielonej

    $ gcc main.c –lmy_library –L ./ -o main
    $ ./main

Problemem jest, że mamy stałą ścieżkę do biblioteki
Biblioteka musi być w tym samym katalogu
Położenie bibliotek współdzielonych może się różnić w zależności do systemu.
$LD_LIBRARY_PATH służy do ustawienia ścieżki poszukiwań bibliotek współdzielonych
Kiedy program wymagający bibliotek współdzielonych jest uruchomiony, system poszukuje ich w katalogach podanych w $LD_LIBRARY_PATH .
Jeśli chcesz zainstalować swoją bibliotekę w systemie, możesz skopiować pliki .so do jednej ze standardowych katalogów - /usr/lib i wywołać ldconfig
Każdy program używający biblioteki może teraz odwołać się do niej poprzez –lmy_library i system znajdzie ją w /usr/lib

### Biblioteki ładowane dynamicznie (Dynamically Loaded Libraries)

Dynamicznie ładowane biblioteki są ładowane przez sam program z poziomu kodu źródłowego.
Biblioteki są zbudowane jako standardowe obiekty lub bublioteki współdzielone, jedyną różnicą jest to, że biblioteki nie są ładowane podczas fazy linkowania przy kompilacji lub uruchomienia, ale w punkcie ustalonym przez programistę.
Funkcje odpowiedzialne za operacje na bibliotekach ładowanych dynamicznie:

    void* dlopen(const char *filename, int flag); - Otwiera bibliotekę, przygotowuje ją do użycia i zwraca wskaźnik/uchwyt na bibliotekę.
    void* dlsym(void *handle, char *symbol); - Przegląda bibliotekę szukając specyficznego symbolu.
    void dlclose(); - Zamyka bibliotekę .

    #include <dlfcn.h>
    int main() {
    void *handle = dlopen("libmy_library.so", RTLD_LAZY);
    if(!handle){/*error*/}

        void (*lib_fun)();
        lib_fun = (void (*)())dlsym(handle,"my_library_function");
    
        if(dlerror() != NULL){/*error*/}
    
        (*lib_fun)();
    
        dlclose(handle);
}