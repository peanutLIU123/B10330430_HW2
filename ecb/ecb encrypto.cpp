#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <time.h>
#pragma pack(2)
//****************structire**********************
typedef unsigned short WORD;		/*�w�q WORD ����Ӧr�`������*/
typedef unsigned long DWORD;		/*�w�q DWORD ��e�|�Ӧr�`������*/ 


typedef struct BMP_FILE_HEADER		/*��Ϥ���Y*/
{
WORD bType; /* ���Хܹ�*/
DWORD bSize; /* ��󪺤j�p */
WORD bReserved1; /* �O�d��,�����]�m��0 */ 
WORD bReserved2; /* �O�d��,�����]�m��0 */
DWORD bOffset; /* ����Y���̫��Ϲ��ƾڦ�}�l�������q */
} BMPFILEHEADER;


/*��ϫH���Y*/
typedef struct BMP_INFO{
DWORD bInfoSize; /* �H���Y���j�p */
DWORD bWidth; /* �Ϲ����e�� */
DWORD bHeight; /* �Ϲ������� */
WORD bPlanes; /* �Ϲ����쭱�� */
WORD bBitCount; /* �C�Ϲ�������� */
DWORD bCompression; /* ���Y���� */
DWORD bmpImageSize; /* �Ϲ����j�p,�H�r�`����� */
DWORD bXPelsPerMeter; /* ��������v */
DWORD bYPelsPerMeter; /* ��������v */
DWORD bClrUsed; /* �ϥΪ���m�� */
DWORD bClrImportant; /* ���n���C��� */
} BMPINF;


/*�m���*/
typedef struct RGB_QUAD {
WORD rgbBlue; /* �Ŧ�j�� */
WORD rgbGreen; /* ���j�� */
WORD rgbRed; /* ����j�� */
WORD rgbReversed; /* �O�d�� */
} RGBQUAD;

typedef struct PData{
	char text[8];
} Plaindata;
//****************structire********************** 

void ip(Plaindata,char *,char *);
void tobinary(char *,Plaindata);
void E(char *,char*);
void F(char *,char *,char *);
void P(char *);
void Xor(char *,char *);
void change(char *,char *);
void Pfinal(char *,char *,char *);
int except(char *);
int DES_MakeSubKeys(char *);
int DES_PC1_Transform(char *, char *);
int DES_PC2_Transform(char *, char *);
int DES_ROL(char *, int );
char subKeys[16][48];

int main(){
FILE *fp;
FILE *fpout;
BMPFILEHEADER fileHeader;
BMPINF infoHeader;
Plaindata plains,result;
char key[56]={'\0'},right[32]={'\0'},left[32]={'\0'};
char keyout[48]={'\0'},frightout[48]={'\0'},Rout[8]={'\0'};
char Finalout[8]={'\0'};
int cnt,i,j,s;
WORD c;
//system("chcp 437");
	if((fp = fopen("cute.bmp", "rb")) == NULL){
		printf("Cann't open the file!\n");
		exit(0);
	}
	fpout = fopen("result.bmp", "wb");
	
	fseek(fp, 0, 0);
	fread(&fileHeader, sizeof(fileHeader), 1, fp);
	fread(&infoHeader, sizeof(infoHeader), 1, fp);
	fwrite (&fileHeader , sizeof(fileHeader) , 1 , fpout);  
	fwrite (&infoHeader , sizeof(infoHeader) , 1 , fpout); 
	char keyin[8]={'1','0','1','0','1','0','1','1'};
	char newkey[64];
	
	for(i=0,j=0;i<sizeof(newkey);i++,j++){
		newkey[i]=keyin[j];
		if(j>=7) j=-1;
	}
	DES_MakeSubKeys(newkey);
	clock_t a,b;
	a = clock(); 
do{ 
	fread(&plains,sizeof(plains),1,fp);		
	ip(plains,left,right);
	for(i=0;i<=15;i++){
	
		E(frightout,right);				    //�i�hf��ƪ�RIGHT(�k�������)32bit to 48bit
		F(frightout,subKeys[i],Rout);		    //  frightout�M keyout ��XOR�� �b�i�JS_BOX �o��Rout 
		P(Rout);						    	
		Xor(Rout,left);					    //F��ƻPLEFT���尵XOR �����s��LEFT 
		if(i!=15)
		change(right,left);				    //�洫 LEFT RIGHT 
		
	}
	
	Pfinal(right,left,Finalout);			
	fwrite (&Finalout , sizeof(Finalout) , 1 , fpout);  
	
}while(!feof(fp));
b = clock();
  printf("�[�K����%d��\n",(b-a)/1000); 
	fclose(fp);
	fclose(fpout);
	getchar();
	return 0;
}
//�ͦ��l�K�_ 
int DES_MakeSubKeys(char *key){   
	int MOVE_TIMES[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1}; 
    char temp[56];   
    int cnt;   
    DES_PC1_Transform(key,temp);//PC1�m�� 
    for(cnt = 0; cnt < 16; cnt++){//  
        DES_ROL(temp,MOVE_TIMES[cnt]);//�`������   
        DES_PC2_Transform(temp,subKeys[cnt]);//PC2�m���A���ͤl�K�_   
    }   
    return 0;   
}  
   
