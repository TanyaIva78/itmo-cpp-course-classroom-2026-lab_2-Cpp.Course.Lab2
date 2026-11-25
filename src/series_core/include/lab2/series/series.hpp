#pragma once

#include <cstddef>

namespace lab2::series
{
struct MeasurementSeries
{
    double* data; // указатель на массив данных
    int size;     // количество элементов
};

// Создать серию на N элементов (выделить память)
MeasurementSeries createSeries(int n);

// Освободить память серии
void destroySeries(MeasurementSeries& s);

// Считать N чисел из stdin в серию (возвращает true если всё считалось успешно)
bool loadFromStdin(MeasurementSeries& s);

// Вывести все элементы в одну строку через пробел
void printSeries(const MeasurementSeries& s);

// Анализ серии
double getMin(const MeasurementSeries& s);
double getMax(const MeasurementSeries& s);
double getMean(const MeasurementSeries& s);
double getStdDev(const MeasurementSeries& s);

// Количество элементов строго больше порога
int countGreater(const MeasurementSeries& s, double threshold);

// Прибавить delta к каждому элементу
void shift(MeasurementSeries& s, double delta);

// Нормализация: x = (x - mean) / stddev
// Если stddev == 0 — ничего не делает
void normalize(MeasurementSeries& s);

// Вернуть новую серию только с элементами > threshold
MeasurementSeries filter(const MeasurementSeries& s, double threshold);

} // namespace lab2::series