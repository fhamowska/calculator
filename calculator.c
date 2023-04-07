#include <stdio.h>
#include <string.h> //memset
#include <ctype.h> //isdigit
#include <math.h> //pow

FILE *fpointer;

char wyrazenie[200],
onp[100],
operators[100];

double liczby[100];

int i=0,
onp_top = -1,
operators_top = -1,
liczby_top = -1,
liczby_top_nowa,
dlg_liczby = 0,
dlg_kropka = 0,
czy_ujemna = 0,
neg_nawias = 0,
dzielenie_przez_zero = 0;

//struktura zawierajaca lewa i prawa strone operacji oraz laczacy je operator
struct op
{
    double lewa;
    double prawa;
    char Operator;
};

int priorytet(char znak);
double oblicz(struct op operacja);
void zapisywanie_liczb();
void zmiana_na_ujemne();
void oblicz_i_zapisz();


int main()
{
    //usuwanie historii poprzedniej sesji
    fpointer = fopen("historia_sesji.txt", "w");
    fclose(fpointer);

    while(1)
    {
    //czyszczenie tablic
    memset(wyrazenie,0,sizeof(wyrazenie));
    memset(liczby,0,sizeof(liczby));
    memset(onp,0,sizeof(onp));
    memset(operators,0,sizeof(operators));

    fpointer = fopen("historia_sesji.txt", "a");
    printf("Podaj wyrazenie arytmetyczne: ");
    scanf("%s", &wyrazenie);
    //zapisanie wyrazenia arytmetycznego do pliku
    fprintf(fpointer, "wyrazenie arytmetyczne: %s\n", wyrazenie);

    for(i=0; i<200; i++)
    {
        //jezeli natrafimy na liczbe liczymy ile ma cyfr
        if(isdigit(wyrazenie[i]) || wyrazenie[i]=='.')
        {
            dlg_liczby++;
            //jesli liczba ulamkowa to zliczamy dlugosc po kropce
            if(wyrazenie[i]=='.' || dlg_kropka!=0)dlg_kropka++;
        }

        else //znalezlismy operator
        {
            if(dlg_liczby)//jezeli dlugosc niezerowa to znalezlismy koniec liczby
            {
                if(dlg_kropka)dlg_kropka--;

                liczby_top_nowa = ++liczby_top;
                liczby[liczby_top_nowa] = 0;

                zapisywanie_liczb(); //wpisywanie liczby do tablic "liczby" i "onp"
                if(dzielenie_przez_zero)break;

                onp[++onp_top] = ' ';

                if(czy_ujemna)
                {
                    zmiana_na_ujemne();
                }

                dlg_liczby = 0;
                dlg_kropka = 0;
                czy_ujemna = 0;
            }

            if (wyrazenie[i]=='-' && (i==0 || !isdigit(wyrazenie[i-1]) && wyrazenie[i-1] != ')') && wyrazenie[i+1]!='(')
            {
                czy_ujemna=1;
            }
            else if (wyrazenie[i]=='-' && !isdigit(wyrazenie[i-1]) && wyrazenie[i-1] != ')' && wyrazenie[i+1]=='(')
            {
                neg_nawias=1;
            }
            else if (wyrazenie[i] == '(')
            {
                operators[++operators_top] = wyrazenie[i];
            }
            else if (wyrazenie[i] == ')')//koniec wyrazenia w nawiasie
            {
                //wykonujemy dzialania w nawiasie
                while (operators[operators_top] != '(')
                {
                    //zapisanie wyniku operacji w tablicy liczby i operatora w tablicy onp
                    oblicz_i_zapisz();
                }
                //usuwamy '('
                operators_top--;
                if(neg_nawias)
                {
                    //zmieniamy znak dla ujemnejgo nawiasu
                    liczby[liczby_top] *= -1;
                    //dopisujemy minus unarny do tablicy onp
                    onp[++onp_top] = '~';
                    onp[++onp_top] = ' ';
                    neg_nawias=0;
                }
            }
            else //operatory logiczne (+,-,*,/,^)
            {
                while(operators_top > -1 && (priorytet(wyrazenie[i]) <= priorytet(operators[operators_top])))
                {
                    //zapisanie wyniku operacji w tablicy liczby i operatora w tablicy onp
                    oblicz_i_zapisz();
                }
                if(wyrazenie[i] != '\0')
                {
                    operators[++operators_top] = wyrazenie[i];
                }
            }
        }
    }

    if(!dzielenie_przez_zero)
    {
        onp[onp_top + 1] = '\0';//zamykamy stringa

        fprintf(fpointer, "zapis w onp: %s\n", onp);
        printf("zapis w onp: %s\n", onp);


        if(liczby[0]-(int)liczby[0]==0)
        {
            fprintf(fpointer, "wynik: %d\n\n", (int)liczby[0]);
            //wynik calkowity
            printf("wynik: %d\n\n", (int)liczby[0]);
        }
        //wynik niecalkowity
        else {printf("wynik: %f\n", liczby[0]); fprintf(fpointer, "wynik: %f\n\n", liczby[0]);}

    }
    i=0;
    onp_top = -1;
    operators_top = -1;
    liczby_top = -1;
    dlg_liczby = 0;
    dlg_kropka = 0;
    czy_ujemna = 0;
    neg_nawias = 0;
    dzielenie_przez_zero = 0;
    fclose(fpointer);
    }
    return 0;
}

