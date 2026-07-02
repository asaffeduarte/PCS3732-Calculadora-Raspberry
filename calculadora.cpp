#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Função auxiliar para calcular o fatorial
unsigned long long calcularFatorial(int n) {
    if (n < 0 || n > 12) return 0; 
    unsigned long long f = 1;
    for (int i = 1; i <= n; i++) {
        f *= i;
    }
    return f;
}

// Estrutura para retornar os resultados das operações de 4 bits
struct ResultadoOp {
    int valorMascarado; // Valor final em 4 bits (0 a 15)
    int valorDecimal;   // Interpretado em Complemento de 2 (-8 a 7)
    bool overflow;
    bool divByZero;
};

// Executa a operação tratando o Complemento de 2 para 4 bits
ResultadoOp operar_4bits(int a, int b, string op) {
    // Garante que estamos pegando apenas os 4 bits brutos da entrada
    int valA = a & 0x0F;
    int valB = b & 0x0F;

    // Interpretação de Complemento de Dois para 4 bits (-8 a 7)
    int sA = (valA & 0x08) ? (valA - 16) : valA;
    int sB = (valB & 0x08) ? (valB - 16) : valB;

    int resultado = 0;
    ResultadoOp res = {0, 0, false, false};

    if (op == "add") {
        resultado = sA + sB;
    } else if (op == "sub") {
        resultado = sA - sB;
    } else if (op == "mul") {
        int calcMul = sA * sB;
        if (calcMul < -8 || calcMul > 7) res.overflow = true;
        resultado = calcMul;
    } else if (op == "div") {
        if (sB == 0) {
            res.divByZero = true;
            resultado = 0;
        } else if (sA == -8 && sB == -1) {
            res.overflow = true; // -8 / -1 = +8 (Estoura o limite de +7)
            resultado = 0;
        } else {
            resultado = sA / sB;
        }
    } else if (op == "fat") {
        if (sA < 0 || sA > 3) { // 3! = 6 (OK), 4! = 24 (Estouro para 4 bits)
            res.overflow = true;
            resultado = 0;
        } else {
            resultado = calcularFatorial(sA);
        }
    }

    // Aplica a máscara de 4 bits limpos para a saída física/binária
    res.valorMascarado = resultado & 0x0F;

    // Reduz o resultado para a interpretação de 4 bits sinalizados
    res.valorDecimal = (res.valorMascarado & 0x08) ? (res.valorMascarado - 16) : res.valorMascarado;

    // Validação de Overflow para Soma e Subtração baseada nos bits de sinal originais
    if (op == "add" || op == "sub") {
        bool signA = (valA >> 3) & 1;
        bool signB = (valB >> 3) & 1;
        bool signRes = (res.valorMascarado >> 3) & 1;

        if (op == "add") {
            if ((signA == signB) && (signRes != signA)) res.overflow = true;
        } else if (op == "sub") {
            if ((signA != signB) && (signRes != signA)) res.overflow = true;
        }
    }

    return res;
}

// Módulo de Benchmark (Simulando o teste de carga em 16 bits do ESP32)
void benchmark(string op, int execucoes = 5) {
    int a = 32767 / 2; 
    int b = 3;
    volatile int resultado_dummy = 0;

    vector<double> tempos;
    double somaTempos = 0;

    for (int i = 0; i < execucoes; i++) {
        auto inicio = high_resolution_clock::now();

        if (op == "add") {
            resultado_dummy = a + b;
        } else if (op == "sub") {
            resultado_dummy = a - b;
        } else if (op == "mul") {
            resultado_dummy = a * b;
        } else if (op == "div") {
            resultado_dummy = (b != 0) ? (a / b) : 0;
        } else if (op == "fat") {
            resultado_dummy = calcularFatorial(min(a, 10));
        }

        auto fim = high_resolution_clock::now();
        duration<double, std::micro> tempo_us = fim - inicio;
        tempos.push_back(tempo_us.count());
        somaTempos += tempo_us.count();
    }

    double media = somaTempos / execucoes;
    double variancia = 0.0;
    for (double t : tempos) {
        variancia += pow(t - media, 2);
    }
    double desvioPadrao = sqrt(variancia / execucoes);

    cout << "\n--- 📊 Estatísticas de Desempenho (" << execucoes << " iterações) ---" << endl;
    cout << "Tempo Médio:   " << fixed << setprecision(4) << media << " µs" << endl;
    cout << "Desvio Padrão: " << desvioPadrao << " µs" << endl;
}

int main() {
    string op;
    int inputA, inputB = 0;

    cout << "--- Calculadora 4-bits Sinalizada (Raspberry Pi 3) ---" << endl;
    cout << "Escolha a operacao (add, sub, mul, div, fat): ";
    cin >> op;

    // Aceita tanto a representação binária convertida para inteiro (0-15) quanto decimais diretos
    cout << "Insira A (Em formato 4-bits bruto, ex: 3 para '0011' ou 12 para '1100'[-4]): ";
    cin >> inputA;

    if (op != "fat") {
        cout << "Insira B (Em formato 4-bits bruto, ex: 4 para '0100' ou 14 para '1110'[-2]): ";
        cin >> inputB;
    }

    // Executa a lógica corrigida
    ResultadoOp res = operar_4bits(inputA, inputB, op);
    
    cout << "\n--- Resultado no Hardware ---" << endl;
    cout << "Saída Binária (4 bits): ";
    for (int i = 3; i >= 0; i--) {
        cout << ((res.valorMascarado >> i) & 1);
    }
    cout << " (Decimal Sinalizado: " << res.valorDecimal << ")" << endl;

    // Alertas de proteção
    if (res.divByZero) {
        cout << "⚠ ERRO: TENTATIVA DE DIVISÃO POR ZERO!" << endl;
    } else if (res.overflow) {
        cout << "⚠ OVERFLOW / ESTOURO DE CAPACIDADE DE 4 BITS!" << endl;
    }

    // Dispara o benchmark de tempo
    benchmark(op, 5);

    return 0;
}