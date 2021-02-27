#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Quantidade de Bytes para cada tipo de dados
#define DOI_SIZE 10
#define AUTHOR_NAME_SIZE 64
#define TITLE_SIZE 64
#define YEAR_SIZE 10

/*
    ---- Cálculo do jump size ----
    char doi[10] -> 10 bytes;
    char author_name[64] -> 64 bytes;
    char title[64] - > 64 bytes;
    char year[10] -> 10 bytes

    JUMP_SIZE = 10 + 64 + 64 + 10 = 148 bytes por pulo
*/

#define JUMP_SIZE (DOI_SIZE + AUTHOR_NAME_SIZE + TITLE_SIZE + YEAR_SIZE)

// Nome dos arquivos que são gerados pelo programa
#define FILE_NAME "data.bin"
#define REWRITE_FILE_NAME "RWdata.bin"

// Utilitário para limpar o terminal independente do OS
#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

//Struct da AVL
typedef struct avl_tree{
    int key;
    unsigned int data_on_file_start_byte;
    struct avl_tree *left;
    struct avl_tree *right;
    int height;
}avl_node;

// Variável global que indica aonde deve começar a escrever os novos dados.
// Utilizada como multiplicador no JUMP_SIZE
int file_bytes = 0;

//########################## HEADERS Árvore AVL ##############################################
int max(int a, int b);
int height(avl_node *n);
int getBalance(avl_node *N);
avl_node* rightRotate(avl_node *y);
avl_node* leftRotate(avl_node *x);
avl_node* minValueNode(avl_node *node);
avl_node* newNode(int key);
avl_node* insert(avl_node* node, int key);
avl_node* deleteNode(avl_node* root, int key);
unsigned int search(avl_node *root, int key);
void printTree(avl_node *root);

//########################## HEADERS Gerenciamento de arquivo  #################################
avl_node* insertNewData(avl_node *root);
void recoverFromFile(avl_node *root, int key);
void editFromFile(avl_node *root, int key);
avl_node* rebuild_tree_from_file(avl_node *root);
void preOrder(avl_node *root, FILE *fp, FILE *fp2);
void saveAndQuit(avl_node *root);


int main(){

    system(CLEAR);

    avl_node *root = NULL; 
    root = rebuild_tree_from_file(root);

    if (root == NULL){
        printf("\n\t---- AVISO ----\n");
        printf("Nenhum arquivo encontrado, criando nova base de dados....\n\n");
    }

    bool exit = false;
    int op;
    int key;

    while (!exit) {
        printf("\n\t---- Bem-vindo! ----\n\n");
        printf("Selecione uma das opcoes:\n");
        printf("(1) - Criar novo livro\n");
        printf("(2) - Editar livro existente\n");
        printf("(3) - Mostrar livro existente\n");
        printf("(4) - REMOVER livro existente\n");
        printf("(5) - Finalizar programa e salvar banco de dados\n\n");
        printf("Resposta: ");
        scanf("%d", &op);

        switch (op){
            case 1:
                root = insertNewData(root);
                break;

            case 2:
                printf("\nDigite o DOI: ");
                setbuf(stdin, NULL);
                scanf("%d", &key);
                editFromFile(root, key);
                break;

            case 3:
                printf("\nDigite o DOI: ");
                setbuf(stdin, NULL);
                scanf("%d", &key);
                recoverFromFile(root, key);
                break;

            case 4:
                printf("\nDigite o DOI: ");
                setbuf(stdin, NULL);
                scanf("%d", &key);

                if(search(root, key) == -1)
                    printf("\nDOI informado ja se encontra no sistema!\n");
                else
                    root = deleteNode(root, key);
                break;

            case 5:
                printf("Finalizando...\n");
                saveAndQuit(root);
                exit = true;
                break;

            case -1: // Opção de DEBUG sempre faz bem
                printf("\n----Iniciando DEBUG da arvore----\n");
                printTree(root);
                break;
                
            default:
                printf("\nOpcao invalida....\n");
                break;
        }
    }

    return 0;
}

//############################################################################################
//                                      Utilitários
//############################################################################################

// Retorna o maior valor entre A e B
int max(int a, int b){ 
    return (a > b)? a : b; 
}

// Retorna a altura do node
int height(avl_node *n){ 
    if (n == NULL){
        return 0;
    }
    return n->height; 
}

// Calcula o balanceamento do node
int getBalance(avl_node *N){ 
    if (N == NULL){
        return 0;
    }

    return height(N->left) - height(N->right); 
}

