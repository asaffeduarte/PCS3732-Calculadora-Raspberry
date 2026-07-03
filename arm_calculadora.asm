.text

// 1. SOMA (A + B)
// Entrada: w0 = A, w1 = B | Saída: w0 = Resultado
    .global add_arm
add_arm:
    add     w0, w0, w1      // w0 = w0 + w1
    ret

// 2. SUBTRAÇÃO (A - B)
// Entrada: w0 = A, w1 = B | Saída: w0 = Resultado
    .global sub_arm
sub_arm:
    sub     w0, w0, w1      // w0 = w0 - w1
    ret

// 3. MULTIPLICAÇÃO (A * B)
// Entrada: w0 = A, w1 = B | Saída: w0 = Resultado
    .global mul_arm
mul_arm:
    mul     w0, w0, w1      // w0 = w0 * w1
    ret

// 4. DIVISÃO (A / B) com proteção contra divisão por zero
// Entrada: w0 = A, w1 = B | Saída: w0 = Resultado
    .global div_arm
div_arm:
    cbz     w1, .div_zero   // "Compare and Branch on Zero": se w1 for 0, desvia
    sdiv    w0, w0, w1      // Divisão sinalizada (Signed Div): w0 = w0 / w1
    ret
.div_zero:
    mov     w0, 0           // Retorna 0 em caso de erro
    ret

// 5. FATORIAL (A!)
// Entrada: w0 = A | Saída: w0 = Resultado
    .global fat_arm
fat_arm:
    cmp     w0, 0           // Verifica se A < 0
    blt     .fat_erro       // Se for menor que 0, desvia para erro
    
    mov     w1, 1           // w1 será o nosso acumulador (resultado começa em 1)
    cmp     w0, 1           // Verifica se A <= 1
    ble     .fat_fim        // Se for, o resultado é 1, pula para o fim

.fat_loop:
    mul     w1, w1, w0      // Acumulador = Acumulador * A
    sub     w0, w0, 1       // A = A - 1
    cmp     w0, 1           // Chegou em 1?
    bgt     .fat_loop       // Se A > 1, repete o loop

.fat_fim:
    mov     w0, w1          // Move o acumulador final para w0 (retorno)
    ret

.fat_erro:
    mov     w0, 0           // Retorna 0 para entradas inválidas
    ret