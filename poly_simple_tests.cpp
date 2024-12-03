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
        constexpr auto p = poly(2, 1);
    }

}

int main() {
    simple_test();
}