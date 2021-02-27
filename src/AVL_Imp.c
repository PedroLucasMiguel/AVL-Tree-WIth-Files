#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
    char doi[10] -> 10 bytes;
    char author_name[64] -> 64 bytes;
    char title[64] - > 64 bytes;
    char year[10] -> 10 bytes

    JUMP_SIZE = 10 + 64 + 64 + 10 = 148 bytes por pulo
*/

// Esta parte pode ser configurável!
#define DOI_SIZE 10
#define AUTHOR_NAME_SIZE 64
#define TITLE_SIZE 64
#define YEAR_SIZE 10

#define JUMP_SIZE (DOI_SIZE + AUTHOR_NAME_SIZE + TITLE_SIZE + YEAR_SIZE)
#define FILE_NAME "data.bin"
#define REWRITE_FILE_NAME "RWdata.bin"

// Apenas para facilitar limpeza de tela :)
#ifdef _WIN32
#define CLEAR "cls"
#else //In any other OS
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

//unsigned int saveAndExitByte = 0;

//######################################## HEADERS AVL ###################################
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

//######################################## HEADERS Arquivo #################################
avl_node* insertNewData(avl_node *root);
void recoverFromFile(avl_node *root, int key);
void editFromFile(avl_node *root, int key);
avl_node* rebuild_tree_from_file(avl_node *root);
void preOrder(avl_node *root, FILE *fp, FILE *fp2);
void saveAndQuit(avl_node *root);


int main(){

    //printf("%d", JUMP_SIZE);

    avl_node *root = NULL;

    //root = insertNewData(root);
    //root = insertNewData(root);
    //root = insertNewData(root);
    //root = insertNewData(root);
    //root = insertNewData(root);
    //root = deleteNode(root, 4);
    //printTree(root);
    //saveAndQuit(root);
    root = rebuild_tree_from_file(root);
    printTree(root);
    recoverFromFile(root, 2);

    //editFromFile(root, 2);

    //recoverFromFile(root, 2);
    //recoverFromFile(root, 2);

    return 0;
}

//############################################################################################
//                                      Utilitários
//############################################################################################

int max(int a, int b){ 
    return (a > b)? a : b; 
}

int height(avl_node *n){ 
    if (n == NULL){
        return 0;
    }
    return n->height; 
}

int getBalance(avl_node *N){ 
    if (N == NULL){
        return 0;
    }

    return height(N->left) - height(N->right); 
}

avl_node* rightRotate(avl_node *y){ 
    avl_node *x = y->left; 
    avl_node *T2 = x->right; 
  
    // Perform rotation 
    x->right = y; 
    y->left = T2; 
  
    // Update heights 
    y->height = max(height(y->left), height(y->right))+1; 
    x->height = max(height(x->left), height(x->right))+1; 
  
    // Return new root 
    return x; 
}

avl_node* leftRotate(avl_node *x){ 
    avl_node *y = x->right; 
    avl_node *T2 = y->left; 
  
    // Perform rotation 
    y->left = x; 
    x->right = T2; 
  
    //  Update heights 
    x->height = max(height(x->left), height(x->right))+1; 
    y->height = max(height(y->left), height(y->right))+1; 
  
    // Return new root 
    return y; 
}

avl_node* minValueNode(avl_node *node){ 
    avl_node *current = node; 
  
    /* loop down to find the leftmost leaf */
    while (current->left != NULL) {
        current = current->left;
    }
         
    return current; 
}

//############################################################################################
//                               Árvore - Operações
//############################################################################################

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

avl_node* insert(avl_node* node, int key){ 
    /* 1.  Perform the normal BST rotation */
    if (node == NULL){
        return(newNode(key));
    }
        
    if (key < node->key){
        node->left  = insert(node->left, key);
    }

    else if (key > node->key){
        node->right = insert(node->right, key);
    }
         
    else{
        return node;
    } // Equal keys not allowed 
        
  
    /* 2. Update height of this ancestor node */
    node->height = 1 + max(height(node->left), height(node->right)); 
  
    /* 3. Get the balance factor of this ancestor 
          node to check whether this node became 
          unbalanced */
    int balance = getBalance(node); 
  
    // If this node becomes unbalanced, then there are 4 cases 
  
    // Left Left Case 
    if (balance > 1 && key < node->left->key){
        return rightRotate(node);
    }
         
  
    // Right Right Case 
    if (balance < -1 && key > node->right->key){
        return leftRotate(node);
    }
         
    // Left Right Case 
    if (balance > 1 && key > node->left->key){ 
        node->left =  leftRotate(node->left); 
        return rightRotate(node); 
    } 
  
    // Right Left Case 
    if (balance < -1 && key < node->right->key){ 
        node->right = rightRotate(node->right); 
        return leftRotate(node); 
    } 
  
    /* return the (unchanged) node pointer */
    return node; 
} 

