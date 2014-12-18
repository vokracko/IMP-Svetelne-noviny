//Lukas Vokracko, xvokra00
//Original
//14.12.2014

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

//Ulozeny login
const unsigned char login[64] = {0, 66, 36, 24, 24, 36, 66, 0, 0, 96, 24, 6, 6, 24, 96, 0, 0, 60, 66, 66, 66, 66, 60, 0, 0, 126, 24, 36, 36, 66, 66, 0, 0, 0, 126, 32, 64, 64, 64, 0, 0, 78, 74, 74, 74, 74, 126, 0, 0, 60, 66, 66, 66, 66, 60, 0, 0, 60, 66, 66, 66, 66, 60, 0};
//zacatek pameti, na kterou jsou mapovany diody
unsigned char * display = (char *) 0xB0;
//pamet, kam je mapovan posuvnik rychlosti
unsigned char * speed = (char *) 0xD0;
//pamet, kam je mapovano tlacitko init
unsigned char * init = (char *) 0xD1;
//pamet. kam je mapovano tlacitko horiz
unsigned char * horizontal = (char *) 0xD2;
//pamet. kam je mapovano tlacitko vert
unsigned char * vertical = (char *) 0xD3;
//smer rotace
unsigned char way = 0;
  
//bytova rotace charu o N bitu pro vertikalni rotaci
unsigned char byteRotate(unsigned char original, int index) 
{
   unsigned char result = original;
   int x;

   for(x = 0; x < index%8; ++x)
   {
     result = (original >> 1);
     result |= ((original & 1) << 7);
     original = result;
   }

   return result;
} 

//urceni smeru rotace dle aktivniho tlacitka
unsigned char determineWay(unsigned char currentWay)  
{
   if(*init & 1) return 0;
   else if(*horizontal & 1) return 1;
   else if(*vertical & 1) return 2;
   
   return currentWay;  
}

//spozdeni dle hodnoty baru
void delay() 
{
  unsigned int i;
  unsigned int limit = 1000 - (*speed << 2);
  
  for(i = 0; i < limit; ++i)
  {
    way = determineWay(way);
    __asm
    {
      nop
    };
      
  }
}

//hlavni fce zajistujici rotaci
void rotate(int indexHorizontal, int indexVertical) 
{
  int x = 0;
  unsigned char original;

  //32 je velikost displeje
  for(x = 0; x < 32; ++x)
  {
    //64 je velikost celeho loginu
    original = login[(indexHorizontal+x) % 64];
    display[x] = byteRotate(original, indexVertical);
  }
}

void main(void) 
{
  int indexHorizontal = 0;
  int indexVertical = 0;

  //defaultni hodnoty  
  *speed = 127;
  *horizontal = 0;
  *vertical = 0;
  *init = 0;
  
  EnableInterrupts; /* enable interrupts */
  /* include your code here */
  
  while(1) 
  {
    //vypsani prvnich 4 znaku loginu, tlacitko init
    rotate(0, 0);
    indexHorizontal = 0;
    indexVertical = 0;
  
    while(1) 
    {
      __RESET_WATCHDOG();
      way = determineWay(way);
      if(!way) break;
      else if(way == 1) ++indexHorizontal;
      else if(way == 2) ++indexVertical;      
      
      rotate(indexHorizontal, indexVertical); 
      delay();    
    }
  }
}
