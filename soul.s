#Willian Takayuki Ozako - RA 207267
#João Alberto Moreira Seródio - RA 218548
  
#Gerenciamento de hardware do sistema

.globl _start

int_handler:
    # Tratamento de interrupcoes
    # Salvando contexto de t0~6 e a1~7 e end. pilha em s0
    csrrw s0, mscratch, s0
    sw a1, 0(s0)
    sw a2, 4(s0)
    sw a3, 8(s0)
    sw a4, 12(s0)
    sw a5, 16(s0)
    sw a6, 20(s0)
    sw a7, 24(s0)
    sw t0, 28(s0)
    sw t1, 32(s0)
    sw t2, 36(s0)
    sw t3, 40(s0)
    sw t4, 44(s0)
    sw t5, 48(s0)
    sw t6, 52(s0)
    # Descobrindo a causa da interrupcao ou da excecao
    csrr t0, mcause
    bgtz t0, Tratar_excecao
    j tratarGPT

        Tratar_excecao:     # Chamada de syscall para valores 8 e 9 e 
                            # para n >= 12 ou n <= 7 necessita parar o programa
            li t1, 0x7FFFFFFF
            and t0, t0, t1
            li t1, 8                    # Syscall pelo modo usuario
            beq t0, t1, chamadaSyscall
            li t1, 9                    # Syscall pelo modo Super
            beq t0, t1, chamadaSyscall
            j sairPrograma

        chamadaSyscall:     # Identificando codigo da syscall
                li t0, 16
                beq a7, t0, syscall16
                li t0, 17
                beq a7, t0, syscall17
                li t0, 18
                beq a7, t0, syscall18
                li t0, 19
                beq a7, t0, syscall19
                li t0, 20
                beq a7, t0, syscall20
                li t0, 21
                beq a7, t0, syscall21
                li t0, 22
                beq a7, t0, syscall22
                li t0, 64
                beq a7, t0, syscall64
                j sairPrograma          # Erro: codigo de syscall desconhecida
    
        # tratar_interrupcao:
        #     li t1, 0x7FFFFFFF
        #     and t0, t0, t1
        #     li t1, 3
        #     beq t0, t1, tratarGPT
        #     j SoRecuperaContexto

        tratarGPT: #100ms / tratar eco
            # Se o end. 0xFFFF0104 for zero, trata-se de falsa interrupcao
            li t0, 0xFFFF0104   
            lb t1, 0(t0)
            beq zero, t1, SoRecuperaContexto
            # Configurando para o GPT interromper no proximo 100ms 
            li t0, 100
            li t1, 0xFFFF0100	
            sw t0, 0(t1)
            # Registrar a interrupcao de GPT como ja tratada
            li t1, 0xFFFF0104
            sb zero, 0(t1)
            # Adicionando 100ms no tempoReal
            la t0, tempoReal
            lw t1, 0(t0)
            addi t1, t1, 100
            sw t1, 0(t0)
            j SoRecuperaContexto
        
        sairPrograma:   # Implementar mais tarde o codigo de saida 
            j SoRecuperaContexto


    int_handler_Return:  #A partir daqui restaurar o contexto e retornar ao codigo
        # Apontar para a instrucao depois daquela que chamou a syscall
        csrr t0, mepc
        addi t0, t0, 4
        csrw mepc, t0#VERIFICAR DEPOIS
        j SoRecuperaContexto

        SoRecuperaContexto:
            # Restaurando contexto
            lw a1, 0(s0)
            lw a2, 4(s0)
            lw a3, 8(s0)
            lw a4, 12(s0)
            lw a5, 16(s0)
            lw a6, 20(s0)
            lw a7, 24(s0)
            lw t0, 28(s0)
            lw t1, 32(s0)
            lw t2, 36(s0)
            lw t3, 40(s0)
            lw t4, 44(s0)
            lw t5, 48(s0)
            lw t6, 52(s0)
            csrrw s0, mscratch, s0
            mret

#Syscalls
syscall16: # read_ultrasonic_sensor
    li t0, 0xFFFF0020 
    sw zero, 0(t0)   #Inicia leitura do sensor de ultrassom
    sys16_While:     #Verificando se o valor em 0xFFFF0020 foi atualizado
        lw t1, 0(t0)
        beqz t1, sys16_While
    lw a0, 4(t0)     #0xFFFF0024
    j int_handler_Return

syscall17:  # set_servo_angles ids: 1-Base/2-Mid/3-Top
    #Verifica qual o servo a ter o ângulo alterado
    li t0, 1
    beq a0, t0, sys17_Base
    li t0, 2
    beq a0, t0, sys17_Mid
    li t0, 3
    beq a0, t0, sys17_Top
    li a0, -2   #Caso servo ID seja inválido
    j int_handler_Return #Retorno caso ID inválido
    #Servo ID 1 - Base (16-116)
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
    #Servo ID 2 - Mid (52-90)
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
    #Servo ID 3 - Top (0-156)
    sys17_Top:
    #Validação do ângulo
    li t0, 156
    bgt a1, t0, sys17_InvalidAngle
    blez a1, sys17_InvalidAngle
    li t0, 0xFFFF001C  #Ângulo servo-motor 3 (top)
    sb a1, 0(t0)
    li a0, 0
    j int_handler_Return
    #ângulo Inválido
    sys17_InvalidAngle:
    li a0, -1
    j int_handler_Return
    
syscall18:  # set_engine_torque
    beqz a0, sys18_Eng1
    li t0, 1
    beq a0, t0, sys18_Eng2
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

syscall19: # read_gps
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

syscall20: # read_gyroscope
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

syscall21:  # get_time
    la t0, tempoReal
    lw a0, 0(t0)
    j int_handler_Return

syscall22:  # set_time
    la t0, tempoReal
    sw a0, 0(t0)
    j int_handler_Return

syscall64: # write
    li a0, 0
    li t0, 0xFFFF0108
    li t1, 0xFFFF0109
    li t3, 1
    mv t4, a1   #Apontador para string
    sys64_While1:
        bge a0, a2, int_handler_Return
        lbu t5, 0(t4)
        sb t5, 0(t1)
        addi t4, t4, 1
        sb t3, 0(t0)
        sys64_While2:
            lbu t6, 0(t0)
            bnez t6, sys64_While2
        addi a0, a0, 1
        j sys64_While1
    j int_handler_Return
    

_start:
    # Config. GPT para interromper a cada 1ms
    li t0, 0xFFFF0100	
    li t1, 100
    sw t1, 0(t0)
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
    csrr t1, mie
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

    # Setar o sp
    li sp, 0x7FFFFFC
    mret

reg_buffer:
    .skip 132
tempoReal:
    .skip 4
