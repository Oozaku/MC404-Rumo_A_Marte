#Willian Takayuki Ozako - RA 207267
#João Alberto Moreira Seródio - RA 218548
  
#Gerenciamento de hardware do sistema

.globl _start

int_handler:
    # Tratamento de interrupcoes


    int_handler_Return:  #A partir daqui restaurar o contexto e retornar ao código

#Syscalls
syscall16:
    li t0, 0xFFFF0020 
    sw zero, 0(t0)   #Inicia leitura do sensor de ultrassom
    sys16_While:     #Verificando se o valor em 0xFFFF0020 foi atualizado
        lw t1, 0(t0)
        beqz t1, sys16_While
    lw a0, 4(t0)     #0xFFFF0024
    j int_handler_Return

syscall17:
    #Verifica qual o servo a ter o ângulo alterado
    beqz a0, sys17_Base
    li t0, 1
    beq a0, t0, sys17_Mid
    li t0, 2
    beq a0, t0, sys17_Mid
    li a0, -1   #Caso servo ID seja inválido
    j int_handler_Return
    #Servo ID 0 - Base (16-116)
    sys17_Base:
    #Validação do ângulo
    li t0, 116
    bgt a1, t0, sys17_InvalidAngle
    li t0, 16
    blt a1, t0, sys17_InvalidAngle
    li t0, 0xFFFF001E  #Ângulo servo-motor 1 (base)
    sb a1, 0(t0)
    li a0, 0
    j int_handler_Return
    #Servo ID 1 - Mid (52-90)
    sys17_Mid:
    #Validação do ângulo
    li t0, 90
    bgt a1, t0, sys17_InvalidAngle
    li t0, 52
    blt a1, t0, sys17_InvalidAngle
    li t0, 0xFFFF001D  #Ângulo servo-motor 2 (mid)
    sb a1, 0(t0)
    li a0, 0
    j int_handler_Return
    #Servo ID 2 - Top (0-156)
    sys17_top:
    #Validação do ângulo
    li t0, 156
    bgt a1, t0, sys17_InvalidAngle
    bltz a1, sys17_InvalidAngle
    li t0, 0xFFFF001C  #Ângulo servo-motor 3 (top)
    sb a1, 0(t0)
    li a0, 0
    j int_handler_Return
    #ângulo Inválido
    sys17_InvalidAngle:
    li a0, -2
    j int_handler_Return
    

syscall18:
    beqz a0, sys18_Eng1
    li t0, 1
    beq a0, to, sys18_Eng2
    li a0, -1    #Caso o ID do motor seja inválido
    j int_handler_Return
    #Configura torque do motor 1
    sys18_Eng1:
    li t0, 0xFFFF001A  #Torque motor 1
    sh a1, 0(t0)
    li a0, 0
    j int_handler_Return
    #Configura torque do motor 2
    sys18_Eng2:
    li t0, 0xFFFF0018  #Torque motor 2
    sh a1, 0(t0)
    li a0, 0
    j int_handler_Return

syscall19:
    li t0, 0xFFFF0004
    sw zero, 0(t0)  #Inicia a leitura da posição do robô
    sys19_While:    #Verificando se o valor em 0xFFFF0020 foi atualizado
        lw t1, 0(t0)
        beqz t1, sys19_While
    lw t1, 4(t0)  # 0xFFFF0008 -> x
    sw t1, 0(a0)  #Armazena x
    lw t1, 8(t0)  # 0xFFFF000C -> y
    sw t1, 4(a0)  #Armazena y
    lw t1, 12(t0) # 0xFFFF0010 -> z
    sw t1, 8(a0)  #Armazena z
    j int_handler_Return

syscall20:
    li t0, 0xFFFF0004
    sw zero, 0(t0)  #Inicia a leitura da posição do robô
    sys20_While:    #Verificando se o valor em 0xFFFF0020 foi atualizado
        lw t1, 0(t0)
        beqz t1, sys20_While
    lw t1, 16(t0)  # 0xFFFF0014
    andi t0, t1, 1023  #z
    sw t0, 8(a0)       #Armazena z
    srli t1, t1, 10
    andi t0, t1, 1023  #y
    sw t0, 4(a0)       #Armazena y
    srli t1, t1, 10    #x
    sw t1, 0(a0)       #Armazena x
    j int_handler_Return

syscall21:

syscall22:

syscall64:

_start:
    # Falta configurar o GPT
    # Iniciando motores com valor 0
    li t0, 0xFFFF0018 # motor 1
    li t1, 0xFFFF001A # motor 2
    li t3, 0
    sh t3, 0(t0)
    sh t3, 0(t1)
    # Iniciando a cabeca do Uoli
    li t0, 0xFFFF001C   # top
    li t1, 0xFFFF001D   # mid
    li t2, 0xFFFF001E   # base
    li t3, 78   # valor inicial de top
    li t4, 80   # valor inicial de mid
    li t5, 31   # valor inicila de base
    sb t3, 0(t0)    # Escrevendo valor de top 
    sb t4, 0(t1)    # Escrevendo valor de mid
    sb t5, 0(t2)    # Escrevendo valor de base
    # Configurando o tratamento de interrupcoes
    la t0, int_handler  # Fala onde esta o tratador de
    csrw mtvec, t0      # interrupcoes
    # Habilita interrupcoes globais
    csrr t1, mstatus
    ori t1, t1, 0x80
    csrw mstatus, t1
    # Habilita interrupcoes externas
    csrr, t1, mie
    li t2, 0x800
    or t1, t1, t2
    csrw mie, t1
    # Da ao mscratch o end. da pilha do sistema
    la t1, reg_buffer
    csrw mscratch, t1
    # Mudar para o Modo Usuario
    csrr t1, mstatus
    li t2, ~0x1800
    and t1, t1, t2
    csrw mstatus, t1

    la t0, main
    csrw mepc, t0

    mret


reg_buffer:
    .skip 132
