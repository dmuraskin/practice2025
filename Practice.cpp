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

using namespace std;
using namespace std::chrono;

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

    void waveAlgorithm() {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isPassable(start.first, start.second)) {
            cerr << "Стартовая позиция непроходима!" << endl;
            return;
        }
        operation_count++;

        for (auto& row : distances) {
            operation_count += row.size();
            fill(row.begin(), row.end(), -1);
        }

        distances[start.second][start.first] = 0;
        waveExecuted = true;
        operation_count += 2;

        queue<pair<int, int>> q;
        q.push(start);
        operation_count++;
        //соседи верхний, правый, нижний, левый
        const int dx[] = { 0, 1, 0, -1 };
        const int dy[] = { -1, 0, 1, 0 };
        operation_count += 2;

        while (!q.empty()) {
            operation_count++;
            auto current = q.front();
            q.pop();
            operation_count += 2;

            for (int i = 0; i < 4; ++i) {
                operation_count += 2;
                int nx = current.first + dx[i];
                int ny = current.second + dy[i];
                operation_count += 2;

                if (isPassable(nx, ny) && distances[ny][nx] == -1) {
                    distances[ny][nx] = distances[current.second][current.first] + 1;
                    q.push({ nx, ny });
                    operation_count += 3;
                }
                operation_count++;
            }
        }

        cout << "Волновой алгоритм выполнен" << endl;
        printMetrics("Волновой алгоритм", start_time);
    }

    void findPath() {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!waveExecuted) {
            cerr << "Сначала выполните волновой алгоритм!" << endl;
            return;
        }
        operation_count++;

        if (!isPassable(end.first, end.second) || distances[end.second][end.first] == -1) {
            cerr << "Путь до конечной позиции не существует!" << endl;
            return;
        }
        operation_count += 2;

        auto originalGrid = grid;
        vector<pair<int, int>> pathCoordinates;
        operation_count += 2;

        int x = end.first, y = end.second;
        operation_count += 2;
        while (!(x == start.first && y == start.second)) {
            operation_count++;
            pathCoordinates.emplace_back(x, y);
            operation_count++;

            if (grid[y][x] != START && grid[y][x] != END) {
                grid[y][x] = PATH;
                operation_count++;
            }

            const int dx[] = { 0, 1, 0, -1 };
            const int dy[] = { -1, 0, 1, 0 };
            operation_count += 2;

            bool found = false;
            operation_count++;
            for (int i = 0; i < 4 && !found; ++i) {
                operation_count += 2;
                int nx = x + dx[i];
                int ny = y + dy[i];
                operation_count += 2;

                if (isValid(nx, ny) && distances[ny][nx] == distances[y][x] - 1) {
                    x = nx;
                    y = ny;
                    found = true;
                    operation_count += 3;
                }
                operation_count++;
            }
            if (!found) break;
            operation_count++;
        }
        pathCoordinates.emplace_back(start.first, start.second);
        operation_count++;

        cout << "Кратчайший путь от S до E:" << endl;
        printMaze();

        reverse(pathCoordinates.begin(), pathCoordinates.end());
        operation_count += pathCoordinates.size();

        cout << "Координаты пути (" << pathCoordinates.size() << " шагов):" << endl;
        for (size_t i = 0; i < pathCoordinates.size(); ++i) {
            operation_count += 4;
            auto coord = pathCoordinates[i];
            cout << i + 1 << ". (" << coord.first << ", " << coord.second << ")";
            if (i == 0) cout << " - Начало";
            else if (i == pathCoordinates.size() - 1) cout << " - Конец";
            cout << endl;
        }

        grid = originalGrid;
        operation_count++;

        printMetrics("Поиск пути", start_time);
    }
};