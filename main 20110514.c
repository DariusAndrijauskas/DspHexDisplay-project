/*****************************************************************************************/
//
// Name: 	BF533 EZ-KIT video ITU-656 receive mode
//
/*****************************************************************************************

(C) Copyright 2006 - Analog Devices, Inc.  All rights reserved.

File Name:				Main.c

Date Modified:			10/25/04				Rev 0.2

Software:       		VisualDSP++4.5

Hardware:				BF533 EZ-KIT Board

Special Connections:  	Connect a CVBS video source to the right bottom video jack "AVIN1"
                        change switch #6 of SW3 to on of the ADSP-BF533 EZ-KIT Lite.
						

Purpose:				After reset the ADV7183 is configured to output digital
						video data in ITU656 mode 8bits wide. Additionally, The DSP is 
						configured to receive video data in ITU-656 format via the PPI Interface.
						The DMA channel 0 is dedicated to transfer the data to the 
						external SDRAM immediately (Address 0x0). 

******************************************************************************************/


#include <stdio.h>
#include <sys\exception.h>		//Interrupt Handling Header
#include "system.h"
#include <stdlib.h>
#include <math.h>

// Sets up here all Prototype Functions 
void Init_EBIU(void);
void Init_Flash(void);
void Init_ADV(void);
void Init_Interrupts(void);
void Init_SDRAM(void);
void Init_DMA(void);
void Init_PPI(void);
void Init_PLL(void);




