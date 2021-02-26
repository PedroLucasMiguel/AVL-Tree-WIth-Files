#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct bookDataToRB{
    int doi;
    char author_name[64];
    char title[64];
    int pub_year;
}bookData;

typedef struct redBlackNode{
    int key;
    int color;
    struct redBlackNode *parent;
    struct redBlackNode *right;
    struct redBlackNode *left;
    struct bookDataToRB *book;
}rbNode;
