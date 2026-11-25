// TODO: Your code
#include "lab2/series/series.hpp"
#include <cmath>
#include <iostream>
namespace lab2::series
{

MeasurementSeries createSeries(int n)
{
    MeasurementSeries s{}; // в {} можно передать начальные значения полей структуры
    s.size = n;
    s.data = new double[static_cast<size_t>(n)]; // выделяем память на n элементов
    return s;
}

void destroySeries(MeasurementSeries& s)
{
    delete[] s.data;  // освобождаем память
    s.data = nullptr; // обнуляем указатель, чтобы не было dangling pointer
    s.size = 0;
}

bool loadFromStdin(MeasurementSeries& s)
{
    for (int i = 0; i < s.size; i++)
    {
        if (!(std::cin >> s.data[i]))
        {
            std::cerr << "Ошибка: не удалось считать число #" << (i + 1) << "\n";
            return false;
        }
    }
    return true;
}

void printSeries(const MeasurementSeries& s)
{
    if (s.size == 0)
    {
        std::cout << "(пусто)\n";
        return;
    }

    for (int i = 0; i < s.size; i++)
    {
        if (i > 0) // для вывода серии без пробелов в начале/конце
            std::cout << ' ';
        std::cout << s.data[i];
    }
    std::cout << '\n';
}

double getMin(const MeasurementSeries& s)
{
    double minVal = s.data[0];
    for (int i = 1; i < s.size; i++)
    {
        if (s.data[i] < minVal)
            minVal = s.data[i];
    }
    return minVal;
}

double getMax(const MeasurementSeries& s)
{
    double maxVal = s.data[0];
    for (int i = 1; i < s.size; i++)
    {
        if (s.data[i] > maxVal)
            maxVal = s.data[i];
    }
    return maxVal;
}

double getMean(const MeasurementSeries& s)
{
    double sum = 0.0;
    for (int i = 0; i < s.size; i++)
        sum += s.data[i];
    return sum / s.size;
}

double getStdDev(const MeasurementSeries& s)
{
    double mean = getMean(s);
    double sumSq = 0.0;

    for (int i = 0; i < s.size; i++)
    {
        double diff = s.data[i] - mean;
        sumSq += diff * diff;
    }

    return std::sqrt(sumSq / s.size);
}

int countGreater(const MeasurementSeries& s, const double threshold)
{
    int count = 0;
    for (int i = 0; i < s.size; i++)
    {
        if (s.data[i] > threshold)
            count++;
    }
    return count;
}

void shift(MeasurementSeries& s, double delta)
{
    for (int i = 0; i < s.size; i++)
        s.data[i] += delta;
}

void normalize(MeasurementSeries& s)
{
    const double stddev = getStdDev(s);

    if (stddev == 0.0)
    {
        std::cerr << "Предупреждение: stddev = 0, нормализация невозможна\n";
        return;
    }

    const double mean = getMean(s);

    for (int i = 0; i < s.size; i++)
        s.data[i] = (s.data[i] - mean) / stddev;
}

MeasurementSeries filter(const MeasurementSeries& s, double threshold)
{
    // Сначала считаем, сколько элементов пройдут фильтр
    const int count = countGreater(s, threshold);

    MeasurementSeries result = createSeries(count);

    // Заполняем новую серию подходящими элементами
    int indx = 0;
    for (int i = 0; i < s.size; i++)
    {
        if (s.data[i] > threshold)
        {
            result.data[indx] = s.data[i];
            indx++;
        }
    }

    return result;
}

} // namespace lab2::series