// Rotação a direita
avl_node* rightRotate(avl_node *y){ 
    avl_node *x = y->left; 
    avl_node *T2 = x->right; 
  
    x->right = y; 
    y->left = T2; 
  
    y->height = max(height(y->left), height(y->right))+1; 
    x->height = max(height(x->left), height(x->right))+1; 
  
    return x; 
}

// Rotação a esquerda
avl_node* leftRotate(avl_node *x){ 
    avl_node *y = x->right; 
    avl_node *T2 = y->left; 
  
    y->left = x; 
    x->right = T2; 
  
    x->height = max(height(x->left), height(x->right))+1; 
    y->height = max(height(y->left), height(y->right))+1; 
  
    return y; 
}

// Encontra o node de menor valor (usado na remoção de um node)
avl_node* minValueNode(avl_node *node){ 
    avl_node *current = node; 
  
    while (current->left != NULL) {
        current = current->left;
    }
         
    return current; 
}

//############################################################################################
//                               Árvore - Operações
//############################################################################################

// Cria novo node
avl_node* newNode(int key){ 
    avl_node* node = (avl_node*)malloc(sizeof(avl_node)); 
    node->key   = key; 
    node->left   = NULL; 
    node->right  = NULL; 
    node->height = 1;
    node->data_on_file_start_byte = file_bytes * JUMP_SIZE; // TODO - Check me later
    file_bytes++;
    return(node); 
}

// Adicionada um novo node na árvore
avl_node* insert(avl_node* node, int key){ 
    if (node == NULL){
        return(newNode(key));
    }
        
    if (key < node->key){
        node->left  = insert(node->left, key);
    }

    else if (key > node->key){
        node->right = insert(node->right, key);
    }
         
    else{ // Proibe inserção de chaves repetidas
        return node;
    } 
        
  
    node->height = 1 + max(height(node->left), height(node->right)); 
  
    int balance = getBalance(node); 
  
    if (balance > 1 && key < node->left->key){
        return rightRotate(node);
    }
         
    if (balance < -1 && key > node->right->key){
        return leftRotate(node);
    }
         
    if (balance > 1 && key > node->left->key){ 
        node->left =  leftRotate(node->left); 
        return rightRotate(node); 
    } 
  
    if (balance < -1 && key < node->right->key){ 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    } 

    return node; 
} 

