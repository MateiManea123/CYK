#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX 300
using namespace std;
char cuvinte[20][200];

struct multime
{
    int nrElem;
    char elem[20][200];
};

void Adaugare(multime& M, char x[200])
{
    M.nrElem++;
    strcpy(M.elem[M.nrElem], x);
}

struct regula
{
    char stanga[200];
    char dreapta1[200];
    char dreapta2[200];
};
const int MAX_REGULI = 100; // Numărul maxim de reguli gramaticale
int nrReg;
regula Reg[MAX_REGULI];

struct lex
{
    char stanga[200];
    char dreapta[200];
};
int nrLex;
lex Lex[200];

void CitesteGramaticaReg(const string& nf,regula Reg[], int& nr)
{
    nr = 0;
    FILE* fis;
    char linie[MAX];

    if (!(fis = fopen(nf.c_str(), "r")))
    {
        perror("Eroare la deschidere.cu mesajul de eroare:");
        exit(1);
    }

    while (fgets(linie, MAX, fis) != NULL)
    {
        size_t len = strlen(linie);

        // Elimină caracterul de linie nouă din șir
        if (len > 0 && linie[len - 1] == '\n')
        {
            linie[len - 1] = '\0';
        }

        nr++;
        size_t p = strcspn(linie, "->");
        strncpy(Reg[nr].stanga, linie, p);
        Reg[nr].stanga[p] = '\0';

        size_t p2 = strcspn(linie + p + 2, " \n");
        strncpy(Reg[nr].dreapta1, linie + p + 2, p2);
        Reg[nr].dreapta1[p2] = '\0';


        if (linie[p + 2 + p2] != '\0' && linie[p + 2 + p2] != '\n')// Verificăm dacă există o a doua parte dreaptă
        {
            strcpy(Reg[nr].dreapta2, linie + p + 2 + p2 + 1);
        }
        else
        {
            Reg[nr].dreapta2[0] = '\0'; // Setăm dr2 ca șir vid dacă nu există o a doua parte dreaptă
        }
    }
    fclose(fis);
    nrReg=nr;
}
void CitesteGramaticaLex(const string& nf,lex Lex[], int& nr)
{
    nr = 0;
    FILE* fis;
    char linie[MAX];

    if (!(fis = fopen(nf.c_str(), "r")))
    {
        perror("Eroare la deschidere.cu mesajul de eroare:");
        exit(1);
    }

    while (fgets(linie, MAX, fis) != NULL)
    {
        size_t len = strlen(linie);

        // Elimină caracterul de linie nouă din șir
        if (len > 0 && linie[len - 1] == '\n')
        {
            linie[len - 1] = '\0';
        }

        nr++;
        size_t p = strcspn(linie, "->");
        strncpy(Lex[nr].stanga, linie, p);
        Lex[nr].stanga[p] = '\0';

        size_t p2 = strcspn(linie + p + 2, " \n");
        strncpy(Lex[nr].dreapta, linie + p + 2, p2);
        Lex[nr].dreapta[p2] = '\0';
    }
    fclose(fis);
    nrLex=nr;
}

// Star(X,Y)={C|(A este în X) şi (B este în Y) şi C->AB este o regulă din gramatică}

multime Star(multime X, multime Y)
{
    multime Rezultat;
    Rezultat.nrElem = 0;  // la inceput multimea asta este vida
    char A[200], B[200], C[200];
    for (int i = 1; i <= X.nrElem; i++)
        for (int j = 1; j <= Y.nrElem; j++)
        {
            strcpy(A, X.elem[i]);
            strcpy(B, Y.elem[j]);

            for (int k = 1; k <= nrReg; k++)
                if (!strcmp(Reg[k].dreapta1, A) && !strcmp(Reg[k].dreapta2, B))
                {
                    strcpy(C, Reg[k].stanga);
                    Adaugare(Rezultat, C);
                }
        }
    return Rezultat;
}

/* Regula nr. k ar fi:
Grup_verbal -> Verb Grup_nominal
   C             A         B
stanga          dreapta1 dreapta2
*/


// Closure(S) = {A|(A este în S) sau ((B este în Closure(S)) şi (A->B este o regulă din gramatică))}

