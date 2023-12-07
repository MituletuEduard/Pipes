#include <iostream>
#include <windows.h>

const int NUM_PROCESSES = 10;
const int RANGE_SIZE = 1000;

// Structura pentru transmiterea datelor prin pipes
struct PipeData {
    int start;
    int end;
};

// Functia pentru verificarea unui numar daca e prim
bool IsPrime(int num) {
    if (num < 2) {
        return false;
    }

    for (int i = 2; i <= num / 2; ++i) {
        if (num % i == 0) {
            return false;
        }
    }

    return true;
}

// Functia executata de fiecare proces
DWORD WINAPI FindPrimes(LPVOID lpParam) {
    PipeData* pipeData = reinterpret_cast<PipeData*>(lpParam);

    for (int num = pipeData->start; num < pipeData->end; ++num) {
        if (IsPrime(num)) {
            // Trimite numarul prim catre procesul parinte prin pipes
            std::cout << num << " ";
        }
    }

    return 0;
}

int main() {
    HANDLE threads[NUM_PROCESSES];
    HANDLE pipes[NUM_PROCESSES][2]; // Pipes pentru fiecare proces

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        // Creeaza un pipe pentru fiecare proces
        if (!CreatePipe(&pipes[i][0], &pipes[i][1], NULL, 0)) {
            std::cerr << "Error creating pipe for process " << i << std::endl;
            return 1;
        }

        // Creeaza o structura pentru datele pipe-ului
        PipeData pipeData;
        pipeData.start = i * RANGE_SIZE;
        pipeData.end = (i + 1) * RANGE_SIZE;

        // Lanseaza un nou proces
        threads[i] = CreateThread(NULL, 0, FindPrimes, &pipeData, 0, NULL);
    }

    // Asteapta ca toate procesele sa se incheie
    WaitForMultipleObjects(NUM_PROCESSES, threads, TRUE, INFINITE);

    // Inchide toate pipe-urile
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        CloseHandle(pipes[i][0]);
        CloseHandle(pipes[i][1]);
    }

    return 0;
}
