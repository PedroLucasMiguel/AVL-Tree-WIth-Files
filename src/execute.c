#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "nodesAndData.c"

#define RED 1
#define BLACK 0

//########################################################################################
//                                  Rotações
//########################################################################################
rbNode *leftRotation(rbNode *x){
    bool right;
    rbNode *aux;

    if (x->parent->parent != NULL){
        if (x->parent->parent->right = x->parent){
            right = true;
        }

        else{
            right = false;
        }

        if(right){
            x->parent->parent->right = x;
            aux = x->parent;
            x->parent = aux->parent;
        }

        else{
            x->parent->parent->left = x;
            aux = x->parent;
            x->parent = aux->parent;
        }

        x->left = aux;
        x->left->parent = x;
        x->left->right = NULL;
        x->left->left = NULL;
    }

    else{
        aux = x->parent;

        x->left = aux;
        x->parent = NULL;
        x->left->parent = x;
        x->left->right = NULL;
        x->left->left = NULL;
    }

    return x;
}

rbNode *rightRotation(rbNode *x){
    bool right;
    rbNode *aux;

    if (x->parent->parent != NULL){
        if (x->parent->parent->right = x->parent){
            right = true;
        }

        else{
            right = false;
        }

        if(right){
            printf("\nCai no right\n");
            x->parent->parent->right = x;
            aux = x->parent;
            x->parent = aux->parent;
        }

        else{
            x->parent->parent->left = x;
            aux = x->parent;
            x->parent = aux->parent;
        }

        x->right = aux;
        x->right->parent = x;
        x->right->right = NULL;
        x->right->left = NULL;
    }

    else{
        printf("to aqui");
        aux = x->parent;

        x->right = aux;
        x->parent = NULL;
        x->right->parent = x;
        x->right->right = NULL;
        x->right->left = NULL;
    }

    return x;
}

//########################################################################################
//                                  Inserção
//######################################################################################## 

rbNode *createRoot(int doi, char *author_name, char *title, int pub_year){
    // Alocando as estruturas
    rbNode *rootNode = (rbNode*)malloc(sizeof(rbNode));
    bookData *book = (bookData*)malloc(sizeof(bookData));

    // Adicionando os dados do livro
    book->doi = doi;
    strcpy(book->author_name, author_name);
    strcpy(book->title, title);
    book->pub_year = pub_year;

    // Criando node
    rootNode->book = book;
    rootNode->key = rootNode->book->doi;
    rootNode->color = BLACK;
    rootNode->parent = NULL; // Se parent  == NULL -> Root node
    rootNode->left = NULL;
    rootNode->right = NULL;

    return rootNode;
}

rbNode *createNode(int doi, char *author_name, char *title, int pub_year){
    // Alocando as estruturas
    rbNode *newNode = (rbNode*)malloc(sizeof(rbNode));
    bookData *book = (bookData*)malloc(sizeof(bookData));

    // Adicionando os dados do livro
    book->doi = doi;
    strcpy(book->author_name, author_name);
    strcpy(book->title, title);
    book->pub_year = pub_year;

    // Criando node
    newNode->book = book;
    newNode->key = newNode->book->doi;
    newNode->color = RED;
    newNode->parent = NULL;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

rbNode *insert_recursion(rbNode *root, int doi, char *author_name, char *title, int pub_year){
    
    if (root == NULL){
        return createNode(doi, author_name, title, pub_year);
    }

    else if(doi > root->key){
        printf("OI2!");
        root->right = insert_recursion(root->right, doi, author_name, title, pub_year);
        root->right->parent = root;
    }

    else if (doi < root->key){
        printf("OI!");
        root->left = insert_recursion(root->left, doi, author_name, title, pub_year);
        root->left->parent = root;
    }

    return root;
}

rbNode *insertRB(rbNode *root, int doi, char *author_name, char *title, int pub_year){
    if (root == NULL){
        // Criar o primeiro root
        return createRoot(doi, author_name, title, pub_year);
    }
    else{
        return insert_recursion(root, doi, author_name, title, pub_year);
    }
}

//########################################################################################
//                                  DEBUG!
//########################################################################################

void printRBTRee(rbNode *root){

    rbNode *aux;
    int op;

    if(root == NULL){
        printf("\nArvore vazia!\n");
    }

    else{
        aux = root;
        while (aux) {
            printf("\n----Node Info----\n");
            printf("Node adress: %p\n", aux);
            printf("Key: %d\n", aux->key);
            printf("Color: %d\n", aux->color);
            printf("Parent: %p\n", aux->parent);
            printf("Right: %p\n", aux->right);
            printf("Left: %p\n", aux->left);
            printf("\n----Book Info----\n");
            printf("DOI: %d\n", aux->book->doi);
            printf("Author name: %s\n", aux->book->author_name);
            printf("Title: %s\n", aux->book->title);
            printf("Pub Year: %d\n", aux->book->pub_year);
            printf("\n(1) <- Left || Right -> (2)\n");
            printf("Answer: ");
            scanf("%d", &op);

            switch (op) {
                case 1:
                    aux = aux->left;
                    break;

                case 2:
                    aux = aux->right;
                    break;
            
                default:
                    aux = aux->right;
                    break;
            }
        }

        printf("\nFim\n");
    }
}

int main(){
    
    rbNode *root = NULL;


    root = insertRB(root, 0, "Jobiscreudo", "desventuras de jobiscreudo", 2020);
    root = insertRB(root, -1, "Jobiscreudo1", "desventuras de jobiscreudo1", 2021);
    //root = insertRB(root, 0, "Jobiscreudo2", "desventuras de jobiscreudo2", 2022);
    //root = insertRB(root, -1, "Jobiscreudo3", "desventuras de jobiscreudo3", 2023);
    root = insertRB(root, -2, "Jobiscreudo4", "desventuras de jobiscreudo4", 2024);
    root = insertRB(root, -3, "Jobiscreudo4", "desventuras de jobiscreudo4", 2024);

    printRBTRee(root);

    return 0;
}