#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef unsigned char ElemType;
typedef struct
{
    ElemType c;
    unsigned int weight;
}CharUnit,*CharArray;
typedef struct
{
    CharUnit Contant;
    unsigned int parent,lchild,rchild;
}HTNode,*HuffmanTree;

typedef struct HC_HEAD
{
	unsigned char flag[3];		    //head flag
	unsigned char CharKind;		    //number of the kinds of the char
	unsigned char lastValidBit;		//valid bits of the last byte
    char name[59];                  //name
} HUF_FILE_HEAD;	

typedef char * * HuffmanCode;

void Select(HuffmanTree &HT,int k,int &s1,int &s2);

void HuffmanCoding(HuffmanTree &HT,HuffmanCode &HC,CharArray w,int n)
{
    if(n<=1)
    {
        printf("this file has only one Character\n");
        return;
    }

    int m=2*n-1;
    HT=(HuffmanTree)malloc((m+1)*sizeof(HTNode));       //unit 0 is unused

    HTNode *p;
    int i;

    for(p=HT+1,i=1,w++;i<=n;++i,++p,++w)
        *p={*w,0,0,0};

    for(;i<=m;++i,++p)
        *p={{0,0},0,0,0};

    for(i=n+1;i<=m;++i)
    {//initilize a Huffman Tree
        //Find two nodes in HT[1..i-1] whose 
        //parent is 0 and who has the lowest weight
        //their no. are s1 and s2
        int s1,s2;
        Select(HT,i-1,s1,s2);

        HT[s1].parent=i;
        HT[s2].parent=i;

        HT[i].lchild=s1;
        HT[i].rchild=s2;

        HT[i].Contant.weight=HT[s1].Contant.weight+HT[s2].Contant.weight;
    }

    //-----find the huffman code for each character reversely from leaves to root-----
    HC=(HuffmanCode)malloc((n+1)*sizeof(char*));        //allocate head pin vector to n codes of character
    char*cd=(char*)malloc(n*sizeof(char));              //allocate workplace when finding the codes
    cd[n-1]='\0';                                       //ending flag

    for(i=1;i<=n;++i)
    {                                                   //find Huffman code character by character
        int start=n-1;                                  //position of end
        int c,f;
        for(c=i,f=HT[i].parent;f!=0;c=f,f=HT[f].parent)
        {                                               //find the code reversely from leaves to root
            if(HT[f].lchild==c)
                cd[--start]='0';
            else
                cd[--start]='1';
        }

        HC[i]=(char*)malloc((n-start)*sizeof(char));    //allocate space for code of character no.i
        strcpy(HC[i],&cd[start]);                       //copy string from cd to HC
    }
    free(cd);                                           //free workplace
}//HuffmanCoding

void Select(HuffmanTree &HT,int k,int &s1,int &s2)
{
    int i;
    s1=1;
    for(i=1;i<=k;i++)
    {
        if(HT[i].parent==0)
        {
            s1=i;
            break;
        }
    }
    for(i=1;i<=k;i++)
    {
        if(HT[i].parent==0&&HT[i].Contant.weight<HT[s1].Contant.weight)
            s1=i;
    }

    for(i=1;i<=k;i++)
    {
        if(HT[i].parent==0&&i!=s1)
        {
            s2=i;
            break;
        }
    }

    for(i=1;i<=k;i++)
    {
        if(HT[i].parent==0&&HT[i].Contant.weight<HT[s2].Contant.weight&&i!=s1)
            s2=i;
    }
}