multime Closure(multime S)
{
    multime Rezultat;
    Rezultat.nrElem = 0;
    char A[200], B[200];
    for (int i = 1; i <= S.nrElem; i++)
    {
        Adaugare(Rezultat, S.elem[i]);
    }
    for (int i = 1; i <= Rezultat.nrElem; i++)
    {
        strcpy(B, Rezultat.elem[i]);
        for (int k = 1; k <= nrReg; k++)
            if (!strcmp(Reg[k].dreapta1, B) && !strcmp(Reg[k].dreapta2, ""))
            {
                strcpy(A, Reg[k].stanga);
                bool gasit = false;
                for (int j = 1; j <= Rezultat.nrElem; j++)
                    if (!strcmp(A, Rezultat.elem[j]))
                        gasit = true;
                if (!gasit) Adaugare(Rezultat, A);
            }
    }
    return Rezultat;
}


// Lookup(k) = {A|A->cuvântul k}

multime Lookup(int k)
{
    multime Rezultat;
    Rezultat.nrElem = 0;
    char cuv[200];
    char a[200];
    strcpy(cuv, cuvinte[k]);  // vectorul cuvinte va fi ceva de genul ("orice","pisica","iubeste...)
    for (int i = 1; i <= nrLex; i++)
    {

        if (!strcmp(Lex[i].dreapta, cuv))
        {
            strcpy(a, Lex[i].stanga);
            Adaugare(Rezultat, a);
        }
    }
    return Rezultat;
}

// Funcție pentru a verifica dacă un element există deja într-o mulțime
bool ExistaInMultime(multime& M, char x[200])
{
    for (int i = 1; i <= M.nrElem; ++i)
    {
        if (!strcmp(M.elem[i], x))
        {
            return true; // Elementul există în mulțime
        }
    }
    return false; // Elementul nu există în mulțime
}

// Funcție pentru a realiza reuniunea a două mulțimi
multime Reuniune(multime& M1, multime& M2)
{
    multime Result;
    Result.nrElem = 0;

    // Adaugă toate elementele din M1 în mulțimea rezultat
    for (int i = 1; i <= M1.nrElem; ++i)
    {
        Adaugare(Result, M1.elem[i]);
    }

    // Adaugă elementele distincte din M2 în mulțimea rezultat
    for (int i = 1; i <= M2.nrElem; ++i)
    {
        if (!ExistaInMultime(Result, M2.elem[i]))
        {
            Adaugare(Result, M2.elem[i]);
        }
    }

    return Result;
}

void afiseaza_multimea(multime M)
{
    cout<<"{";
    for (int i=1; i<M.nrElem; i++)
        cout<<M.elem[i]<<",";
    cout<<M.elem[M.nrElem];
    cout<<"}";
}

int main()
{
    multime chart[20][20];
    cout << "Introduceti propozitia: ";
    char prop[251], *p;
    cin.getline(prop, 250);
    int n;
    CitesteGramaticaReg("gram.txt", Reg, nrReg);

    CitesteGramaticaLex("lex.txt", Lex, nrLex);
    cout << "Cuvintele propozitiei:\n";
    n=0;
    p = strtok(prop, " ");
    while (p != NULL)
    {

        strcpy(cuvinte[++n], p);
        p = strtok(NULL, " ");
    }
    int nr=0;
    for (int k = 1; k <= n; k++)
    {
        chart[k - 1][k]= Closure(Lookup(k));//i 0 j 1  Star(chart[1][1], chart[1][2])
        for (int i = k - 2; i >= 0; i--)
        {
            chart[i][k].nrElem=0;
            for (int j = k - 1; j >= i+1 ; j--)
            {
                multime s=Star(chart[i][j], chart[j][k]);
                chart[i][k] = Reuniune(chart[i][k], s);
            }
            //chart[i][k] = Closure(chart[i][k]);
        }
    }


    for(int i = 0; i <n; i++)
    {
        for(int j = 1; j <=n; j++)
        {
            {
                afiseaza_multimea(chart[i][j]);
                cout<<"      ";
            }

        }
        cout << endl;
    }
    int ok;
    for(int k=1; k<=n; k++)
    {
        ok=0;
        for(int j=1; j<=nrLex; j++)
        {

            if(strcmp(Lex[j].dreapta,cuvinte[k])==0)
                ok=1;
        }
        if(ok==0)
        {
            cout<<"Reject";
            return 0;
        }
    }


    if (strcmp("S", chart[0][n].elem[1])==0)
        cout << "Accept";
    else cout << "Reject";
    return 0;
}


