#include <iostream>
#include <cuda_runtime.h>
using namespace std;

// ─── Kernel: corre en GPU ─────────────────────────────────────
__global__ void SumaVectores(const float *a, const float *b, float *c, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n)
        c[i] = a[i] + b[i];
}

int main() {
    const int N     = 1024;
    const int bytes = N * sizeof(float);

    // ─── Memoria en CPU (host) ────────────────────────────────
    float *h_a = new float[N];
    float *h_b = new float[N];
    float *h_c = new float[N];

    for (int i = 0; i < N; ++i) {
        h_a[i] = i * 1.0f;
        h_b[i] = i * 2.0f;
    }

    // ─── Memoria en GPU (device) ──────────────────────────────
    float *d_a, *d_b, *d_c;
    cudaMalloc(&d_a, bytes);
    cudaMalloc(&d_b, bytes);
    cudaMalloc(&d_c, bytes);

    // ─── Copiar CPU -> GPU ─────────────────────────────────────
    cudaMemcpy(d_a, h_a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, bytes, cudaMemcpyHostToDevice);

    // ─── Lanzar kernel ────────────────────────────────────────
    int threadsPerBlock = 256;
    int blocksPerGrid   = (N + threadsPerBlock - 1) / threadsPerBlock;
    SumaVectores<<<blocksPerGrid, threadsPerBlock>>>(d_a, d_b, d_c, N);

    // ─── Copiar GPU -> CPU ─────────────────────────────────────
    cudaMemcpy(h_c, d_c, bytes, cudaMemcpyDeviceToHost);

    // ─── Verificar resultados ─────────────────────────────────
    bool ok = true;
    for (int i = 0; i < N; ++i)
        if (h_c[i] != h_a[i] + h_b[i]) { ok = false; break; }

    cout << (ok ? "Suma correcta!!" : "Error en suma!!") << endl;
    cout << "Ejemplo: a[10]=" << h_a[10] 
         << " + b[10]=" << h_b[10] 
         << " = c[10]=" << h_c[10] << endl;

    // ─── Liberar memoria ──────────────────────────────────────
    cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);
    delete[] h_a; delete[] h_b; delete[] h_c;

    return 0;
}