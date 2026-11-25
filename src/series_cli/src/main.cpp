#include "lab2/series/series.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace ser = lab2::series;

void printHelp()
{
    std::cout << "\nДоступные команды:\n"
              << "  load N        — загрузить серию из N чисел (вводятся с новой строки)\n"
              << "  print         — вывести все числа\n"
              << "  stats         — минимум, максимум, среднее\n"
              << "  greater T     — количество элементов больше порога T\n"
              << "  shift D       — прибавить D ко всем элементам\n"
              << "  normalize     — нормализовать (x - mean) / stddev\n"
              << "  filter T      — заменить серию элементами > T\n"
              << "  help          — эта справка\n"
              << "  exit          — выйти из программы\n\n";
}
int main()
{
    ser::MeasurementSeries series{};
    series.data = nullptr;
    series.size = 0;

    bool isLoaded = false;

    std::cout << "=== series_cli (интерактивный режим) ===\n";
    std::cout << "Введите 'help' для списка команд.\n\n";

    std::string line;

    while (true)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, line))
            break;

        // убираем лишние пробелы в начале
        size_t start = 0;
        while (start < line.size() && line[start] == ' ')
            start++;
        line = line.substr(start);

        if (line.empty())
            continue;

        // Разбиваем строку на слова
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "help")
        {
            printHelp();
        }

        else if (command == "exit")
        {
            std::cout << "До свидания!\n";
            break;
        }

        else if (command == "load")
        {
            int n = 0;
            if (!(iss >> n) || n <= 0)
            {
                std::cerr << "Ошибка: укажите корректный размер N > 0\n";
                continue;
            }

            // Если серия уже была загружена — освобождаем старую память
            if (isLoaded)
                ser::destroySeries(series);

            series = ser::createSeries(n);

            std::cout << "Введите " << n << " чисел (через пробел или с новой строки):\n";

            if (ser::loadFromStdin(series))
            {
                isLoaded = true;
                std::cout << "Загружена серия из " << n << " элементов.\n";
            }
            else
            {
                ser::destroySeries(series);
                isLoaded = false;
            }
        }

        else if (command == "print")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            ser::printSeries(series);
        }

        else if (command == "stats")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            // setprecision(4) - установили точность 4 знака после запятой
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "min=" << ser::getMin(series) << " max=" << ser::getMax(series)
                      << " mean=" << ser::getMean(series) << " stddev=" << ser::getStdDev(series) << '\n';
        }

        else if (command == "greater")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            double threshold = 0.0;
            if (!(iss >> threshold))
            {
                std::cerr << "Ошибка: укажите пороговое значение T\n";
                continue;
            }

            const int count = ser::countGreater(series, threshold);
            std::cout << "Количество элементов > " << threshold << ": " << count << '\n';
        }
        else if (command == "shift")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            double delta = 0.0;
            if (!(iss >> delta))
            {
                std::cerr << "Ошибка: укажите значение сдвига D\n";
                continue;
            }

            ser::shift(series, delta);
            std::cout << "Сдвиг на " << delta << " выполнен.\n";
        }

        else if (command == "normalize")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            ser::normalize(series);
            std::cout << "Нормализация выполнена.\n";
        }

        else if (command == "filter")
        {
            if (!isLoaded)
            {
                std::cerr << "Ошибка: серия не загружена. Используйте 'load N'\n";
                continue;
            }

            double threshold = 0.0;
            if (!(iss >> threshold))
            {
                std::cerr << "Ошибка: укажите пороговое значение T\n";
                continue;
            }

            // Обмен сериями старая <-> отфильтрованная
            ser::MeasurementSeries filtered = ser::filter(series, threshold);
            ser::destroySeries(series);
            series = filtered;

            if (series.size == 0)
            {
                std::cout << "Нет элементов больше " << threshold << " — серия теперь пуста\n";
            }
            else
            {
                std::cout << "Серия обновлена (filter > " << threshold << "): ";
                ser::printSeries(series);
            }
        }

        // обрабатываем неизвестную команду
        else
        {
            std::cerr << "Неизвестная команда: '" << command << "'. Введите 'help'.\n";
        }
    }

    // Освобождаем память перед завершением программы
    if (isLoaded)
        ser::destroySeries(series);

     return 0;
}