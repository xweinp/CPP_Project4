#include "poly.h"
#include <cstddef>
#include <array>
#include <type_traits>

namespace {

    template <typename T, std::size_t N>
    constexpr bool operator==(const poly<T, N>& a, const poly<T, N>& b)
    {
        for (std::size_t i = 0; i < N; ++i)
        if (!(a[i] == b[i]))
            return false;
        return true;
    }

    void simple_test() {
        // testy konstruktora bezargumentowego
        constexpr auto zero = poly<int>();
        auto zero1 = poly<int>();
        auto zero2 = poly<int, 2>();

        // testy konstruktora wieloargumentowego
        constexpr auto p = poly(2, 1);
        static_assert(std::is_same_v<decltype(p), const poly<int, 2>>);
        static_assert(std::is_same_v<std::common_type_t<double, poly<int, 2>>, poly<double, 2>>);
        static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, poly<double, 2>>, poly<double, 2>>);
        
        constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
        static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);

        constexpr auto q2 = poly(1.0, 2.0);

        // testy operatora przypisania
        poly<int, 2> p1;
        constexpr poly<int, 2> p2;
        p1 = p2;
        zero1 = zero;

        // testy operatora indeksującego

        static_assert(q2[0] == 1.0);
        static_assert(q2[1] == 2.0);

        p1[0] = 1;
        static_assert(q[0][0] == 1.0);
        static_assert(q[0][1] == 2.0);
        static_assert(q[1][0] == 3.0);
        static_assert(q[1][1] == 0.0);
        static_assert(q[2][0] == 4.0);
        static_assert(q[2][1] == 0.0);

        // testy operatora przypisania i operatora indeksującego
        p1[0] = p2[0];
        zero2 = p;

        constexpr auto constant_poly = const_poly(p);
        p1 = constant_poly[0];

        // testy const_poly
        constexpr auto p3 = poly(1, 2, 3);
        static_assert(const_poly(p3)[0] == p3);
        
        // testy metody size
        static_assert(const_poly(p3).size() == 1);

        // testy konwersji typów
        static_assert(std::is_convertible_v<poly<int, 3>, poly<long long, 4>>);

        static_assert(std::is_same_v<std::common_type_t<poly<int, 3>, poly<long long, 4>>, poly<long long, 4>>);
        static_assert(std::is_convertible_v<poly <int, 3>, poly<long long, 4>>);

    }

    void test_static_without_at() {
        constexpr auto p = poly(2, 1);
        static_assert(std::is_same_v<decltype(p), const poly<int, 2>>);
        static_assert(p[0] == 2);
        static_assert(p.at(1) == 3);
        static_assert(p.at(0.5) == 2.5);

        static_assert(std::is_same_v<std::common_type_t<double, poly<int, 2>>, poly<double, 2>>);
        static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, poly<double, 2>>, poly<double, 2>>);

        constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
        static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);
        static_assert(q[0][0] == 1.0);
        static_assert(q[0][1] == 2.0);
        static_assert(q[1][0] == 3.0);
        static_assert(q[1][1] == 0.0);
        static_assert(q[2][0] == 4.0);
        static_assert(q[2][1] == 0.0);

        static_assert(std::is_same_v<decltype(p + q), poly<poly<double, 2>, 3>>);
        static_assert(p + q == poly(poly(3.0, 2.0), 4.0, 4.0));
        static_assert(-q == poly(poly(-1.0, -2.0), -3.0, -4.0));
        static_assert(p - q == poly(poly(1.0, -2.0), -2.0, -4.0));
        static_assert(q - p == poly(poly(-1.0, 2.0), 2.0, 4.0));
        
        //static_assert(std::is_same_v<decltype(p * q), poly<poly<double, 2>, 4>>);
        //static_assert(p * q == poly(poly(2.0, 4.0), poly(7.0, 2.0), 11.0, 4.0));
        
        static_assert(cross(p, q) == poly(poly(poly(2.0, 4.0), 6.0, 4.0), q));
        //static_assert(cross(p, q) == poly(2.0 * q, q));
        //static_assert(cross(q, p) == poly(poly(p, 2.0 * p), const_poly(3.0 * p), const_poly(4.0 * p)));

        constexpr auto pp(p);
        static_assert(p == pp);
        constexpr auto qq(q);
        static_assert(q == qq);
    }

/*
    void cross_test() {
        constexpr auto p = poly(2, 1);
        constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
        constexpr auto r = poly(poly(poly(2.0, 4.0), 6.0, 8.0), q);

        //static_assert(cross(p, q) == r);
    }
*/
}

int main() {
    simple_test();
    test_static_without_at();
}