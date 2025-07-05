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
#include <sstream>

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
    vector<vector<CellType>> grid;
    vector<vector<int>> distances;
    pair<int, int> start;
    pair<int, int> end;
    vector<vector<int>> distances;
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

    bool isValid(int x, int y) const {
        return x >= 0 && y >= 0 && x < width && y < height;
    }

    bool canMove(int x1, int y1, int x2, int y2) const {
        if (!isValid(x2, y2)) return false;

        if (x1 == x2) { // Движение по вертикали
            int minY = min(y1, y2);
            if (minY < 0 || minY >= horizWalls.size()) return false;
            return !horizWalls[minY][x1];
        }
        else if (y1 == y2) { // Движение по горизонтали
            int minX = min(x1, x2);
            if (minX < 0 || minX >= vertWalls[0].size()) return false;
            return !vertWalls[y1][minX];
        }
        return false;
    }

    char getCellChar(int x, int y) const {
        switch (cells[y][x]) {
        case START: return 'S';
        case END: return 'E';
        case PATH: return '*';
        default: return ' ';
        }
    }

public:
    Maze(int w, int h) : width(w), height(h), waveExecuted(false), operation_count(0) {
        grid.resize(height, vector<CellType>(width, EMPTY));
        distances.resize(height, vector<int>(width, -1));
    }

        // Установка старта и финиша по умолчанию
        setStart(0, 0);
        setEnd(width - 1, height - 1);
    }

        vector<vector<char>> tempGrid;
        string line;
        while (getline(file, line)) {
            operation_count += 2;
            vector<char> row(line.begin(), line.end());
            tempGrid.push_back(row);
        }

        height = tempGrid.size();
        operation_count++;
        if (height == 0) {
            cerr << "Файл пустой!" << endl;
            return;
        }
        width = tempGrid[0].size();
        operation_count++;

        grid.resize(height, vector<CellType>(width));
        distances.resize(height, vector<int>(width, -1));
        operation_count += 2;

        bool startFound = false;
        bool endFound = false;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                operation_count += 4;
                switch (tempGrid[y][x]) {
                case '#': grid[y][x] = WALL; break;
                case '.': grid[y][x] = EMPTY; break;
                case 'S':
                    grid[y][x] = START;
                    start = { x, y };
                    startFound = true;
                    operation_count += 3;
                    break;
                case 'E':
                    grid[y][x] = END;
                    end = { x, y };
                    endFound = true;
                    operation_count += 3;
                    break;
                default:
                    grid[y][x] = EMPTY;
                }
            }
        }

        if (!startFound) {
            cerr << "Стартовая точка не найдена! Установите её вручную." << endl;
        }
        if (!endFound) {
            cerr << "Конечная точка не найдена! Установите её вручную." << endl;
        }

        cout << "Лабиринт загружен из файла " << filename << endl;
        printMetrics("Загрузка из файла", start_time);
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

    void setStart(int x, int y) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(x, y)) {
            cerr << "Неверные координаты!" << endl;
            return;
        }
        operation_count++;

        if (grid[y][x] == WALL) {
            cerr << "Нельзя установить старт в стену!" << endl;
            return;
        }
        operation_count++;

        if (grid[y][x] == END) {
            cerr << "Старт и финиш не могут совпадать!" << endl;
            return;
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

        if (grid[y][x] == WALL) {
            cerr << "Нельзя установить финиш в стену!" << endl;
            return;
        }
        operation_count++;

        if (grid[y][x] == START) {
            cerr << "Старт и финиш не могут совпадать!" << endl;
            return;
        }

        end = { x, y };
        cells[y][x] = END;
        waveExecuted = false;
        operation_count += 3;

        cout << "Финиш установлен в (" << x << ", " << y << ")" << endl;
        printMetrics("Установка конечной точки", start_time);
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

    void waveAlgorithm() {
        // Пока использует старую логику (будет изменено в другой ветке)
        // ...
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

    void printMaze() const {
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                cout << static_cast<char>(cell) << ' ';
            }
            cout << '\n';
        }
        cout << endl;
    }

    void saveToFile(const string& filename) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        ofstream file(filename);
        operation_count++;
        if (!file) {
            cerr << "Ошибка создания файла!" << endl;
            return;
        }

        for (const auto& row : grid) {
            for (const auto& cell : row) {
                operation_count++;
                file << static_cast<char>(cell);
            }
            file << '\n';
            operation_count++;
        }

        cout << "Лабиринт сохранен в файл " << filename << endl;
        printMetrics("Сохранение в файл", start_time);
    }

    void editCell(int x, int y, CellType type) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(x, y)) {
            cerr << "Неверные координаты!" << endl;
            return;
        }
        operation_count++;

        if (type == START) {
            setStart(x, y);
        }
        else if (type == END) {
            setEnd(x, y);
        }
        else {
            grid[y][x] = type;
            waveExecuted = false;
            operation_count += 2;
        }
        operation_count++;

        printMetrics("Редактирование клетки", start_time);
    }
};

void showMenu() {
    cout << "\n=== Меню управления лабиринтом ===" << endl;
    cout << "1. Загрузить лабиринт из файла" << endl;
    cout << "2. Сгенерировать случайный лабиринт" << endl;
    cout << "3. Установить стартовую точку" << endl;
    cout << "4. Установить конечную точку" << endl;
    cout << "5. Редактировать клетку" << endl;
    cout << "6. Выполнить волновой алгоритм" << endl;
    cout << "7. Найти и показать путь" << endl;
    cout << "8. Сохранить лабиринт в файл" << endl;
    cout << "9. Показать лабиринт" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите действие: ";
}

int main() {
    setlocale(LC_ALL, "RUS");
    srand(time(0));
    Maze maze(5, 5);
    int choice;
    string filename;
    int x, y;

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
            cout << "Введите ширину и высоту лабиринта: ";
            cin >> x >> y;
            if (x <= 0 || y <= 0) {
                cout << "Размеры должны быть положительными!" << endl;
                break;
            }
            maze.generateRandomWithPath(x, y);
            break;
        case 4:
            cout << "Введите координаты (x y) для старта: ";
            cin >> x >> y;
            maze.setStart(x, y);
            break;
        case 5:
            cout << "Введите координаты (x y) для финиша: ";
            cin >> x >> y;
            maze.setEnd(x, y);
            break;
        case 5:
            cout << "Введите координаты (x y) и тип клетки (0-стена, 1-путь): ";
            cin >> x >> y >> choice;
            if (choice < 0 || choice > 1) {
                cout << "Некорректный тип клетки!" << endl;
                break;
            }
            maze.editCell(x, y, choice == 0 ? Maze::WALL : Maze::EMPTY);
            break;
        case 6:
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