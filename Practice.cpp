#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <utility>
#include <algorithm>
#include <chrono>
class Maze {
public:
    enum CellType {
        WALL = '#',
        EMPTY = '.',
        PATH = '*',
        START = 'S',
        END = 'E'
    };

private:
    int width;
    int height;
    vector<vector<CellType>> grid;
    vector<vector<int>> distances;
    pair<int, int> start;
    pair<int, int> end;
    bool waveExecuted;
    size_t operation_count;

    // Вспомогательные методы для замера производительности
    void resetMetrics() {
        operation_count = 0;
    }

    void printMetrics(const string& operationName, high_resolution_clock::time_point start_time) {
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end_time - start_time);

        cout << "=== Метрики производительности ===" << endl;
        cout << "Операция: " << operationName << endl;
        cout << "Время выполнения: " << duration.count() << " нс" << endl;
        cout << "Количество операций: " << operation_count << endl;
        cout << "===============================" << endl << endl;
    }

public:
    Maze(int w, int h) : width(w), height(h), waveExecuted(false), operation_count(0) {
        grid.resize(height, vector<CellType>(width, EMPTY));
        distances.resize(height, vector<int>(width, -1));
    }
}