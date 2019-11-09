/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"
#include <math.h>

int melhorAngulo(Vector3 atual, Vector3 destino){ 
  /* Angulo que leva do atual para o destino em linha reta */
  int angulo = (int) ((180/3.14)*atan((atual.x - destino.x)/(atual.z - destino.z)));
  int aux = atual.x - destino.x;
  if (aux < 0 & angulo < 0) return 180 - angulo;
  if (aux < 0 & angulo > 0) return angulo;
  if (aux > 0 & angulo < 0) return 360 - angulo;
  return angulo;
}

void vire(int angulo){  
  /* Vira para angulo-graus absoluto do giroscopio */
  Vector3 atual;
  get_gyro_angles(&atual);
  while (atual.z != angulo){
    set_torque(5,-5);
    get_gyro_angles(&atual);
  }
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
    set_torque(10,10);
    get_current_GPS_position(&atual);
    dx = atual.x - inicio.x;
    dz = atual.z - inicio.z;
    dx = dx * dx;
    dz = dz * dz;
  }
}

int checaPerigo(Vector3 *atual, int angulo, int distancia){
  /* Ve se nessa direcao o robo se aproxima demais de uma zona de perigo
  Se o Uoli se aproximar demais no caminho planejado, retorna -1, caso contrario 1*/
  int i,j;
  for (i=0;i<sizeof(dangerous_locations);i++){
    for (j=0;j<distancia;j=j+10){
      int dX = atual->x + sin(angulo)*j-dangerous_locations[i].x;
      int dZ = atual->z + cos(angulo)*j-dangerous_locations[i].z;
      dX = dX * dX;
      dZ = dZ * dZ;
      if (dX + dZ < 10000){
        return -1;  /* Aproxima demais do perigo */
      } 
    }
  }
  return 1;
}
int main(){
  /* Procurando o melhor caminho */
  // while (1){
  //   set_head_servo(0, 17);
  //   set_head_servo(1, 90);
  //   set_head_servo(2, 155);
  //   set_head_servo(0, 100);
  //   set_head_servo(1, 60);
  //   set_head_servo(2, 155);
  // }
  set_time(0);
  int i;
  int numberOfFriends = sizeof(friends_locations);
  int numberOfEnemies = sizeof(dangerous_locations);
  for (i=0;i<numberOfFriends;i++){
    Vector3 posicao;
    get_current_GPS_position(&posicao);
    int dx, dz;
    dx = friends_locations[i].x - posicao.x;
    dz = friends_locations[i].z - posicao.z;
    dx = dx * dx;
    dz = dz * dz;
    while(dx+dz > 2500){
      int angulo = melhorAngulo(posicao, friends_locations[i]), dAngulo = 0, incremento = 5;
      while(1){ /* Procura um caminho em linha reta para aproximar do alvo */
        if (dAngulo > 40){
          dAngulo = -5;
          incremento = -5;
        }
        vire(angulo + dAngulo);
        int distancia = get_us_distance();
        if (distancia == -1 | distancia > 200){ /* Sem obstaculo ou caminho livre ate 2m */
          if (checaPerigo(&posicao,angulo + dAngulo,10) == 1){/*Caminho seguro nos proximos 1 m */
            ande(10);
            break;
          }
          else{
            dAngulo = dAngulo + incremento;
            continue;
          }
        }
        else{
          dAngulo = dAngulo + incremento;
          continue;
        }
      }
      dx = friends_locations[i].x - posicao.x;
      dz = friends_locations[i].z - posicao.z;
      dx = dx * dx;
      dz = dz * dz;
    }

  }
  return 0;
}