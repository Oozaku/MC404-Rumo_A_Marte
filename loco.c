/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot.h"

/*variáveis globais*/
int numberOfFriends = sizeof(friends_locations)/sizeof(Vector3);
int numberOfDangLoc = sizeof(dangerous_locations)/sizeof(Vector3);
static int tabelaTan[18] = {0, 87, 176, 268, 363, 466, 577, 700, 839, 1000,
                     1191, 1428, 1732, 2144, 2747, 3732, 5671, 11430};
Vector3 caixaDagua = {.x = 453, .y = 105, .z = -38}; //Checkpoint pro mapa teste

/*Protótipos de Função*/
void intToASCII(int numero, int quebraLinha);
int friendIndex(Vector3 alvo);
Vector3 closestPosition();
int quadrado(int numero);
int buscaLinearTan(int chave);
void wait(unsigned int tempo);
int distanciaQuadrada(Vector3 alvo, Vector3 atual);
int getAngulo(Vector3 alvo);
void vire(int angulo);
void parar();
void ande(int distancia, Vector3 alvo);
int verificarObstaculos(Vector3 alvo);
int verificarInimigos();
void contornarInimigo(Vector3 anterior, Vector3 atual, Vector3 alvo);
void desviar(int varreduraObstaculo[5], Vector3 alvo);
void printLocation(Vector3 local);


/*Funções utilitárias*/
//Printa int
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

//Retorna a posição do alvo no vetor de amigos, -1 caso nao esteja no vetor
int friendIndex(Vector3 alvo){
  for (int i = 0; i < numberOfFriends; i++){
    if(alvo.x == friends_locations[i].x && alvo.y == friends_locations[i].y && alvo.z == friends_locations[i].z)
      return i;
  }
  return -1;
}

//Retorna a posição a ser visitada mais próxima que não foi visitada ainda dentre os amigos e checkpoint
Vector3 closestPosition(){
  Vector3 atual, posicao;
  get_current_GPS_position(&atual);
  int distanciaMin = -1, distanciaTeste;
  for (int i = 0; i < numberOfFriends; i++){
    if (friends_locations[i].y){
      distanciaTeste = distanciaQuadrada(atual, friends_locations[i]);
      if(distanciaMin < 0 || distanciaTeste < distanciaMin){
        distanciaMin = distanciaTeste;
        posicao = friends_locations[i];
      }
    }
  }
  if (caixaDagua.y)
    if (distanciaMin < 0 || distanciaQuadrada(atual, caixaDagua) < distanciaMin)
      posicao = caixaDagua;
  return posicao;
}

//Retorna o quadrado de um número
int quadrado(int numero){
  return numero * numero;
}

//Busca linear no vetor de valores de tangente
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

//Função de espera, a entrada deve estar em ms
void wait(unsigned int tempo){
  unsigned int inicio = get_time();
  while (get_time() - inicio < tempo){
    //Nada
  };
}

//Retorna a distância entre dois pontos no espaço
int distanciaQuadrada(Vector3 p1, Vector3 p2){
  int distancia  = quadrado(p1.x - p2.x)+
                   quadrado(p1.y - p2.y)+
                   quadrado(p1.z - p2.z);
  return distancia;
}

