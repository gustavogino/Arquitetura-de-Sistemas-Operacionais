/******************************************************
 *          Gustavo Gino Scotton - 15201974           *
 *      VPL2.1 - Localizar e rotacionar inteiro       *
 ******************************************************/
 
#include <stdio.h> 
#include <stdlib.h> 

// Define funções que vão ser usadas
FILE* OpenFile(const char* path, const char* mode);
int GetSize(FILE* f);

int main() {

    FILE *fs = OpenFile("arquivo.bin", "rb");           // Abre o arquivo   
    int fsize = GetSize(fs) / sizeof(int);              // Pega a quantidade de inteiros
    int *buffer = (int*)malloc(sizeof(int)*fsize);      // Aloca memória para o buffer (quantidade de inteiros * 4bytes)
    size_t r = fread(buffer, sizeof(int), fsize, fs);   // Le o arquivo para o buffer
    fclose(fs);                                         // Fecha o arquivo aberto
    
    if (r != fsize) printf("Erro ao ler arquivo");      // Verifica se o tamanho lido é o mesmo do tamanho do arquivo 

    for (int i = 0; i < fsize; i++)                     // Verifica cada inteiro esccrito no arquivo
        if(buffer[i] == 6664)                           // Verifica se o inteiro da posição é "6664"    
            buffer[i] = buffer[i] >> 1;                 // Caso seja, faz um Right Shift
    
    
    fs = OpenFile("arquivo.bin", "wb");                 // Abre novamente o arquivo, apagando todo o conteudo para escrever novamente
    fwrite (buffer , sizeof(int), fsize, fs);           // Escreve nosso buffer no arquivo
    free(buffer);                                       // Libera a memória alocada pro buffer
    fclose(fs);                                         // Fecha o arquivo
    
    return 0;
}


int GetSize(FILE* f){
    fseek (f , 0 , SEEK_END);
    long LongSize = ftell(f);
    rewind(f);
    return LongSize;
}

FILE* OpenFile(const char* path, const char* mode){
    FILE* f = fopen(path, mode);

    if (f == NULL)
    {
        printf("Erro ao abrir arquivo");
        exit(1);
    }
    
    return f;
}
