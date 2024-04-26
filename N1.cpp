#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <thread>
#include <time.h>
#include <memory>
#include <math.h>

#define POTOCKS 20
 
double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

void matrix_vector_product_thread(std::shared_ptr<double []> a, std::shared_ptr<double []> b, std::shared_ptr<double []> c, int m, int n, int threadid, int items_per_thread)
{
    int lb = threadid * items_per_thread;
    int ub = (threadid == POTOCKS - 1) ? (m - 1) : (lb + items_per_thread - 1);

    for (int i = lb; i <= ub; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

void matrix_vector_init_thread(std::shared_ptr<double []> a, std::shared_ptr<double []> b, int m, int n, int threadid, int items_per_thread)
{
    int lb = threadid * items_per_thread;
    int ub = (threadid == POTOCKS - 1) ? (m - 1) : (lb + items_per_thread - 1);

    for (int i = lb; i < ub; i++) {
        b[i] = n+1;
        for (int j = 0; j < n; j++)
            a[i * n + j] = 1.0;
        a[i * n + i] = 2.0;
    } 
}

int main(){
    int m = 40000;
    int n = 40000;

    std::shared_ptr<double[]>    a(new double[sizeof(double) * m * n], [] (double* i) { 
        delete[] i; // Кастомное удаление
    });
    std::shared_ptr<double[]>    b(new double[sizeof(double) * n], [] (double* i) { 
        delete[] i; // Кастомное удаление
    });
    std::shared_ptr<double[]>    c(new double[sizeof(double) * m], [] (double* i) { 
        delete[] i; // Кастомное удаление
    });

    std::vector<std::thread> threads1;
    int items_per_thread1 = m / POTOCKS;
    for (int i = 0; i < POTOCKS; i++)
    {
        threads1.push_back(std::thread(matrix_vector_init_thread, a, b, m, n, i, items_per_thread1));
    }
    for (auto& thread : threads1)
    {
        thread.join();
    }
    

/*     for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            a[i * n + j] = 1.0;
        a[i * n + i] = 2.0;
    } 
    for (int i = 0; i < n; i++) {
        b[n] = n+1;
    }  */

    double t_start = cpuSecond();

    std::vector<std::thread> threads;
    int items_per_thread = m / POTOCKS;
    for (int i = 0; i < POTOCKS; i++)
    {
        threads.push_back(std::thread(matrix_vector_product_thread, a, b, c, m, n, i, items_per_thread));
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    double t_end = cpuSecond();

    std::cout << "Elapsed time: " << t_end - t_start << " sec." << std::endl;

}