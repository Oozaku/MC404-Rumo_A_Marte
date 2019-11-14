/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

void intToASCII(int numero, int quebraLinha){
  char ascii[2];
  ascii[0] = (char) numero%10 + 48;
  ascii[1] = '\0';
  if (numero/10 < 1){
    puts(ascii);
  } else{
    intToASCII(numero/10, 0);
    puts(ascii);
  }
  if (quebraLinha)
    puts("\n");
}

int quadrado(int numero){
  return numero * numero;
}

void wait(unsigned int tempo){
  unsigned int inicio = get_time();
  while (get_time() - inicio < tempo){
    //Nada
  };
}

void vire(int angulo){  
  /* Vira para angulo-graus absoluto do giroscopio */
  Vector3 atual;
  get_gyro_angles(&atual);
  int aux = angulo - atual.y, torque = 15;
  if (aux > 0 & aux <= 180){
    set_torque(torque,-torque);
  }
  else if (aux > 0 & aux > 180){
    set_torque(-torque,torque);
  }
  else if (aux < 0 & -aux < 180){
    set_torque(-torque,torque);
  }
  else{
    set_torque(torque,-torque);
  }
  while (atual.y != angulo){
    get_gyro_angles(&atual);
  }
  set_torque(0,0);
}

void ABS(){
  
}
void ande(int distancia, int impulso){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  int dx, dz;
  dx = quadrado(atual.x - inicio.x);
  dz = quadrado(atual.z - inicio.z);
  distancia = quadrado(distancia);
  while (distancia > dx + dz){
    set_torque(impulso,impulso);
    get_current_GPS_position(&atual);
    dx = quadrado(atual.x - inicio.x);
    dz = quadrado(atual.z - inicio.z);
  }
  set_torque(0,0);
}

void andeX(int alvo, int impulso){
  Vector3 atual;
  get_current_GPS_position(&atual);
  int distancia = friends_locations[alvo].x - atual.x;
  if (distancia > 0){
    vire(270);
    int bloqueio = get_us_distance();
    while(bloqueio == -1 | bloqueio > 100){
      set_torque(impulso,impulso);
      bloqueio = get_us_distance();
      get_current_GPS_position(&atual);
      if (friends_locations[alvo].x - atual.x <= 0){
        set_torque(0,0);
        return;
      }
      get_gyro_angles(&atual);
      int desvioAngular = quadrado(atual.y - 270);
      if (desvioAngular > 100){
        set_torque(0,0);
        return;
      }
    }
  }
  if (distancia < 0){
    vire(90);
    int bloqueio = get_us_distance();
    while(bloqueio == -1 | bloqueio > 100){
      set_torque(impulso,impulso);
      bloqueio = get_us_distance();
      get_current_GPS_position(&atual);
      if (friends_locations[alvo].x - atual.x >= 0){
        set_torque(0,0);
        return;
      }
      get_gyro_angles(&atual);
      int desvioAngular = quadrado(atual.y - 90);
      if (desvioAngular > 100){
        set_torque(0,0);
        return;
      }
    }
  }
  set_torque(0,0);
  return;
}

void andeZ(int alvo, int impulso){
  Vector3 atual;
  get_current_GPS_position(&atual);
  int distancia = friends_locations[alvo].z - atual.z;
  if (distancia > 0){
    vire(0);
    int bloqueio = get_us_distance();
    while(bloqueio == -1 | bloqueio > 100){
      set_torque(impulso,impulso);
      bloqueio = get_us_distance();
      get_current_GPS_position(&atual);
      if (friends_locations[alvo].z - atual.z <= 0){
        set_torque(0,0);
        return;
      }
      get_gyro_angles(&atual);
      int desvioAngular = quadrado(atual.y - 0);
      if (desvioAngular > 100){
        set_torque(0,0);
        return;
      }
    }
  }
  if (distancia < 0){
    vire(180);
    int bloqueio = get_us_distance();
    while(bloqueio == -1 | bloqueio > 100){
      set_torque(impulso,impulso);
      bloqueio = get_us_distance();
      get_current_GPS_position(&atual);
      if (friends_locations[alvo].z - atual.z >= 0){
        set_torque(0,0);
        return;
      }
      get_gyro_angles(&atual);
      int desvioAngular = quadrado(atual.y - 90);
      if (desvioAngular > 100){
        set_torque(0,0);
        return;
      }
    }
  }
  set_torque(0,0);
  return;
}

int distanciaQuadrada(int alvo, Vector3 atual){
  int distancia  = quadrado(friends_locations[alvo].x - atual.x)+
                   quadrado(friends_locations[alvo].y - atual.y)+
                   quadrado(friends_locations[alvo].z - atual.z);
  return distancia;
}

int main(){
  Vector3 atual;
  get_current_GPS_position(&atual);
  int alvo, qtdeAmigos = sizeof(friends_locations),
            qtdeInimigos = sizeof(dangerous_locations);
  for (alvo=0;alvo<qtdeAmigos;alvo++){
    while(distanciaQuadrada(alvo,atual) > 2500){
      andeX(alvo,10);
      get_current_GPS_position(&atual);
      if (distanciaQuadrada(alvo,atual) <= 2500) break;
      andeZ(alvo,10);
      get_current_GPS_position(&atual);
    }
  }
  while(1){
  }
  return 0;
}