#include <18F4550.h>

#fuses NOLVP, NOPROTECT, NOMCLR, NOWDT
#use delay(clock = 20M, crystal = 20M)

#include <HDM64GS12.c>

#include <math.h>
#include <stdlibm.h>

#include "setup_disp.h"

#use rs232(rcv = pin_c7, xmit = pin_c6, baud = 9600, bits = 8, parity = n)

// nave
float last_ship_angle_1 = 0;
float ship_angle_1 = 0;
int last_x_pos_1;
int last_y_pos_1;
int x_pos_1 = 64;
int y_pos_1 = 32;
float dx_1 = 0;
float dy_1 = 0;

float last_ship_angle_2 = 0;
float ship_angle_2 = 0;
int last_x_pos_2;
int last_y_pos_2;
int x_pos_2 = 64;
int y_pos_2 = 32;
float dx_2 = 0;
float dy_2 = 0;

int1 shoot_1 = 1;
int cooldown_1 = 20;
int aiming_1 = 0;

void rect(int x1, int y1, int x2, int y2);

int1 shoot_2 = 1;
int cooldown_2 = 20;
int aiming_2 = 0;

float last_ship_angle = 0;
float ship_angle = 0;
int last_x_pos;
int last_y_pos;
int x_pos = 64;
int y_pos = 32;
float dx = 0;
float dy = 0;

void explic_s();
void explic_m();
// bala
int1 bullet = 0;
signed int last_last_bullet_x_pos;
signed int last_last_bullet_y_pos;
signed int last_bullet_x_pos;
signed int last_bullet_y_pos;
signed int16 bullet_x_pos;
signed int16 bullet_y_pos;
float bullet_dx = 0;
float bullet_dy = 0;
const float bullet_speed = 6;

int1 bullet_1 = 0;
signed int last_last_bullet_x_pos_1;
signed int last_last_bullet_y_pos_1;
signed int last_bullet_x_pos_1;
signed int last_bullet_y_pos_1;
signed int16 bullet_x_pos_1;
signed int16 bullet_y_pos_1;
float bullet_dx_1 = 0;
float bullet_dy_1 = 0;
int1 bullet_collision_1 = 0;

int1 bullet_2 = 0;
signed int last_last_bullet_x_pos_2;
signed int last_last_bullet_y_pos_2;
signed int last_bullet_x_pos_2;
signed int last_bullet_y_pos_2;
signed int16 bullet_x_pos_2;
signed int16 bullet_y_pos_2;
float bullet_dx_2 = 0;
float bullet_dy_2 = 0;
int1 bullet_collision_2 = 0;
// asteroides

int last_asteroids_x_pos[20];
int last_asteroids_y_pos[20];
unsigned int asteroids_x_pos[20];
unsigned int asteroids_y_pos[20];
signed int asteroids_dx[20] = {-2, 1, -3, 3, -2, 3, -1, 2, -3, 3, -2, 2, -1, 2, -1, 2, -2, 3, -2, -1};
signed int asteroids_dy[20] = {2, -1, -2, 3, -2, 2, -1, 1, -2, 3, -1, 3, -2, -1, 2, -3, -3, 3, -2, 1};

unsigned int level = 0;
unsigned int best_level = 0;
int1 new_level = 1;
unsigned int16 score = 0;
unsigned int16 best_score = 0;
unsigned int16 last_score = 10;
unsigned int last_level = 10;

const float internal_angle = (2 * (float)PI) / 3.0f;
// dibujar
void glcd_line(int x1, int y1, int x2, int y2, int1 color);
void draw_triangle(int x, int y, float starting_angle, int1 color);
void glcd_lineTracing(int *xx, int *yy, int n_points, int1 color);
void draw_asteroid(int x, int y, int life, int1 color);
void glcd_text57(int x, int y, char *textptr, int size, int1 color);
void inicio(int sec);
void scores(unsigned int16 scr, unsigned int lvl, int1 color);
void single_menu(int1 play);
int laser_waza(int x, int y);

float get_ship_angle(int x, int y);
float get_x_vel();
float get_y_vel();

char dataRS232;

char mode = 'i';
char mode_inic = 't';

int1 flagg = 1;
int1 flag_rec = 0;

int current_data = 0;

unsigned int p1_pot_x = 125;
unsigned int p1_pot_y = 125;
unsigned int p1_pot_xx = 125;
unsigned int p1_pot_yy = 125;
int1 p1_L = 0;
int1 p1_R = 0;

float calc_ang_x(int x, int y);
float calc_ang_y(int x, int y);

float get_d(unsigned int pp)
{
   return (float)((float)pp - 127.0f) / 30.0f;
}

unsigned int p2_pot_x = 125;
unsigned int p2_pot_y = 125;
unsigned int p2_pot_xx = 125;
unsigned int p2_pot_yy = 125;
int1 p2_L = 0;
int1 p2_R = 0;

