/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

int getAngulo(int alvo){
  Vector3 atual;
  get_current_GPS_position(&atual);
  int dx = friends_locations[alvo].x - atual.x,
      dz = friends_locations[alvo].z - atual.z;
  if((dz != 0) & (dx/dz*10 == 5)){
    if (dx > 0)return 45;
    else return 225;
  }
  if ((dz != 0) & (dx/dz*10 == -5)){
    if (dx < 0) return 315;
    else return 135;
  }
  int modDx = dx, modDz = dz;
  if (modDx < 0) modDx = - modDx;
  if (modDz < 0) modDz = - modDz;
  if (modDx > modDz){
    if (dx > 0) return 90;
    else return 270;
  } 
  else{
    if (dz > 0) return 0;
    else return 180;
  }
}
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

int declive(Vector3 atual){
  int inclinacao = 0;
  if (atual.x > 300) inclinacao = inclinacao + 360 - atual.x;
  else inclinacao = inclinacao + atual.x; 
  if (atual.z > 300) inclinacao = inclinacao + 360 - atual.z;
  else inclinacao = inclinacao + atual.z;
  puts("Declive: ");
  intToASCII(inclinacao,1);
  return inclinacao; 
}

int quadrado(int numero){
  return numero * numero;
}

int distanciaQuadrada(Vector3 p1, Vector3 p2){
  int distancia  = quadrado(p1.x - p2.x)+
                   quadrado(p1.y - p2.y)+
                   quadrado(p1.z - p2.z);
  return distancia;
}

void wait(unsigned int tempo){
  unsigned int inicio = get_time();
  while (get_time() - inicio < tempo){
    //Nada
  };
}

void vire(int angulo){  
  /* Vira para angulo-graus absoluto do giroscopio */
  if (angulo < 0) angulo = angulo + 360;
  if (angulo > 360) angulo = angulo % 360;
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
void ande(int distancia, int alvo, int angulo){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio;
  int impulso = 10;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  int sinalizador = 1;
  set_torque(impulso,impulso);
  while(distanciaQuadrada(atual,inicio) < quadrado(distancia)){
    // Vector3 anterior;
    // get_current_GPS_position(&anterior);
    // wait(3000);
    // get_current_GPS_position(&atual);
    // if (sinalizador == 1 & distanciaQuadrada(anterior,atual) < 20){
    //   vire(angulo + 90);
    //   sinalizador = -1;
    // }
    // else if (sinalizador == -1 & distanciaQuadrada(anterior, atual) < 20){
    //   vire(angulo - 90);
    // }
    // if (get_time() % 800 == 0){
    //   Vector3 angulos;
    //   get_gyro_angles(&angulos);
    //   puts("Angulo em x: ");
    //   intToASCII(angulos.x,1);
    //   puts("Angulo em y: ");
    //   intToASCII(angulos.y,1);
    //   puts("Angulo em z: ");
    //   intToASCII(angulos.z,1);
    //   puts("-----------------------------------\n");
    // }
    if (distanciaQuadrada(friends_locations[alvo],atual) <= 2500) break;
    Vector3 giroscopio;
    get_gyro_angles(&giroscopio);
    int dAngulo = giroscopio.y - angulo;
    if (dAngulo < 0) dAngulo = - dAngulo;
    if (dAngulo > 90) break;
    if (declive(giroscopio) > 40){        
      /* Ve se o Uoli esta tentando escalar morro, se sim, faz desvio */    
      puts("...\nDesviando...\n...\n");
      vire(angulo - 90);
      set_torque(10,10);
      wait(2500);
    }
  }
  puts("fim de andar");
  set_torque(0,0);
}

/* Nao estao sendo utilizados no momento
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
*/

int main(){
  Vector3 atual;
  get_current_GPS_position(&atual);
  int indices[sizeof(friends_locations)];
  int distancias[sizeof(friends_locations)];
  int alvo, qtdeAmigos = sizeof(friends_locations),
            qtdeInimigos = sizeof(dangerous_locations);
  /* Ordenando os amigos por ordem crescente de distancia da origem */
  int i;
  for (i=0;i<qtdeAmigos;i++){
    indices[i] = i;
    distancias[i] = distanciaQuadrada(friends_locations[i],atual);
  }
  int ini = 0, fim = qtdeAmigos - 1, j =1;
  i = 0;
  while(ini<fim){
    if (distancias[i] > distancias[j]){
      int aux = distancias[i];
      distancias[i] = distancias[j];
      distancias[j] = aux;
      aux = indices[i];
      indices[i] = indices[j];
      indices[j] = aux;
    }
    i++;
    j++;
    if (j > fim){
      i = 0;
      j = 1;
      fim--;
    }
  }
  /* Buscando cada amigo, d_min**2 = 2500*/
  for (i=0;i<qtdeAmigos;i++){
    alvo = indices[i];
    while(distanciaQuadrada(friends_locations[alvo],atual) > 2500){
      int direcao = getAngulo(alvo);
      vire(direcao);
      ande(10,alvo,direcao);
      get_current_GPS_position(&atual);
    }
    puts("Amigo encontrado\n");
  }
  while(1){
  }
  return 0;
}