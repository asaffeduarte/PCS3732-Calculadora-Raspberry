.global _start
    .text

// --- Ponto de entrada do programa ---
_start:
    // Teste: Soma (12 + 3)
    mov     w0, 12          // Carrega A
    mov     w1, 3           // Carrega B
    bl      add_arm         // "Branch with Link": pula para a função e salva retorno em x30
    
    // O resultado da soma agora está em w0.
    // Pulamos para o fim para encerrar o simulador corretamente.
    b       fim_programa

// --- Funções Matemáticas ---

    .global add_arm
add_arm:
    add     w0, w0, w1
    ret

    .global sub_arm
sub_arm:
    sub     w0, w0, w1
    ret

    .global mul_arm
mul_arm:
    mul     w0, w0, w1
    ret

    .global div_arm
div_arm:
    cbz     w1, div_zero    // "Compare and Branch if Zero"
    sdiv    w0, w0, w1
    ret
div_zero:
    mov     w0, 0
    ret

    .global fat_arm
fat_arm:
    cmp     w0, 0           // Verifica se A < 0
    blt     fat_erro
    mov     w1, 1           // Acumulador
    cmp     w0, 1
    ble     fat_fim
fat_loop:
    mul     w1, w1, w0
    sub     w0, w0, 1
    cmp     w0, 1
    bgt     fat_loop
fat_fim:
    mov     w0, w1
    ret
fat_erro:
    mov     w0, 0
    ret

// --- Finalização Segura ---
fim_programa:
    mov     x8, 93          // Código da syscall para exit (Linux AArch64)
    svc     0               // "Supervisor Call": executa a chamada ao sistema