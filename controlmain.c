#include <18F4550.h>

#device adc = 8

#include <math.h>
#include <stdlibm.h>

#fuses INTRC, CPUDIV1, PLL1, NOLVP, NOPROTECT, NOMCLR, NOWDT
#use delay(clock = 8M)

#use rs232(rcv=pin_c7,xmit=pin_c6,baud=9600,bits=8,parity=n)

char dataRS232;
int1 flagg = 1;

#int_rda
void rcv_data(){
   dataRS232 = getch();
   flagg = 1;
   output_toggle(PIN_B0);
}


// obtener entradas
void get_inputs_1();
unsigned int p1_pot_x = 125;
unsigned int p1_pot_y = 125;
unsigned int p1_pot_xx = 125;
unsigned int p1_pot_yy = 125;
int1 p1_L = 0;
int1 p1_R = 0;

void get_inputs_2();
unsigned int p2_pot_x = 125;
unsigned int p2_pot_y = 125;
unsigned int p2_pot_xx = 125;
unsigned int p2_pot_yy = 125;
int1 p2_L = 0;
int1 p2_R = 0;

unsigned int level = 0;
unsigned int best_level = 0;
int1 new_level = 1;
unsigned int16 score = 0;
unsigned int16 best_score = 0;
unsigned int16 last_score = 10;
unsigned int last_level = 10;

int buttons = 0;

char mode = 'i';
char last_mode = 'i';

void main(){

   enable_interrupts(GLOBAL);
   enable_interrupts(int_rda);

   setup_adc_ports(AN0_TO_AN7);
   setup_adc(ADC_CLOCK_DIV_2);
   
   int menu_button = 0;
   
   delay_ms(500);

   while(TRUE){

      get_inputs_1();
      get_inputs_2();
      
      if(flagg){
      if(p1_L)
         buttons |= 1;
      if(p1_R)
         buttons |= 2;
      if(p2_L)
         buttons |= 4;
      if(p2_R)
         buttons |= 8;
      printf("%c", p1_pot_x);
      printf("%c", p1_pot_y);
      printf("%c", p1_pot_xx);
      printf("%c", p1_pot_yy);
      printf("%c", p2_pot_x);
      printf("%c", p2_pot_y);
      printf("%c", p2_pot_xx);
      printf("%c", p2_pot_yy);
      printf("%c", buttons);
      
      buttons = 0;
      
      flagg = 0;
      }
   }

}

void get_inputs_1(){
   set_adc_channel(0);
   delay_us(10);
   p1_pot_x = read_adc();
   set_adc_channel(1);
   delay_us(10);
   p1_pot_y = read_adc();
   set_adc_channel(2);
   delay_us(10);
   p1_pot_xx = read_adc();
   set_adc_channel(3);
   delay_us(10);
   p1_pot_yy = read_adc();
   if(input(PIN_D0))
      p1_L = 1;
   else
      p1_L = 0;
   if(input(PIN_D1))
      p1_R = 1;
   else
      p1_R = 0;   
}

void get_inputs_2(){
   set_adc_channel(4);
   delay_us(10);
   p2_pot_x = read_adc();
   set_adc_channel(5);
   delay_us(10);
   p2_pot_y = read_adc();
   set_adc_channel(6);
   delay_us(10);
   p2_pot_xx = read_adc();
   set_adc_channel(7);
   delay_us(10);
   p2_pot_yy = read_adc();
   if(input(PIN_D0))
      p2_L = 1;
   else
      p2_L = 0;
   if(input(PIN_D1))
      p2_R = 1;
   else
      p2_R = 0;   
}

