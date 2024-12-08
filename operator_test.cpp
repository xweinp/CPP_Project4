#include "poly.h"

template <typename T, std::size_t N>
constexpr bool operator==(const poly<T, N> &a, const poly<T, N> &b) {
    for (std::size_t i = 0; i < N; ++i)
        if (!(a[i] == b[i]))
            return false;
    return true;
}

int main() {
    // Basic operations with different types
    constexpr auto p1 = poly<int, 3>(3, 2, 1);
    constexpr auto p2 = poly<long long, 4>(1, 2, 3, 4);
    constexpr auto p3 = poly<double, 2>(1.5, 2.5);
    
    // Addition tests
    static_assert(p1 + p2 == poly<long long, 4>(4, 4, 4, 4));
    static_assert(p1 + 2 == poly<int, 3>(5, 2, 1));
    static_assert(2 + p1 == poly<int, 3>(5, 2, 1));
    static_assert(p1 + p3 == poly<double, 3>(4.5, 4.5, 1.0));

    // Subtraction tests
    static_assert(p1 - p2 == poly<long long, 4>(2, 0, -2, -4));
    static_assert(p1 - 2 == poly<int, 3>(1, 2, 1));
    static_assert(2 - p1 == poly<int, 3>(-1, -2, -1));
    static_assert(p1 - p3 == poly<double, 3>(1.5, -0.5, 1.0));

    // Multiplication tests
    static_assert(p1 * p2 == poly<long long, 6>(3, 8, 14, 20, 11, 4));
    static_assert(p1 * 2 == poly<int, 3>(6, 4, 2));
    static_assert(2 * p1 == poly<int, 3>(6, 4, 2));
    static_assert(p1 * p3 == poly<double, 4>(4.5, 10.5, 6.5, 2.5));

    // Unary minus test
    static_assert(-p1 == poly<int, 3>(-3, -2, -1));
    static_assert(-p2 == poly<long long, 4>(-1, -2, -3, -4));
    
    // Nested polynomial tests
    constexpr auto np1 = poly<poly<int, 2>, 2>(poly<int, 2>(1, 2), poly<int, 2>(3, 4));
    constexpr auto np2 = poly<poly<double, 2>, 2>(poly<double, 2>(1.5, 2.5), poly<double, 2>(3.5, 4.5));
    
    static_assert((np1 + np2)[0] == poly<double, 2>(2.5, 4.5));
    static_assert((np1 - np2)[1] == poly<double, 2>(-0.5, -0.5));
    static_assert((np1 * 2)[0] == poly<int, 2>(2, 4));
    
    // Triple-nested polynomial tests
    constexpr auto tnp1 = poly<poly<poly<int, 2>, 2>, 2>(
        poly<poly<int, 2>, 2>(
            poly<int, 2>(1, 2),
            poly<int, 2>(3, 4)
        ),
        poly<poly<int, 2>, 2>(
            poly<int, 2>(5, 6),
            poly<int, 2>(7, 8)
        )
    );
    
    static_assert((-tnp1)[0][0] == poly<int, 2>(-1, -2));
    static_assert((tnp1 * 2)[1][1] == poly<int, 2>(14, 16));
    
    // Compound assignment tests
    constexpr auto compound_test = []() {
        poly<int, 3> result(1, 1, 1);
        result += poly<int, 2>(2, 2);
        result -= poly<int, 2>(1, 1);
        result *= 2;
        return result;
    };
    static_assert(compound_test() == poly<int, 3>(4, 4, 2));
    
    // Empty polynomial tests
    constexpr auto empty1 = poly<int, 0>();
    constexpr auto empty2 = poly<double, 0>();
    static_assert((empty1 + empty2) == poly<double, 0>());
    static_assert((empty1 * empty2) == poly<double, 0>());
    
    // Const correctness tests
    const constexpr auto const_p = poly<int, 2>(1, 2);
    constexpr auto non_const_p = poly<int, 2>(3, 4);
    static_assert((const_p + non_const_p) == poly<int, 2>(4, 6));
    static_assert((const_p * non_const_p) == poly<int, 3>(3, 10, 8));

    // Different nestedness multiplication tests
    constexpr auto single_p = poly<int, 2>(1, 2);
    constexpr auto nested_p = poly<poly<int, 2>, 2>(poly<int, 2>(1, 2), poly<int, 2>(3, 4));
    constexpr auto double_nested_p = poly<poly<poly<int, 2>, 2>, 1>(
        poly<poly<int, 2>, 2>(poly<int, 2>(1, 2), poly<int, 2>(3, 4))
    );

    // Test single * nested
    static_assert((single_p * nested_p)[0] == poly<int, 2>(1, 2));
    static_assert((single_p * nested_p)[1] == poly<int, 2>(5, 8));
    static_assert((nested_p * single_p)[0] == poly<int, 2>(1, 2));

    // Test nested * double nested
    static_assert((nested_p * double_nested_p)[0][0] == poly<int, 2>(1, 2));
    static_assert((nested_p * double_nested_p)[0][1] == poly<int, 2>(5, 8));

    // Test with different base types
    constexpr auto double_p = poly<double, 2>(1.5, 2.5);
    constexpr auto nested_double_p = poly<poly<double, 2>, 2>(
        poly<double, 2>(1.5, 2.5), 
        poly<double, 2>(3.5, 4.5)
    );

    static_assert((single_p * nested_double_p)[0] == poly<double, 2>(1.5, 2.5));
    static_assert((nested_p * double_p)[0] == poly<double, 2>(1.5, 3.0));
}
