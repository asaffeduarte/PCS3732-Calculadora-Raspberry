.global _start
    .text

# --- Ponto de entrada do programa ---
_start:
    # Teste: Soma (12 + 3)
    li      a0, 12
    li      a1, 3
    jal     ra, add_riscv
    
    # O resultado da soma agora está em a0. 
    # Pulamos para o fim para encerrar o simulador corretamente.
    j       fim_programa

# --- Funções Matemáticas ---

    .global add_riscv
add_riscv:
    add     a0, a0, a1
    ret

    .global sub_riscv
sub_riscv:
    sub     a0, a0, a1
    ret

    .global mul_riscv
mul_riscv:
    mul     a0, a0, a1
    ret

    .global div_riscv
div_riscv:
    beqz    a1, div_zero
    div     a0, a0, a1
    ret
div_zero:
    li      a0, 0
    ret

    .global fat_riscv
fat_riscv:
    bltz    a0, fat_erro
    li      a1, 1
    li      t0, 1
    ble     a0, t0, fat_fim
fat_loop:
    mul     a1, a1, a0
    addi    a0, a0, -1
    bgt     a0, t0, fat_loop
fat_fim:
    mv      a0, a1
    ret
fat_erro:
    li      a0, 0
    ret

# --- Finalização Segura ---
fim_programa:
    li      a7, 93          # Código da syscall para exit
    li      a0, 0           # Código de retorno (0 = sucesso)
    ecall                   # Encerra o programa