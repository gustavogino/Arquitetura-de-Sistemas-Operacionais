/******************************************************
 *          Gustavo Gino Scotton - 15201974           *
 *         VPL2.2 - Lista encadeada em disco          *
 ******************************************************/

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

typedef struct 
{
	int used; 
    char name[20];
	int next;
} node;

FILE* OpenFile(const char* path, const char* mode);

int main() {

    // Definição de nodos, posições e nomes
    node Empty, Everton, Chico;
    int pEmpty, pEverton, pChico;
    char charEverton[20] = "Everton", charChico[20] = "Chico", charAnderson[20] = "Anderson";


    // Abre o arquivo
    FILE *fs = OpenFile("lista.bin", "r+");  
    
    
    // Ignora os 4 primeiros bytes [1]
    fseek(fs, 4, SEEK_SET); 
    

    // Localiza a posição do nodo vazio
    fread(&Empty, sizeof(node), 1, fs);
    while (Empty.used != 0)
        fread(&Empty, sizeof(node), 1, fs);
    pEmpty = ftell(fs) - sizeof(node);


    // Localiza a posição do nodo Everton
    fseek(fs, 4, SEEK_SET); //[1]
    fread(&Everton, sizeof(node), 1, fs);   
    while (strcmp(Everton.name, charEverton) != 0)
        fread(&Everton, sizeof(node), 1, fs);
    pEverton = ftell(fs) - sizeof(node);


    // Localiza a posição do nodo Chico
    fseek(fs, 4, SEEK_SET); //[1]
    fread(&Chico, sizeof(node), 1, fs);
    while (strcmp(Chico.name, charChico) != 0)
        fread(&Chico, sizeof(node), 1, fs);
    pChico = ftell(fs) - sizeof(node);
    

/*******************************************************
 *                                                     *
 *          "Everton" -> "Empty" -> "Chico"            *
 *                                                     *
 * *****************************************************/

    //Escreve Anderson no nodo vazio e faz apontar para Chico
    Empty.used = 1;
    strcpy(Empty.name, charAnderson);
    Empty.next = pChico;
    fseek(fs, pEmpty, SEEK_SET);
    fwrite(&Empty, sizeof(node), 1, fs);


    // Everton aponta para Empty (Anderson)
    Everton.next = pEmpty;
    fseek(fs, pEverton, SEEK_SET);
    fwrite(&Everton, sizeof(node), 1, fs);
    
    
    // Fecha o arquivo
    fclose(fs);
    return 0;
}

// Função para abrir arquivo com filtro de erros
FILE* OpenFile(const char* path, const char* mode){
    FILE* f = fopen(path, mode);

    if (f == NULL)
    {
        printf("Erro ao abrir arquivo");
        exit(1);
    }
    
    return f;
}
