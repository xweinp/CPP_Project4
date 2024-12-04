#ifndef POLY_H
#define POLY_H

#include <cstddef>
#include <type_traits>
#include <concepts>
#include <vector>

template <typename T, std::size_t N = 0> 
class poly
{
public:

    // KONSTRUKTORY


    // Konstruktor bezargumentowy tworzy wielomian tożsamościowo równy zeru
    constexpr poly() : a() {}

    // Konstruktor kopiujący bądź przenoszący (jednoargumentowe), których argument jest odpowiednio typu const poly<U, M>& bądź poly<U, M>&&, gdzie M <= N, a typ U jest konwertowalny do typu T.
    
    // tego poniżej lepiej nie dawać z tego co Pawlewicz mówił, bo wywalają się błędy
    // taka sytuacja powoduje, że lvalue będzie przekierowane do constructora poly(poly<U, M>&& other)
    // a następnie dzięki init i std::forward nie będzie już wywalało błędów i skonstruuje się odpowiedni obiekt

    /*template <typename U, typename M>
    constexpr poly(const poly<U, M>&) requires std::convertible_to<U, T> {
        
    } */
    
    template <typename U, std::size_t M>
    constexpr poly(poly<U, M>&& other) requires (M <= N && std::convertible_to<U, T>) : a(N) {
        init(std::forward<poly<U, M>>(other));
    } 

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    template <typename U>
    constexpr poly(U other) requires std::convertible_to<U, T> : a{static_cast<T>(other)} {}

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów. 
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T. 
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.

    template <typename... U>
    constexpr poly(U&&... args) requires (sizeof...(args) == N) && (std::convertible_to<U, T> && ...) {
        poly({static_cast<T>(std::forward<U>(args))...});
    }

    // ^^^^^^^^^^^^
    // Należy zapoznać się z szablonem std::is_convertible i konceptem std::convertible_to


    // Powyższe konstruktory nie umożliwiają stworzenia wielomianu rozmiaru jeden (czyli stałego), którego jedyny współczynnik jest wielomianem
    // Dlatego należy zaimplementować funkcję const_poly, której argumentem jest wielomian p (obiekt typu poly) i która zwraca wielomian rozmiaru jeden, którego jedyny współczynnik to p.
    // TODO: declare and implement

    // OPERATORY PRZYPISANIA
    // TODO: declare and implement

    // OPERATORY ARYTMETYCZNE
    // TODO: declare and implement

    // OPERATOR INDEKSUJĄCY
    // TODO: declare and implement

    // METODA AT
    // TODO: declare and implement

    // METODA SIZE
    // TODO: declare and implement

private:
    std::vector<T> a;

    constexpr poly(std::initializer_list<T> init_list) : a(init_list) {}

    template<typename U, std::size_t M>
    constexpr void init(const poly<U, M>& other) requires (M == N) {
        for (std::size_t i = 0; i < N; ++i) {
            a[i] = static_cast<T>(other.a[i]);
        }
    }

    template<typename U, std::size_t M>
    constexpr void init(poly<U, M>&& other) requires (M == N) {
        for (std::size_t i = 0; i < N; ++i) {
            a[i] = static_cast<T>(std::move(other.a[i]));
        }
    }
};

// deduktor do constructora
template <typename... U>
poly(U&&...) -> poly<std::common_type_t<U...>, sizeof...(U)>;

// reguły konwersji
template <typename T, typename U, std::size_t N>
struct std::common_type<poly<T, N>, poly<U, N>> {
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, std::size_t N, typename U>
struct std::common_type<poly<T, N>, U> {
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, std::size_t N, typename U>
struct std::common_type<U, poly<T, N>> {
    using type = poly<std::common_type_t<T, U>, N>;
};



// FUNKCJA CROSS
// TODO: declare and implement


#endif // POLY_H