// Remove node da árvore
avl_node* deleteNode(avl_node* root, int key){ 

    if (root == NULL){
        return root;
    }
         
    if (key < root->key){
        root->left = deleteNode(root->left, key);
    } 
         
    else if(key > root->key){
        root->right = deleteNode(root->right, key);
    } 
         
    else{  
        if((root->left == NULL) || (root->right == NULL)){ 
            avl_node *temp = root->left ? root->left : 
                                             root->right; 
  
            if (temp == NULL){ 
                temp = root; 
                root = NULL; 
            } 
            else{
                *root = *temp; 
            }

            free(temp); 
        } 
        else{ 
            avl_node *temp = minValueNode(root->right); 
  
            root->key = temp->key; 

            root->right = deleteNode(root->right, temp->key); 
        } 
    } 
  
    if (root == NULL){
        return root; 
    }
      
    root->height = 1 + max(height(root->left), height(root->right)); 
  
    int balance = getBalance(root); 
  
    if (balance > 1 && getBalance(root->left) >= 0){
        return rightRotate(root);
    } 
         
    if (balance > 1 && getBalance(root->left) < 0){ 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
  
    if (balance < -1 && getBalance(root->right) <= 0){
        return leftRotate(root);
    }
         
    if (balance < -1 && getBalance(root->right) > 0){ 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    } 
  
    return root; 
}

// Realiza a busca de um nó pelo DOI, e retorna o byte onde este DOI está no arquivo
unsigned int search(avl_node *root, int key){

    if(root == NULL){
        return -1; //Não encontrado
    }

    if(key == root->key){
        return root->data_on_file_start_byte; //Encontrado
    }

    else if(key > root->key){
        search(root->right, key);
    }

    else{
        search(root->left, key);
    }
}

//############################################################################################
//                               Gerenciamento do arquivo
//############################################################################################

// Utilitário que realiza toda a inserção dentro da árvore e arquivo
avl_node* insertNewData(avl_node *root){
    FILE *fp;
    
    int doi;
    char doiStr[DOI_SIZE];
    char author_name[AUTHOR_NAME_SIZE];
    char title[TITLE_SIZE];
    int year;
    char yearStr[YEAR_SIZE];

    system(CLEAR);
    printf("\t----- Insercao -----\n\n");

    printf("Digite o DOI: ");
    setbuf(stdin, NULL);
    scanf("%d", &doi);
    snprintf(doiStr, sizeof(doiStr), "%d", doi);
    strcat(doiStr, "\n");

    printf("Digite o nome do autor: ");
    setbuf(stdin, NULL);
    scanf("%[^\n]s", author_name);
    strcat(author_name, "\n");

    printf("Digite o titulo do livro: ");
    setbuf(stdin, NULL);
    scanf("%[^\n]s", title);
    strcat(title, "\n");

    printf("Digite o ano de publicacao: ");
    setbuf(stdin, NULL);
    scanf("%d", &year);
    snprintf(yearStr, sizeof(yearStr), "%d", year);
    strcat(yearStr, "\n");

    root = insert(root, doi);

    if((fp = fopen(FILE_NAME, "a+b")) == NULL){ // Se o arquivo não existir, crie-o
        fclose(fp);
        fp = fopen(FILE_NAME, "w+b");
    }

    fwrite(doiStr, sizeof(doiStr), sizeof(char), fp);
    fwrite(author_name, sizeof(author_name), sizeof(char), fp);
    fwrite(title, sizeof(title), sizeof(char), fp);
    fwrite(yearStr, sizeof(yearStr), sizeof(char), fp);
    fclose(fp);

    return root;
}

// Recupera as informações do arquivo dada uma certa chave
void recoverFromFile(avl_node *root, int key){
    FILE *fp;
    
    unsigned int startByte = search(root, key);

    if(startByte == -1){
        printf("\n\tDOI informado não se econtra presente no sistema!\n");
    }

    else{
        if((fp = fopen(FILE_NAME, "r+b")) == NULL){ // Se o arquivo não existir
            printf("\n\n ERRO NA ABERTURA DO ARQUIVO! \n ARQUIVO INEXISTENTE OU CORROMPIDO!\n");
        }

        else{
            char readBuffer[64];

            printf("\n----Resultado da busca----\n");

            fseek(fp, startByte, 0);
            strcpy(readBuffer, "");
            fread(readBuffer, DOI_SIZE, sizeof(char), fp);
            printf("DOI: %s", readBuffer);

            startByte += DOI_SIZE;

            fseek(fp, startByte, 0);
            strcpy(readBuffer, "");
            fread(readBuffer, AUTHOR_NAME_SIZE, sizeof(char), fp);
            printf("Nome do autor: %s", readBuffer);

            startByte += AUTHOR_NAME_SIZE;

            fseek(fp, startByte, 0);
            strcpy(readBuffer, "");
            fread(readBuffer, TITLE_SIZE, sizeof(char), fp);
            printf("Titulo: %s", readBuffer);

            startByte += TITLE_SIZE;

            fseek(fp, startByte, 0);
            strcpy(readBuffer, "");
            fread(readBuffer, YEAR_SIZE, sizeof(char), fp);
            printf("Ano: %s", readBuffer);

            fclose(fp);
        }
    }
}

// Edita os valores de um certo dado dentro do arquivo, este dado é encontrado pelo DOI
void editFromFile(avl_node *root, int key){
    FILE *fp;
    
    unsigned int startByte = search(root, key);

    if(startByte == -1){
        printf("\n\tDOI informado não se econtra presente no sistema!\n");
    }

    else{
        if ((fp = fopen(FILE_NAME, "r+b")) == NULL){
            printf("\n\n ERRO NA ABERTURA DO ARQUIVO! \n ARQUIVO INEXISTENTE OU CORROMPIDO!\n");
        }
        else{
            printf("\n----Dados anteriores----\n");
            recoverFromFile(root, key);

            char author_name[AUTHOR_NAME_SIZE];
            char title[TITLE_SIZE];
            int year;
            char yearStr[YEAR_SIZE];

            printf("Digite o nome do autor: ");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", author_name);
            strcat(author_name, "\n");

            printf("Digite o titulo do livro: ");
            setbuf(stdin, NULL);
            scanf("%[^\n]s", title);
            strcat(title, "\n");

            printf("Digite o ano de publicacao: ");
            scanf("%d", &year);
            snprintf(yearStr, sizeof(yearStr), "%d", year);
            strcat(yearStr, "\n");

            //fseek(fp, startByte, 0);
            //fputs(doiStr, fp);

            startByte += DOI_SIZE;

            fseek(fp, startByte, 0);
            fputs(author_name, fp);

            startByte += AUTHOR_NAME_SIZE;

            fseek(fp, startByte, 0);
            fputs(title, fp);

            startByte += TITLE_SIZE;

            fseek(fp, startByte, 0);
            fputs(yearStr, fp);

            fclose(fp);
        }
    }
}

// Reconstroi a árvore caso exista previamente um arquivo de dados
avl_node* rebuild_tree_from_file(avl_node *root){
    FILE *fp;

    if((fp = fopen(FILE_NAME, "r+b")) == NULL){
        return NULL;
    }

    else{
        unsigned int auxByte;
        char buffer[DOI_SIZE];
        int auxKey;
        bool stop = false;
        int cnt = 0;
        while (!stop){
            auxByte = cnt * JUMP_SIZE;
            fseek(fp, auxByte, 0);
            fread(buffer, DOI_SIZE, sizeof(char), fp);
            auxKey = atoi(buffer);

            root = insert(root, auxKey);
            cnt++;

            if(getc(fp) != EOF){
                fseek(fp ,auxByte, 0);
            }
            else{
                stop = true;
            }
        }

        fclose(fp);
        return root;
    }
}

// Utilizado para percorrer a árvore toda e escrever os valores referentes a cada node no arquivo
void preOrder(avl_node *root, FILE *fp, FILE *fp2){ 
    if(root != NULL){ 
        char readBuffer[64];
        int auxByte = root->data_on_file_start_byte;

        fseek(fp, auxByte, 0);
        strcpy(readBuffer, "");
        fread(readBuffer, DOI_SIZE, sizeof(char), fp);
        fwrite(readBuffer, DOI_SIZE, sizeof(char), fp2);

        auxByte += DOI_SIZE;

        fseek(fp, auxByte, 0);
        strcpy(readBuffer, "");
        fread(readBuffer, AUTHOR_NAME_SIZE, sizeof(char), fp);
        fwrite(readBuffer, AUTHOR_NAME_SIZE, sizeof(char), fp2);

        auxByte += AUTHOR_NAME_SIZE;

        fseek(fp, auxByte, 0);
        strcpy(readBuffer, "");
        fread(readBuffer, TITLE_SIZE, sizeof(char), fp);
        fwrite(readBuffer, TITLE_SIZE, sizeof(char), fp2);

        auxByte += TITLE_SIZE;

        fseek(fp, auxByte, 0);
        strcpy(readBuffer, "");
        fread(readBuffer, YEAR_SIZE, sizeof(char), fp);
        fwrite(readBuffer, YEAR_SIZE, sizeof(char), fp2);
        
        preOrder(root->left, fp, fp2); 
        preOrder(root->right, fp, fp2); 
    } 
}

// Chamaa rotina de salvar o arquivo e finalizar o programa
void saveAndQuit(avl_node *root){
    FILE *fp;
    FILE *fp2;

    if(root == NULL){
        printf("\nArvore vazia, nada sera salvo...\n");
    }

    else if ((fp2 = fopen(REWRITE_FILE_NAME, "w+b")) == NULL){
        printf("\nDeu merda ao salvar o arquivo :(\n");
    }

    else{
        if((fp = fopen(FILE_NAME, "r+b")) == NULL){
            printf("DEu outra merda");
        }

        else{
            preOrder(root, fp, fp2);
            fclose(fp);
            fclose(fp2);
            remove(FILE_NAME);
            rename(REWRITE_FILE_NAME, FILE_NAME);
        }
    }
}

//############################################################################################
//                                      DEBUG!
//############################################################################################

// Utiliada para navegar diretamente pela árvore, apenas usada para casos de DEBUG
void printTree(avl_node *root){
    avl_node *aux = root;
    int op;

    if(!aux){
        printf("Árvore vazia!");
    }

    else{
        while (aux){
            printf("\n------Node info------\n");
            printf("Node addres: %p\n", aux);
            printf("Key: %d\n", aux->key);
            printf("Height: %d\n", aux->height);
            printf("Start Byte: %d\n", aux->data_on_file_start_byte);
            printf("Left: %p\n", aux->left);
            printf("Right: %p\n", aux->right);
            printf("\n<- (1) || (2) ->\n");
            printf("Answer: ");
            scanf("%d", &op);

            switch (op){
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
    }
}