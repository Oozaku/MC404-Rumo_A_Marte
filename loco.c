/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

int main(){
  /* Procurando o melhor caminho */
  set_time(0);
  Vector3 p;
  get_current_GPS_position(&p);
  return 0;
}