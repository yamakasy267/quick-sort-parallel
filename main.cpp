#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

class OmpRealize {
public:
    int Count;
    unsigned short int Count_threads;
    int* Arr;


    OmpRealize(int* arr, int n, unsigned short int count_threads) {

        Arr = arr;
        Count = n / 100 * 0.8;
        Count_threads = count_threads;
    }


    void omp_section(int start, int end)
    {

        int TempEnd = end;
        int TempStart = start;
        int n = (end + start) / 2;
        int tmp;
        int mid = Arr[n];
        do {
            while (Arr[TempStart] < mid) {
                TempStart++;
            }

            while (Arr[TempEnd] > mid) {
                TempEnd--;
            }

            if (TempStart <= TempEnd) {
                tmp = Arr[TempStart];
                Arr[TempStart] = Arr[TempEnd];
                Arr[TempEnd] = tmp;

                TempStart++;
                TempEnd--;
            }
        } while (TempStart <= TempEnd);

        if (end - start < Count || (end - TempStart < Count || TempEnd - start < Count))
        {
            if (start < TempEnd)
            {
                single(start, TempEnd);
            }
            if (TempStart < end)
            {
                single(TempStart, end);
            }
        }
        else
        {
#pragma omp parallel sections num_threads(2)
            {
#pragma omp section
                {
                    omp_section(TempStart, end);
                }
#pragma omp section
                {
                    omp_section(start, TempEnd);
                }
            }
        }
    }

    void omp_task(int start, int end)
    {
        int TempEnd = end;
        int TempStart = start;
        int n = (end + start) / 2;
        int tmp;
        int mid = Arr[n];

        do {
            while (Arr[TempStart] < mid) {
                TempStart++;
            }

            while (Arr[TempEnd] > mid) {
                TempEnd--;
            }

            if (TempStart <= TempEnd) {
                tmp = Arr[TempStart];
                Arr[TempStart] = Arr[TempEnd];
                Arr[TempEnd] = tmp;
                TempStart++;
                TempEnd--;
            }
        } while (TempStart <= TempEnd);
        if (end - start < Count || (end - TempStart < Count || TempEnd - start < Count))
        {
            if (start < TempEnd)
            {
#pragma omp task
                single(start, TempEnd);
            }
            if (TempStart < end)
            {
#pragma omp task
                single(TempStart, end);
            }
        }
        else
        {
#pragma omp task 
            {
                omp_task(start, TempEnd);
            }
            omp_task(TempStart, end);
        }
    }

    void single(int start, int end)
    {
        int TempEnd = end;
        int TempStart = start;
        int n = (end + start) / 2;
        int tmp;
        int mid = Arr[n];

        do {
            while (Arr[TempStart] < mid) {
                TempStart++;
            }

            while (Arr[TempEnd] > mid) {
                TempEnd--;
            }

            if (TempStart <= TempEnd) {
                tmp = Arr[TempStart];
                Arr[TempStart] = Arr[TempEnd];
                Arr[TempEnd] = tmp;
                TempStart++;
                TempEnd--;
            }
        } while (TempStart <= TempEnd);


        if (TempStart < end)
        {
            single(TempStart, end);
        }
        if (TempEnd > start)
        {
            single(start, TempEnd);
        }
    }
};


int main(int argc, char* argv[])
{
    unsigned short int  realize;
    unsigned int n, max_lvl;
    int chech_int, count_treads;

    if (argc != 5) {
        cerr << "error: invalid number of arguments" << endl;
        return 1;
    }
    else {
        try
        {
            count_treads = stoi(argv[3]);
            if (count_treads == 0 )
                count_treads = omp_get_num_threads();
            if (count_treads == -1)
            {
                realize = 0;
            }
            else
            {
                realize = stoi(argv[4]);
            }

        }
        catch (const std::exception&)
        {
            cerr << "error: The passed value is not a number" << endl;
            return 1;
        }
    }


    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "error: Could not open the file in" << endl;
        return 1;
    }
    file >> n;
    int* arr = new(nothrow) int[n];
    for (int i = 0; i < n; i++) {
        if (file >> chech_int) {
            try
            {
                arr[i] = chech_int;
            }
            catch (const std::exception& e)
            {
                cerr << "It is impossible to record due to the lack of allocated memory" << '\n';
                return 1;
            }


        }
        else {
            cout << "Enter not numbers" << endl;
            delete[] arr;
            return 1;
        }
    }
    file.close();

    double start, end;
    OmpRealize omp = OmpRealize(arr, n, count_treads);


    switch (realize)
    {
    case 0:
        start = omp_get_wtime();
        omp.single(0, n - 1);
        end = omp_get_wtime();
        printf("Time (%i thread(s)): %.7f ms\n", omp_get_num_threads(), (end - start) * 1000);
        break;
    case 1:
        omp_set_max_active_levels(log2(count_treads));
        start = omp_get_wtime();
        omp.omp_section(0, n - 1);
        end = omp_get_wtime();
        printf("Time (%i thread(s)): %.7f ms\n", count_treads, (end - start) * 1000);
        break;
    case 2:
        start = omp_get_wtime();
#pragma omp parallel num_threads(count_treads)
        {
#pragma omp master
            omp.omp_task(0, n - 1);
        }
        end = omp_get_wtime();
        printf("Time (%i thread(s)): %.7f ms\n", count_treads, (end - start) * 1000);
        break;
    default:
        cerr << "error:The key is not recognized" << endl;
        delete[] arr;
        return 1;
    }

    ofstream file_out(argv[2]);
    if (!file_out.is_open())
    {
        cerr << "error: Could not open the file out" << endl;
        delete[] arr;
        return 1;
    }
    for (int i = 0; i < n; i++) {
        file_out << arr[i] << ' ';
    }
    file_out << '\n';
    file_out.close();
    delete[] arr;

    return 0;
}