#include <cassert>
#include <complex>
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "poly.h"

namespace RingWrappers {
template <typename T>
class Simple {
private:
    template <typename U>
    friend class Simple;
    T value;

public:
    constexpr Simple() : value(T()) {}

    constexpr Simple(const T& val) : value(val) {}

    constexpr Simple(const Simple& other) : value(other.value) {}

    constexpr Simple& operator=(const Simple& other) {
        value = other.value;
        return *this;
    }

    constexpr Simple& operator=(Simple&& other) {
        value = std::move(other.value);
        return *this;
    }

    constexpr Simple(Simple&& other) : value(std::move(other.value)) {}

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr Simple(const U& u) : value(static_cast<T>(u)) {}

    template <typename U>
        requires std::is_convertible_v<U, T>
    constexpr Simple(const Simple<U>& u) : value(static_cast<T>(u.value)) {}

    constexpr Simple& operator+=(const Simple& other) {
        value += other.value;
        return *this;
    }

    constexpr Simple& operator-=(const Simple& other) {
        value -= other.value;
        return *this;
    }

    constexpr Simple& operator*=(const Simple& other) {
        value *= other.value;
        return *this;
    }

    constexpr Simple operator+(const Simple& other) const {
        return Simple(this->value + other.value);
    }

    constexpr Simple operator-(const Simple& other) const {
        return Simple(this->value - other.value);
    }

    constexpr Simple operator*(const Simple& other) const {
        return Simple(this->value * other.value);
    }

    constexpr Simple operator-() const {
        return Simple(-value);
    }

    // optional, do testów
    constexpr bool operator==(const Simple& other) const {
        return value == other.value;
    }

    std::ostream& operator<<(std::ostream& os) const {
        os << value;
        return os;
    }
};
}  // namespace RingWrappers

// w teście ring_wrappers poniższe jest potrzebne wyłącznie dla konstruktorów
// wieloargumentowych
template <>
struct std::common_type<RingWrappers::Simple<int>, double> {
    using type = RingWrappers::Simple<double>;
};

template <>
struct std::common_type<double, RingWrappers::Simple<int>> {
    using type = RingWrappers::Simple<double>;
};

template <typename T, std::size_t N>
constexpr bool operator==(const poly<T, N>& a, const poly<T, N>& b) {
    for (std::size_t i = 0; i < N; ++i) {
        if (!(a[i] == b[i])) {
            return false;
        }
    }
    return true;
}

template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const poly<T, N>& p) {
    os << "poly(";
    for (std::size_t i = 0; i < N; ++i) {
        os << p[i] << ", ";
    }
    os << ")";
    return os;
}

void constructor_tests() {
    // Test 1: Konstruktor bezargumentowy

    constexpr poly<poly<int, 3>, 2> pp;
    constexpr poly<poly<long long, 3>, 2> ppp(pp);
    constexpr poly<poly<long long, 3>, 2> pppp = pp;

    constexpr poly<int, 5> p;
    constexpr poly<int, 5> p2(p);
    static_assert(p == p2);

    // OPCJONALNE - std::string bynajmniej nie jest pierścieniem.
    // std::string ma poprawny move ctor który zeruje wartość przenoszonego
    // obiektu
    poly<std::string, 5> p4("a", "b", "c", "d", "e");
    poly p5(std::move(p4));
    assert(p4 != p5);

    constexpr poly<int, 1> p6(42);
    constexpr poly<double, 1> p7(42ll);

    static_assert(p7 == poly<double, 1>{42.0});

    //TODO constexpr poly<double, 5> p8(p6);

    //TODO static_assert(poly<double, 5>(p7) == p8);

    constexpr poly<int, 3> p9(1, 2, 3);
    constexpr poly<int, 5> p10(1, 2, 3);

    //TODO static_assert((poly<int, 5>)p9 == p10);

    constexpr poly<double, 3> p11(1.0, 2.0, 3.0);

    //TODO cconstexpr poly<int, 5> p12(p11);  // Konwersja double -> int

    constexpr poly<double, 3> p13(4.0, 5.0, 6.0);
    //TODO constexpr poly<int, 5> p14(std::move(p11));

    constexpr poly<int, 3> inner_poly(1, 2, 3);
    constexpr auto constant_poly = const_poly(inner_poly);

    constexpr poly<poly<int, 2>, 3> p15(poly<int, 2>(1, 2), poly<int, 2>(3, 4),
                                        poly<int, 2>(5, 6));
    //TODO constexpr poly<poly<int, 2>, 3> p16(poly<int, 1>(1), poly<int, 2>(3, 4),
    //TODO                                     poly<int, 2>(5, 6));

    (void)constant_poly;
    (void)ppp;
    (void)pppp;
    (void)p2;  // Prevent unused variable warning xD
    (void)p4;
    (void)p5;
    (void)p6;
    (void)p7;
    //TODO (void)p8;
    (void)p9;
    (void)p10;
    (void)p11;
    //TODO (void)p12;
    (void)p13;
    //TODO (void)p14;
    (void)p15;
    //TODO (void)p16;
}

