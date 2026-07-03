.text

# 1. SOMA (A + B)
# Entrada: a0 = A, a1 = B | Saída: a0 = Resultado
    .global add_riscv
add_riscv:
    add     a0, a0, a1      # a0 = a0 + a1
    ret

# 2. SUBTRAÇÃO (A - B)
# Entrada: a0 = A, a1 = B | Saída: a0 = Resultado
    .global sub_riscv
sub_riscv:
    sub     a0, a0, a1      # a0 = a0 - a1
    ret

# 3. MULTIPLICAÇÃO (A * B)
# Entrada: a0 = A, a1 = B | Saída: a0 = Resultado
    .global mul_riscv
mul_riscv:
    mul     a0, a0, a1      # a0 = a0 * a1 (Requer Extensão M)
    ret

# 4. DIVISÃO (A / B) com proteção contra divisão por zero
# Entrada: a0 = A, a1 = B | Saída: a0 = Resultado
    .global div_riscv
div_riscv:
    beqz    a1, div_zero    # "Branch if Equal to Zero": se a1 for 0, desvia
    div     a0, a0, a1      # Divisão sinalizada (Requer Extensão M)
    ret
div_zero:
    li      a0, 0           # "Load Immediate": carrega 0 no registrador de retorno
    ret

# 5. FATORIAL (A!)
# Entrada: a0 = A | Saída: a0 = Resultado
    .global fat_riscv
fat_riscv:
    bltz    a0, fat_erro    # "Branch if Less Than Zero": se A < 0, desvia
    
    li      a1, 1           # a1 = Acumulador (inicia em 1)
    li      t0, 1           # t0 = Constante 1 (usada para comparação)
    ble     a0, t0, fat_fim # Se A <= 1, vai direto para o fim

fat_loop:
    mul     a1, a1, a0      # Acumulador = Acumulador * A
    addi    a0, a0, -1      # A = A - 1 (RISC-V não tem 'sub' com imediato)
    bgt     a0, t0, fat_loop# Se A > 1, repete o loop

fat_fim:
    mv      a0, a1          # Move o acumulador para a0 (retorno)
    ret

fat_erro:
    li      a0, 0           # Retorna 0 para entradas inválidas
    ret