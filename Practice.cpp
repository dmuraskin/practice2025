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
    vector<vector<CellState>> cells;
    vector<vector<bool>> horizWalls; // Горизонтальные стены между строками [height-1][width]
    vector<vector<bool>> vertWalls;  // Вертикальные стены между столбцами [height][width-1]
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
        cells.assign(height, vector<CellState>(width, EMPTY));
        horizWalls.assign(height - 1, vector<bool>(width, false));
        vertWalls.assign(height, vector<bool>(width - 1, false));
        distances.assign(height, vector<int>(width, -1));

        // Установка старта и финиша по умолчанию
        setStart(0, 0);
        setEnd(width - 1, height - 1);
    }

    void loadFromFile(const string& filename) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        ifstream file(filename);
        operation_count++;
        if (!file) {
            cerr << "Ошибка открытия файла!" << endl;
            return;
        }

        string line;
        // Чтение размеров
        getline(file, line);
        width = stoi(line);
        getline(file, line);
        height = stoi(line);

        cells.assign(height, vector<CellState>(width, EMPTY));
        horizWalls.assign(height - 1, vector<bool>(width, false));
        vertWalls.assign(height, vector<bool>(width - 1, false));
        distances.assign(height, vector<int>(width, -1));

        // Чтение горизонтальных стен
        for (int y = 0; y < height - 1; ++y) {
            getline(file, line);
            for (int x = 0; x < width; ++x) {
                horizWalls[y][x] = (line[x] == '1');
                operation_count++;
            }
        }

        // Чтение вертикальных стен
        for (int y = 0; y < height; ++y) {
            getline(file, line);
            for (int x = 0; x < width - 1; ++x) {
                vertWalls[y][x] = (line[x] == '1');
                operation_count++;
            }
        }

        // Чтение стартовой позиции
        getline(file, line);
        istringstream iss_start(line);
        iss_start >> start.first >> start.second;
        cells[start.second][start.first] = START;
        operation_count += 2;

        // Чтение конечной позиции
        getline(file, line);
        istringstream iss_end(line);
        iss_end >> end.first >> end.second;
        cells[end.second][end.first] = END;
        operation_count += 2;

        waveExecuted = false;
        cout << "Лабиринт загружен из файла " << filename << endl;
        printMetrics("Загрузка из файла", start_time);
    }

    void generateRandom(int w, int h, double wallProbability = 0.3) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        width = w;
        height = h;
        cells.assign(height, vector<CellState>(width, EMPTY));
        horizWalls.assign(height - 1, vector<bool>(width, false));
        vertWalls.assign(height, vector<bool>(width - 1, false));
        distances.assign(height, vector<int>(width, -1));
        waveExecuted = false;

        srand(time(0));

        // Генерация горизонтальных стен
        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width; ++x) {
                horizWalls[y][x] = (rand() / (double)RAND_MAX) < wallProbability;
                operation_count += 2;
            }
        }

        // Генерация вертикальных стен
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                vertWalls[y][x] = (rand() / (double)RAND_MAX) < wallProbability;
                operation_count += 2;
            }
        }

        // Установка старта и финиша
        setStart(0, 0);
        setEnd(width - 1, height - 1);

        cout << "Случайный лабиринт " << width << "x" << height << " сгенерирован" << endl;
        printMetrics("Генерация лабиринта", start_time);
    }

    void setStart(int x, int y) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(x, y)) {
            cerr << "Неверные координаты!" << endl;
            return;
        }
        operation_count++;

        // Сбрасываем предыдущий старт
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

        // Сбрасываем предыдущий финиш
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

    void waveAlgorithm() {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        if (!isValid(start.first, start.second)) {
            cerr << "Стартовая позиция недействительна!" << endl;
            return;
        }
        operation_count++;

        distances.assign(height, vector<int>(width, -1));
        distances[start.second][start.first] = 0;
        waveExecuted = true;
        operation_count += 2;

        queue<pair<int, int>> q;
        q.push(start);
        operation_count++;

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

                if (canMove(current.first, current.second, nx, ny) &&
                    distances[ny][nx] == -1) {
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

        if (distances[end.second][end.first] == -1) {
            cerr << "Путь до конечной позиции не существует!" << endl;
            return;
        }
        operation_count++;

        // Сохраняем оригинальное состояние
        auto originalCells = cells;
        vector<pair<int, int>> pathCoordinates;
        operation_count += 2;

        int x = end.first, y = end.second;
        pathCoordinates.emplace_back(x, y);
        operation_count += 3;

        while (!(x == start.first && y == start.second)) {
            operation_count++;
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

                if (isValid(nx, ny) &&
                    distances[ny][nx] == distances[y][x] - 1 &&
                    canMove(x, y, nx, ny)) {
                    x = nx;
                    y = ny;
                    found = true;
                    pathCoordinates.emplace_back(x, y);
                    if (cells[y][x] == EMPTY) {
                        cells[y][x] = PATH;
                    }
                    operation_count += 5;
                }
                operation_count++;
            }

            if (!found) {
                cerr << "Ошибка восстановления пути!" << endl;
                break;
            }
            operation_count++;
        }

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

        // Восстанавливаем оригинальное состояние
        cells = originalCells;
        operation_count++;

        printMetrics("Поиск пути", start_time);
    }

    void printMaze() const {
        // Верхняя граница
        for (int x = 0; x < width; ++x) {
            cout << "+---";
        }
        cout << "+\n";

        for (int y = 0; y < height; ++y) {
            // Вертикальные стены и клетки
            cout << "|";
            for (int x = 0; x < width; ++x) {
                cout << " " << getCellChar(x, y) << " ";
                if (x < width - 1) {
                    cout << (vertWalls[y][x] ? "|" : " ");
                }
            }
            cout << "|\n";

            // Горизонтальные стены
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
        for (int x = 0; x < width; ++x) {
            cout << "+---";
        }
        cout << "+\n";
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

        // Запись размеров
        file << width << endl << height << endl;

        // Запись горизонтальных стен
        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width; ++x) {
                file << (horizWalls[y][x] ? '1' : '0');
                operation_count++;
            }
            file << endl;
        }

        // Запись вертикальных стен
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                file << (vertWalls[y][x] ? '1' : '0');
                operation_count++;
            }
            file << endl;
        }

        // Запись стартовой и конечной позиций
        file << start.first << " " << start.second << endl;
        file << end.first << " " << end.second << endl;
        operation_count += 2;

        cout << "Лабиринт сохранен в файл " << filename << endl;
        printMetrics("Сохранение в файл", start_time);
    }

    void editWall(int x, int y, char direction, bool isWall) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        bool success = false;
        switch (tolower(direction)) {
        case 'n': // север (верх)
            if (y > 0) {
                horizWalls[y - 1][x] = isWall;
                success = true;
            }
            break;
        case 's': // юг (низ)
            if (y < height - 1) {
                horizWalls[y][x] = isWall;
                success = true;
            }
            break;
        case 'w': // запад (лево)
            if (x > 0) {
                vertWalls[y][x - 1] = isWall;
                success = true;
            }
            break;
        case 'e': // восток (право)
            if (x < width - 1) {
                vertWalls[y][x] = isWall;
                success = true;
            }
            break;
        }

        if (success) {
            waveExecuted = false;
            cout << "Стена изменена успешно" << endl;
        }
        else {
            cerr << "Неверные координаты или направление!" << endl;
        }

        printMetrics("Редактирование стены", start_time);
    }

    void generateRandomWithPath(int w, int h) {
        resetMetrics();
        auto start_time = high_resolution_clock::now();

        width = w;
        height = h;
        cells.assign(height, vector<CellState>(width, EMPTY));
        horizWalls.assign(height - 1, vector<bool>(width, true));
        vertWalls.assign(height, vector<bool>(width - 1, true));
        distances.assign(height, vector<int>(width, -1));

        // Алгоритм Prim для генерации лабиринта с гарантированным путем
        vector<vector<bool>> visited(height, vector<bool>(width, false));
        priority_queue<pair<int, pair<int, int>>> walls;

        // Начинаем со случайной клетки
        int x = rand() % width;
        int y = rand() % height;
        visited[y][x] = true;

        // Добавляем соседние стены
        if (x > 0) walls.push({ rand(), {x - 1, y} });
        if (x < width - 1) walls.push({ rand(), {x + 1, y} });
        if (y > 0) walls.push({ rand(), {x, y - 1} });
        if (y < height - 1) walls.push({ rand(), {x, y + 1} });

        while (!walls.empty()) {
            auto wall = walls.top().second;
            walls.pop();
            x = wall.first;
            y = wall.second;

            vector<pair<int, int>> unvisited;
            if (x > 0 && !visited[y][x - 1]) unvisited.emplace_back(x - 1, y);
            if (x < width - 1 && !visited[y][x + 1]) unvisited.emplace_back(x + 1, y);
            if (y > 0 && !visited[y - 1][x]) unvisited.emplace_back(x, y - 1);
            if (y < height - 1 && !visited[y + 1][x]) unvisited.emplace_back(x, y + 1);

            if (!unvisited.empty()) {
                auto cell = unvisited[rand() % unvisited.size()];
                visited[cell.second][cell.first] = true;

                // Убираем стену
                if (cell.first == x - 1) vertWalls[y][x - 1] = false;
                else if (cell.first == x + 1) vertWalls[y][x] = false;
                else if (cell.second == y - 1) horizWalls[y - 1][x] = false;
                else if (cell.second == y + 1) horizWalls[y][x] = false;

                // Добавляем новые стены
                if (cell.first > 0 && !visited[cell.second][cell.first - 1])
                    walls.push({ rand(), {cell.first - 1, cell.second} });
                if (cell.first < width - 1 && !visited[cell.second][cell.first + 1])
                    walls.push({ rand(), {cell.first + 1, cell.second} });
                if (cell.second > 0 && !visited[cell.second - 1][cell.first])
                    walls.push({ rand(), {cell.first, cell.second - 1} });
                if (cell.second < height - 1 && !visited[cell.second + 1][cell.first])
                    walls.push({ rand(), {cell.first, cell.second + 1} });
            }
        }

        setStart(0, 0);
        setEnd(width - 1, height - 1);
        cout << "Лабиринт с гарантированным путем сгенерирован" << endl;
        printMetrics("Генерация лабиринта с путем", start_time);
    }
};

void showMenu() {
    cout << "\n=== Меню управления лабиринтом ===" << endl;
    cout << "1. Загрузить лабиринт из файла" << endl;
    cout << "2. Сгенерировать случайный лабиринт" << endl;
    cout << "3. Сгенерировать лабиринт с гарантированным путем" << endl;
    cout << "4. Установить стартовую точку" << endl;
    cout << "5. Установить конечную точку" << endl;
    cout << "6. Редактировать стену" << endl;
    cout << "7. Выполнить волновой алгоритм" << endl;
    cout << "8. Найти и показать путь" << endl;
    cout << "9. Сохранить лабиринт в файл" << endl;
    cout << "10. Показать лабиринт" << endl;
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
    char dir;
    int wallAction;

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
        case 6:
            cout << "Введите координаты (x y), направление (N/S/W/E) и действие (0-удалить, 1-добавить): ";
            cin >> x >> y >> dir >> wallAction;
            maze.editWall(x, y, dir, wallAction == 1);
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