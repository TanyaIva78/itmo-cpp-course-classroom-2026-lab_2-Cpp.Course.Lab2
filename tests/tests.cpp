
#include "lab2/series/series.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace ser = lab2::series;

// вместо == используем near
static bool near(double a, double b, double eps = 1e-9)
{
    return std::fabs(a - b) < eps;
}

TEST_CASE("createSeries выделяет память нужного размера", "[1]")
{
    ser::MeasurementSeries s = ser::createSeries(5);

    REQUIRE(s.size == 5);
    REQUIRE(s.data != nullptr);

    ser::destroySeries(s);

    REQUIRE(s.data == nullptr);
    REQUIRE(s.size == 0);
}

TEST_CASE("createSeries работает для одного элемента", "[2]")
{
    ser::MeasurementSeries s = ser::createSeries(1);

    REQUIRE(s.size == 1);
    REQUIRE(s.data != nullptr);

    s.data[0] = 42.0;
    REQUIRE(near(s.data[0], 42.0));

    ser::destroySeries(s);
}

TEST_CASE("getMin, getMax, getMean возвращают верные значения", "[3]")
{
    ser::MeasurementSeries s = ser::createSeries(5);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;
    s.data[3] = 4.0;
    s.data[4] = 5.0;

    REQUIRE(near(ser::getMin(s), 1.0));
    REQUIRE(near(ser::getMax(s), 5.0));
    REQUIRE(near(ser::getMean(s), 3.0));

    // Данные НЕ изменились (функции stats не модифицируют серию)
    REQUIRE(near(s.data[0], 1.0));
    REQUIRE(near(s.data[4], 5.0));

    ser::destroySeries(s);
}

TEST_CASE("getStdDev возвращает верное стандартное отклонение", "[4]")
{
    ser::MeasurementSeries s = ser::createSeries(8);
    double values[] = {2, 4, 4, 4, 5, 5, 7, 9};
    for (int i = 0; i < 8; i++)
        s.data[i] = values[i];

    REQUIRE(near(ser::getMean(s), 5.0));
    REQUIRE(near(ser::getStdDev(s), 2.0));

    ser::destroySeries(s);
}

TEST_CASE("getStdDev не модифицирует исходную серию", "[5]")
{
    ser::MeasurementSeries s = ser::createSeries(3);
    s.data[0] = 10.0;
    s.data[1] = 20.0;
    s.data[2] = 30.0;

    ser::getStdDev(s);

    REQUIRE(near(s.data[0], 10.0));
    REQUIRE(near(s.data[1], 20.0));
    REQUIRE(near(s.data[2], 30.0));

    ser::destroySeries(s);
}

TEST_CASE("countGreater возвращает верный счётчик и не трогает данные", "[6]")
{
    ser::MeasurementSeries s = ser::createSeries(5);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;
    s.data[3] = 4.0;
    s.data[4] = 5.0;

    REQUIRE(ser::countGreater(s, 3.0) == 2);
    REQUIRE(ser::countGreater(s, 0.0) == 5);
    REQUIRE(ser::countGreater(s, 5.0) == 0);

    REQUIRE(near(s.data[0], 1.0));
    REQUIRE(near(s.data[4], 5.0));

    ser::destroySeries(s);
}

TEST_CASE("shift прибавляет дельту ко всем элементам", "[7]")
{
    ser::MeasurementSeries s = ser::createSeries(3);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;

    ser::shift(s, -1.0);
    REQUIRE(near(s.data[0], 0.0));
    REQUIRE(near(s.data[1], 1.0));
    REQUIRE(near(s.data[2], 2.0));

    ser::shift(s, 10.0);
    REQUIRE(near(s.data[0], 10.0));
    REQUIRE(near(s.data[1], 11.0));
    REQUIRE(near(s.data[2], 12.0));

    ser::destroySeries(s);
}

TEST_CASE("normalize изменяет серию так что mean=0 и stddev=1", "[8]")
{
    ser::MeasurementSeries s = ser::createSeries(5);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;
    s.data[3] = 4.0;
    s.data[4] = 5.0;

    // До нормализации mean не равен нулю
    REQUIRE_FALSE(near(ser::getMean(s), 0.0));

    ser::normalize(s);

    // После — mean=0 и stddev=1
    REQUIRE(near(ser::getMean(s), 0.0));
    REQUIRE(near(ser::getStdDev(s), 1.0));

    ser::destroySeries(s);
}

TEST_CASE("normalize при всех одинаковых элементах", "[9]")
{
    ser::MeasurementSeries s = ser::createSeries(3);
    s.data[0] = 7.0;
    s.data[1] = 7.0;
    s.data[2] = 7.0;

    ser::normalize(s);

    REQUIRE(near(s.data[0], 7.0));
    REQUIRE(near(s.data[1], 7.0));
    REQUIRE(near(s.data[2], 7.0));

    ser::destroySeries(s);
}

TEST_CASE("filter возвращает новую серию только с элементами больше порога", "[10]")
{
    ser::MeasurementSeries s = ser::createSeries(5);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;
    s.data[3] = 4.0;
    s.data[4] = 5.0;

    ser::MeasurementSeries f = ser::filter(s, 3.0);
    REQUIRE(f.size == 2);
    REQUIRE(near(f.data[0], 4.0));
    REQUIRE(near(f.data[1], 5.0));

    // Исходная серия не изменилась
    REQUIRE(s.size == 5);
    REQUIRE(near(s.data[0], 1.0));

    ser::destroySeries(f);
    ser::destroySeries(s);
}

TEST_CASE("filter возвращает пустую серию если ничего не прошло фильтр", "[11]")
{
    ser::MeasurementSeries s = ser::createSeries(3);
    s.data[0] = 1.0;
    s.data[1] = 2.0;
    s.data[2] = 3.0;

    ser::MeasurementSeries f = ser::filter(s, 239.0);

    REQUIRE(f.size == 0);

    ser::destroySeries(f);
    ser::destroySeries(s);
}