#int_rda
void rcv_data()
{

   dataRS232 = getch();

   switch (current_data)
   {
   case 0:
      p1_pot_y = dataRS232; // x
      break;
   case 1:
      p1_pot_x = dataRS232; // y
      break;
   case 2:
      p1_pot_xx = dataRS232;
      break;
   case 3:
      p1_pot_yy = dataRS232;
      break;
   case 4:
      p2_pot_y = dataRS232;
      break;
   case 5:
      p2_pot_x = dataRS232;
      break;
   case 6:
      p2_pot_xx = dataRS232;
      break;
   case 7:
      p2_pot_yy = dataRS232;
      break;
   case 8:
      p1_L = (dataRS232 & 0x01) ? 1 : 0;
      p1_R = (dataRS232 & 0x02) ? 1 : 0;
      p2_L = (dataRS232 & 0x04) ? 1 : 0;
      p2_R = (dataRS232 & 0x08) ? 1 : 0;
      break;
   }

   current_data++;
   if (current_data == 9)
      current_data = 0;
}

void draw_cuadrito(int x, int y, int1 col);

void draw_win(int1 w){
   if(w){
      glcd_fillScreen(1);
      char text_asteroids[] = "PLAYER 1 WINS!";
      glcd_text57(29, 22, text_asteroids, 1, 0);
      delay_ms(500);
   }
   else{
      glcd_fillScreen(1);
      char text_asteroids[] = "PLAYER 2 WINS!";
      glcd_text57(29, 22, text_asteroids, 1, 0);
      delay_ms(500);
   }
}

void restart_screen()
{
   glcd_fillScreen(0);
   char text_asteroids[] = "Presiona para restart";
   glcd_text57(0, 0, text_asteroids, 1, 1);
}
void grafico_control();

