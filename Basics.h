#pragma once
#include <iostream>
#include <random>

class RandInt {
    int max_num, min_num, rand_int, seed;
public:
    RandInt(int _max = 1, int _min = 0, int _seed = 1) :max_num(_max), min_num(_min), rand_int(1), seed(_seed) {};
    int generate() {
        rand_int = (rand() + seed) % max_num + 1;
        if (rand_int < min_num) {
            return generate();
        }
        return rand_int;
    }
    double uniform(double a, double b) {
        return a + generate() * (b - a) / max_num;
    }
};

class IntPair {
public:
    static inline int w;
    int x, y;
    IntPair(int x = -1, int y = -1) : x(x), y(y) {};

    bool operator == (const IntPair& a) const {
        return a.x == x && a.y == y;
    }
    bool operator != (const IntPair& a) const {
        return (a.x != x) && (a.y != y);
    }
    bool operator>(const IntPair& a) const {
        return (x + y * w) > (a.x + a.y * w);
    }
    bool operator<(const IntPair& a) const {
        return (x + y * w) < (a.x + a.y * w);
    }
};


class Point {
public:
    int x, y, height;
    Point(int x = 0.0, int y = 0.0, int height = 0.0) : x(x), y(y), height(height) {};
    Point(const Point& a) {
        x = a.x;
        y = a.y;
        height = a.height;
    }
    Point operator = (const Point& a) {
        x = a.x;
        y = a.y;
        height = a.height;
        return *this;
    }
    friend bool operator == (const Point& a, const Point& b) {
        return a.x == b.x && a.y == b.y && a.height == b.height;
    }
    friend bool operator != (const Point& a, const Point& b) {
        return !(a == b);
    }
    friend bool operator > (const Point& a, const Point& b) {
        return a.height > b.height;
    }
    friend bool operator >= (const Point& a, const Point& b) {
        return a.height >= b.height;
    }
    friend bool operator < (const Point& a, const Point& b) {
        return !(a.height >= b.height);
    }
    friend bool operator <= (const Point& a, const Point& b) {
        return !(a.height > b.height);
    }
};

class Percent {
public:
    double data;

    Percent(double _data = 0.0) : data(_data) {};
    Percent(double _data, double _divider) : data((_data / _divider) * 100) {};
    Percent(const Percent& a) {
        data = a.data;
    }

    friend double operator * (double num, const Percent& p) {
        return num * (p.data / 100);
    }

    friend double operator + (double num, const Percent& p) {
        return num + num * p;
    }
    friend double operator - (double num, const Percent& p) {
        return num - num * p;
    }

    friend Percent operator + (const Percent& a, const Percent& b) {
        return Percent(a.data + b.data);
    }

    friend Percent operator - (const Percent& a, const Percent& b) {
        return Percent(a.data - b.data);
    }

    friend Percent operator - (const Percent& a) {
        return Percent(-a.data);
    }

    friend Percent operator += (Percent& a, const Percent& b) {
        a = a + b;
        return a;
    }

    friend Percent operator -= (Percent& a, const Percent& b) {
        a = a - b;
        return a;
    }

    friend bool operator == (const Percent& a, const Percent& b) {
        return a.data == b.data;
    }
    friend bool operator != (const Percent& a, const Percent& b) {
        return a.data != b.data;
    }
    friend bool operator > (const Percent& a, const Percent& b) {
        return a.data > b.data;
    }
    friend bool operator < (const Percent& a, const Percent& b) {
        return a.data < b.data;
    }
    friend bool operator >= (const Percent& a, const Percent& b) {
        return a.data >= b.data;
    }
    friend bool operator <= (const Percent& a, const Percent& b) {
        return a.data <= b.data;
    }

    Percent operator = (const Percent& b) {
        data = b.data;
        return *this;
    }

    Percent operator = (const double& b) {
        data = b;
        return *this;
    }

    friend std::ostream& operator << (std::ostream& stream, const Percent& a) {
        stream << a.data;
        return stream;
    }

    friend std::istream& operator >> (std::istream& stream, Percent& a) {
        stream >> a.data;
        return stream;
    }

};

class PhysicalPercent : public Percent {
    void Normalization() {
        if (data < 0.0) {
            data = 0;
        }
        else if (data > 100.0) {
            data = 100;
        }
    }
public:
    PhysicalPercent(double _data = 0.0) : Percent(_data) {
        Normalization();
    };

    PhysicalPercent(double _data, double _divider) : Percent(_data, _divider) {
        Normalization();
    };

    PhysicalPercent(const PhysicalPercent& a) : Percent(a) {
        Normalization();
    }

    friend PhysicalPercent operator + (const PhysicalPercent& a, const PhysicalPercent& b) {
        double data = a.data + b.data;
        return PhysicalPercent(data);
    }
    friend PhysicalPercent operator - (const PhysicalPercent& a, const PhysicalPercent& b) {
        double data = a.data - b.data;
        return PhysicalPercent(data);
    }

    friend PhysicalPercent operator += (PhysicalPercent& a, const PhysicalPercent& b) {
        a = a + b;
        return a;
    }

    friend PhysicalPercent operator -= (PhysicalPercent& a, const PhysicalPercent& b) {
        a = a - b;
        return a;
    }

    PhysicalPercent operator = (const PhysicalPercent& b) {
        data = b.data;
        Normalization();
        return *this;
    }

    PhysicalPercent operator = (const double& b) {
        data = b;
        Normalization();
        return *this;
    }
};