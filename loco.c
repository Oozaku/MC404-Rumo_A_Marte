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
void ande(int distancia, int alvo, int angulo);
void goToFriend(int alvo);
int verificarInimigos();
int angulo(Vector3 alvo, Vector3 atual);
void contornarInimigo();
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
  };
}

int distanciaQuadrada(Vector3 p1, Vector3 p2){
  int distancia  = quadrado(p1.x - p2.x)+
                   quadrado(p1.y - p2.y)+
                   quadrado(p1.z - p2.z);
  return distancia;
}

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

/*Funções de Movimentação*/

int declive(Vector3 atual){
  int inclinacao = 0;
  if (atual.x > 300) inclinacao = inclinacao + 360 - atual.x;
  else inclinacao = inclinacao + atual.x; 
  if (atual.z > 300) inclinacao = inclinacao + 360 - atual.z;
  else inclinacao = inclinacao + atual.z;
  if (get_time() % 500 == 0){
    puts("Declive: ");
    intToASCII(inclinacao,1);
  }
  return inclinacao; 
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
  set_torque(-1, -1);
  wait(1000);
  set_torque(0, 0);
  wait(500);
  set_torque(-1, -1);
  wait(1000);
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

void ande(int distancia, int alvo, int angulo){
  /* Anda distancia em decimetros em linha reta */
  Vector3 atual, inicio;
  int torque = 20;
  get_current_GPS_position(&atual);
  get_current_GPS_position(&inicio);
  set_torque(torque,torque);
  while(distanciaQuadrada(atual,inicio) < quadrado(distancia)){
    if (distanciaQuadrada(friends_locations[alvo],atual) <= 2500) break;
    Vector3 giroscopio;
    get_gyro_angles(&giroscopio);
    int dAngulo = giroscopio.y - angulo;
    if (dAngulo < 0) dAngulo = - dAngulo;
    if (dAngulo > 90) break;
    if (declive(giroscopio) > 30){        
      /* Ve se o Uoli esta tentando escalar morro, se sim, faz desvio */    
      angulo = getAngulo(alvo);
      puts("...\nDesviando...\n...\n");
      desvio(angulo-90,10000);
    }
    angulo = getAngulo(alvo);
    int bloqueio = get_us_distance();
    if (bloqueio >  0 & bloqueio < 100){
      ande(10, alvo, angulo + 90);
    }
    set_torque(torque,torque);
    if (get_time() % 1000 == 0){
      printLocation(atual);
    }
    get_current_GPS_position(&atual); 
  }
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

void goToFriend(int alvo){
  int torque = 15;
  Vector3 uoli, amigo = friends_locations[alvo];
  get_current_GPS_position(&uoli);
  vire(angulo(amigo, uoli));
  set_torque(torque, torque);
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
  // int indices[sizeof(friends_locations)];
  // int distancias[sizeof(friends_locations)];
  int alvo;
  /* Ordenando os amigos por ordem crescente de distancia da origem */
  // int i;
  // for (i=0;i<numberOfFriends;i++){
  //   indices[i] = i;
  //   distancias[i] = distanciaQuadrada(friends_locations[i],atual);
  // }
  // int ini = 0, fim = numberOfFriends - 1, j =1;
  // i = 0;
  // while(ini<fim){
  //   if (distancias[i] > distancias[j]){
  //     int aux = distancias[i];
  //     distancias[i] = distancias[j];
  //     distancias[j] = aux;
  //     aux = indices[i];
  //     indices[i] = indices[j];
  //     indices[j] = aux;
  //   }
  //   i++;
  //   j++;
  //   if (j > fim){
  //     i = 0;
  //     j = 1;
  //     fim--;
  //   }
  // }
  /* Buscando cada amigo*/
  int i;
  for (i=0;i<numberOfFriends;i++){
    get_current_GPS_position(&atual);
    // alvo = indices[i];
    alvo = i;
    puts("--------Localizacao atual---------\n");
    printLocation(atual);
    puts("------Localizacao do amigo--------\n");
    printLocation(friends_locations[alvo]);
    int angulo = getAngulo(alvo);
    while(distanciaQuadrada(atual,friends_locations[alvo]) > 2500){
      angulo = getAngulo(alvo);
      vire(angulo);
      int obstaculo = get_us_distance();
      if (obstaculo > 0 & obstaculo < 100){
        vire(angulo+90);
        ande(10,alvo,angulo+90);
      }
      if (temInimigo(atual,angulo,50) == 1){
        if (temInimigo(atual,angulo+45,50) == 0){
          angulo = angulo + 45;
        }
        else{
          angulo = angulo - 45;
        }
      }
      ande(50,alvo,angulo);
    }
    printLocation(atual);
    puts("amigo encontrado");
  }
  while(1){
  }
  return 0;
}

// /*Main*/
// int main(){
//   Vector3 atual;
//   get_current_GPS_position(&atual);
//   intToASCII(atual.x, 1);
//   intToASCII(atual.y, 1);
//   intToASCII(atual.z, 1);
//   goToFriend(0);
//   while(1){
//   }
//   return 0;
// }