avl_node* deleteNode(avl_node* root, int key){ 
    // STEP 1: PERFORM STANDARD BST DELETE 
  
    if (root == NULL){
        return root;
    }
         
    // If the key to be deleted is smaller than the 
    // root's key, then it lies in left subtree 
    if (key < root->key){
        root->left = deleteNode(root->left, key);
    } 
         
  
    // If the key to be deleted is greater than the 
    // root's key, then it lies in right subtree 
    else if(key > root->key){
        root->right = deleteNode(root->right, key);
    } 
         
  
    // if key is same as root's key, then This is 
    // the node to be deleted 
    else{ 
        // node with only one child or no child 
        if((root->left == NULL) || (root->right == NULL)){ 
            avl_node *temp = root->left ? root->left : 
                                             root->right; 
  
            // No child case 
            if (temp == NULL){ 
                temp = root; 
                root = NULL; 
            } 
            else{
                *root = *temp; 
            } // One child case 
             // Copy the contents of 
            // the non-empty child 
            free(temp); 
        } 
        else{ 
            // node with two children: Get the inorder 
            // successor (smallest in the right subtree) 
            avl_node *temp = minValueNode(root->right); 
  
            // Copy the inorder successor's data to this node 
            root->key = temp->key; 
  
            // Delete the inorder successor 
            root->right = deleteNode(root->right, temp->key); 
        } 
    } 
  
    // If the tree had only one node then return 
    if (root == NULL){
        return root; 
    }
      
    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE 
    root->height = 1 + max(height(root->left), height(root->right)); 
  
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to 
    // check whether this node became unbalanced) 
    int balance = getBalance(root); 
  
    // If this node becomes unbalanced, then there are 4 cases 
  
    // Left Left Case 
    if (balance > 1 && getBalance(root->left) >= 0){
        return rightRotate(root);
    } 
         
  
    // Left Right Case 
    if (balance > 1 && getBalance(root->left) < 0){ 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
  
    // Right Right Case 
    if (balance < -1 && getBalance(root->right) <= 0){
        return leftRotate(root);
    }
         
    // Right Left Case 
    if (balance < -1 && getBalance(root->right) > 0){ 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    } 
  
    return root; 
}

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
avl_node* insertNewData(avl_node *root){
    FILE *fp;
    
    int doi;
    char doiStr[DOI_SIZE];
    char author_name[AUTHOR_NAME_SIZE];
    char title[TITLE_SIZE];
    int year;
    char yearStr[YEAR_SIZE];

    system(CLEAR);
    printf("\t----- Insersao -----\n\n");

    printf("Digite o DOI: ");
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
    scanf("%d", &year);
    snprintf(yearStr, sizeof(yearStr), "%d", year);
    strcat(yearStr, "\n");

    root = insert(root, doi);

    if((fp = fopen(FILE_NAME, "a+b")) == NULL){ // Se o arquivo existir
        fclose(fp);
        fp = fopen(FILE_NAME, "w+b");
    }

    fwrite(doiStr, sizeof(doiStr), sizeof(char), fp);
    fwrite(author_name, sizeof(author_name), sizeof(char), fp);
    fwrite(title, sizeof(title), sizeof(char), fp);
    fwrite(yearStr, sizeof(yearStr), sizeof(char), fp);
    fclose(fp);

    file_bytes++;

    return root;
}

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
            /*
                Realizei o bloqueio para permitir que edito o doi, pois vai ser um inferno
                do kct, caso o cara faça isso, pq a árvore vai quebrar muito hard

                printf("Digite o DOI: ");
                scanf("%d", &doi);
                snprintf(doiStr, sizeof(doiStr), "%d", doi);
                strcat(doiStr, "\n");
            */
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

avl_node* rebuild_tree_from_file(avl_node *root){
    FILE *fp;

    if((fp = fopen(FILE_NAME, "r+b")) == NULL){
        return NULL;
    }

    else{
        unsigned int auxByte;
        char buffer[DOI_SIZE];
        int auxKey;
        bool teste = false;

        while (!teste){
            auxByte = file_bytes * JUMP_SIZE;
            fseek(fp, auxByte, 0);
            fread(buffer, DOI_SIZE, sizeof(char), fp);
            auxKey = atoi(buffer);

            root = insert(root, auxKey);

            if(getc(fp) != EOF){
                fseek(fp ,auxByte, 0);
            }
            else{
                teste = true;
            }
        }

        fclose(fp);
        return root;
    }
}

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


void saveAndQuit(avl_node *root){
    FILE *fp;
    FILE *fp2;

    if ((fp2 = fopen(REWRITE_FILE_NAME, "w+b")) == NULL){
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