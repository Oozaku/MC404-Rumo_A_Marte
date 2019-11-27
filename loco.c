/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

/*variáveis globais*/
int numberOfFriends = sizeof(friends_locations)/sizeof(Vector3);
int numberOfDangLoc = sizeof(dangerous_locations)/sizeof(Vector3);
static int tabelaTan[18] = {0, 87, 176, 268, 363, 466, 577, 700, 839, 1000,
                     1191, 1428, 1732, 2144, 2747, 3732, 5671, 11430};

/*Protótipos de Função*/
void intToASCII(int numero, int quebraLinha);
int quadrado(int numero);
int buscaBinariaTan(int chave);
void wait(unsigned int tempo);
int distanciaQuadrada(Vector3 alvo, Vector3 atual);
void vire(int angulo);
void ande(int distancia, int alvo);
int verificarInimigos();
void contornarInimigo();
void desviar(int varreduraObstaculo[5], int alvo);
void printLocation(Vector3 local);


/*Funções utilitárias*/
void intToASCII(int numero, int quebraLinha){
  char ascii[2];
  if (numero < 0){
    numero = numero*(-1);
    puts("-");
  }
  ascii[0] = (char) numero%10 + 48;
  ascii[1] = '\0';
  if (numero/10 == 0){
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

// int buscaBinariaTan(int chave){
//   int tam = 17, n = tam/2;
//   while (1){
//     if (tabelaTan[n] == chave){
//       return n;
//     }
//     if (chave > tabelaTan[n]){
//       if (n == tam)
//         return n;
//       if (chave<tabelaTan[n+1]){
//         if ((chave - tabelaTan[n]) > (tabelaTan[n+1] - chave))
//           return n+1;
//         else
//           return n;
//       }
//       n = (tam + n)/2;
//     }else{
//       if (chave>tabelaTan[n-1]){
//         if((chave - tabelaTan[n-1]) > (tabelaTan[n] - chave))
//           return n;
//         else 
//           return n-1;
//       }
//       n = n/2;
//     }
//   }
// }

int buscaLinearTan(int chave){
  for (int i = 0; i < 17; i++){
    if (chave >= tabelaTan[i] && chave < tabelaTan[i+1]){
      if (chave - tabelaTan[i] > tabelaTan[i+1] - chave)
        return i+1;
      else
        return i;
    }
  }
  return 17;
}

void wait(unsigned int tempo){
  unsigned int inicio = get_time();
  while (get_time() - inicio < tempo){
    //Nada
  };
}

int distanciaQuadrada(Vector3 p1, Vector3 p2){
  int distancia  = quadrado(p1.x - p2.x)+
                   quadrado(p1.y - p2.y)+
                   quadrado(p1.z - p2.z);
  return distancia;
}

int getAngulo(int amigo){
  Vector3 alvo = friends_locations[amigo], atual;
  get_current_GPS_position(&atual);
  int dx = alvo.x - atual.x, dz = alvo.z - atual.z, tan, angulo;
  if (dz != 0){
    tan = (dx*1000)/dz;
    if (tan < 0)
      tan = tan*(-1);
    angulo = 5*buscaLinearTan(tan);
  }
  else
    angulo = 90;
  if (dx >= 0){
    if (dz >= 0)
      return angulo;
    else
      return (180 - angulo);
  } else {
    if (dz >= 0)
      return (360 - angulo);
    else
      return (angulo + 180);
  }
}


/*Funções de Movimentação*/

int declive(Vector3 atual){
  int inclinacao = 0;
  if (atual.x > 300) inclinacao = inclinacao + 360 - atual.x;
  else inclinacao = inclinacao + atual.x; 
  if (atual.z > 300) inclinacao = inclinacao + 360 - atual.z;
  else inclinacao = inclinacao + atual.z;
  // if (get_time() % 500 == 0){
  //   puts("Declive: ");
  //   intToASCII(inclinacao,1);
  // }
  return inclinacao; 
}

int elevacao(){
  Vector3 giroscopio;
  get_gyro_angles(&giroscopio);
  if (giroscopio.x > 5 && giroscopio.x < 60){
    if (giroscopio.z > 5 && giroscopio.z < 60)
      return 1;
    else if (giroscopio.z > 300 && giroscopio.z < 355)
      return 2;
    return 3;
  }else if (giroscopio.x > 300 && giroscopio.x < 355){
    if (giroscopio.z > 5 && giroscopio.z < 60)
      return -1;
    else if (giroscopio.z > 300 && giroscopio.z < 355)
      return -2;
    return -3;
  }
  return 0;
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

void parar(){
  set_torque(-8, -8);
  wait(800);
  set_torque(0, 0);
  wait(500);
  set_torque(-1, -1);
  wait(600);
  set_torque(0,0);
}

void desvio(int angulo, int tempo){
  vire(angulo);
  int aux = tempo / 100 + 1;
  int i;
  Vector3 giroscopio;
  for(i=0;i<aux;i++){
    get_gyro_angles(&giroscopio);
    set_torque(15,15);
    wait(100);
    int desvioAngular = angulo - giroscopio.y;
    if (desvioAngular < 0) desvioAngular = - desvioAngular;
    if (desvioAngular > 60){
      set_torque(0,0);
      vire(angulo);
      set_torque(15,15);
    }
  }
}  

void ande(int distancia, int alvo){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio, giroscopio;
  int torque = 20, angulo, Tinicial;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  set_torque(torque,torque);
  Tinicial = get_time();
  while(distanciaQuadrada(atual,inicio) < quadrado(distancia)){
    if (distanciaQuadrada(atual, friends_locations[alvo]) < 25)
      break;
    get_current_GPS_position(&atual);
    if (get_time() - Tinicial > 2000){
      puts("Tempo\n");
      break;
    }
  }
  parar();
  set_torque(0,0);
}

void printLocation(Vector3 local){
  puts("(");
  intToASCII(local.x,0);
  puts(", ");
  intToASCII(local.y,0);
  puts(", ");
  intToASCII(local.z,0);
  puts(")\n");
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

int verificarObstaculos(int alvo){
  int varreduraObstaculo[5] = {0,0,0,0,0}; //[dir, dir45, frente, esq45, esq]
  for(int i; i < 27; i++){
    set_head_servo(2, 0 + i*6);
    wait(50);
    if (get_us_distance() > 0 && get_us_distance() < 600){
      if (i*3 < 45)
        varreduraObstaculo[0] = 1;
      else if (i*3 < 70)
        varreduraObstaculo[1] = 1;
      else if (i*3 < 86)
        varreduraObstaculo[2] = 1;
      else if (i*3 < 110)
        varreduraObstaculo[3] = 1;
      else
        varreduraObstaculo[4] = 1;
    }
  }
  set_head_servo(2, 78);
  if (varreduraObstaculo[2]){
    puts("Desviar\n");
    desviar(varreduraObstaculo, alvo);
    return 1;
  }
  return 0;
}

int temInimigo(Vector3 atual, int angulo, int distancia){
  /* Se o destino entrar na zona de perigo, retorna 1, 
  caso o destino seja seguro retorna 0 */
  int X=atual.x,
      Y=atual.y,
      Z=atual.z,
      aux=distancia*71/100; /*distancia*sqrt(2)/2 = distancia*sen(45)*/
  switch (angulo)
  {
  case 0:
    Z = Z + distancia;
    break;
  case 90:
    X = X + distancia;
    break;
  case 180:
    Z = Z - distancia;
    break;
  case 270:
    X = X - distancia;
    break;
  case 45:
    Z = Z + aux;
    X = X + aux;
    break;
  case 135:
    Z = Z - aux;
    X = X + aux;
    break;
  case 225:
    Z = Z - aux;
    X = X - aux;
    break;
  case 315:
    Z = Z + aux;
    X = X - aux;
    break;
  }
  int i;
  for (i=0;i<numberOfDangLoc;i++){
    if ((quadrado(X - dangerous_locations[i].x) + 
        quadrado(Z - dangerous_locations[i].z)) < 3600){
          return 1;
        }
    return 0;
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

void desviar(int varreduraObstaculo[5], int alvo){
  Vector3 giroscopio;
  get_gyro_angles(&giroscopio);
  int anguloY = giroscopio.y;
  if (!varreduraObstaculo[1]){
    	vire(anguloY + 45);
      ande(4,alvo);
  }
  else if (!varreduraObstaculo[3]){
      vire(anguloY - 45);
      ande(4, alvo);
  }
  else if (!varreduraObstaculo[0]){
    vire(anguloY + 90);
    ande(4, alvo);
    vire(anguloY);
    verificarObstaculos(alvo);
    ande(4, alvo);
  }
  else if (!varreduraObstaculo[4]){
    vire(anguloY - 90);
    ande(4, alvo);
    vire(anguloY);
    verificarObstaculos(alvo);
    ande(4, alvo);
  }
}

/*Main*/
int main(){
  Vector3 atual;
  get_gyro_angles(&atual);
  vire(atual.y + 45);
  get_current_GPS_position(&atual);
  /* Buscando cada amigo*/
  int i, alvo, angulo;
  for (i=0;i<numberOfFriends;i++){
    get_current_GPS_position(&atual);
    // alvo = indices[i];
    alvo = i;
    puts("--------Localizacao atual---------\n");
    printLocation(atual);
    puts("------Localizacao do amigo--------\n");
    printLocation(friends_locations[alvo]);
    while(distanciaQuadrada(atual, friends_locations[alvo]) > 25){
      intToASCII(get_us_distance(), 1);
      get_current_GPS_position(&atual);
      angulo = getAngulo(alvo);
      vire(angulo);
      if (verificarObstaculos(alvo)){
        continue;
      }
      ande(2, alvo);
    }
    puts("--------Localizacao atual---------\n");
    printLocation(atual);
    puts("------Localizacao do amigo--------\n");
    printLocation(friends_locations[alvo]);
    puts("Amigo encontrado!!!\n");
  }
  while(1){
  }
  return 0;
}