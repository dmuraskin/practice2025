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
    enum CellState {
        EMPTY,
        START,
        END,
        PATH
    };

private:
    int width;
    int height;
    vector<vector<CellState>> cells;    // Состояния клеток (без информации о стенах)
    vector<vector<bool>> horizWalls;    // Горизонтальные стены [height-1][width]
    vector<vector<bool>> vertWalls;     // Вертикальные стены [height][width-1]
    vector<vector<int>> distances;
    pair<int, int> start;
    pair<int, int> end;
    bool waveExecuted;
    size_t operation_count;

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
        cells.assign(height, vector<CellState>(width, EMPTY));
        horizWalls.assign(height - 1, vector<bool>(width, false));
        vertWalls.assign(height, vector<bool>(width - 1, false));
        distances.assign(height, vector<int>(width, -1));

        // Установка старта и финиша по умолчанию
        setStart(0, 0);
        setEnd(width - 1, height - 1);
    }

    // НОВЫЕ МЕТОДЫ ДЛЯ РАБОТЫ СО СТЕНАМИ
    void setHorizontalWall(int x, int y, bool exists) {
        if (y >= 0 && y < height - 1 && x >= 0 && x < width) {
            horizWalls[y][x] = exists;
            waveExecuted = false;
        }
    }

    void setVerticalWall(int x, int y, bool exists) {
        if (y >= 0 && y < height && x >= 0 && x < width - 1) {
            vertWalls[y][x] = exists;
            waveExecuted = false;
        }
    }

    bool canMove(int x1, int y1, int x2, int y2) const {
        if (!isValid(x2, y2)) return false;

        if (x1 == x2) { // Движение по вертикали
            int minY = min(y1, y2);
            return !horizWalls[minY][x1];
        }
        else if (y1 == y2) { // Движение по горизонтали
            int minX = min(x1, x2);
            return !vertWalls[y1][minX];
        }
        return false;
    }

    // ПЕРЕРАБОТАННЫЙ МЕТОД ОТОБРАЖЕНИЯ
    void printMaze() const {
        // Верхняя граница
        for (int x = 0; x < width; ++x) cout << "+---";
        cout << "+\n";

        for (int y = 0; y < height; ++y) {
            cout << "|"; // Левая граница
            for (int x = 0; x < width; ++x) {
                // Содержимое клетки
                char c = ' ';
                if (cells[y][x] == START) c = 'S';
                else if (cells[y][x] == END) c = 'E';
                else if (cells[y][x] == PATH) c = '*';

                cout << " " << c << " ";

                // Вертикальные стены
                if (x < width - 1) {
                    cout << (vertWalls[y][x] ? "|" : " ");
                }
            }
            cout << "|\n"; // Правая граница

            // Горизонтальные стены между рядами
            if (y < height - 1) {
                cout << "+";
                for (int x = 0; x < width; ++x) {
                    cout << (horizWalls[y][x] ? "---" : "   ");
                    cout << "+";
                }
                cout << "\n";
            }
        }

        // Нижняя граница
        for (int x = 0; x < width; ++x) cout << "+---";
        cout << "+\n";
    }

    // СТАРЫЕ МЕТОДЫ (ПОКА БЕЗ ИЗМЕНЕНИЙ)
    bool isValid(int x, int y) const {
        return x >= 0 && y >= 0 && x < width && y < height;
    }

    void setStart(int x, int y) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(x, y)) {
            cerr << "Неверные координаты!" << endl;
            return;
        }
        operation_count++;

        // Сброс предыдущего старта
        if (isValid(start.first, start.second)) {
            cells[start.second][start.first] = EMPTY;
            operation_count++;
        }

        start = { x, y };
        cells[y][x] = START;
        waveExecuted = false;
        operation_count += 3;

        cout << "Старт установлен в (" << x << ", " << y << ")" << endl;
        printMetrics("Установка стартовой точки", start_time);
    }

    void setEnd(int x, int y) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(x, y)) {
            cerr << "Неверные координаты!" << endl;
            return;
        }
        operation_count++;

        // Сброс предыдущего финиша
        if (isValid(end.first, end.second)) {
            cells[end.second][end.first] = EMPTY;
            operation_count++;
        }

        end = { x, y };
        cells[y][x] = END;
        waveExecuted = false;
        operation_count += 3;

        cout << "Финиш установлен в (" << x << ", " << y << ")" << endl;
        printMetrics("Установка конечной точки", start_time);
    }

    // ОСТАЛЬНЫЕ МЕТОДЫ ПОКА ОСТАЮТСЯ БЕЗ ИЗМЕНЕНИЙ
    void loadFromFile(const string& filename) {
        // Старая реализация (будет изменена в другой ветке)
        // ...
    }

    void saveToFile(const string& filename) {
        // Старая реализация (будет изменена в другой ветке)
        // ...
    }

    void generateRandom(int w, int h, double wallProbability = 0.3) {
        // Старая реализация (будет изменена в другой ветке)
        // ...
    }

    void waveAlgorithm() {
        // Пока использует старую логику (будет изменено в другой ветке)
        // ...
    }

    void findPath() {
        // Пока использует старую логику (будет изменено в другой ветке)
        // ...
    }
};

void showMenu() {
    cout << "\n=== Меню управления лабиринтом ===" << endl;
    cout << "1. Загрузить лабиринт из файла" << endl;
    cout << "2. Сгенерировать случайный лабиринт" << endl;
    cout << "3. Установить стартовую точку" << endl;
    cout << "4. Установить конечную точку" << endl;
    cout << "5. Установить горизонтальную стену" << endl;
    cout << "6. Установить вертикальную стену" << endl;
    cout << "7. Выполнить волновой алгоритм" << endl;
    cout << "8. Найти и показать путь" << endl;
    cout << "9. Сохранить лабиринт в файл" << endl;
    cout << "10. Показать лабиринт" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "RUS");
    Maze maze(5, 5);
    int choice;
    string filename;
    int x, y;
    bool wall;

    while (true) {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод! Пожалуйста, введите число." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Введите имя файла: ";
            cin >> filename;
            maze.loadFromFile(filename);
            break;
        case 2:
            cout << "Введите ширину и высоту лабиринта: ";
            cin >> x >> y;
            if (x <= 0 || y <= 0) {
                cout << "Размеры должны быть положительными!" << endl;
                break;
            }
            maze.generateRandom(x, y);
            break;
        case 3:
            cout << "Введите координаты (x y) для старта: ";
            cin >> x >> y;
            maze.setStart(x, y);
            break;
        case 4:
            cout << "Введите координаты (x y) для финиша: ";
            cin >> x >> y;
            maze.setEnd(x, y);
            break;
        case 5:
            cout << "Введите координаты (x y) и наличие стены (0/1): ";
            cin >> x >> y >> wall;
            maze.setHorizontalWall(x, y, wall);
            break;
        case 6:
            cout << "Введите координаты (x y) и наличие стены (0/1): ";
            cin >> x >> y >> wall;
            maze.setVerticalWall(x, y, wall);
            break;
        case 7:
            maze.waveAlgorithm();
            break;
        case 8:
            maze.findPath();
            break;
        case 9:
            cout << "Введите имя файла для сохранения: ";
            cin >> filename;
            maze.saveToFile(filename);
            break;
        case 10:
            maze.printMaze();
            break;
        case 0:
            cout << "Выход из программы..." << endl;
            return 0;
        default:
            cout << "Неизвестная команда!" << endl;
        }
    }

    return 0;
}