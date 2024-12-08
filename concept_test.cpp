#include "poly.h"
#include <typeinfo>
#include <iostream>

int main() {
    // Test detail::is_poly trait
    static_assert(detail::is_poly_v<poly<int, 3>> == true);
    static_assert(detail::is_poly_v<int> == false);
    static_assert(detail::is_poly_v<poly<poly<int, 2>, 3>> == true);
    static_assert(detail::is_poly_v<poly<poly<poly<int, 2>, 3>, 4>> == true);
    static_assert(detail::is_poly_v<double> == false);
    
    /*// Test detail::EitherPoly concept
    //static_assert(detail::EitherPoly<poly<int, 3>, int>);
    //static_assert(detail::EitherPoly<int, poly<int, 3>>);
    //static_assert(detail::EitherPoly<poly<int, 3>, poly<double, 2>>);
    //static_assert(detail::EitherPoly<poly<poly<int, 2>, 3>, poly<int, 4>>);
    //static_assert(detail::EitherPoly<poly<poly<poly<int, 1>, 2>, 3>, int>);
    //static_assert(!detail::EitherPoly<int, double>);

    // Test detail::multiplication_result_type trait
    //static_assert(std::is_same_v<detail::multiplication_result_type_t<int, double>, double>);
    // Basic polynomial multiplication
    //static_assert(std::is_same_v<
    //    detail::multiplication_result_type_t<poly<int, 2>, poly<double, 3>>,
    //    poly<double, 4>>);
    // Empty polynomial multiplication
    static_assert(std::is_same_v<
        detail::multiplication_result_type_t<poly<int, 0>, poly<double, 3>>,
        poly<double, 0>>);
    // Nested polynomial multiplication
    static_assert(std::is_same_v<
        detail::multiplication_result_type_t<poly<poly<int, 2>, 3>, poly<double, 2>>,
        poly<poly<double, 2>, 4>>);
    // Triple-nested polynomial multiplication
    static_assert(std::is_same_v<
        detail::multiplication_result_type_t<
            poly<poly<poly<int, 1>, 2>, 3>,
            poly<poly<double, 2>, 2>>,
        poly<poly<poly<double, 1>, 3>, 4>>);*/

    // Test std::common_type specializations
    // Basic common type tests
    static_assert(std::is_same_v<std::common_type_t<double, poly<int, 2>>,
                                poly<double, 2>>);
    static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, double>,
                                poly<double, 2>>);
    static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, poly<double, 3>>,
                                poly<double, 3>>);
    static_assert(std::is_same_v<std::common_type_t<poly<int, 3>, poly<double, 2>>,
                                poly<double, 3>>);
    // Nested polynomial common type
    static_assert(std::is_same_v<
        std::common_type_t<poly<poly<int, 2>, 3>, poly<poly<double, 2>, 2>>,
        poly<poly<double, 2>, 3>>);
    // Triple-nested polynomial common type
    static_assert(std::is_same_v<
        std::common_type_t<
            poly<poly<poly<int, 1>, 4>, 3>,
            poly<poly<poly<double, 2>, 2>, 2>>,
        poly<poly<poly<double, 2>, 4>, 3>>);
}
