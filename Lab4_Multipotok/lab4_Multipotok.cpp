#include <iostream> 
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include <random>

std::mutex mtx; // Мьютекс для управления доступом к общей очереди
std::condition_variable cv; // Условная переменная для синхронизации потоков
std::queue<int> queue; // Очередь для хранения производимых элементов
std::atomic<bool> running(true); // Атомарная переменная для обозначения состояния работы потоков
const size_t maxQueueSize = 10;

void producer(int id) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 100);

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(distribution(generator))); // Имитация задержки
        int item = distribution(generator);

        std::unique_lock<std::mutex> lock(mtx); // Заблокировать мьютекс для безопасного доступа к очереди
        cv.wait(lock, [] { return queue.size() < maxQueueSize; });

        queue.push(item);
        std::cout << "Producer " << id << " produced: " << item << "\n";
        lock.unlock();
        cv.notify_all();
    }
}

void consumer(int id) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !queue.empty(); });

        int item = queue.front();
        queue.pop();
        std::cout << "Consumer " << id << " consumed: " << item << "\n"; // Вывод сообщения о потреблении
        lock.unlock();
        cv.notify_all();
    }
}

int main() {
    int numProducers, numConsumers;

    std::cout << "Enter number of producers: ";
    std::cin >> numProducers;
    
    std::cout << "Enter number of consumers: ";
    std::cin >> numConsumers;

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < numProducers; ++i) {
        producers.push_back(std::thread(producer, i));
    }

    for (int i = 0; i < numConsumers; ++i) {
        consumers.push_back(std::thread(consumer, i));
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    running = false;

    for (auto& producer : producers) {
        producer.join(); // Ожидание завершения каждого потока производителя
    }

    for (auto& consumer : consumers) {
        consumer.join(); // Ожидание завершения каждого потока потребителя
    }

    std::cout << "Finished processing.\n";
}