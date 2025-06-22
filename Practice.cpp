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

    void generateRandom(int w, int h, double wallProbability = 0.3) {
        // Сброс состояния
        width = w;
        height = h;
        grid.assign(height, vector<CellType>(width, EMPTY));
        distances.assign(height, vector<int>(width, -1));
        waveExecuted = false;
        operation_count = 0;

        // Генерация случайных стен
        srand(time(0));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (rand() / (double)RAND_MAX < wallProbability) {
                    grid[y][x] = WALL;
                }
            }
        }
        // Старт в левом верхнем углу
        start = { 0, 0 };
        grid[0][0] = START;

        // Финиш в правом нижнем углу
        end = { width - 1, height - 1 };

        grid[height - 1][width - 1] = END;

        cout << "Случайный лабиринт " << width << "x" << height << " сгенерирован" << endl;
    }

    void printMaze() const {
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                cout << static_cast<char>(cell) << ' ';
            }
            cout << '\n';
        }
        cout << endl;
    }
    bool isValid(int x, int y) const {
        return x >= 0 && y >= 0 && x < width && y < height;
    }

    bool isPassable(int x, int y) const {
        return isValid(x, y) && grid[y][x] != WALL;
    }

}