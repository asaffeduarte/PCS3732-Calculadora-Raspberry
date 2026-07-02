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
    int valor;
    bool erro;
    string mensagemErro;
};

// Executa a operação mascarando para 4 bits (0 a 15)
ResultadoOp operar_4bits(int a, int b, string op) {
    int valA = a & 0x0F;
    int valB = b & 0x0F;
    ResultadoOp res = {0, false, ""};

    if (op == "add") {
        res.valor = valA + valB;
    } else if (op == "sub") {
        res.valor = valA - valB;
    } else if (op == "mul") {
        res.valor = valA * valB;
    } else if (op == "div") {
        if (valB == 0) {
            res.erro = true;
            res.mensagemErro = "Divisão por zero!";
            res.valor = 0;
        } else {
            res.valor = valA / valB;
        }
    } else if (op == "fat") {
        if (valA > 7) {
            res.erro = true;
            res.mensagemErro = "Overflow de Fatorial para 4-bits";
        } else {
            res.valor = calcularFatorial(valA);
        }
    } else {
        res.erro = true;
        res.mensagemErro = "Operação inválida.";
    }

    res.valor = res.valor & 0x0F; // Garante saída em 4 bits
    return res;
}

// Função de Benchmark para TODAS as operações (Simulando 16 bits)
void benchmark(string op, int execucoes = 5) {
    int a = 32767 / 2; // Valor de teste (16-bits)
    int b = 3;
    volatile int resultado_dummy = 0; // 'volatile' impede o compilador de ignorar a conta

    vector<double> tempos;
    double somaTempos = 0;

    for (int i = 0; i < execucoes; i++) {
        auto inicio = high_resolution_clock::now();

        // Avalia a operação escolhida dentro do bloco de tempo
        if (op == "add") {
            resultado_dummy = a + b;
        } else if (op == "sub") {
            resultado_dummy = a - b;
        } else if (op == "mul") {
            resultado_dummy = a * b;
        } else if (op == "div") {
            resultado_dummy = (b != 0) ? (a / b) : 0;
        } else if (op == "fat") {
            resultado_dummy = calcularFatorial(min(a, 10)); // Limitado a 10 para evitar estouro interno
        }

        auto fim = high_resolution_clock::now();
        
        duration<double, std::micro> tempo_us = fim - inicio;
        tempos.push_back(tempo_us.count());
        somaTempos += tempo_us.count();
    }

    // Cálculos Estatísticos
    double media = somaTempos / execucoes;
    double variancia = 0.0;
    for (double t : tempos) {
        variancia += pow(t - media, 2);
    }
    double desvioPadrao = sqrt(variancia / execucoes);

    // Exibição dos resultados do Benchmark
    cout << "\n--- Estatísticas de Desempenho (" << execucoes << " iterações) ---" << endl;
    cout << "Operação avaliada: " << op << endl;
    cout << "Vetor de Tempos (µs): [";
    for (size_t i = 0; i < tempos.size(); i++) {
        cout << fixed << setprecision(4) << tempos[i] << (i < tempos.size() - 1 ? ", " : "");
    }
    cout << "]" << endl;
    cout << "Tempo Médio:   " << media << " µs" << endl;
    cout << "Desvio Padrão: " << desvioPadrao << " µs" << endl;
}

int main() {
    string op;
    int a, b = 0;

    cout << "--- Calculadora Completa com Benchmark (Raspberry Pi 3) ---" << endl;
    cout << "Escolha a operacao (add, sub, mul, div, fat): ";
    cin >> op;

    if (op != "add" && op != "sub" && op != "mul" && op != "div" && op != "fat") {
        cout << "Operação inválida!" << endl;
        return 1;
    }

    cout << "Insira A (0-15): ";
    cin >> a;

    if (op != "fat") {
        cout << "Insira B (0-15): ";
        cin >> b;
    }

    // 1. Execução da operação de 4 bits informada pelo teclado
    ResultadoOp res = operar_4bits(a, b, op);
    
    if (res.erro) {
        cout << "\n[AVISO] " << res.mensagemErro << endl;
    }
    cout << "Resultado (4-bits) em Decimal: " << res.valor << endl;

    // 2. Dispara o Benchmark para a operação selecionada
    benchmark(op, 5);

    return 0;
}