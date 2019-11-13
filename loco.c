/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

void traduz(int numero){
  int aux = numero;
  char a[] = "123\n";
  a[2] = aux % 10 + 48;
  aux = aux / 10;
  a[1] = aux % 10 + 48;
  aux = aux / 10;
  a[0] = aux + 48;
  puts(a);
}
void vire(int angulo){  
  /* Vira para angulo-graus absoluto do giroscopio */
  Vector3 atual;
  get_gyro_angles(&atual);
  set_torque(20,-20);
  while (atual.y != angulo){
    set_torque(20,-20);
    get_gyro_angles(&atual);
  }
  set_torque(0,0);
}

void ande(int distancia){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  int dx, dz;
  dx = atual.x - inicio.x;
  dz = atual.z - inicio.z;
  dx = dx * dx;
  dz = dz * dz;
  distancia = distancia * distancia;
  while (distancia > dx + dz){
    set_torque(90,90);
    get_current_GPS_position(&atual);
    dx = atual.x - inicio.x;
    dz = atual.z - inicio.z;
    dx = dx * dx;
    dz = dz * dz;
  }
  set_torque(0,0);
}

int main(){
  Vector3 atual;
  get_current_GPS_position(&atual);
  traduz(atual.x);
  traduz(atual.y);
  traduz(atual.z);
  while(1){
  }
  return 0;
}