/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

int main(){
  /* Procurando o melhor caminho */
  while (1){
    set_head_servo(0, 17);
    set_head_servo(1, 90);
    set_head_servo(2, 155);
    set_head_servo(0, 100);
    set_head_servo(1, 60);
    set_head_servo(2, 155);
  }
  return 0;
}