void assigment_test() {
    // Deduction guide test
    poly p1(1, 2, 3);  // Deduction should infer poly<int, 3>
    static_assert(std::is_same_v<decltype(p1), poly<int, 3>>);

    poly p2(1.0, 2.0, 3.0);  // Deduction should infer poly<double, 3>
    static_assert(std::is_same_v<decltype(p2), poly<double, 3>>);

    // Copy assignment with conversion
    poly<int, 5> p3;
    poly<double, 3> p4(1.1, 2.2, 3.3);
    p3 = p4;                                // Conversion from double -> int
    assert((p3 == poly<int, 5>(1, 2, 3)));  // Values truncated to integers

    // Move assignment with conversion
    poly<int, 5> p5;
    p5 = std::move(p4);                     // p4 is moved, conversion applied
    assert((p5 == poly<int, 5>(1, 2, 3)));  // p4 is now in a moved-from state

    // Nested poly conversion
    poly<poly<int, 2>, 3> p7(poly<int, 2>(1, 2), poly<int, 2>(3, 4),
                             poly<int, 2>(5, 6));
    poly<poly<double, 2>, 3> p8 =
        p7;  // Conversion from int -> double in nested poly
    assert((p8 == poly<poly<double, 2>, 3>(poly<double, 2>(1.0, 2.0),
                                           poly<double, 2>(3.0, 4.0),
                                           poly<double, 2>(5.0, 6.0))));

    // Testing resizing via assignment
    poly<int, 3> p9(1, 2, 3);
    poly<int, 5> p10;
    p10 = p9;  // p10 should take the values of p9 and pad with default values
    assert((p10 == poly<int, 5>(1, 2, 3, 0, 0)));

    // Large poly to smaller poly assignment
    poly<int, 5> p11(1, 2, 3, 0, 0);
    poly<int, 5> p12;
    p12 = p11;  // Only the first 3 elements should be assigned
    assert((p12 == poly<int, 5>(1, 2, 3)));

    // Test type conversion during deduction
    poly p13 = poly<double, 3>(1.1, 2.2, 3.3);  // Deduction guides in play
    static_assert(std::is_same_v<decltype(p13), poly<double, 3>>);
    assert((p13 != poly<double, 3>(1, 2, 3)));  // Values truncated

    // Additional move and copy assignments
    poly<int, 3> p14(7, 8, 9);
    poly<int, 5> p15;
    p15 = std::move(p14);  // Move assignment
    assert((p15 == poly<int, 5>(7, 8, 9, 0, 0)));
}

void operator_tests() {
    // Test operator +=
    poly<int, 3> p1(1, 2, 3);
    poly<int, 3> p2(4, 5, 6);
    p1 += p2;
    assert((p1 == poly<int, 3>(5, 7, 9)));  // Element-wise addition

    poly<int, 3> p3(1, 2, 3);
    p3 += 10;  // Adding scalar
    assert((p3 == poly<int, 3>(11, 2, 3)));

    // Test operator -=
    poly<int, 3> p4(10, 20, 30);
    poly<int, 3> p5(5, 10, 15);
    p4 -= p5;
    assert((p4 == poly<int, 3>(5, 10, 15)));  // Element-wise subtraction

    poly<int, 3> p6(10, 20, 30);
    p6 -= 10;  // Subtracting scalar
    assert((p6 == poly<int, 3>(0, 20, 30)));

    // Test operator *=
    poly<int, 3> p7(1, 2, 3);
    p7 *= 2;  // Multiplying by scalar
    assert((p7 == poly<int, 3>(2, 4, 6)));

    // Test binary operators +
    poly<int, 3> p10(1, 2, 3);
    poly<int, 3> p11(4, 5, 6);
    auto p12 = p10 + p11;
    assert((p12 == poly<int, 3>(5, 7, 9)));

    auto p13 = p10 + 10;
    assert((p13 == poly<int, 3>(11, 2, 3)));

    auto p14 = 10 + p10;
    assert(p14 == p13);

    // Test binary operators -
    poly<int, 3> p15(10, 20, 30);
    poly<int, 3> p16(1, 2, 3);
    auto p17 = p15 - p16;
    assert((p17 == poly<int, 3>(9, 18, 27)));

    auto p18 = p15 - 10;
    assert((p18 == poly<int, 3>(0, 20, 30)));

    auto p19 = 40 - p16;
    assert((p19 == poly<int, 3>(39, -2, -3)));

    // Test unary operator -
    auto p20 = -p16;
    assert((p20 == poly<int, 3>(-1, -2, -3)));

    // Test binary operator *
    poly<int, 2> p21(1, 2);
    poly<int, 2> p22(3, 4);
    auto p23 = p21 * p22;
    assert((p23 == poly<int, 3>(3, 10, 8)));

    auto p24 = p21 * 3;
    assert((p24 == poly<int, 2>(3, 6)));

    auto p25 = 3 * p21;
    assert((p25 == poly<int, 2>(3, 6)));

    // Mixed type operations with deduced result type
    poly<int, 2> p26(1, 2);
    poly<double, 2> p27(0.5, 1.5);
    auto p28 = p26 + p27;
    static_assert((std::is_same_v<decltype(p28), poly<double, 2>>));
    assert((p28 == poly<double, 2>(1.5, 3.5)));

    auto p29 = p26 * p27;
    static_assert((std::is_same_v<decltype(p29), poly<double, 3>>));
    assert((p29 == poly<double, 3>(0.5, 2.5, 3.0)));

    // Wielomiany zerowe
    poly<int, 0> p30;
    assert(p30 + p29 == p29);
    assert(p29 + p30 == p29);
    assert(p30 - p29 == -p29);
    assert(p29 - p30 == p29);
    static_assert(std::is_same_v<decltype(p29 * p30), poly<double, 0>>);
    static_assert(std::is_same_v<decltype(p30 * p29), poly<double, 0>>);
}



