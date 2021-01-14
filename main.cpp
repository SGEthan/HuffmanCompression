#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include"head.h"
#include"huffman.h"
using namespace std;

Status CharCount(FILE *fp,CharArray& CA);

Status File_Write_Compress(char* Output_File_Name,FILE *Input_File,char* Input_File_Name);

Status File_Write_Decompress(char* Output_File_Name,FILE *Input_File,int is_name_appointed);

int GetBit(ElemType s,int n);

int main(int argc,char**argv)
{
    if(argc<2)
    {
        cout<<"too few parameter"<<endl<<"use parameter -ls to list all the functions"<<endl;
        return ERROR;
    }

    if(!strcmp(argv[1],"-ls"))
    {
        cout<<"-ls................................................list all the functions"<<endl;
        cout<<"-c <inputfilename> <outputfilename(optional)>......compress a file"<<endl;
        cout<<"-d <inputfilename> <outputfilename(optional)>......decompress a file"<<endl;
        exit;
    }

    else if(argc==2)
    {
        cout<<"ERROR: too few or wrong parameter"<<endl;
        return ERROR;
    }
    
    else if(!strcmp(argv[1],"-c"))
    {
        char InFilename[99],OutFilename[99];
        strcpy(InFilename,argv[2]);//get the inputfile name
        
        char extension[10]=".huff";

        FILE *Input_File=NULL;
        Input_File=fopen(InFilename,"rb");//open the input file to read

        if(Input_File==NULL)
        {
            cout<<"The input file does not exist"<<endl;
            system("pause");
            return ERROR;
        }

        if(argc==3)
        {
            strcpy(OutFilename,argv[2]);
            strcat(OutFilename,extension);//make the output filename
        }

        else if(argc==4)
            strcpy(OutFilename,argv[3]);

        else
        {
            cout<<"Too many Parameters"<<endl;
            return ERROR;
        }
        
        File_Write_Compress(OutFilename,Input_File,InFilename);//compress
        
        cout<<endl<<"Done!"<<endl;
        fclose(Input_File);
    }

    else if(!strcmp(argv[1],"-d"))
    {   
        char name[99];
        // cout<<"plz enter the filename(must be sure that the file is in the same directory as the program):"<<endl;
        strcpy(name,argv[2]);//get the inputfile name

        FILE *Input_File=NULL;
        Input_File=fopen(name,"rb");//open the input file to read

        if(Input_File==NULL)
        {//can't find the input file
            cout<<"The input file does not exist"<<endl;
            system("pause");
            return ERROR;
        }

        char *ptr=NULL;
         ptr=(strstr(name,".huff"));

        if(ptr==NULL)
        {//if its extension is not .huff
            cout<<"The input file is the wrong form"<<endl;
            return ERROR;
        }

        int is_name_appointed;

        if(argc==3)//if the output name is not appointed
            is_name_appointed=NO;

        else if(argc==4)
        {//if it's appointed
            is_name_appointed=YES;
            strcpy(name,argv[3]);
        }

        else
        {
            cout<<"Too many Parameters"<<endl;
            return ERROR;
        }

        File_Write_Decompress(name,Input_File,is_name_appointed);//decompress

        cout<<endl<<"Done!"<<endl;
        fclose(Input_File);
    }

    else
    {
        cout<<"parameter error"<<endl;
        return ERROR;
    }
    

    cout<<endl;
    return OK;
}

int CharCount(FILE *fp,CharArray &CA)
{
    int j,n=1;
    ElemType s;
    CharUnit *p;

    for(;fread(&s,1,1,fp);)
    {//Get Char byte by byte
        for(j=1;j<n+1;j++)
        {
            if(CA[j].c==s)
            {//find the same char already stored
                CA[j].weight+=1;
                break;
            }
        }
        if(j==n+1)
        {//if not find it stored, give a new space to store it
            CA=(CharArray)realloc(CA,(n+2)*sizeof(CharUnit));
            CA[j].c=s;
            CA[j].weight=1;
            n++;
        }
    }

    fseek(fp,0,SEEK_SET);
    return (n);
}