void main()
{
int ksk=5;
unsigned char tempReg;
unsigned int i1=0, eilute=0, eilute1, eilout, tdbr, dcr, dcg, dcb, j, j0, spalva1, spalva2, zingsnis;
int kadras, k, i=0;
unsigned int massp[ksk][30][8], masst[ksk][30][8], massg[ksk][32][8], dc[32];
float x0, y0, a, c, dkoord[30][8][2], linija[2], y, dkamp[7][2], kerta[3][2], dalplot, k1, b1, k2, b2, r, statmuo, dplot, konvert, centrui, kaimynams;
bool dkbool[7];
double const Pi=4*atan(1); // Pi=3.14
	//setup;
Init_PLL();
*pFIO_FLAG_S=0x40;				//pakelia/ijungia blank rezima
//Init_EBIU();	//Async Memory Interface Setup
//Init_Flash();	//Flash Setup to config the flash GPIOs
Init_ADV();		//Link the ADV7183 clock to the PPI and ADV7183 reset
//printf("\n ADV Setup Lock Delay!\n"); //Gives the ADV7183 time to lock after reset
//Init_Interrupts();	//Enter an interrupt subroutine after DMA has been finished
//Init_SDRAM();		//SDRAM Setup to store the video image
//Init_DMA();		//DMA Setup "PPI->DMA->SDRAM" Its configured in Stop Mode
//Init_PPI();		//PPI Setup
*pSPI_BAUD = 58;
*pSPI_CTL = 0x5c21;		//0101 1100  0010 0001  8bit zodziai.

//--timer----------udzuodame pwmo dazni
*pTIMER0_CONFIG =0x0009;               
*pTIMER0_WIDTH  =6;
*pTIMER0_PERIOD =12;
*pTIMER_ENABLE  =1;
//--End

*pFIO_FLAG_S=0x40;				//pakelia/ijungia blank rezima
//-------------------------------------------------------
//Vieta darbui su masyvais: piesimas editinimas ir t.t.
dcr=0x00ff;
dcg=0x0006;
dcb=0x0006;
dc[0]=dcr;
dc[1]=dcg;
dc[2]=dcb;
dc[3]=dcr;
dc[4]=dcg;
dc[5]=dcb;
dc[6]=dcr;
dc[7]=dcg;
dc[8]=dcb;
dc[9]=dcr;
dc[10]=dcg;
dc[11]=dcb;
dc[12]=dcr;
dc[13]=dcg;
dc[14]=0;
dc[15]=0;
dc[16]=dcb;
dc[17]=dcr;
dc[18]=dcg;
dc[19]=dcb;
dc[20]=dcr;
dc[21]=dcg;
dc[22]=dcb;
dc[23]=dcr;
dc[24]=dcg;
dc[25]=dcb;
dc[26]=dcr;
dc[27]=dcg;
dc[28]=dcb;
dc[29]=dcr;
dc[30]=dcg;
dc[31]=dcb;

a=1;
x0=sqrtf(3)/2*a;
y0=a/2;
for (k=0; k<8; k++)						//Randame diodu koordinates
{
	for (i=0; i<15; i++)
	{
		dkoord[i*2][k][0]=i*2*x0+x0;
		dkoord[i*2][k][1]=k*6*y0+5*y0;
		dkoord[i*2+1][k][0]=i*2*x0+2*x0;
		dkoord[i*2+1][k][1]=k*6*y0+2*y0;
	}
}

dkamp[0][0]=0;							//pataisu masyvas skirtas rasti diodo efektinio ploto kampu koordinatem,
dkamp[0][1]=-y0;						//zinant diodo koordinates
dkamp[1][0]=x0;
dkamp[1][1]=(-y0)/2;
dkamp[2][0]=x0;
dkamp[2][1]=(y0)/2;
dkamp[3][0]=0;
dkamp[3][1]=y0;
dkamp[4][0]=-x0;
dkamp[4][1]=(y0)/2;
dkamp[5][0]=-x0;
dkamp[5][1]=(-y0)/2;
dkamp[6][0]=dkamp[0][0];
dkamp[6][1]=dkamp[0][1];



//-------Pirminio masyvos kurimas (piesinys) (linija dalinanti ekrana i dvi dalis)---------------------------
linija[0]=(16*1.5+0.5)/(31*x0);//k							// y=k*linija[0]+linija[1];
linija[1]=0;//-20;//b
spalva1=0x0ff;
spalva2=0x002;
r= x0;//sqrtf(3*sqrtf(3)/2/Pi);
dplot=Pi*r*r;
c=r/(cos(atan(linija[0])));
k1=linija[0];
b1=linija[1];
dalplot=0;
for (kadras=0; kadras<ksk; kadras++)
{
	linija[0]=tan((atan((24.5)/(31*x0))/Pi*180+15-6*kadras)*Pi/180);
	for (i=0; i<30; i++)					
	{
		for (k=0; k<8; k++)
		{

			y=linija[0]*dkoord[i][k][0]+linija[1]+2.2*kadras-5;
			if (dkoord[i][k][1]<(y-c)) massp[kadras][i][k]=spalva1;	//jei zemiau linijos y minus diodo radiusas, tada priskiriam spalva
			if (((y-c)<dkoord[i][k][1])&(dkoord[i][k][1]<=y))
			{
				statmuo=abs((y-dkoord[i][k][1])*cos(atan(linija[0])));
				dalplot= r*r*(acos(statmuo/r)-0.5*sin(2*acos(statmuo/r)));
				massp[kadras][i][k]=(int)((spalva1*(1-dalplot/dplot))+(spalva2*dalplot/dplot));
			}
			if ((y<dkoord[i][k][1])&(dkoord[i][k][1]<=(y+c))) 
			{
				statmuo=fabs((y-dkoord[i][k][1])*cos(atan(linija[0])));
				dalplot= r*r*(acos(statmuo/r)-0.5*sin(2*acos(statmuo/r)));
				massp[kadras][i][k]=(int)((spalva2*(1-dalplot/dplot))+(spalva1*dalplot/dplot));
			}
			if (dkoord[i][k][1]>(y+c)) massp[kadras][i][k]=spalva2;
		}
	}//--------END massg masyvo kurimas (piesimas)-----------------------------------------------------------
}
/*linija[0]=-1;
linija[1]=48*y0;
for (i=0; i<30; i++)					//Pirminio masyvos kurimas (piesinys)
{
	for (k=0; k<8; k++)
	{
		y=linija[0]*dkoord[i][k][0]+linija[1];
		if (y>dkoord[i][k][1]) massp[i][k]/=5;
	}
}


spalva1=0x0;
for (kadras=0; kadras<ksk; kadras++)
{for (i=0; i<30; i++)
{for (k=0; k<8; k++)
{massp[kadras][i][k]=spalva1;}}}
for (kadras=0; kadras<ksk; kadras++)
{massp[kadras][15][3]=0x0ff;
massp[kadras][5][2]=0x0ff;
massp[kadras][10][5]=0x0ff;
massp[kadras][25][6]=0x0ff;}*/
//--------------Tarpinio masyvo masst perskaiciavimas is pirminio massp----------------------------------
centrui=0.8;
kaimynams=(1-centrui)/6;
for (kadras=0; kadras<ksk; kadras++)
{
	for (i=0; i<30; i++)					
	{
		for (k=0; k<8; k++)
		{
			masst[kadras][i][k]=(centrui*massp[kadras][i][k])+
								kaimynams*(((0<=(i-2))&&((i-2)<=29))? massp[kadras][i-2][k] : 0)+
								kaimynams*(((0<=(i-1))&&((i-1)<=29))? massp[kadras][i-1][k] : 0)+
								kaimynams*(((0<=(i+1))&&((i+1)<=29))? massp[kadras][i+1][k] : 0)+
								kaimynams*(((0<=(i+2))&&((i+2)<=29))? massp[kadras][i+2][k] : 0)+								 //Nelyginiai
								kaimynams*((i%2)==0? ((((i+1)<=29)&&((k+1)<=7))? massp[kadras][i+1][k+1] : massp[kadras][i][k]) : ((((i+1)<=29)&&((k-1)>=0))? massp[kadras][i+1][k-1] : massp[kadras][i][k]))+
								kaimynams*((i%2)==0? ((((i-1)>= 0)&&((k+1)<=7))? massp[kadras][i-1][k+1] : massp[kadras][i][k]) : ((((i-1)>= 0)&&((k-1)>=0))? massp[kadras][i-1][k-1] : massp[kadras][i][k]));

		//	masst[kadras][i][k]=(int)konvert;
		}
	}//-------------END--------------------------------------------------------------------------------------
}
//--------------Galutinio masyvo massg uzpildymas is tarpinio masst--------------------------------------
for (kadras=0; kadras<ksk; kadras++)
{
	for (i=0; i<14; i++)					//massg uzpildomas iki 13 stuplelio
	{
		for (k=0; k<8; k++)
		{
			massg[kadras][i][k]=masst[kadras][i][k];
		}
	}
	for (k=0; k<8; k++)						//massg 14 ir 15 stulpeliai uzpildomi nuliais, nes sie adressai neprijungti prie realiu diodu.
	{
		massg[kadras][14][k]=0;
		massg[kadras][15][k]=0;
	}
	for (i=14; i<30; i++)					//massg uzpildomas nuo 16 iki 31
	{
		for (k=0; k<8; k++)
		{
			massg[kadras][i+2][k]=masst[kadras][i][k];
		}
	}//-------------END------------------------------------------	
}

//---DC registro uzpildymas-----------------
*pFIO_FLAG_S=0x40;				//pakelia/ijungia blank rezima
*pFIO_FLAG_S=0x8;				//MODE pakeltas = DC darbo rezimas
i=0;
for (k=23; k>=0; k--)			//Dot Corection duomenu siuntimo ciklas
	{		
		i1=k+k/3;
		if (i==0) *pSPI_TDBR= ((dc[i1+1] & 0x003f)<<2) | ((dc[i1] & 0x0030)>>4);
		while (*pSPI_STAT & 8);		//kol istustes buferis	
		if (i==1) *pSPI_TDBR= ((dc[i1+1] & 0x000f)<<4) | ((dc[i1] & 0x003c)>>2);
		while (*pSPI_STAT & 8);		//kol istustes buferis	
		if (i==2) *pSPI_TDBR= ((dc[i1+1] & 0x0003)<<6) |  (dc[i1] & 0x003f);
		while (*pSPI_STAT & 8);		//kol istustes buferis	
		i++;
		if (i>=3) i=0;
	}
	while ((*pSPI_STAT & 1)==0);	//kol isssius paskutini worda
*pFIO_FLAG_C=0x10;				//numeta Xlat
*pFIO_FLAG_S=0x10;				//pakelia Xlat (irasymas vyksta ties kylanciu frontu)
//---END, DC registras uzpildytas-----------------


*pFIO_FLAG_C=0x8;				//mode=0 Grey Scale rezimas
*pFIO_FLAG_S=0x40;				//pakelia/ijungia blank rezima
refresh:

//---Duomenu siuntimo ciklas-----------------------
for (kadras=0; kadras<ksk; kadras++)
{
	for (zingsnis=0; zingsnis<40; zingsnis++)
	{
		for (eilute=0; eilute<8; eilute++)	//ciklas eiluciu skleidimui
		{	
			i=0;		
			k=47;
			for (k=47; k>=0; k--)			//Grey scale duomenu siuntimo ciklas
		  	{
		  		i1=k-k/3;
				if (i==0) {tdbr=(massg[kadras][i1-1][eilute] & 0x0ff0)>>4; *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				if (i==1) {tdbr=((massg[kadras][i1][eilute] & 0x00f)<<4) | ((massg[kadras][i1-1][eilute] & 0x0f00)>>8); *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				if (i==2) {tdbr= (massg[kadras][i1][eilute] & 0x00ff); *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				i++;				
				if (i>=3) i=0;
		//		printf("%u eilute %u baitas tdbr= %X i1= %u, massg[%u, %u]=%X\n",eilute, 48-k, tdbr, i1, i1, eilute, massg[i][eilute]);
			}
			while ((*pSPI_STAT & 1)==0);	//kol isssius paskutini  worda
			*pFIO_FLAG_S=0x40;	 
			*pFIO_FLAG_C=0x10;	
			*pFIO_FLAG_S=0x10;
  			*pFIO_FLAG_C=7;					//per eilutes
			eilute1=(eilute) & 7;
			*pFIO_FLAG_S=((eilute1 & 0x1)<<2) | (eilute1 & 0x2) | ((eilute1 & 0x4)>>2);	//aktyvi eilute=eilute
			*pFIO_FLAG_C=0x40;				//nuleidzia/isjungia blank rezima

		}//--END-------------------------------------------
	}	
}
for (kadras=ksk-2; kadras>=1; kadras--)
{
	for (zingsnis=0; zingsnis<40; zingsnis++)
	{
		for (eilute=0; eilute<8; eilute++)	//ciklas eiluciu skleidimui
		{	
			i=0;		
			k=47;
			for (k=47; k>=0; k--)			//Grey scale duomenu siuntimo ciklas
		  	{
		  		i1=k-k/3;
				if (i==0) {tdbr=(massg[kadras][i1-1][eilute] & 0x0ff0)>>4; *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				if (i==1) {tdbr=((massg[kadras][i1][eilute] & 0x000f)<<4) | ((massg[kadras][i1-1][eilute] & 0x0f00)>>8); *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				if (i==2) {tdbr= (massg[kadras][i1][eilute] & 0x00ff); *pSPI_TDBR=tdbr;}
				while (*pSPI_STAT & 8);		//kol istustes buferis	
				i++;				
				if (i>=3) i=0;
		//		printf("%u eilute %u baitas tdbr= %X i1= %u, massg[%u, %u]=%X\n",eilute, 48-k, tdbr, i1, i1, eilute, massg[i][eilute]);
			}
			while ((*pSPI_STAT & 1)==0);	//kol isssius paskutini  worda
			*pFIO_FLAG_S=0x40;	 
			*pFIO_FLAG_C=0x10;	
			*pFIO_FLAG_S=0x10;
			*pFIO_FLAG_C=7;					//per eilutes
			eilute1=(eilute) & 7;
			*pFIO_FLAG_S=((eilute1 & 0x1)<<2) | (eilute1 & 0x2) | ((eilute1 & 0x4)>>2);	//aktyvi eilute=eilute
			*pFIO_FLAG_C=0x40;				//nuleidzia/isjungia blank rezima

		}//--END-------------------------------------------
	}	
}
goto refresh;
	
}//end main
