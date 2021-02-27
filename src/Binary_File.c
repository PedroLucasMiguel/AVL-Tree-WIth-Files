#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define JUMP 256

typedef struct exemplo{
    char doi[64];
    char author_name[64];
    char title[64];
    char year[64];
}ex;


int main(){
    FILE *fp;

    ex a;
    ex b;

    char resultado[64];

    strcpy(a.doi, "1\n");
    strcpy(a.author_name, "Jooj\n");
    strcpy(a.title, "livro1\n");
    strcpy(a.year, "2020\n");

    strcpy(b.doi, "2\n");
    strcpy(b.author_name, "Jiij\n");
    strcpy(b.title, "livro2\n");
    strcpy(b.year, "2024\n");


    fp = fopen("teste.bin", "w+");

    fwrite(a.doi, sizeof(a.doi), sizeof(char), fp);
    fwrite(a.year, sizeof(a.year), sizeof(char), fp);
    fwrite(a.author_name, sizeof(a.author_name), sizeof(char), fp);
    fwrite(a.title, sizeof(a.title), sizeof(char), fp);

    fwrite(b.doi, sizeof(b.doi), sizeof(char), fp);
    fwrite(b.year, sizeof(b.year), sizeof(char), fp);
    fwrite(b.author_name, sizeof(b.author_name), sizeof(char), fp);
    fwrite(b.title, sizeof(b.title), sizeof(char), fp);

    fclose(fp);

    fp = fopen("teste.bin", "rb+");

    int start = JUMP;

    for (int i = 0; i < 4; i++){
        fseek(fp, start, 0);
        fread(resultado, sizeof(b.doi), sizeof(char), fp);
        printf("%s", resultado);
        start+=64;
    }
    
    fclose(fp);

    return 0;
}