 /******************************************************
 *            Gustavo Gino Scotton - 15201974          *
 *      Sistema de arquivos que simula padrão EXT3     *
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "fs.h"

FILE* CreateFile(std::string path);
FILE* OpenFile(std::string path);
std::vector<std::string> separaFilePath(std::string filename);

void initFs(std::string fsFileName, int blockSize, int numBlocks, int numInodes)
{
    char mByte = 0x01;
    char MapaSize = floor((numBlocks - 1) / 8);
    int iVetorSize = (numInodes - 1) * sizeof(INODE);
    int bVetorSize = blockSize * numBlocks;
    INODE iRaiz = { 1, 1, "/", 0, {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
    int VazioSize;
    
    if(iVetorSize >= bVetorSize)
        VazioSize = iVetorSize;
    else
        VazioSize =  bVetorSize;

    char *Vazio = (char*)malloc(VazioSize * sizeof(char));

    for (int i = 0; i < VazioSize; i++)
            Vazio[i] = 0;
        
    FILE* file = CreateFile(fsFileName);
    {
        fwrite(&blockSize, sizeof(char), 1, file);
        fwrite(&numBlocks, sizeof(char), 1, file);
        fwrite(&numInodes, sizeof(char), 1, file);
        fwrite(&mByte, sizeof(char), 1, file);
        fwrite(Vazio, sizeof(char), MapaSize, file);
        fwrite(&iRaiz, sizeof(char), sizeof(INODE), file);
        fwrite(Vazio, sizeof(char), iVetorSize, file);
        fwrite(Vazio, sizeof(char), 1, file);
        fwrite(Vazio, sizeof(char), bVetorSize, file);
    }
    fclose(file);
    free(Vazio);
}


void addFile(std::string fsFileName, std::string filePath, std::string fileContent)
{
    FILE* fs = OpenFile(fsFileName);
    
    int i, j;
    char c[10];
    fread(c, sizeof(char), 3, fs);
    
    int tamanhoBlocos = c[0]; 
    int numeroBlocos = c[1];
    int numInodes = c[2];
    
    int inicioInodes = 4 + floor((numeroBlocos - 1) / 8);
    int inicioBlocos = 1 + inicioInodes + numInodes * sizeof(INODE);
    
    int tamanho = fileContent.length();
    int novosBlocos = 1 + (int)floor((tamanho - 1) / tamanhoBlocos);
    char inodeLivre;
    int inodePai = 0;
    int byteLivre = 0;
    unsigned char blocosDiretos[3];
    
    
    for (i = 0; i < numInodes; i++)
    {
        char ocupado;
        fseek(fs, inicioInodes + i * sizeof(INODE), SEEK_SET);
        fread(&ocupado, sizeof(char), 1, fs);
        if (!ocupado) {
            inodeLivre = (char)i;
            break;
        }
    }

    filePath.erase(0, 1); //remove primeiro "/"
    std::vector<std::string> path = separaFilePath(filePath);
    for (int k = 0; k < path.size(); k++) {
        fseek(fs, inicioInodes + inodePai * sizeof(INODE) + 13, SEEK_SET);
        fread(blocosDiretos, sizeof(char), 3, fs);
        
        for (i = 0; i < 3; i++)
        {
            char blocoId = blocosDiretos[i];
            for (j = 0; j < tamanhoBlocos; j++)
            {
                char inodeId;
                int readPos = inicioBlocos + blocoId * tamanhoBlocos + j;
                fseek(fs, readPos, SEEK_SET);
                fread(&inodeId, sizeof(char), 1, fs);
                
                if (inodeId == 0)
                {
                    byteLivre = readPos;
                    break;
                }
                
                fseek(fs, inicioInodes + inodeId * sizeof(INODE) + 2, SEEK_SET);
                fread(c, sizeof(char), 10, fs);
                if (strcmp(c, path.at(k).c_str()) == 0)
                {
                    inodePai = inodeId;
                    break;
                }
            }
            if (j < tamanhoBlocos) break;
        }
    }
    
    char *blocos = (char*)malloc(novosBlocos * sizeof(char));
    char blocosEncontrados = 0;
    char mascara, mapa;
    
    for (i = 0; i < numeroBlocos; i++)
    {
        fseek(fs, 3 + floor(i / 8), SEEK_SET);
        fread(&mapa, sizeof(char), 1, fs);
        mascara = 1 << (i % 8);
        
        if ((mascara & mapa) == 0)
        {
            blocos[blocosEncontrados] = (char)i;
            blocosEncontrados++;
            if (blocosEncontrados == novosBlocos) break;
        }
    }
    
    for (i = 0; i < novosBlocos; i++)
    {
        fseek(fs, 3 + floor(blocos[i] / 8), SEEK_SET);
        fread(&mapa, sizeof(char), 1, fs);
        mapa = mapa | (1 << (blocos[i] % 8));
        fseek(fs, 3 + floor(blocos[i] / 8), SEEK_SET);
        fwrite(&mapa, sizeof(char), 1, fs);
    }
    
    
    char tamanhoDir;
    fseek(fs, inicioInodes + inodePai * sizeof(INODE) + 12, SEEK_SET);
    fread(&tamanhoDir, sizeof(char), 1, fs);
    tamanhoDir++;
    fseek(fs, -1, SEEK_CUR);
    fwrite(&tamanhoDir, sizeof(char), 1, fs);
    fseek(fs, byteLivre, SEEK_SET);
    fwrite(&inodeLivre, sizeof(unsigned char), 1, fs);
    
    for (i = 0; i < 3; i++)
    {
      if (i < novosBlocos) blocosDiretos[i] = blocos[i];
      else blocosDiretos[i] = 0;
    }
    
    
    INODE inode = {
      1,
      0,
      {0},
      (char)tamanho,
      {blocosDiretos[0], blocosDiretos[1], blocosDiretos[2]},
      {0, 0, 0},
      {0, 0, 0}
    };

    std::string name = path.back();
    for (i = 0; i < name.length(); i++) {
        inode.NAME[i] = name[i];
    }
    fseek(fs, inicioInodes + inodeLivre * sizeof(INODE), SEEK_SET);
    fwrite(&inode, sizeof(INODE), 1, fs);
    
    
    char *dados = (char*)malloc(tamanhoBlocos * sizeof(char));
    
    for (i = 0; i < novosBlocos; i++)
    {
        for(int j = 0; j < tamanhoBlocos; j++) {
            int n = i * novosBlocos + j;
            if (n < fileContent.length()) dados[j] = fileContent[i * novosBlocos + j];
            else dados[j] = 0;
        }
        fseek(fs, inicioBlocos + blocosDiretos[i] * tamanhoBlocos, SEEK_SET);
        fwrite(dados, sizeof(char), tamanhoBlocos, fs);
    }
    
    free(blocos);
    free(dados);
    fclose(fs);
    return;
}


void addDir(std::string fsFileName, std::string dirPath)
{
    FILE* fs = OpenFile(fsFileName);
    
    int i, j;
    char c[10];
    fread(c, sizeof(char), 3, fs);
    
    int tamanhoBlocos = c[0]; 
    int numeroBlocos = c[1];
    int numInodes = c[2];
    
    char inodeLivre;
    int inicioInodes = 4 + floor((numeroBlocos - 1) / 8);
    int inicioBlocos = inicioInodes + numInodes * sizeof(INODE) + 1;
    
    int novosBlocos = 1;
    int inodePai = 0;
    int byteLivre = 0;
    unsigned char blocosDiretos[3];
    
    
    for (i = 0; i < numInodes; i++)
    {
        char ocupado;
        fseek(fs, inicioInodes + i * sizeof(INODE), SEEK_SET);
        fread(&ocupado, sizeof(char), 1, fs);
        if (!ocupado) {
            inodeLivre = (char)i;
            break;
        }
    }

    dirPath.erase(0, 1);       //remove primeiro "/"
    std::vector<std::string> path = separaFilePath(dirPath);
    for (int k = 0; k < path.size(); k++) {
        fseek(fs, inicioInodes + inodePai * sizeof(INODE) + 13, SEEK_SET);
        fread(blocosDiretos, sizeof(char), 3, fs);
        
        for (i = 0; i < 3; i++)
        {
            char blocoId = blocosDiretos[i];
            for (j = 0; j < tamanhoBlocos; j++)
            {
                char inodeId;
                int readPos = inicioBlocos + blocoId * tamanhoBlocos + j;
                fseek(fs, readPos, SEEK_SET);
                fread(&inodeId, sizeof(char), 1, fs);
                
                if (inodeId == 0)
                {
                    byteLivre = inicioBlocos + blocosDiretos[i] * tamanhoBlocos + j;
                    break;
                }

                fseek(fs, inicioInodes + inodeId * sizeof(INODE) + 2, SEEK_SET);
                fread(c, sizeof(char), 10, fs);
                if (strcmp(c, path.at(k).c_str()) == 0)
                {
                    inodePai = inodeId;
                    break;
                }
            }
            if (j < tamanhoBlocos) break;
        }
    }
    
    char *blocos = (char*)malloc(novosBlocos * sizeof(char));
    char blocosEncontrados = 0;
    char mascara, mapa;
    
    for (i = 0; i < numeroBlocos; i++)
    {
        fseek(fs, 3 + floor(i / 8), SEEK_SET);
        fread(&mapa, sizeof(char), 1, fs);
        mascara = 1 << (i % 8);
        
        if ((mascara & mapa) == 0)
        {
            blocos[blocosEncontrados] = (char)i;
            blocosEncontrados++;
            if (blocosEncontrados == novosBlocos) break;
        }
    }
    
    for (i = 0; i < novosBlocos; i++)
    {
        fseek(fs, 3 + floor(blocos[i] / 8), SEEK_SET);
        fread(&mapa, sizeof(char), 1, fs);
        mapa = mapa | (1 << (blocos[i] % 8));
        fseek(fs, 3 + floor(blocos[i] / 8), SEEK_SET);
        fwrite(&mapa, sizeof(char), 1, fs);
    }
    
    
    char tamanhoDir;
    fseek(fs, inicioInodes + inodePai * sizeof(INODE) + 12, SEEK_SET);
    fread(&tamanhoDir, sizeof(char), 1, fs);
    tamanhoDir++;
    fseek(fs, -1, SEEK_CUR);
    fwrite(&tamanhoDir, sizeof(char), 1, fs);
    fseek(fs, byteLivre, SEEK_SET);
    fwrite(&inodeLivre, sizeof(unsigned char), 1, fs);
    
    for (i = 0; i < 3; i++)
    {
        if (i < novosBlocos) blocosDiretos[i] = blocos[i];
        else blocosDiretos[i] = 0;
    }
    
    
    INODE inode = {
        1,
        1,
        {0},
        0,
        {blocosDiretos[0], blocosDiretos[1], blocosDiretos[2]},
        {0, 0, 0},
        {0, 0, 0}
    };

    std::string name = path.back();
    for (i = 0; i < name.length(); i++) {
        inode.NAME[i] = name[i];
    }
    fseek(fs, inicioInodes + inodeLivre * sizeof(INODE), SEEK_SET);
    fwrite(&inode, sizeof(INODE), 1, fs);
    
    free(blocos);
    fclose(fs);
    return;
}


// Utils

FILE* CreateFile(std::string path){
    FILE* f = fopen(path.c_str(), "wb");

    if (f == NULL)
    {
        printf("Erro ao criar arquivo");
        exit(1);
    }
    
    return f;
}

FILE* OpenFile(std::string path){
    FILE* f = fopen(path.c_str(), "rb+");
    
    if (f == NULL)
    {
        printf("Erro ao abrir arquivo");
        exit(1);
    }
    
    return f;
}

std::vector<std::string> separaFilePath(std::string filePath)
{
    std::vector<std::string> vec;
    int pos = 1;
    while (pos != std::string::npos)
    {
        pos = filePath.find(" ");
        vec.push_back(filePath.substr(0, pos));
        filePath.erase(0, pos + 1);
    }
    return vec;
}