//nadawanie priorytetu operatorowi
int priorytet(char znak)
{
    int p =-1;

    switch (znak)
    {
        case '(': p = 0; break;
        case '+': p = 1; break;
        case '-': p = 1; break;
        case '*': p = 2; break;
        case '/': p = 2; break;
        case '^': p = 3; break;
    }

    return p;
}

//obliczanie operacji
double oblicz(struct op operacja)
{
    double wynik = 0;

    switch (operacja.Operator)
    {
        case '+': wynik = operacja.prawa + operacja.lewa; break;
        case '-': wynik = operacja.prawa - operacja.lewa; break;
        case '*': wynik = operacja.prawa * operacja.lewa; break;
        case '/': wynik = operacja.prawa / operacja.lewa; break;
        case '^': wynik = pow(operacja.prawa,operacja.lewa); break;
    }

    return wynik;
}

//wpisywanie liczby do tablic "liczby" i "onp"
void zapisywanie_liczb()
{
    for(int j=0; j<dlg_liczby; j++)
    {
        //zapisujemy liczbe w tablicy liczb dodajac kolejne czesci
        if(wyrazenie[i-j-1] == '.') dlg_kropka++;
        else liczby[liczby_top_nowa] += (wyrazenie[i-j-1] - '0')*pow(10, j - dlg_kropka);
        //zapisujemy liczbe w tablicy onp poczawszy od najwyzszej czesci
        onp[++onp_top] = wyrazenie[i-dlg_liczby+j];
    }
}

//negacja
void zmiana_na_ujemne()
{
    //zmieniamy znak dla ujemnej liczby
    liczby[liczby_top_nowa] *= -1;
    //dopisujemy minus unarny do tablicy onp
    onp[++onp_top] = '~';
    onp[++onp_top] = ' ';
}

//zapisanie wyniku operacji w tablicy liczby i operatora w tablicy onp
void oblicz_i_zapisz()
{
    if (liczby_top &&
        (
            operators[operators_top == '+'] ||
            operators[operators_top == '-'] ||
            operators[operators_top == '*'] ||
            operators[operators_top == '/'] ||
            operators[operators_top == '^']
        ))
    {

        struct op operacja;
        operacja.lewa = liczby[liczby_top--];
        operacja.prawa = liczby[liczby_top--];

        //przerywamy jezeli wystepuje dzielenie przez zero
        if(operacja.lewa==0 && operators[operators_top]=='/')
        {
            fprintf(fpointer, "%s\n\n", "Blad: dzielenie przez zero");
            printf("Blad: dzielenie przez zero\n");
            dzielenie_przez_zero = 1;
        }
        operacja.Operator = operators[operators_top];
        //zapisujemy wynik operacji
        liczby[++liczby_top] = oblicz(operacja);
    }

    //dopisujemy operator to tablicy onp
    onp[++onp_top] = operators[operators_top--];
    onp[++onp_top] = ' ';
}
