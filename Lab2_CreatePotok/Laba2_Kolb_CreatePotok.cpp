#include <iostream>
#include <windows.h>
#include <vector>
#include <thread>

using namespace std;

vector<int> numbers;
int min_value, max_value;
double average_value;

DWORD WINAPI min_max_thread(LPVOID) {
    min_value = numbers[0];
    max_value = numbers[0];

    for (const int& num : numbers) {
        if (num < min_value) {
            min_value = num;
        }
        Sleep(7);
        if (num > max_value) {
            max_value = num;
        }
        Sleep(7);
    }

    cout << "Min element: " << min_value << endl;
    cout << "Max element: " << max_value << endl;
    return 0;
}

DWORD WINAPI average_thread(LPVOID) {
    double sum = 0;

    for (const int& num : numbers) {
        sum += num;
        Sleep(12);
    }

    average_value = sum / numbers.size();
    cout << "Average: " << average_value << endl;
    return 0;
}

int main() {
    int size;
    cout << "Enter array size: ";
    cin >> size;

    numbers.resize(size);
    cout << "Enter array elements: ";
    for (int i = 0; i < size; ++i) {
        cin >> numbers[i];
    }

    HANDLE hMinMax = CreateThread(NULL, 0, min_max_thread, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average_thread, NULL, 0, NULL);

    // Ожидание завершения потоков
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    // Замена на средний
    for (int& num : numbers) {
        if (num == min_value || num == max_value) {
            num = static_cast<int>(average_value);
        }
    }

    cout << "New array: ";
    for (const int& num : numbers) {
        cout << num << " ";
    }
    cout << endl;

    // Закрытие дескрипторов потоков
    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    return 0;
}