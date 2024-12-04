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

        // testy operatora przypisania
        poly<int, 2> p1;
        constexpr poly<int, 2> p2;
        p1 = p2;
        zero1 = zero;

        // testy operatora indeksującego
        p1[0] = 1;

        // testy operatora przypisania i operatora indeksującego
        p1[0] = p2[0];
        zero2 = p;

        constexpr auto constant_poly = const_poly(p);
        p1 = constant_poly[0];
    }

}

int main() {
    simple_test();
}