void main()
{

   enable_interrupts(GLOBAL);
   enable_interrupts(int_rda);

   delay_ms(100);

   glcd_init(ON);
   glcd_fillScreen(0);

   int inic_men = 0;
   int last_inic_men = 0;

   int single_men = 0;
   int last_single_men = 0;

   

   int1 win = 0;
   int1 who = 0;

   int1 bullet_collision = 0;
   int sfx_time;

   int asteroids_lifes[20]; // puede haber hasta 20 asteroides en la pantalla;
   int1 asteroid_lost_a_life[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

   int temp = 0;
   int1 new_game = 0;

   grafico_control();
   delay_ms(5000);

   inicio(1);

   while (TRUE)
   {
      switch (mode)
      {
      case 'i':
      { // inicio
         if (p1_pot_y > 125)
         {
            inic_men = 0;
         }
         else
         {
            if (p1_pot_x > 125)
            {
               inic_men = 2;
            }
            else
            {
               inic_men = 1;
            }
         }

         if (inic_men != last_inic_men)
         {
            glcd_fillScreen(0);
            inicio(inic_men);
         }
         if (p1_R)
         {
            switch (inic_men)
            {
            case 1:
               mode = 's';
               glcd_fillScreen(0);
               single_menu(1);
               break;
            case 2:
               mode = 'm';
               glcd_fillScreen(0);
               single_menu(1);
               break;
            default:
               mode = 'i';
               new_game = 1;
               break;
            }
         }

         last_inic_men = inic_men;
         break;
      }
      case 's':
      { // single
         if (p1_pot_y >= 125)
         {
            single_men = 1;
         }
         else
         {
            single_men = 0;
         }

         if (single_men != last_single_men)
         {
            glcd_fillScreen(0);
            single_menu(single_men);
         }

         if (p1_L)
         {
            switch (single_men)
            {
            case 0:
               mode = 'i';
               glcd_fillScreen(0);
               break;
            case 1:
               mode = 'j';
               glcd_fillScreen(0);
               explic_s();
               delay_ms(3000);
               glcd_fillScreen(0);
               break;
            }
         }

         last_single_men = single_men;
         break;
      }
      case 'm':
      { // multiplayer
         if (p1_pot_y >= 125)
         {
            single_men = 1;
         }
         else
         {
            single_men = 0;
         }

         if (single_men != last_single_men)
         {
            glcd_fillScreen(0);
            single_menu(single_men);
         }

         if (p1_L)
         {
            switch (single_men)
            {
            case 0:
               mode = 'i';
               glcd_fillScreen(0);
               break;
            case 1:
               mode = 'J';
               new_game = 1;
               glcd_fillScreen(0);
               explic_m();
               delay_ms(3000);
               glcd_fillScreen(0);
               break;
            }
         }

         last_single_men = single_men;
         break;
      }
      case 'j':
      { // jugnado single
         if (new_level)
         {
            new_level = 0;
            for (int i = 0; i < 20; i++)
            {
               asteroids_lifes[i] = levels[level][i];
            }
            if (!level)
            {
               restart_screen();
               // while(!p1_R);
               x_pos = 63;
               y_pos = 31;
            }
            level++;
            for (unsigned int i = 0; i < 20; i++)
            { // inicializar la posicion de los asteroides a la esquina superior izquierda de manera random
               asteroids_x_pos[i] = (int)(((float)i) / 2.0f) % 10;
               asteroids_x_pos[i] = (int)((20.0f - (float)i) / 2.0f) % 10;
            }
            glcd_fillScreen(0);
         }
         else
         {
            ship_angle = (get_ship_angle(p1_pot_xx,p1_pot_yy) * 3.1416f) / 180.0f; // potenciometros angulos

            dx = get_x_vel();          // potenciometro movimiento
            dy = get_y_vel();          // potenciometro movimiento
            if ((float)x_pos + dx < 0) // mover jugador
               x_pos = 127 + (int)((float)x_pos + dx);
            else
               x_pos = (x_pos + (int)dx) % 127;

            if ((float)y_pos - dy < 0) // mover jugador
               y_pos = 63 + (int)((float)y_pos - dy);
            else
               y_pos = (y_pos - (int)dy) % 63;

            for (unsigned int i = 0; i < 20; i++)
            { // mover asteroides
               if ((signed int)asteroids_x_pos[i] + asteroids_dx[i] < 0)
                  asteroids_x_pos[i] = 127 + ((signed int)asteroids_x_pos[i] + asteroids_dx[i]);
               else if (asteroids_x_pos[i] + (unsigned int)asteroids_dx[i] > 127)
                  asteroids_x_pos[i] = 0 + ((asteroids_x_pos[i] + (unsigned int)asteroids_dx[i]) - 127);
               else
                  asteroids_x_pos[i] += asteroids_dx[i];

               if ((signed int)asteroids_y_pos[i] + asteroids_dy[i] < 0)
                  asteroids_y_pos[i] = 63 + ((signed int)asteroids_y_pos[i] + asteroids_dy[i]);
               else if (asteroids_y_pos[i] + (unsigned int)asteroids_dy[i] > 63)
                  asteroids_y_pos[i] = 0 + ((asteroids_y_pos[i] + (unsigned int)asteroids_dy[i]) - 63);
               else
                  asteroids_y_pos[i] += asteroids_dy[i];
            }

            for (unsigned int i = 0; i < 20; i++)
            { // checar si la posicion del jugador coincide con algun asteroide
               if (asteroids_lifes[i])
               {
                  if (((x_pos > asteroids_x_pos[i]) && (x_pos < (asteroids_x_pos[i] + asteroids_lifes[i] * 8))) && ((y_pos > asteroids_y_pos[i]) && (y_pos < (asteroids_y_pos[i] + asteroids_lifes[i] * 8))))
                  { // perder
                     mode = 'i';
                     new_level = 1;
                     glcd_fillScreen(1);
                     char text_asteroids[] = "YOU LOSE!";
                     glcd_text57(29, 22, text_asteroids, 1, 0);
                     delay_ms(1000);
                     glcd_fillScreen(0);
                     inicio(0);
                     if (score > best_score)
                        best_score = score;
                     if (level > best_level)
                        best_level = level;
                     score = 0;
                     level = 0;
                  }
               }
            }

            if (bullet)
            { // si hay una bala, calcular su posicion, y si hay impacto o se pierde la bala
               bullet_x_pos += (signed int)bullet_dx;
               bullet_y_pos += (signed int)bullet_dy;
               // checar collision con asteroides
               for (int i = 0; i < 20; i++)
               {
                  if (asteroids_lifes[i])
                  {
                     if ((((bullet_x_pos > asteroids_x_pos[i]) && (bullet_x_pos < (asteroids_x_pos[i] + asteroids_lifes[i] * 8))) && ((bullet_y_pos > asteroids_y_pos[i]) && (bullet_y_pos < (asteroids_y_pos[i] + asteroids_lifes[i] * 8)))) || (((last_bullet_x_pos > asteroids_x_pos[i]) && (last_bullet_x_pos < (asteroids_x_pos[i] + asteroids_lifes[i] * 8))) && ((last_bullet_y_pos > asteroids_y_pos[i]) && (last_bullet_y_pos < (asteroids_y_pos[i] + asteroids_lifes[i] * 8)))))
                     {
                        score += (4 - asteroids_lifes[i]);
                        asteroids_lifes[i]--;
                        asteroid_lost_a_life[i] = 1;
                        bullet_collision = 1;
                     }
                  }
               }

               glcd_line(last_last_bullet_x_pos, last_last_bullet_y_pos, last_bullet_x_pos, last_bullet_y_pos, 0);
               glcd_line(last_bullet_x_pos, last_bullet_y_pos, bullet_x_pos, bullet_y_pos, 1);
               if (!bullet_collision)
               { // si no hay colision, checar si se pierde la bala
                  if (bullet_x_pos > 127 || bullet_x_pos < 0 || bullet_y_pos > 63 || bullet_y_pos < 0)
                  { // bala sali?
                     bullet = 0;
                     glcd_line(last_bullet_x_pos, last_bullet_y_pos, bullet_x_pos, bullet_y_pos, 0);
                  }
               }
               else
               {
                  bullet_collision = 0;
                  bullet = 0;
                  glcd_line(last_bullet_x_pos, last_bullet_y_pos, bullet_x_pos, bullet_y_pos, 0);
               }
            }
            else
            {
               if (p1_R)
               { // disparar si no hay una bala actualmente
                  bullet = 1;
                  bullet_dx = cos(ship_angle) * (float)bullet_speed; // dispararla segun el angulo q se mira
                  bullet_dy = sin(ship_angle) * (float)bullet_speed;
                  bullet_x_pos = x_pos;
                  bullet_y_pos = y_pos;
                  sfx_time = 2;
               }
            }

            for (unsigned int i = 0; i < 20; i++)
            { // dibuar asteroides actuales
               if (asteroids_lifes[i])
               {
                  if (asteroid_lost_a_life[i])
                  {
                     draw_asteroid(last_asteroids_x_pos[i], last_asteroids_y_pos[i], asteroids_lifes[i] + 1, 0);
                     asteroid_lost_a_life[i] = 0;
                  }
                  else
                  {
                     draw_asteroid(last_asteroids_x_pos[i], last_asteroids_y_pos[i], asteroids_lifes[i], 0);
                  }
                  draw_asteroid(asteroids_x_pos[i], asteroids_y_pos[i], asteroids_lifes[i], 1);
               }
               else
               {
                  if (asteroid_lost_a_life[i])
                  {
                     draw_asteroid(last_asteroids_x_pos[i], last_asteroids_y_pos[i], asteroids_lifes[i] + 1, 0);
                     asteroid_lost_a_life[i] = 0;
                  }
               }
               last_asteroids_x_pos[i] = asteroids_x_pos[i];
               last_asteroids_y_pos[i] = asteroids_y_pos[i];
            }

            if (bullet)
            { // dibujar la nueva bala
               last_last_bullet_x_pos = last_bullet_x_pos;
               last_last_bullet_y_pos = last_bullet_y_pos;
               last_bullet_x_pos = bullet_x_pos;
               last_bullet_y_pos = bullet_y_pos;
            }
            else
            {
               last_last_bullet_x_pos = x_pos;
               last_last_bullet_y_pos = y_pos;
               last_bullet_x_pos = x_pos;
               last_bullet_y_pos = y_pos;
            }

            glcd_line(last_x_pos - 1, last_y_pos + 1, last_x_pos + 1, last_y_pos + 1, 0);
            glcd_line(last_x_pos - 1, last_y_pos - 1, last_x_pos + 1, last_y_pos - 1, 0);
            glcd_line(last_x_pos - 1, last_y_pos + 1, last_x_pos - 1, last_y_pos - 1, 0);
            glcd_line(last_x_pos + 1, last_y_pos + 1, last_x_pos + 1, last_y_pos - 1, 0);

            glcd_line(x_pos - 1, y_pos + 1, x_pos + 1, y_pos + 1, 1);
            glcd_line(x_pos - 1, y_pos - 1, x_pos + 1, y_pos - 1, 1);
            glcd_line(x_pos - 1, y_pos + 1, x_pos - 1, y_pos - 1, 1);
            glcd_line(x_pos + 1, y_pos + 1, x_pos + 1, y_pos - 1, 1);

            last_ship_angle = ship_angle; // guardar la posicion anterior
            last_x_pos = x_pos;
            last_y_pos = y_pos;

            for (int i = 0; i < 20; i++)
            { // checa si ya mataste todos los asteroides
               temp += asteroids_lifes[i];
            }
            if (!temp || new_level)
            {
               new_level = 1;
               // glcd_fillScreen(1);
            }
            scores(last_score, last_level, 0);
            scores(score, level, 1);
            last_score = score;
            last_level = level;

            delay_ms((int16)(22.0f - ((float)temp * 0.2f))); // manten los frames constantes segun cuantas cosas hayas dibujado

            temp = 0;
         }
         break;
      }
      case 'J':
      { // jugando multiplayer
         if (new_game)
         {
            new_game = 0;
            x_pos_1 = 10;
            y_pos_1 = 20;
            x_pos_2 = 120;
            y_pos_2 = 40;
         }

         dx_1 = get_d(p1_pot_x);        // potenciometro movimiento
         dy_1 = get_d(p1_pot_y);        // potenciometro movimiento
         if ((float)x_pos_1 + dx_1 < 4) // mover jugador
            x_pos_1 = 123 + (int)((float)x_pos_1 + dx_1);
         else
            x_pos_1 = (x_pos_1 + (int)dx_1) % 123;

         if ((float)y_pos_1 - dy_1 < 4) // mover jugador
            y_pos_1 = 59 + (int)((float)y_pos_1 - dy_1);
         else
            y_pos_1 = (y_pos_1 - (int)dy_1) % 59;

         dx_2 = get_d(p2_pot_x);        // potenciometro movimiento
         dy_2 = get_d(p2_pot_y);        // potenciometro movimiento
         if ((float)x_pos_2 + dx_2 < 4) // mover jugador
            x_pos_2 = 123 + (int)((float)x_pos_2 + dx_2);
         else
            x_pos_2 = (x_pos_2 + (int)dx_2) % 123;

         if ((float)y_pos_2 - dy_2 < 4) // mover jugador
            y_pos_2 = 59 + (int)((float)y_pos_2 - dy_2);
         else
            y_pos_2 = (y_pos_2 - (int)dy_2) % 59;

         if(shoot_1){
            cooldown_1 = 0;
         }
         else{
            cooldown_1--;
            if(cooldown_1 == 0){
               shoot_1 = 1;
            }
            if(cooldown_1 == 10){
               switch(aiming_1){
                  case 0: 
                  case 1:{
                     glcd_line(last_bullet_x_pos_1, last_bullet_y_pos_1, 128, last_bullet_y_pos_1, 0);
                     break;
                  }
                  case 2:{
                     glcd_line(last_bullet_x_pos_1, last_bullet_y_pos_1, last_bullet_x_pos_1, 0, 0);
                     break;
                  }
                  case 3:{
                     glcd_line(0, last_bullet_y_pos_1, last_bullet_x_pos_1, last_bullet_y_pos_1, 0);
                     break;
                  }
                  case 4:{
                     glcd_line(last_bullet_x_pos_1, last_bullet_y_pos_1, last_bullet_x_pos_1, 63, 0);
                     break;
                  }
               }
            }
         }

         if(p1_L && shoot_1){
            aiming_1 = laser_waza(p1_pot_xx, p1_pot_yy);
            switch(aiming_1){
               case 0: 
               case 1:{
                  glcd_line(x_pos_1, y_pos_1, 128, y_pos_1, 1);
                  if(x_pos_2 > x_pos_1){
                     if( (y_pos_2 < (y_pos_1 + 4)) && (y_pos_2 > (y_pos_1 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(1);
                     }
                  }
                  break;
               }
               case 2:{
                  glcd_line(x_pos_1, y_pos_1, x_pos_1, 0, 1);
                  if(y_pos_2 < y_pos_1){
                     if( (x_pos_2 < (x_pos_1 + 4)) && (x_pos_2 > (x_pos_1 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(1);
                     }
                  }
                  break;
               }
               case 3:{
                  glcd_line(0, y_pos_1, x_pos_1, y_pos_1, 1);
                  if(x_pos_2 < x_pos_1){
                     if( (y_pos_2 < (y_pos_1 + 4)) && (y_pos_2 > (y_pos_1 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(1);
                     }
                  }
                  break;
               }
               case 4:{
                  glcd_line(x_pos_1, y_pos_1, x_pos_1, 63, 1);
                  if(y_pos_2 > y_pos_1){
                     if( (x_pos_2 < (x_pos_1 + 4)) && (x_pos_2 > (x_pos_1 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(1);
                     }
                  }
                  break;
               }
            }
            cooldown_1 = 15;
            shoot_1 = 0;
            last_bullet_x_pos_1 = x_pos_1;
            last_bullet_y_pos_1 = y_pos_1;

         }

         if(shoot_2){
            cooldown_2 = 0;
         }
         else{
            cooldown_2--;
            if(cooldown_2 == 0){
               shoot_2 = 1;
            }
            if(cooldown_2 == 10){
               switch(aiming_2){
                  case 0: 
                  case 1:{
                     glcd_line(last_bullet_x_pos_2, last_bullet_y_pos_2, 128, last_bullet_y_pos_2, 0);
                     break;
                  }
                  case 2:{
                     glcd_line(last_bullet_x_pos_2, last_bullet_y_pos_2, last_bullet_x_pos_2, 0, 0);
                     break;
                  }
                  case 3:{
                     glcd_line(0, last_bullet_y_pos_2, last_bullet_x_pos_2, last_bullet_y_pos_2, 0);
                     break;
                  }
                  case 4:{
                     glcd_line(last_bullet_x_pos_2, last_bullet_y_pos_2, last_bullet_x_pos_2, 63, 0);
                     break;
                  }
               }
            }
         }

         if(p2_L && shoot_2){
            aiming_2 = laser_waza(p2_pot_xx, p2_pot_yy);
            switch(aiming_2){
               case 0: 
               case 1:{
                  glcd_line(x_pos_2, y_pos_2, 128, y_pos_2, 1);
                  if(x_pos_1 > x_pos_2){
                     if( (y_pos_1 < (y_pos_2 + 4)) && (y_pos_1 > (y_pos_2 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(0);
                     }
                  }
                  break;
               }
               case 2:{
                  glcd_line(x_pos_2, y_pos_2, x_pos_2, 0, 1);
                  if(y_pos_1 < y_pos_2){
                     if( (x_pos_1 < (x_pos_2 + 4)) && (x_pos_1 > (x_pos_2 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(0);
                     }
                  }
                  break;
               }
               case 3:{
                  glcd_line(0, y_pos_2, x_pos_2, y_pos_2, 1);
                  if(x_pos_1 < x_pos_2){
                     if( (y_pos_1 < (y_pos_2 + 4)) && (y_pos_1 > (y_pos_2 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(0);
                     }
                  }
                  break;
               }
               case 4:{
                  glcd_line(x_pos_2, y_pos_2, x_pos_2, 63, 1);
                  if(y_pos_1 > y_pos_2){
                     if( (x_pos_1 < (x_pos_2 + 4)) && (x_pos_1 > (x_pos_2 - 4))){
                        new_game = 1;
                        mode = 'i';
                        delay_ms(500);
                        draw_win(0);
                     }
                  }
                  break;
               }
            }
            cooldown_2 = 15;
            shoot_2 = 0;
            last_bullet_x_pos_2 = x_pos_2;
            last_bullet_y_pos_2 = y_pos_2;
         }

         

         
         draw_cuadrito(last_x_pos_1, last_y_pos_1, 0);
         draw_cuadrito(x_pos_1, y_pos_1, 1);

         last_x_pos_1 = x_pos_1;
         last_y_pos_1 = y_pos_1;

         draw_cuadrito(last_x_pos_2, last_y_pos_2, 0);
         draw_cuadrito(x_pos_2, y_pos_2, 1);

         last_x_pos_2 = x_pos_2;
         last_y_pos_2 = y_pos_2;
         delay_ms(35);

         if (win)
         {
            win = 0;
            if (!who)
            {
               glcd_fillScreen(0);
               inicio(1);
               char tewion[] = "GANA 1";
               glcd_text57(22, 50, tewion, 1, 1);
            }
            else
            {
               glcd_fillScreen(0);
               inicio(1);
               char tewion[] = "GANA 2";
               glcd_text57(22, 50, tewion, 1, 1);
            }
         }

         break;
      }
      default:
      {
         break;
      }
      }

      printf("p");
   }
}

void glcd_line(int x1, int y1, int x2, int y2, int1 color)
{
   signed int x, y, addx, addy, dx, dy;
   signed long P;
   int i;
   dx = abs((signed int)(x2 - x1));
   dy = abs((signed int)(y2 - y1));
   x = x1;
   y = y1;

   if (x1 > x2)
      addx = -1;
   else
      addx = 1;
   if (y1 > y2)
      addy = -1;
   else
      addy = 1;

   if (dx >= dy)
   {
      P = 2 * dy - dx;

      for (i = 0; i <= dx; ++i)
      {
         glcd_pixel(x, y, color);

         if (P < 0)
         {
            P += 2 * dy;
            x += addx;
         }
         else
         {
            P += 2 * dy - 2 * dx;
            x += addx;
            y += addy;
         }
      }
   }
   else
   {
      P = 2 * dx - dy;

      for (i = 0; i <= dy; ++i)
      {
         glcd_pixel(x, y, color);

         if (P < 0)
         {
            P += 2 * dx;
            y += addy;
         }
         else
         {
            P += 2 * dx - 2 * dy;
            x += addx;
            y += addy;
         }
      }
   }
}
void glcd_lineTracing(int *xx, int *yy, int n_points, int1 color)
{
   int i;
   for (i = 0; i < n_points - 1; i++)
   { // Iterate thru all points, drawing a line between each point
      glcd_line((xx + i), (yy + i), (xx + i + 1), (yy + i + 1), color);
   }
   glcd_line((xx + i), (yy + i), (xx), (yy), color);
}
void draw_triangle(int x, int y, float starting_angle, int1 color)
{
   int x_points_on_glcd = (int)malloc(3 * sizeof(int));
   int y_points_on_glcd = (int)malloc(3 * sizeof(int));

   for (char i = 0; i < 3; i++)
   {
      *(x_points_on_glcd + i) = x + (char)((float)5 * cos((float)i * internal_angle - starting_angle));
      *(y_points_on_glcd + i) = y + (char)((float)5 * sin((float)i * internal_angle - starting_angle));
   }

   glcd_lineTracing(x_points_on_glcd, y_points_on_glcd, 3, color);

   free(x_points_on_glcd);
   free(y_points_on_glcd);
}
void draw_asteroid(int x, int y, int life, int1 color)
{
   int x2 = x + life * 8;
   int y2 = y + life * 8;

   if (x2 > 127)
   {
      glcd_line(x, y, 127, y, color);
      glcd_line(0, y, x2 - 127, y, color);
      if (y2 > 63)
      {
         glcd_line(x, y2 - 63, 127, y2 - 63, color);
         glcd_line(0, y2 - 63, x2 - 127, y2 - 63, color);

         glcd_line(x, y, x, 63, color);
         glcd_line(x, 0, x, y2 - 63, color);

         glcd_line(x2 - 127, y, x2 - 127, 63, color);
         glcd_line(x2 - 127, 0, x2 - 127, y2 - 63, color);
      }
      else
      {
         glcd_line(x, y2, 127, y2, color);
         glcd_line(0, y2, x2 - 127, y2, color);

         glcd_line(x, y, x, y2, color);

         glcd_line(x2 - 127, y, x2 - 127, y2, color);
      }
   }
   else
   {
      glcd_line(x, y, x2, y, color);
      if (y2 > 63)
      {
         glcd_line(x, y2 - 63, x2, y2 - 63, color);

         glcd_line(x, y, x, 63, color);
         glcd_line(x, 0, x, y2 - 63, color);

         glcd_line(x2, y, x2, 63, color);
         glcd_line(x2, 0, x2, y2 - 63, color);
      }
      else
      {
         glcd_line(x, y2, x2, y2, color);

         glcd_line(x, y, x, y2, color);

         glcd_line(x2, y, x2, y2, color);
      }
   }
}

void glcd_text57(int x, int y, char *textptr, int size, int1 color)
{

   int i, j, k, l, m; // Loop counters
   BYTE pixelData[5]; // Stores character data

   for (i = 0; textptr[i] != '\0'; ++i, ++x) // Loop through the passed string
   {
      if (textptr[i] < 'S') // Checks if the letter is in the first text array
         memcpy(pixelData, TEXT[textptr[i] - ' '], 5);
      else if (textptr[i] <= '~') // Check if the letter is in the second array
         memcpy(pixelData, TEXT2[textptr[i] - 'S'], 5);
      else
         memcpy(pixelData, TEXT[0], 5); // Default to space

      if (x + 5 * size >= GLCD_WIDTH) // Performs character wrapping
      {
         x = 0;             // Set x at far left position
         y += 7 * size + 1; // Set y at next position down
      }
      for (j = 0; j < 5; ++j, x += size) // Loop through character byte data
      {
         for (k = 0; k < 7 * size; ++k) // Loop through the vertical pixels
         {
            if (bit_test(pixelData[j], k)) // Check if the pixel should be set
            {
               for (l = 0; l < size; ++l) // The next two loops change the
               {                          // character's size
                  for (m = 0; m < size; ++m)
                  {
                     glcd_pixel(x + m, y + k * size + l, color); // Draws the pixel
                  }
               }
            }
         }
      }
   }
}

void inicio(int sec)
{
   switch (sec)
   {
   case 1:
   {
      char text_asteroids[] = "ASTEROIDS";
      glcd_text57(38, 4, text_asteroids, 1, 1);
      
      char text_aid[] = "SELECT & HOLD: [LJ]";
      glcd_text57(10, 49, text_aid, 1, 1);
      char text_aid2[] = "PRESS: [BR]";
      glcd_text57(40, 57, text_aid2, 1, 1);


      for (int i = 0; i < 16; i++)
      {
         for (int j = 0; j < 16; j++)
         {
            if (personaje[j][i] == 1)
            {
               glcd_pixel(20 + i, 26 + j, 0);

               glcd_pixel(82 + i, 26 + j, 1);
               glcd_pixel(96 + i, 26 + j, 1);
            }
            else
            {
               glcd_pixel(20 + i, 26 + j, 1);

               glcd_pixel(82 + i, 26 + j, 0);
               glcd_pixel(96 + i, 26 + j, 0);
            }
         }
      }
      break;
   }
   case 2:
   {
      char text_asteroids[] = "ASTEROIDS";
      glcd_text57(38, 4, text_asteroids, 1, 1);
      char text_aid[] = "SELECT & HOLD: [LJ]";
      glcd_text57(10, 49, text_aid, 1, 1);
      char text_aid2[] = "PRESS: [BR]";
      glcd_text57(40, 57, text_aid2, 1, 1);

      for (int i = 0; i < 16; i++)
      {
         for (int j = 0; j < 16; j++)
         {
            if (personaje[j][i] == 1)
            {
               glcd_pixel(20 + i, 26 + j, 1);

               glcd_pixel(82 + i, 26 + j, 0);
               glcd_pixel(96 + i, 26 + j, 0);
            }
            else
            {
               glcd_pixel(20 + i, 26 + j, 0);

               glcd_pixel(82 + i, 26 + j, 1);
               glcd_pixel(96 + i, 26 + j, 1);
            }
         }
      }
      break;
   }
   default:
   {
      for (int i = 2; i < 13; i++)
      {
         glcd_line(32, i, 97, i, 1);
      }
      char text_asteroids[] = "ASTEROIDS";
      glcd_text57(38, 4, text_asteroids, 1, 0);
      char text_aid[] = "SELECT & HOLD: [LJ]";
      glcd_text57(10, 49, text_aid, 1, 1);
      char text_aid2[] = "PRESS: [BR]";
      glcd_text57(40, 57, text_aid2, 1, 1);

      for (int i = 0; i < 16; i++)
      {
         for (int j = 0; j < 16; j++)
         {
            if (personaje[j][i] == 1)
            {
               glcd_pixel(20 + i, 26 + j, 1);

               glcd_pixel(82 + i, 26 + j, 1);
               glcd_pixel(96 + i, 26 + j, 1);
            }
            else
            {
               glcd_pixel(20 + i, 26 + j, 0);

               glcd_pixel(82 + i, 26 + j, 0);
               glcd_pixel(96 + i, 26 + j, 0);
            }
         }
      }
      break;
   }
   }
}

void single_menu(int1 play)
{
   char text_ret[] = "RETURN";
   char text_play[] = "PLAY";
   if (play)
   {
      for (int i = 29; i <= 41; i++)
      {
         glcd_line(20, i, 90, i, 1);
      }
      glcd_text57(24, 30, text_play, 1, 0);

      for (int i = 43; i <= 55; i++)
      {
         glcd_line(20, i, 90, i, 0);
      }
      glcd_text57(24, 44, text_ret, 1, 1);

      char text_aid[] = "SELECT & HOLD: [LJ]";
      glcd_text57(10, 5, text_aid, 1, 1);
      char text_aid2[] = "PRESS: [BL]";
      glcd_text57(10, 13, text_aid2, 1, 1);
   }
   else
   {
      for (int i = 29; i <= 41; i++)
      {
         glcd_line(20, i, 90, i, 0);
      }
      glcd_text57(24, 30, text_play, 1, 1);

      for (int i = 43; i <= 55; i++)
      {
         glcd_line(20, i, 90, i, 1);
      }
      glcd_text57(24, 44, text_ret, 1, 0);

      char text_aid[] = "SELECT & HOLD: [LJ]";
      glcd_text57(10, 5, text_aid, 1, 1);
      char text_aid2[] = "PRESS: [BL]";
      glcd_text57(10, 13, text_aid2, 1, 1);
   }
}

void scores(unsigned int16 scr, unsigned int lvl, int1 color)
{
   char text_score[6];
   sprintf(text_score, "S:%lu", scr);
   glcd_text57(0, 0, text_score, 1, color);
   char text_level[4];
   sprintf(text_level, "L:%u", lvl);
   glcd_text57(103, 0, text_level, 1, color);
}

float get_ship_angle(int x, int y)
{
   if(x > 185){ // derecha
   return 270.0f;
   return 90.0f;
      return 0.0f;
   }
   else if(x < 70){// izquierda
   return 90.0f;
   return 270.0f;
      return 180.0f;
   }
   if(y > 185){ // arriba
   return 0.0f;
      return 180.0f;
      return 90.0f;
   }
   else if(y < 70){ // abajo
   return 180.0f;
      return 0.0f;
      return 270.0f;
   }
   return 0.0f;
}
float get_x_vel()
{
   return (float)((float)p1_pot_x - 127.0f) / 30.0f;
}
float get_y_vel()
{
   return (float)((float)p1_pot_y - 127.0f) / 30.0f;
}


int laser_waza(int x, int y){
   if(x > 185){ // derecha
      return 2;
   }
   else if(x < 70){// izquierda
      return 4;
   }
   if(y > 185){ // arriba
      return 1;
   }
   else if(y < 70){ // abajo
      return 3;
   }
   return 0;
}

void draw_cuadrito(int x, int y, int1 col)
{
   glcd_line(x - 2, y + 2, x + 2, y + 2, col);
   glcd_line(x - 2, y - 2, x + 2, y - 2, col);
   glcd_line(x - 2, y + 2, x - 2, y - 2, col);
   glcd_line(x + 2, y + 2, x + 2, y - 2, col);
   
/*   glcd_line(x - 4, y + 4, x + 4, y + 4, col);
   glcd_line(x - 4, y - 4, x + 4, y - 4, col);
   glcd_line(x - 4, y + 4, x - 4, y - 4, col);
   glcd_line(x + 4, y + 4, x + 4, y - 4, col);*/
}

void rect(int x1, int y1, int x2, int y2){
   glcd_line(x1, y1, x2, y1, 1);
   glcd_line(x1, y1, x1, y2, 1);
   glcd_line(x2, y1, x2, y2, 1);
   glcd_line(x1, y2, x2, y2, 1);
}

void grafico_control(){// rectangulo control
   rect(20,20,108,44);
   rect(20,10,40,20);
   rect(88,10,108,20);
   rect(22,24,54,34);
   rect(74,24,106,34);
   char bl[] = "BL";
   glcd_text57(22,12, bl, 1, 1);
   char br[] = "BR";
   glcd_text57(90,12, br, 1, 1);
   char jl[] = "LJ";
   glcd_text57(24,26, jl, 1, 1);
   char jr[] = "RJ";
   glcd_text57(76,26, jr, 1, 1);
   char p[] = "Use P1 in menus";
   glcd_text57(20,56, p, 1, 1);
}

void explic_m(){// rectangulo control
   rect(20,20,108,44);
   rect(20,10,60,20);
   rect(88,10,108,20);
   rect(22,24,54,34);
   rect(74,24,106,34);
   char bl[] = "SHOOT";
   glcd_text57(22,12, bl, 1, 1);
   char br[] = "-";
   glcd_text57(90,12, br, 1, 1);
   char jl[] = "MOVE";
   glcd_text57(24,26, jl, 1, 1);
   char jr[] = "AIM";
   glcd_text57(76,26, jr, 1, 1);
   char p[] = "For P1 & P2";
   glcd_text57(20,56, p, 1, 1);
}

void explic_s(){// rectangulo control
   rect(20,20,108,44);
   rect(20,10,40,20);
   rect(68,10,108,20);
   rect(22,24,54,34);
   rect(74,24,106,34);
   char bl[] = "-";
   glcd_text57(22,12, bl, 1, 1);
   char br[] = "SHOOT";
   glcd_text57(70,12, br, 1, 1);
   char jl[] = "MOVE";
   glcd_text57(24,26, jl, 1, 1);
   char jr[] = "AIM";
   glcd_text57(76,26, jr, 1, 1);
   char p[] = "For P1 only";
   glcd_text57(20,56, p, 1, 1);
}