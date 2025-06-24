#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>
#include <chrono>
#include <iomanip>

const double PI = 3.1415926535;
std::mutex consoleMutex;

struct Position {
    double x, y;
};

class WorkerAnt {
public:
    Position start;
    double speed;
    int id;

    WorkerAnt(Position s, double v, int i) : start(s), speed(v), id(i) {}

    void operator()() {
        Position current = start;
        Position target = {0, 0};
        double dx = target.x - current.x;
        double dy = target.y - current.y;
        double dist = std::sqrt(dx*dx + dy*dy);
        double stepX = dx / dist * speed;
        double stepY = dy / dist * speed;

        // Йде до [0,0]
        while (std::sqrt((current.x - target.x)*(current.x - target.x) +
                         (current.y - target.y)*(current.y - target.y)) > speed) {
            current.x += stepX;
            current.y += stepY;
            printPosition("Worker", id, current);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Назад
        stepX = -stepX;
        stepY = -stepY;
        while (std::sqrt((current.x - start.x)*(current.x - start.x) +
                         (current.y - start.y)*(current.y - start.y)) > speed) {
            current.x += stepX;
            current.y += stepY;
            printPosition("Worker", id, current);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        printPosition("Worker", id, start);
    }

    void printPosition(const std::string& type, int id, Position pos) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << type << " #" << id << " at (" << pos.x << ", " << pos.y << ")\n";
    }
};

class WarriorAnt {
public:
    double radius;
    double speed;
    int id;

    WarriorAnt(double r, double v, int i) : radius(r), speed(v), id(i) {}

    void operator()() {
        double angle = 0;
        double angularSpeed = speed / radius;

        for (int i = 0; i < 100; ++i) { // 100 ітерацій по колу
            Position pos;
            pos.x = radius * cos(angle);
            pos.y = radius * sin(angle);

            printPosition("Warrior", id, pos);
            angle += angularSpeed;
            if (angle > 2 * PI) angle -= 2 * PI;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void printPosition(const std::string& type, int id, Position pos) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << type << " #" << id << " at (" << pos.x << ", " << pos.y << ")\n";
    }
};

int main() {
    std::vector<std::thread> threads;

    // Створюємо 3 мурах-робочих
    threads.emplace_back(WorkerAnt({5, 5}, 0.5, 1));
    threads.emplace_back(WorkerAnt({-6, 4}, 0.4, 2));
    threads.emplace_back(WorkerAnt({3, -7}, 0.6, 3));

    // Створюємо 2 мурах-воїнів
    threads.emplace_back(WarriorAnt(5, 0.3, 1));
    threads.emplace_back(WarriorAnt(7, 0.4, 2));

    for (auto& t : threads)
        t.join();

    return 0;
}