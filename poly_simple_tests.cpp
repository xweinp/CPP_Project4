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
        constexpr auto zero = poly<int>();

        constexpr auto p = poly(2, 1);
        static_assert(std::is_same_v<decltype(p), const poly<int, 2>>);
        static_assert(std::is_same_v<std::common_type_t<double, poly<int, 2>>, poly<double, 2>>);
        static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, poly<double, 2>>, poly<double, 2>>);
        
        constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
        static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);

        //constexpr auto constant_poly = const_poly(p);
    }

}

int main() {
    simple_test();
}