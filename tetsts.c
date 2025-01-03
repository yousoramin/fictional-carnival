#include <18F4550.h>

#fuses NOLVP, NOPROTECT, NOMCLR, NOWDT
#use delay(clock = 20M, crystal = 20M)

#include <HDM64GS12.c>

void glcd_line(int x1, int y1, int x2, int y2, int1 color){
   signed int  x, y, addx, addy, dx, dy;
   signed long P;
   int i;
   dx = abs((signed int)(x2 - x1));
   dy = abs((signed int)(y2 - y1));
   x = x1;
   y = y1;

   if(x1 > x2)
      addx = -1;
   else
      addx = 1;
   if(y1 > y2)
      addy = -1;
   else
      addy = 1;

   if(dx >= dy)
   {
      P = 2*dy - dx;

      for(i=0; i<=dx; ++i)
      {
         glcd_pixel(x, y, color);

         if(P < 0)
         {
            P += 2*dy;
            x += addx;
         }
         else
         {
            P += 2*dy - 2*dx;
            x += addx;
            y += addy;
         }
      }
   }
   else
   {
      P = 2*dx - dy;

      for(i=0; i<=dy; ++i)
      {
         glcd_pixel(x, y, color);

         if(P < 0)
         {
            P += 2*dx;
            y += addy;
         }
         else
         {
            P += 2*dx - 2*dy;
            x += addx;
            y += addy;
         }
      }
   }
}

void main(){

   glcd_init(ON);
   glcd_fillScreen(0);

   while(TRUE){
      for(int i = 0; i < 64; i++){
         glcd_line(0,i,120,i,1);
      }
      delay_ms(600);
      for(int i = 0; i < 64; i++){
         glcd_line(0,i,120,i,0);
      }
      delay_ms(600);
   }

}
