/*Willian Takayuki Ozako - RA 207267
  João Alberto Moreira Seródio - RA 218548*/
  
  /*Gerenciamento de hardware do sistema*/
  .globl _start

int_handler:
    # Tratamento de interrupcoes

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
