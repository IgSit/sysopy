## Zadanie 1 (20%)

Napisz program, w którym z procesu macierzystego stworzysz dokładnie n procesów potomnych (n parametr wywołania programu). W każdym z procesów potomnych wypisz komunikat, w którym poinformujesz z którego procesu napis pochodzi.


## Zadanie 2 (40%)

Napisz program, który liczy numerycznie wartość całki oznaczonej z funkcji 4/(x2+1) w przedziale od 0 do 1 metodą prostokątów (z definicji całki oznaczonej Riemanna). Pierwszy parametr programu to szerokość każdego prostokąta, określająca dokładność obliczeń. Obliczenia należy rozdzielić na n procesów potomnych (n drugi parametr wywołania programu). Każdy z procesów powinien wynik swojej części obliczeń wpisywać do pliku o nazwie "wN.txt" , gdzie N oznacza numer procesu potomnego liczony od 1 i nadawany procesom w kolejności ich tworzenia. Proces macierzysty powinien oczekiwać na zakończenie wszystkich procesów potomnych po czym powinien dodać wyniki cząstkowe z plików stworzonych przez wszystkie procesy potomne i wyświetlić wynik na standardowym wyjściu. W programie zmierz, wypisz na konsolę i zapisz do pliku z raportem czasy realizacji dla różnej liczby procesów potomnych oraz różnych dokładności obliczeń. Dokładności obliczeń należy dobrać w ten sposób by obliczenia trwały co najmniej kilka sekund.

## Zadanie 3 (40%)

Napisz program, który rozpoczynając od katalogu podanego jako pierwszy parametr uruchomienia, idąc w głąb drzewa katalogów, znajdzie pliki zawierające łańcuch podany jako drugi parametr uruchomienia programu. Przeszukiwanie każdego z podkatalogów powinno odbyć się w osobnym procesie potomnym. Wydruk wyniku wyszukiwania poprzedź wypisaniem ścieżki względnej od katalogu podanego jako argument uruchomienia oraz numeru PID procesu odpowiedzialnego za przeglądanie określonego (pod)katalogu. Przeszukiwanie powinno obejmować pliki tekstowe i pomijać pliki binarne/wykonywalne/obiektowe etc. Program jako trzeci parametr powinien przyjmować maksymalną głębokość przeszukiwania licząc od katalogu podanego jako pierwszy parametr.