constexpr void nested_poly() {
    static_assert(
        std::is_same_v<decltype(poly(poly(1, 2), poly(1.0, 2.0, 3.0, 4.0, 5))),
                       poly<poly<double, 5>, 2>>);
    static_assert(std::is_same_v<decltype(poly(poly(1, 2), poly(3, 4)) * 2.0),
                                 poly<poly<double, 2>, 2>>);
    static_assert(poly(poly(poly(1, 2.0), 3LL)) + 1 ==
                  poly(poly(poly(2.0, 2.0), poly(3.0, 0.0))));
    static_assert(
        std::is_same_v<std::common_type_t<double, poly<poly<int, 2>, 5>>,
                       poly<poly<double, 2>, 5>>);
    static_assert(std::is_same_v<
                  std::common_type_t<poly<double, 1>,
                                     poly<poly<poly<poly<int, 2>, 1>, 1>, 1>>,
                  poly<poly<poly<poly<double, 2>, 1>, 1>, 1>>);
    static_assert(
        std::is_same_v<
            std::common_type_t<poly<int, 2>,
                               poly<poly<poly<poly<double, 1>, 2>, 1>, 1>>,
            poly<poly<poly<poly<double, 1>, 2>, 1>, 2>>);
    // static_assert(poly(poly(2.0), poly(3.0, 4.0)) + poly(1, 2, 3) ==
    //               poly(poly(3.0, 0.0), poly(5.0, 4.0), poly(3.0, 0.0)));
    static_assert(std::is_same_v<decltype(poly(poly(2.0), poly(3.0, 4.0)) +
                                          poly(1, 2, 3)),
                                 poly<poly<double, 2>, 3>>);
    static_assert(
        std::is_same_v<decltype(const_poly(poly(1.0, 2.0)) + poly(5, 10)),
                       poly<poly<double, 2>, 2>>);
    static_assert(
        const_poly(const_poly(const_poly(poly(1)))) + poly(1, 4) ==
        poly(const_poly(const_poly(poly(2))), const_poly(const_poly(poly(4)))));
}

constexpr void ring_wrappers() {
    using RingWrappers::Simple;
    // operatory i konwersje
    static_assert(poly(Simple<int>(2)) + poly(Simple<int>(3)) ==
                  poly(Simple<int>(5)));
    static_assert((poly(Simple<double>(2.0)) += poly(Simple<int>(3))) ==
                  poly(Simple<double>(5.0)));
    static_assert((poly(Simple<int>(2)) += poly(Simple<double>(3.0))) ==
                  poly(Simple<int>(5)));
    static_assert((poly(Simple<Simple<int>>(Simple<int>(2))) +=
                   poly(Simple<Simple<double>>(Simple<double>(3.0)))) ==
                  poly(Simple<Simple<int>>(Simple<int>(5))));
    // ctor wieloargumentowy
    static_assert(poly(Simple<int>(5), 1.1) ==
                  poly(Simple<double>(5.0), Simple<double>(1.1)));
    static_assert(poly(1.1, Simple<int>(5)) ==
                  poly(Simple<double>(1.1), Simple<double>(5.0)));
    static_assert(poly(1.1, Simple<int>(5), 1.1) == poly(Simple<double>(1.1),
                                                         Simple<double>(5.0),
                                                         Simple<double>(1.1)));
}


int main() {
    assigment_test();
    operator_tests();
    constructor_tests();
    nested_poly();
    ring_wrappers();
    std::cout << "OK\n";
}
