/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

/*variáveis globais*/
int numberOfFriends = sizeof(friends_locations)/sizeof(Vector3);
int numberOfDangLoc = sizeof(dangerous_locations)/sizeof(Vector3);
int tabelaTan[18] = {0, 87, 176, 268, 363, 466, 577, 700, 839, 1000,
                     1191, 1428, 1732, 2144, 2747, 3732, 5671, 11430};

/*Protótipos de Função*/
void intToASCII(int numero, int quebraLinha);
int buscaBinariaTan(int chave);
int quadrado(int numero);
void wait(unsigned int tempo);
int distanciaQuadrada(Vector3 alvo, Vector3 atual);
void vire(int angulo);
void ande(int distancia, int impulso);
void goToFriend(int alvo);
int verificarInimigos();
int angulo(Vector3 alvo, Vector3 atual);
void contornarInimigo();


/*Funções utilitárias*/
void intToASCII(int numero, int quebraLinha){
  char ascii[2];
  if (numero < 0){
    numero = numero*(-1);
    puts("-");
  }
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

int buscaBinariaTan(int chave){
  int tam = 17, n = tam/2;
  while (1){
    if (tabelaTan[n] == chave){
      return n;
    }
    if (chave > tabelaTan[n]){
      if (n == tam)
        return n;
      if (chave<tabelaTan[n+1]){
        if ((chave - tabelaTan[n]) > (tabelaTan[n+1] - chave))
          return n+1;
        else
          return n;
      }
      n = (tam + n)/2;
    }
    else{
      if (chave>tabelaTan[n-1]){
        if((chave - tabelaTan[n-1]) > (tabelaTan[n] - chave))
          return n;
        else 
          return n-1;
      }
      n = n/2;
    }
  }
}

int quadrado(int numero){
  return numero * numero;
}

void wait(unsigned int tempo){
  unsigned int inicio = get_time();
  while (get_time() - inicio < tempo){
    //Nada
  }
}

int distanciaQuadrada(Vector3 alvo, Vector3 atual){
  int distancia  = quadrado(alvo.x - atual.x)+
                   quadrado(alvo.y - atual.y)+
                   quadrado(alvo.z - atual.z);
  return distancia;
}

/*Funções de Movimentação*/
void vire(int angulo){  
  /* Vira para angulo-graus pararoluto do giroscopio */
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

void parar(){
  set_torque(-1, -1);
  wait(1000);
  set_torque(0, 0);
  wait(500);
  set_torque(-1, -1);
  wait(1000);
  set_torque(0,0);
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

// void andeX(int alvo, int impulso){
//   Vector3 atual;
//   get_current_GPS_position(&atual);
//   int distancia = friends_locations[alvo].x - atual.x;
//   if (distancia > 0){
//     vire(270);
//     int bloqueio = get_us_distance();
//     while(bloqueio == -1 | bloqueio > 100){
//       set_torque(impulso,impulso);
//       bloqueio = get_us_distance();
//       get_current_GPS_position(&atual);
//       if (friends_locations[alvo].x - atual.x <= 0){
//         set_torque(0,0);
//         return;
//       }
//       get_gyro_angles(&atual);
//       int desvioAngular = quadrado(atual.y - 270);
//       if (desvioAngular > 100){
//         set_torque(0,0);
//         return;
//       }
//     }
//   }
//   if (distancia < 0){
//     vire(90);
//     int bloqueio = get_us_distance();
//     while(bloqueio == -1 | bloqueio > 100){
//       set_torque(impulso,impulso);
//       bloqueio = get_us_distance();
//       get_current_GPS_position(&atual);
//       if (friends_locations[alvo].x - atual.x >= 0){
//         set_torque(0,0);
//         return;
//       }
//       get_gyro_angles(&atual);
//       int desvioAngular = quadrado(atual.y - 90);
//       if (desvioAngular > 100){
//         set_torque(0,0);
//         return;
//       }
//     }
//   }
//   set_torque(0,0);
//   return;
// }

// void andeZ(int alvo, int impulso){
//   Vector3 atual;
//   get_current_GPS_position(&atual);
//   int distancia = friends_locations[alvo].z - atual.z;
//   if (distancia > 0){
//     vire(0);
//     int bloqueio = get_us_distance();
//     while(bloqueio == -1 | bloqueio > 100){
//       set_torque(impulso,impulso);
//       bloqueio = get_us_distance();
//       get_current_GPS_position(&atual);
//       if (friends_locations[alvo].z - atual.z <= 0){
//         set_torque(0,0);
//         return;
//       }
//       get_gyro_angles(&atual);
//       int desvioAngular = quadrado(atual.y - 0);
//       if (desvioAngular > 100){
//         set_torque(0,0);
//         return;
//       }
//     }
//   }
//   if (distancia < 0){
//     vire(180);
//     int bloqueio = get_us_distance();
//     while(bloqueio == -1 | bloqueio > 100){
//       set_torque(impulso,impulso);
//       bloqueio = get_us_distance();
//       get_current_GPS_position(&atual);
//       if (friends_locations[alvo].z - atual.z >= 0){
//         set_torque(0,0);
//         return;
//       }
//       get_gyro_angles(&atual);
//       int desvioAngular = quadrado(atual.y - 90);
//       if (desvioAngular > 100){
//         set_torque(0,0);
//         return;
//       }
//     }
//   }
//   set_torque(0,0);
//   return;
// }

void goToFriend(int alvo){
  Vector3 uoli, amigo = friends_locations[alvo];
  get_current_GPS_position(&uoli);
  vire(angulo(amigo, uoli));

  set_torque(10, 10);
  while (1){
    if (distanciaQuadrada(amigo, uoli) < 1000)
      break;
    if (verificarInimigos()){
      parar();
      contornarInimigo();
    }
    // if (obstaculo()){
    //   parar();
    //   desviarObstaculo();
    // }
  }
  parar();
}



/*Funções de Verificação*/
int verificarInimigos(){
  Vector3 atual;
  for(int i = 0; i < numberOfDangLoc; i++){
    get_current_GPS_position(&atual);
    if (distanciaQuadrada(dangerous_locations[i], atual) < 225)
      return 1;
  }
  return 0;
}

int angulo(Vector3 alvo, Vector3 atual){
  int dx = alvo.x - atual.x, dz = alvo.z - atual.z, tan, angulo;
  puts("dx");
  intToASCII(dx, 1);
  puts("dz");
  intToASCII(dz, 1);
  if (dz != 0){
    tan = (dx*1000)/dz;
    if (tan < 0)
      tan = tan*(-1);
    angulo = 5*buscaBinariaTan(tan);
  }
  else
    angulo = 90;
  puts("Angulo Tabela");
  intToASCII(angulo, 1);
  if (dx >= 0){
    if (dz >= 0)
      return angulo;
    else
      return (angulo + 90);
  } else {
    if (dz >= 0)
      return (angulo + 270);
    else
      return (angulo + 180);
  }
    

}

/*Funções de tratamento situacional*/
void contornarInimigo(){
  Vector3 atual, inimigo;
  for(int i = 0; i < numberOfDangLoc; i++){
    get_current_GPS_position(&atual);
    if (distanciaQuadrada(dangerous_locations[i], atual) < 225){
      inimigo = dangerous_locations[i];
      break;
    }
  }
}

/*Main*/
int main(){
  Vector3 atual;
  get_current_GPS_position(&atual);
  intToASCII(atual.x, 1);
  intToASCII(atual.y, 1);
  intToASCII(atual.z, 1);
  goToFriend(0);
  while(1){
  }
  return 0;
}