Status File_Write_Compress(char* Output_File_Name,FILE *Input_File,char* Input_File_Name)
{
    cout<<"Compressing...."<<endl;
    ElemType a;             //unsigned char a to get character
    fread(&a,1,1,Input_File);       //store the first char

    CharArray CA;           //initilize the CharArray
    CA=(CharArray)malloc(2*sizeof(CharUnit));
    CA[1].c=a;              //give the first char
    CA[1].weight=1;         //give the first char

    int i,CharKind;         
    CharKind=CharCount(Input_File,CA);  //Count the variety of Char

    HuffmanTree HT;
    HuffmanCode HC;

    HuffmanCoding(HT,HC,CA,CharKind);//encode

    FILE *Output_File;                       //initilize the file pointer
    Output_File=fopen(Output_File_Name,"wb"); //open a file to write

    HUF_FILE_HEAD head;             //huffman file head
    head.flag[0]='d';
    head.flag[1]='y';
    head.flag[2]='c';

    head.CharKind=CharKind-1;

    strcpy(head.name,Input_File_Name);//store the name
    
    fwrite(&head,sizeof(HUF_FILE_HEAD),1,Output_File);//write the file head

    fwrite(CA,sizeof(CharUnit),CharKind+1,Output_File);//write the CharArray

    ElemType s;
    int j,k,l,BitCount=0,TempChar[8],ValidBit,IntTemp=1;//BitCount is the count of the 0 and 1 stored,TempChar is the int form of input char
    ElemType CharTemp=0;
    for(;fread(&s,1,1,Input_File);)
    {//write the main body of the input file
        for(i=1;i<=CharKind;i++)//find the char
            if(s==CA[i].c)
                break;
                
        for(j=0;j<strlen(HC[i]);j++)
        {//input the code of a char
            TempChar[BitCount]=HC[i][j]-48;
            BitCount++;
            if(BitCount==8)
            {//if a char is full,input it
                BitCount=0;
                for(k=0;k<8;k++)//store the char bit by bit
                {
                    if(IntTemp=TempChar[k])
                        for(l=0;l<7-k;l++)
                            IntTemp*=2;
                    
                    CharTemp+=IntTemp;
                }
                fwrite(&CharTemp,1,1,Output_File);
                CharTemp=0;
            }
        }
    }

    if(BitCount!=0)
    {//deal with the last bit
        for(i=BitCount;i<8;i++)//give 0 to the rest of the bit of the last char
            TempChar[i]=0;
            
        for(k=0;k<8;k++)//store the char bit by bit
        {
            if(IntTemp=TempChar[k])
                for(l=0;l<7-k;l++)
                    IntTemp*=2;
                    
            CharTemp+=IntTemp;
        }

        fwrite(&CharTemp,1,1,Output_File);
        CharTemp=0;
    }
    else
        BitCount=8;
    
    
    ValidBit=BitCount;
    head.lastValidBit=ValidBit;

    fseek(Output_File,0,SEEK_SET);
    fwrite(&head,sizeof(HUF_FILE_HEAD),1,Output_File);//rewrite the file head

    cout<<"Compression Completed"<<endl;
    fclose(Output_File);
    return OK;
}

Status File_Write_Decompress(char* Output_File_Name,FILE *Input_File,Status is_name_appointed)
{
    cout<<"Decompressing...."<<endl;

    HUF_FILE_HEAD head;
    fread(&head,sizeof(HUF_FILE_HEAD),1,Input_File);

    if(head.flag[0]!='d'||head.flag[1]!='y'||head.flag[2]!='c')
    {   
        cout<<"File form error"<<endl;
        exit;
    }
    
    FILE *Output_File;                       //initilize the file pointer
    if(is_name_appointed==YES)
        Output_File=fopen(Output_File_Name,"wb"); //open a file to write
    else
    {
        char name[26];
        strcpy(name,head.name);
        Output_File=fopen(name,"wb");
    }
    
    
    int CharKind=head.CharKind+1;//get the variaty of the char

    CharArray CA=(CharArray)malloc((CharKind+1)*sizeof(CharUnit));
    fread(CA,sizeof(CharUnit),CharKind+1,Input_File);//get the chararray

    HuffmanTree HT;
    HuffmanCode HC;
    HuffmanCoding(HT,HC,CA,CharKind);//get the huffman tree and code

    int root=2*CharKind-1;//root node
    int HT_ptr=root;//pointer to the Huffman Tree
    int IntBuffer;//buffer to get where to go
    ElemType CharBuffer,CharBuffer2,OutputBuffer;//buffer to get the char from the Output file

    int i;
    fread(&CharBuffer,1,1,Input_File);
    while(fread(&CharBuffer2,1,1,Input_File))
    {//Get the code from the file and write it into the output file
        for(i=0;i<8;i++)
        {
            IntBuffer=GetBit(CharBuffer,7-i);
            if(IntBuffer==0)
                HT_ptr=HT[HT_ptr].lchild;
            else
                HT_ptr=HT[HT_ptr].rchild;

            if(HT[HT_ptr].lchild==0&&HT[HT_ptr].rchild==0)
            {//when arrive at a leaf node, write it
                OutputBuffer=CA[HT_ptr].c;
                fwrite(&OutputBuffer,1,1,Output_File);
                HT_ptr=root;
            }
        }
        CharBuffer=CharBuffer2;
    }

    //deal with the last byte
    for(i=0;i<head.lastValidBit;i++)
    {
        IntBuffer=GetBit(CharBuffer,7-i);
        if(IntBuffer==0)
            HT_ptr=HT[HT_ptr].lchild;
        else
            HT_ptr=HT[HT_ptr].rchild;

        if(HT[HT_ptr].lchild==0&&HT[HT_ptr].rchild==0)
        {//when arrive at a leaf node, write it
            OutputBuffer=CA[HT_ptr].c;
            fwrite(&OutputBuffer,1,1,Output_File);
            HT_ptr=root;
        }
    }
    fclose(Output_File);
    cout<<"Decompression complete"<<endl;
    return OK;
}

int GetBit(ElemType s,int n)
{//get the no.n bit of s
    return (s&(1<<n))?1:0;
}