//Retorna o ângulo da reta entre a posição atual e o alvo
int getAngulo(Vector3 alvo){
  Vector3 atual;
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

// int declive(Vector3 atual){
//   int inclinacao = 0;
//   if (atual.x > 300) inclinacao = inclinacao + 360 - atual.x;
//   else inclinacao = inclinacao + atual.x; 
//   if (atual.z > 300) inclinacao = inclinacao + 360 - atual.z;
//   else inclinacao = inclinacao + atual.z;
//   // if (get_time() % 500 == 0){
//   //   puts("Declive: ");
//   //   intToASCII(inclinacao,1);
//   // }
//   return inclinacao; 
// }

// int elevacao(){
//   Vector3 giroscopio;
//   get_gyro_angles(&giroscopio);
//   if (giroscopio.x > 5 && giroscopio.x < 60){
//     if (giroscopio.z > 5 && giroscopio.z < 60)
//       return 1;
//     else if (giroscopio.z > 300 && giroscopio.z < 355)
//       return 2;
//     return 3;
//   }else if (giroscopio.x > 300 && giroscopio.x < 355){
//     if (giroscopio.z > 5 && giroscopio.z < 60)
//       return -1;
//     else if (giroscopio.z > 300 && giroscopio.z < 355)
//       return -2;
//     return -3;
//   }
//   return 0;
// }

//Função que vira o Uoli para o que giroscópio.y = angulo
void vire(int angulo){  
  /* Vira para angulo-graus absoluto do giroscopio */
  if (angulo < 0) angulo = angulo + 360;
  if (angulo >= 360) angulo = angulo % 360;
  Vector3 atual;
  get_gyro_angles(&atual);
  int aux = angulo - atual.y, torque = 20;
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

//Função que "para" o Uoli, podendo dar ré
void parar(){
  set_torque(-10, -10);
  wait(800);
  set_torque(0, 0);
  wait(500);
  set_torque(-1, -1);
  wait(600);
  set_torque(0,0);
}

//Função de faz o Uoli andar a distância em U.M.
void ande(int distancia, Vector3 alvo){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio, giroscopio;
  int torque = 40, angulo, Tinicial;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  set_torque(torque,torque);
  Tinicial = get_time();
  while(distanciaQuadrada(atual,inicio) < quadrado(distancia)){
    if (distanciaQuadrada(atual, alvo) < 25)
      break;
    if (get_us_distance() > 0 && get_us_distance() < 600){
      intToASCII(get_us_distance(), 1);
      parar();
      parar();
      parar();
      return;
    }
    get_current_GPS_position(&atual);
    if (get_time() - Tinicial > 2000){
      break;
    }
  }
  parar();
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

//Verifica se há obstaculos na região de varredura dentre de 6 metros
int verificarObstaculos(Vector3 alvo){
  int varredura[3] = {0,0,0}; 
  for(int i = 45; i < 111; i++){
    set_head_servo(2, i);
    if (get_us_distance() > 0 && get_us_distance() < 600){
      if (i < 55)
        varredura[0] = 1;
      if (i < 91)
        varredura[1] = 1;
      else
        varredura[2] = 1;
    }
  }
  set_head_servo(2, 78);
  if (varredura[1]){
    desviar(varredura, alvo);
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

/* Ele ve se o uoli esta se aproximando demais de um perigo, se sim ele faz um desvio */
void contornarInimigo(Vector3 anterior, Vector3 atual, Vector3 alvo){
  for(int i = 0; i < numberOfDangLoc; i++){
    int distancia_antes = distanciaQuadrada(anterior,dangerous_locations[i]);
    int distancia_agora = distanciaQuadrada(atual,dangerous_locations[i]);
    if (distancia_antes < distancia_agora && distancia_agora < 100){
      Vector3 giroscopio;
      get_gyro_angles(&giroscopio);
      vire(giroscopio.y+90);
      ande(4,alvo);
      break;
    }
  }
}

//Desvia de obstáculo
void desviar(int varreduraObstaculo[3], Vector3 alvo){
  Vector3 giroscopio;
  get_gyro_angles(&giroscopio);
  int anguloY = giroscopio.y, dir, esq;
  if (!varreduraObstaculo[0]){
    	vire(anguloY + 45);
      ande(4,alvo);
  }
  else if (!varreduraObstaculo[2]){
      vire(anguloY - 45);
      ande(4, alvo);
  }
  else{
    set_head_servo(2, 0);
    wait(100);
    if (get_us_distance() > 0 && get_us_distance() < 600){
      vire(anguloY - 90);
      ande(3, alvo);
      vire(anguloY);
      ande(4, alvo);
    }
    else{
      set_head_servo(2, 156);
      wait(100);
      if (get_us_distance() > 0 && get_us_distance() < 600){
        vire(anguloY + 90);
        ande(3, alvo);
        vire(anguloY);
        ande(4, alvo);  
      }
    }
  }
}


/*Main*/
int main(){
  Vector3 atual, alvo, anterior;
  int angulo, aux;
  intToASCII(numberOfFriends, 1);
  /* Buscando cada amigo*/
  for (int i = 0; i<=numberOfFriends; i++){
    alvo = closestPosition();
    get_current_GPS_position(&atual);
    get_current_GPS_position(&anterior);
    // alvo = indices[i];
    puts("--------Localizacao atual---------\n");
    printLocation(atual);
    puts("------Localizacao do amigo--------\n");
    printLocation(alvo);
    angulo = getAngulo(alvo);
    vire(angulo);
    while(distanciaQuadrada(atual, alvo) > 25){
      anterior.x = atual.x;
      anterior.y = atual.y;
      anterior.z = atual.z;
      get_current_GPS_position(&atual);
      contornarInimigo(anterior, atual, alvo);
      angulo = getAngulo(alvo);
      vire(angulo);
      if (verificarObstaculos(alvo)){
        continue;
      }
      ande(2, alvo);
    }
    aux = friendIndex(alvo);
    if (aux >= 0)
      friends_locations[aux].y = 0;
    else
      caixaDagua.y = 0;
    puts("Amigo encontrado!!!\n");
  }
  while(1){
  }
  return 0;
}