#Willian Takayuki Ozako - RA 207267
#João Alberto Moreira Seródio - RA 218548
  
#Implementação das rotinas descritas em api_robot.h

.globl set_torque
set_torque:
  #Verificação dos valores de torque
  li t0, 100
  bgt a0, t0, st_OutOfRange
  bgt a1, t0, st_OutOfRange
  neg t0, t0
  blt a0, t0, st_OutOfRange
  blt a1, t0, st_OutOfRange
  #Set torque motor 2
  mv t0, a0
  li a0, 1
  li a7, 18
  ecall
  #Set torque motor 1
  li a0, 0 #NÃO É NECESSÁRIO SE QUISER PODE TIRAR
  mv a1, t0
  li a7, 18 #NÃO É NECESSÁRIO SE QUISER PODE TIRAR
  ecall
  j st_Return
  st_OutOfRange:
  li a0, -1
  st_Return:
  ret

.globl set_engine_torque
set_engine_torque:
  #Verificação do valor de torque
  li t0, 100
  bgt a1, t0, sengt_OutOfRange
  neg t0, t0
  blt a1, t0, sengt_OutOfRange
  #Syscall set_engine_torque
  li a7, 18
  ecall
  #Verificação da validade de engine_ID
  beqz a0, sengt_Return  #Ambos os valores válidos
  j sengt_InvalidID
  sengt_OutOfRange:
  li a0, -1              #Caso o valor de torque seja inválido
  j sengt_Return
  sengt_InvalidID:
  li a0, -2              #Caso o valor de engine_ID seja inválido
  sengt_Return:
  ret

.globl set_head_servo
set_head_servo:
  li a7, 17
  ecall
  ret

.globl get_us_distance
get_us_distance:
  li a7, 16
  ecall
  ret

.globl get_current_GPS_position
get_current_GPS_position:
  li a7, 19
  ecall
  ret

.globl get_gyro_angles
get_gyro_angles:
  li a7, 20
  ecall
  ret

.globl get_time
get_time:
  li a7, 21
  ecall
  ret

.globl set_time
set_time:
  li a7, 22
  ecall
  ret

.globl puts
puts:
  li a2, 0  #Contador de caracteres da string
  mv t0, a0 #Ponteiro para percorrer string
  puts_While:
    lbu t1, 0(t0)
    beqz t1, puts_wBreak  #\0 indica final da string
    addi a2, a2, 1
    addi t0, t0, 1
    j puts_While
  puts_wBreak:
  mv a1, a0
  li a0, 1
  li a7, 64
  ecall
  ret
