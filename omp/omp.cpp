#include <iostream>
#include <omp.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
int constN = 10;


// последовательно
vector<double> SummSequence(vector<double> a) {
    for (int i = 1; i < a.size(); i++)
    {
        a[i] += a[i - 1];
    }
    return a;
}


vector<double> sumParalell(vector<double> sum, int p = 4) {
    int N = sum.size();
    int d = N / p;
    // если размер не кратен числу потоков
    if (N % p > 0)
        d += 1;  
  

#pragma omp parallel num_threads(p) // распараллеливает основной поток на несколько p = 4 
    {   
        /// здесь мы бежим с помощью нескольких потоков и находим частичные сумму для каждой части(их у нас всего p)
#pragma omp for
        for (int i = 0; i < p; i++)
        {
            //cout << i<< " ";
            for (int j = 1; j < d && i * d + j < N; j++)
            {
                sum[i * d + j] += sum[i * d + j - 1];
            }
        }
        /// остановили потоки, чтобы остальные закончили свою работу.
#pragma omp barrier

#pragma omp single
        /// увеличиваем последовательно коследние элементы каждой части, чтобы 
        for (int i = 1; i < p && (i + 1) * d - 1 < N; i++)
        {
            sum[(i + 1) * d - 1] += sum[(i)*d - 1];
        }
#pragma omp for
        for (int i = d; i < N; i++) 
        {
            if ((i + 1) % d > 0)
            {
                sum[i] += sum[d * int((i + 1) / d) - 1];
            }
        }
    }
    return sum;
}
//Гибридная функция
//для нахождения частиных функция, выбирает сама использовать
//параллельный или последовательный метод для работы алгоритма
vector<double> SummArray(vector<double> a, int N) {
    ifstream fin;
    fin.open("SizeArray.txt");
    int maxN;
    fin >> maxN;
    if (N > maxN)
        return(sumParalell(a, 4));
    return(SummSequence(a));
}


double DiffParalellSequence(int N) {
    vector<double> a;
    long double beginTime1, endTime1, beginTime2, endTime2;
    for (int i = 0; i < N; i++)
        a.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 100)));
    
    // измеряем начала работы паралельного метода и конец

    beginTime2 = omp_get_wtime();
    vector<double> sum2 = sumParalell(a, 4);
    endTime2 = omp_get_wtime();

    beginTime1 = omp_get_wtime();
    vector<double> sum1 = SummSequence(a);
    endTime1 = omp_get_wtime();
    //cout << "parallel = " << endTime2 - beginTime2 << " sequency" << endTime1 - beginTime1 << endl;
    return ((endTime2 - beginTime2 -(endTime1 - beginTime1))); //разница времени работы паралелльного и последовательного

}

// находит и записывает в файл размер массива, при котором параллельный лучше последовательного
int outSizeArrayParallel() {
    int N = 40;

    int count = 0;
    double dif = 0;
    while (count < 10) {
        N = N + 20;
        dif = DiffParalellSequence(N);
        if (dif < 0)
            count++;
        else
            count = 0;
    }
    ofstream fout;
    fout.open("SizeArray.txt");
    fout << N;
    fout.close();

    return N;
}


vector<double> OutTimesParallel() {
    vector<double> times;
    ifstream fin;
    fin.open("SizeArray.txt");
    int M;
    fin >> M;
    int N = 2 * M;
    cout << "N = " << N << endl;
    vector<double> a;
    for (int i = 0; i < N; i++)
        a.push_back(static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 100000)));
    //for (int i = 0; i < 5; i++) // для того, чтобы собрать хорошие данные  можем несколько раз запустить
        for (int p = 1; p <= 16; p++)
        {
            long double begin, end;
            begin = omp_get_wtime();
            vector<double> sum1 = sumParalell(a, p);
            end = omp_get_wtime();
            times.push_back(end - begin);
        }//cекунду
    ofstream fout;
    fout.open("diagramma.txt");
    for (int i = 0; i < times.size(); i++)
        fout << (times[i]) << endl;
    fout.close();
    return times;
}


int main()
{
    setlocale(LC_ALL, "ru");
    vector<double> a;
    vector<double> b;
    vector<double> times;
    int M;
    cout << "Изначальный массив:" << endl;
    for (int i = 0; i < 50; i++)
    {
        a.push_back(i );
        b.push_back(i);
        cout << a[i] << ' ';
    }
    cout << "Нахождение частичных сумм, паралельно :" << endl;
    vector<double> sum = sumParalell(a);
    for (int i = 0; i < 50; i++)
        cout << sum[i] << ' ';
    cout << "Нахождение частичных сумм последовательно" << endl;
    vector<double> sum2 = SummSequence(b);
    for (int i = 0; i < 50; i++)
        cout << sum2[i] << ' ';
    cout << endl;
    outSizeArrayParallel();

    ifstream fin;
    fin.open("SizeArray.txt");
    fin >> M;
    fin.close();
    cout << "Частичная сумма находится быстрее паралельно при N > " << M << endl;
    times = OutTimesParallel();
    for (int i = 0; i < times.size(); i++)
    {
        if( i % 16 == 0)
        cout << (i) / 16  + 1<<"выборка";
        printf("\nкол-во потоков = %d \tвремя = %.9g секунд\n", (i) % 16 + 1, times[i]);

    }
}
