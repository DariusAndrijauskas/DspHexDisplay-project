#include <stdio.h>
#include <sys\exception.h>		//Interrupt Handling Header
#include "system.h"
#include <stdlib.h>
#include <math.h>

// Sets up here all Prototype Functions 
void Init_Interrupts(void);
void Init_PLL(void);

unsigned int aeilute=0, akadras=0;
unsigned char mass4[2][8][48];


void main()
{
unsigned char tempReg;
unsigned int i1=0, eilute=0, eilute1, eilout, tdbr, dcr, dcg, dcb, j, j0, spalva1, spalva2, zingsnis;
int kadras, k, i=0;
unsigned int mass1[30][8], mass2[30][8], mass3[32][8], dc[32];
float dkoord[30][8][2], linija[2], dkamp[7][2], kerta[3][2], dalplot, k1, y, b1, k2, b2, r, statmuo, dplot, konvert, centrui, kaimynams, x0, y0, a, c;
bool dkbool[7];
double const Pi=4*atan(1); // Pi=3.14
	//setup;
Init_PLL();
*pFIO_FLAG_S=0x40;		//pakelia/ijungia blank rezima
*pFIO_DIR = 0xdf;	//Blackfin PF2-pf4,pf6 pin must be set as output
*pSPI_BAUD = 2;
*pSPI_CTL = 0x5c21;		//0101 1100  0010 0001  8bit zodziai.
Init_Interrupts();		//Enter an interrupt subroutine after DMA has been finished
//---PWM timer------udzuodame pwmo dazni-------------------------------------------------------------------------------
*pTIMER0_CONFIG =0x0009;               
*pTIMER0_WIDTH  =1;
*pTIMER0_PERIOD =2;
*pTIMER_ENABLE  =1;
//---End---------------------------------------------------------------------------------------------------------------
*pFIO_FLAG_S=0x40;				//pakelia/ijungia blank rezima
//---DC registro uzpildymas--------------------------------------------------------------------------------------------
dcr=255;			//baltos spalvos kalibravimo balancas
dcg=3;
dcb=3;

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
*pFIO_FLAG_C=0x8;				//MODE nuleistas = GS darbo rezimas
//---END, DC registras uzpildytas--------------------------------------------------------------------------------------


//---Diodu koordinaciu skaiciavimas------------------------------------------------------------------------------------
a=1;									//sesiakampio krastine
x0=sqrtf(3)/2*a;						//x zingsnis lygus statmeniui
y0=a/2;									//y zingsnis lygus pusei a (kita staciojo trikampio statine krastine)
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
//---END diodu koordinaciu skaicavimas---------------------------------------------------------------------------------

//---mass4 masyvo uzpildymas-------------------------------------------------------------------------------------------
for (i=0; i<48; i++)
{
	for (k=0; k<8; k++)
	{
		mass4[0][k][i]=0;
		mass4[1][k][i]=0;
	}
}
//---END mass4 masyvo uzpildymas---------------------------------------------------------------------------------------

akadras=0;
*pDMA5_X_COUNT = 48;
*pDMA5_X_MODIFY = 0x1;	
*pDMA5_PERIPHERAL_MAP = 0x5000;	

*pSPI_CTL = 0x5c23;		//SPI nustatomas DMA rezimui

//*pDMA5_START_ADDR = &(mass4[akadras][aeilute][0]);	
//*pDMA5_CONFIG =  0x0001; //DMAEN | DI_EN | WNR | WDSIZE_16| DMA2D | RESTART | DI_EN;	
*pWDOG_CNT = 10;
*pWDOG_CTL = 0x8014;


refresh:



/*


padaryti linijos funkcija kuri alpha masyvui nutrintu krasta
1. padaryti trikampio/daugiakampio alpha masyva.
2. sudeti visus alpha masyvus i viena.
3. masteralpha masyva uzklijuoti ant freimo.


*/










//---Pirminio masyvos kurimas (piesinys) (linija dalinanti ekrana i dvi dalis)-----------------------------------------
linija[0]=(16*1.5+0.5)/(31*x0);//k							// y=k*linija[0]+linija[1];
linija[1]=-2;//b
spalva1=0xfff;
spalva2=0x000;
r= 1.5;//sqrtf(1.5*sqrtf(3)/Pi);
dplot=Pi*r*r;
c=r/(cos(atan(linija[0])));
k1=linija[0];
b1=linija[1];
dalplot=0;

//linija[0]=tan((atan((24.5)/(31*x0))/Pi*180+15-6*kadras)*Pi/180);
for (i=0; i<30; i++)					
{
	for (k=0; k<8; k++)
	{

		if (i<=14)
		{
			if ((i%2)==0) mass1[i][k]=4095;
			else mass1[i][k]=0;
		}
		else mass1[i][k]=2045;
	/*	y=linija[0]*dkoord[i][k][0]+linija[1];
		if (dkoord[i][k][1]<(y-c)) mass1[i][k]=spalva1;	//jei zemiau linijos y minus diodo radiusas, tada priskiriam spalva
		if (((y-c)<dkoord[i][k][1])&(dkoord[i][k][1]<=y))
		{
			statmuo=abs((y-dkoord[i][k][1])*cos(atan(linija[0])));
			dalplot= r*r*(acos(statmuo/r)-0.5*sin(2*acos(statmuo/r)));
			mass1[i][k]=(int)((spalva1*(1-dalplot/dplot))+(spalva2*dalplot/dplot));
		}
		if ((y<dkoord[i][k][1])&(dkoord[i][k][1]<=(y+c))) 
		{
			statmuo=abs((y-dkoord[i][k][1])*cos(atan(linija[0])));
			dalplot= r*r*(acos(statmuo/r)-0.5*sin(2*acos(statmuo/r)));
			mass1[i][k]=(int)((spalva2*(1-dalplot/dplot))+(spalva1*dalplot/dplot));
		}
		if (dkoord[i][k][1]>(y+c)) mass1[i][k]=spalva2;*/
		
	}
}
/*
mass1[12][1]=spalva2;
mass1[23][3]=spalva2;
mass1[10][4]=spalva2;
mass1[18][6]=spalva2;
*/

//--------END mass1 masyvo kurimas (piesimas)------------------------------------------------------------------------



//--------------Tarpinio masyvo mass2 perskaiciavimas is pirminio mass1------------------------------------------------
centrui=1;
kaimynams=(1-centrui)/6;
for (i=0; i<30; i++)					
{
	for (k=0; k<8; k++)
	{
		mass2[i][k]=(centrui*mass1[i][k])+
						kaimynams*(((0<=(i-2))&&((i-2)<=29))? mass1[i-2][k] : 0)+
						kaimynams*(((0<=(i-1))&&((i-1)<=29))? mass1[i-1][k] : 0)+
						kaimynams*(((0<=(i+1))&&((i+1)<=29))? mass1[i+1][k] : 0)+
						kaimynams*(((0<=(i+2))&&((i+2)<=29))? mass1[i+2][k] : 0)+								 //Nelyginiai
						kaimynams*((i%2)==0? ((((i+1)<=29)&&((k+1)<=7))? mass1[i+1][k+1] : mass1[i][k]) : ((((i+1)<=29)&&((k-1)>=0))? mass1[i+1][k-1] : mass1[i][k]))+
						kaimynams*((i%2)==0? ((((i-1)>= 0)&&((k+1)<=7))? mass1[i-1][k+1] : mass1[i][k]) : ((((i-1)>= 0)&&((k-1)>=0))? mass1[i-1][k-1] : mass1[i][k]));

	//	mass2[i][k]=(int)konvert;
	}
}//-------------END----------------------------------------------------------------------------------------------------



//--------------Galutinio masyvo mass3 uzpildymas is tarpinio mass2----------------------------------------------------
for (k=0; k<8; k++)
{
	for (i=0; i<14; i++) mass3[i][k]=mass2[i][k];	//mass3 uzpildomas iki 13 stuplelio
	mass3[14][k]=0;									//mass3 14 ir 15 stulpeliai uzpildomi nuliais, nes sie adressai neprijungti prie realiu diodu.
	mass3[15][k]=0;
	for (i=14; i<30; i++) mass3[i+2][k]=mass2[i][k];	//mass3 uzpildomas nuo 16 iki 31
}//-------------END----------------------------------------------------------------------------------------------------



//---Techninio masyvo generavimas--------------------------------------------------------------------------------------
for (eilute=0; eilute<8; eilute++)	//ciklas eiluciu skleidimui
{	
	i=0;		
	k=47;
	for (k=47; k>=0; k--)			//Grey scale duomenu siuntimo ciklas
  	{
  		i1=k-k/3;
		if (i==0) mass4[akadras][eilute][47-k]=(mass3[i1-1][eilute] & 0x0ff0)>>4;
		if (i==1) mass4[akadras][eilute][47-k]=((mass3[i1][eilute] & 0x00f)<<4) | ((mass3[i1-1][eilute] & 0x0f00)>>8);
		if (i==2) mass4[akadras][eilute][47-k]= (mass3[i1][eilute] & 0x00ff);
		i++;				
		if (i>=3) i=0;
	}
}//--END---------------------------------------------------------------------------------------------------------------


akadras = !akadras;
goto refresh;
}//end main
