/*Willian Takayuki Ozako - RA 207267
João Alberto Moreira Seródio - RA 218548*/
  
/*Utiliza as rotinas descritas em api_robot.h para enviar os comandos para o robô*/

#include "api_robot2.h"

int arcoTangente(int valor){
  valor = valor * 100;
  int angulos[] = 
                  {-90,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-74,-73,-72,
                  -71,-70,-69,-68,-67,-66,-65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,
                  -52,-51,-50,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,
                  -34,-33,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,
                  -14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
                  15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,
                  43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
                  72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90};
  int tangente[] = 
                  {-10000000000,-5714,-2860,-1906,-1429,-1142,-951,-814,-711,-631,-567,-514,-470,-433,
                  -401,-373,-349,-327,-308,-290,-275,-260,-247,-236,-225,-214,-205,-196,-188,-180,-173,
                  -166,-160,-154,-148,-143,-138,-133,-128,-123,-119,-115,-111,-107,-104,-100,-97,-93,-90,
                  -87,-84,-81,-78,-75,-73,-70,-67,-65,-62,-60,-58,-55,-53,-51,-49,-47,-45,-42,-40,-38,-36,
                  -34,-32,-31,-29,-27,-25,-23,-21,-19,-18,-16,-14,-12,-11,-9,-7,-5,-3,-2,0,2,3,5,7,9,11,12,
                  14,16,18,19,21,23,25,27,29,31,32,34,36,38,40,42,45,47,49,51,53,55,58,60,62,65,67,70,73,75,
                  78,81,84,87,90,93,97,100,104,107,111,115,119,123,128,133,138,143,148,154,160,166,173,180,188,
                  196,205,214,225,236,247,260,275,290,308,327,349,373,401,433,470,514,567,631,711,814,951,1142,
                  1429,1906,2860,5714,10000000000};
  int i, menorDiff = 99999999999999, maisProximo = -90;
  for (i=0;i<181;i++){
    int aux = valorVezes100 - tangente[i];
    if (aux < 0) aux = -aux;
    if (aux < menorDiff){
      menorDiff = aux;
      maisProximo = angulos[i];
    }
  }
  return maisProximo;
}

int senoVezes100(int angulo){ /* Retorna 100*seno(angulo) */
  int posicao = angulo + 180;
  int tabela[] = 
                  {-0,-2,-3,-5,-7,-9,-10,-12,-14,-16,-17,-19,-21,-23,-24,-26,-28,-29,-31,-33,-34,-36,-37,-39,-41,-42,-44,-45,
                  -47,-48,-50,-52,-53,-54,-56,-57,-59,-60,-62,-63,-64,-66,-67,-68,-69,-71,-72,-73,-74,-75,-77,-78,-79,-80,-81,
                  -82,-83,-84,-85,-86,-87,-87,-88,-89,-90,-91,-91,-92,-93,-93,-94,-95,-95,-96,-96,-97,-97,-97,-98,-98,-98,-99,
                  -99,-99,-99,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-100,-99,-99,-99,-99,-98,-98,-98,-97,-97,-97,-96,
                  -96,-95,-95,-94,-93,-93,-92,-91,-91,-90,-89,-88,-87,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-75,-74,-73,-72,
                  -71,-69,-68,-67,-66,-64,-63,-62,-60,-59,-57,-56,-54,-53,-52,-50,-48,-47,-45,-44,-42,-41,-39,-37,-36,-34,-33,-31,
                  -29,-28,-26,-24,-22,-21,-19,-17,-16,-14,-12,-10,-9,-7,-5,-3,-2,0,2,3,5,7,9,10,12,14,16,17,19,21,22,24,26,28,29,31,
                  33,34,36,37,39,41,42,44,45,47,48,50,52,53,54,56,57,59,60,62,63,64,66,67,68,69,71,72,73,74,75,77,78,79,80,81,82,83,
                  84,85,86,87,87,88,89,90,91,91,92,93,93,94,95,95,96,96,97,97,97,98,98,98,99,99,99,99,100,100,100,100,100,100,100,
                  100,100,100,100,99,99,99,99,98,98,98,97,97,97,96,96,95,95,94,93,93,92,91,91,90,89,88,87,87,86,85,84,83,82,81,80,
                  79,78,77,75,74,73,72,71,69,68,67,66,64,63,62,60,59,57,56,54,53,52,50,48,47,45,44,42,41,39,37,36,34,33,31,29,28,26,
                  24,23,21,19,17,16,14,12,10,9,7,5,3,2,0};

  return tabela[posicao];
}

int cossenoVezes100(int angulo){  /* Retorna 100*cosseno(angulo) */
  int posicao = angulo + 180;
  int tabela[] = 
                  {-100,-100,-100,-100,-100,-100,-99,-99,-99,-99,-98,-98,-98,-97,-97,-97,-96,-96,-95,-95,-94,-93,-93,-92,-91,-91,
                  -90,-89,-88,-87,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-75,-74,-73,-72,-71,-69,-68,-67,-66,-64,-63,-62,-60,
                  -59,-57,-56,-54,-53,-51,-50,-48,-47,-45,-44,-42,-41,-39,-37,-36,-34,-33,-31,-29,-28,-26,-24,-22,-21,-19,-17,-16,
                  -14,-12,-10,-9,-7,-5,-3,-2,0,2,3,5,7,9,10,12,14,16,17,19,21,22,24,26,28,29,31,33,34,36,37,39,41,42,44,45,47,48,
                  50,52,53,54,56,57,59,60,62,63,64,66,67,68,69,71,72,73,74,75,77,78,79,80,81,82,83,84,85,86,87,87,88,89,90,91,91,
                  92,93,93,94,95,95,96,96,97,97,97,98,98,98,99,99,99,99,100,100,100,100,100,100,100,100,100,100,100,99,99,99,99,98,
                  98,98,97,97,97,96,96,95,95,94,93,93,92,91,91,90,89,88,87,87,86,85,84,83,82,81,80,79,78,77,75,74,73,72,71,69,68,67,
                  66,64,63,62,60,59,57,56,54,53,52,50,48,47,45,44,42,41,39,37,36,34,33,31,29,28,26,24,22,21,19,17,16,14,12,10,9,7,5,
                  3,2,0,-2,-3,-5,-7,-9,-10,-12,-14,-16,-17,-19,-21,-22,-24,-26,-28,-29,-31,-33,-34,-36,-37,-39,-41,-42,-44,-45,-47,
                  -48,-50,-51,-53,-54,-56,-57,-59,-60,-62,-63,-64,-66,-67,-68,-69,-71,-72,-73,-74,-75,-77,-78,-79,-80,-81,-82,-83,
                  -84,-85,-86,-87,-87,-88,-89,-90,-91,-91,-92,-93,-93,-94,-95,-95,-96,-96,-97,-97,-97,-98,-98,-98,-99,-99,-99,-99,
                  -100,-100,-100,-100,-100,-100};
  return tabela[posicao];
}

int melhorAngulo(Vector3 atual, Vector3 destino){ 
  /* Angulo que leva do atual para o destino em linha reta */
  int angulo = arcoTangente((atual.x - destino.x)/(atual.z - destino.z));
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
      int dX = atual->x + senoVezes100(angulo)*j/100-dangerous_locations[i].x;
      int dZ = atual->z + cossenoVezes100(angulo)*j/100-dangerous_locations[i].z;
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