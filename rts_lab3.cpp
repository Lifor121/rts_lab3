#include <iostream>
#include <thread>
#include <mutex>
#include <string>


std::mutex m;
int coins = 101;
int Bob_coins = 0;
int Tom_coins = 0;

void coin_sharing(std::string name, int& my_coins, int& other_coins)
{
    while (true)
    {
        m.lock();

        // Если монет не осталось
        if (coins == 0) {
            m.unlock();
            break;
        }

        // "Последняя монета должна достаться покойнику" 
        if (coins == 1 and my_coins == other_coins) {
            m.unlock();
            break;
        }

        // "В цикле вор берет себе монету, пока... у вора равно или меньше монет, чем у подельника" 
        if (my_coins > other_coins) {
            m.unlock();
            // Даем шанс другому потоку захватить мьютекс
            std::this_thread::yield();
            continue;
        }

        coins--;     // Берем из кучи
        my_coins++;  // Кладем себе

        // Вывод состояния
        std::cout << name << " took 1. Me: " << my_coins
            << ", Companion: " << other_coins
            << " | Coins left: " << coins << std::endl;

        m.unlock(); // Освобождаем мьютекс

        // Небольшая пауза, чтобы потоки не "жадничали" мьютекс слишком агрессивно
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    std::thread t1(coin_sharing, "Bob", std::ref(Bob_coins), std::ref(Tom_coins));

    std::thread t2(coin_sharing, "Tom", std::ref(Tom_coins), std::ref(Bob_coins));

    t1.join();
    t2.join();

    std::cout << "\n--- Result ---" << std::endl;
    std::cout << "Bob coins: " << Bob_coins << std::endl;
    std::cout << "Tom coins: " << Tom_coins << std::endl;
    std::cout << "Coins left (Dead Man's): " << coins << std::endl;
    std::cout << "Total sum: " << Bob_coins + Tom_coins + coins << std::endl; // Должно быть 101
}