int DES_PC1_Transform(char *key, char *temp){  
	int PC_1[56] = {56,48,40,32,24,16,8,
	          0,57,49,41,33,25,17,
	          9,1,58,50,42,34,26,
	          18,10,2,59,51,43,35,
	          62,54,46,38,30,22,14,
	          6,61,53,45,37,29,21,
	          13,5,60,52,44,36,28,
	          20,12,4,27,19,11,3}; 
    int cnt;       
    for(cnt = 0; cnt < 56; cnt++){   
        temp[cnt] = key[PC_1[cnt]]; 
    }   
    return 0;   
}   
  
  
int DES_PC2_Transform(char *key, char *tempbts){   
	int PC_2[48] = {13,16,10,23,0,4,2,27,   
              14,5,20,9,22,18,11,3,   
              25,7,15,6,26,19,12,1,   
              40,51,30,36,46,54,29,39,   
              50,44,32,46,43,48,38,55,   
              33,52,45,41,49,35,28,31};  
    int cnt;   
    for(cnt = 0; cnt < 48; cnt++){   
        tempbts[cnt] = key[PC_2[cnt]];   
    }   
    return 0;   
}   
  
   
int DES_ROL(char *data, int time){      
    char temp[56];   
  
      
    memcpy(temp,data,time);   
    memcpy(temp+time,data+28,time);   
       
       
    memcpy(data,data+time,28-time);   
    memcpy(data+28-time,temp,time);   
  
     
    memcpy(data+28,data+28+time,28-time);   
    memcpy(data+56-time,temp+time,time);       
  
    return 0;   
}   
//*******************function out to cipher******************************
void Pfinal(char *right,char *left,char *result){
char fip[64]={40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,
			   38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,
			   36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,
			   34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25}; 
int i,j,final[8],input=0,index=0,rindex=0;
char temp[64]={'\0'},IP_1[64]={'\0'},bytewise[8]={'\0'};
	for(i=0;i<32;i++) temp[i]=left[i];
	i=0; 
	for(j=32;j<64;j++){temp[j]=right[i];i++;}
	
	for(i=0;i<64;i++) IP_1[i]=temp[fip[i]-1];
	while(index<64){
		for(j=0;j<8;j++) {
			*(bytewise+j)=IP_1[index];
			index++;
		}
		index++;			
		input=except(bytewise);
		if(input>255) getchar();
		for(i=0;i<8;i++) bytewise[i]='\0';
			*(result+rindex)=input;
		rindex++;
		//printf("%d  ",input);
	}
}
int except(char *bytewise){						//��Q�i��
int sum=0,i;
	for(i=7;i>=0;i--){
		if(bytewise[i]=='1') sum+=1*pow(2,i);
		else sum+=0;
	}
	return sum;
}
//*******************function 16round************************************
void change(char *right,char *left){		
char temp[32]={'\0'};
int i;
	for(i=0;i<32;i++)temp[i]=right[i];
	for(i=0;i<32;i++)*(right+i)=left[i];
	for(i=0;i<32;i++)*(left+i)=temp[i];
}
void Xor(char *Rout,char *left){			 
char t[1]={'\0'},temp[32]={'\0'};
int j,i=0;  
	for(j=0;j<32;j++) temp[j]=left[j];
	for(j=0;j<32;j++){
		if(Rout[j]^temp[j]) left[j]='1';
		else if(!Rout[j]^temp[j]) left[j]='0';
		else left[j]='0';
	}	
}
void P(char *Rout){									 
int Pm[32]={16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,
			10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25};
int i;
char temp[32]={'\0'};
		for(i=0;i<32;i++) temp[i]=Rout[i];
		for(i=0;i<32;i++) *(Rout+i)=temp[Pm[i]-1];
}
void F(char *inright,char *inkey,char *Rout){			
int i,j,get1=0,get2=0,cnt=0,emp=0,in=0;
char inS[48]={'\0'},temp1[2]={'\0'},temp2[4]={'\0'},temp[4],t[1];

				 int S[8][4][16] =//S1
 	         {{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
   	         {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
  	          {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
  	          {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},
  	          //S2
 	           {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
 	           {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
 	           {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
 	           {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},
 	           //S3
 	           {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
 	           {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
   	           {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
 	           {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},
 	           //S4
 	           {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
 	           {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
 	           {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
 	           {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},
 	           //S5
 	           {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
 	           {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
 	           {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
 	           {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},
 	           //S6
 	           {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
 	           {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
 	           {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
 	           {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},
 	           //S7
 	           {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
 	           {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
 	           {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
 	           {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},
 	           //S8
 	           {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
 	           {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
 	           {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
 	           {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};
	//xor
	for(i=0;i<48;i++){
		if(inright[i]^inkey[i]==1)
			*(inS+i)='1';
		else if(inright[i]^inkey[i]==0)
			*(inS+i)='0';
	}
	
	while(cnt<48){
		temp1[0]=inS[cnt];
		temp1[1]=inS[cnt+5];
		for(i=0;i<4;i++) temp2[i]=inS[cnt+i+1];
		get1=strtol(temp1,NULL,2);
		get2=strtol(temp2,NULL,2);
		itoa(S[cnt/6][get1][get2],temp,2);
		emp=4-strlen(temp);
		for(i=3;i>=0;i--){
			temp[i]=temp[i-emp];
			if(i<=(emp-1)) temp[i]='0';
		}
		emp=0;
		for(j=0;j<4;j++){ *(Rout+in)=temp[j];in++;}
		cnt+=6;
	}
}
void E(char *frightout,char *right){					  //32 to 48g bit 
int Esqu[48]={32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
	 	  	   8, 9,10,11,12,13,12,13,14,15,16,17,
		  	  16,17,18,19,20,21,20,21,22,23,24,25,
	          24,25,26,27,28,29,28,29,30,31,32,1};
int i;
	for(i=0;i<48;i++) *(frightout+i)=right[Esqu[i]-1];
}
void ip(Plaindata pla,char *left,char *right){  			//initial permutation
int ipm[64]={58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
			 62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
			 57,49,41,33,25,17, 9,1,59,51,43,35,27,19,11,3,
			 61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7};
char temp[64]={'\0'},platemp[64]={'\0'};//bits
int i,j;
	tobinary(temp,pla);
	for(i=0;i<64;i++) *(platemp+i)=temp[ipm[i]-1];
	for(i=0;i<32;i++) *(left+i)=platemp[i];
	for(i=0,j=32;i<32;i++,j++) *(right+i)=platemp[j];
}
void tobinary(char temp[64],Plaindata pla){     //char to binary
	int cnt=0,i,j,emp=0,val=0,newval=0,store=0,inde,error[8];
	char te1[8]={'\0'},te[8]={'\0'};
	
	printf("\n\n");
	while(cnt<8){
		val=pla.text[cnt];
		if(val>0){
			itoa(pla.text[cnt],te1,2);
			emp=8-strlen(te1);
			for(i=7;i>=0;i--){
				te[i]=te1[i-emp];
				if(i<=(emp-1)) te1[i]='0';
			}
		}
		else{ 									 
			newval=val*-1;
			for(inde=0;inde<8;inde++){
					if(newval%2==1) te[7-inde]='1';
					else if(newval%2==0) te[7-inde]='0';
					newval=newval/2;
					if((inde==7&&newval==1)||val*-1==128) te[0]='1';
					else if(inde==7&&newval==0) te[0]='0';
			}
			val=0;newval=0;
		}
		for(i=0;i<8;i++) *(temp+i+store)=te[i];
		store+=8;
		for(i=0;i<8;i++) *(te+i)='\0';
		cnt++;
		emp=0;
	}
	//for(i=0;i<64;i++)  {
	//	printf("%c   ",temp[i]);
	//}
	for(i=0;i<64;i++)if(temp[i]=='\0') temp[i]='0';
	*(temp+65)='\0';
}




