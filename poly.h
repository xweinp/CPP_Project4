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
    constexpr poly(poly<U, M>&& other) requires (N >= M) && (std::convertible_to<U, T>) {
        init(std::forward<poly<U, M>>(other));
    } 

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    template <typename U>
    constexpr poly(U other) requires std::convertible_to<U, T> : a{static_cast<T>(other)} {
    }

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów. 
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T. 
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.

    template <typename... U>
    constexpr poly(U&&... args) requires (sizeof...(args) >= 2) && (sizeof...(args) <= N) && (std::convertible_to<U, T> && ...) : a{} {
        T arr[] = {static_cast<T>(std::forward<U>(args))...};
        for (std::size_t i = 0; i < N; ++i) {
            a[i] = arr[i]; // Inicjalizuj tablicę 'a' wartościami
        }
    }

    // ^^^^^^^^^^^^
    // Należy zapoznać się z szablonem std::is_convertible i konceptem std::convertible_to


    // Powyższe konstruktory nie umożliwiają stworzenia wielomianu rozmiaru jeden (czyli stałego), którego jedyny współczynnik jest wielomianem
    // Dlatego należy zaimplementować funkcję const_poly, której argumentem jest wielomian p (obiekt typu poly) i która zwraca wielomian rozmiaru jeden, którego jedyny współczynnik to p.
    // TODO: declare and implement

    // OPERATORY PRZYPISANIA
    // TODO: declare and implement
    template <typename U, std::size_t M>
    constexpr auto operator=(const poly<U, M>& other) const -> poly<T, N>& requires goodArgument<U, M> {
        if (this != &other) {
            assign_elements(other);
        }
        return *this;
    }
    
    template <typename U, std::size_t M>
    constexpr auto operator=(poly<U, M>&& other) -> poly<T, N>& requires goodArgument<U, M> {
        if (this != &other) {
           assign_elements(std::move(other));
        }
        return *this;
    }

    // OPERATORY ARYTMETYCZNE
    template<typename U, std::size_t M>
    poly& operator+=(const poly<U, M>& other) requires goodArgument<U, M> {
        for (size_t i = 0; i < M; ++i) {
            a[i] += other.a[i];
        }
        return *this;
    }

    template<typename U, std::size_t M>
    poly& operator-=(const poly<U, M>& other) requires goodArgument<U, M> {
        for (size_t i = 0; i < M; ++i) {
            a[i] -= other.a[i];
        }
        return *this;
    }

    template<typename U>
    poly& operator+=(const U& u) requires std::is_convertible<U, T> {
        a[0] += u;
        return *this;
    }

    template<typename U>
    poly& operator-=(const U& u) requires std::is_convertible<U, T> {
        a[0] -= u;
        return *this;
    }

    template<typename U>
    poly& operator*=(const U& u) requires std::is_convertible<U, T> {
        for (auto& x: a)
            x *= u;
        return *this;
    }


    constexpr poly& operator-(const poly& other) {

    }   

    constexpr poly& operator+(const poly& other) {

    }   

    constexpr poly& operator*(const poly& other) {

    } 


    template<typename U>
    constexpr poly& operator-(const U& other) {

    }   
    template<typename U>
    constexpr poly& operator+(const U& other) {

    }   
    template<typename U>
    constexpr poly& operator*(const U& other) {

    } 


    template<typename U>
    constexpr U& operator-(const poly& other) {

    }   
    template<typename U>
    constexpr U& operator+(const poly& other) {

    }   
    template<typename U>
    constexpr U& operator*(const poly& other) {

    } 
    

    // OPERATOR INDEKSUJĄCY
    constexpr T& operator[](std::size_t i) {
        return a[i];
    }

    constexpr const T& operator[](std::size_t i) const {
        return a[i];
    }

    // METODA AT
    // TODO: declare and implement

    // METODA SIZE
    constexpr std::size_t size() {
        return a.size();
    }

private:
    std::array<T, N> a;

    template<typename U, std::size_t M>
    concept goodArgument = (std::convertible_to<U, T>) && (N >= M);

    constexpr void assign_elements(const poly<U, M>& other) {
        std::size_t i = 0;
        while (i < M) {
            a[i] = other.a[i++];
        }
        while (i < N) {
            a[i++] = 0;
        }
    }

    template<typename U, std::size_t M>
    constexpr void init(const poly<U, M>& other) requires (M == N) {
        for (std::size_t i = 0; i < N; ++i) {
            a.push_back(static_cast<T>(other.a[i]));
        }
    }

    template<typename U, std::size_t M>
    constexpr void init(poly<U, M>&& other) requires (M == N) {
        for (std::size_t i = 0; i < N; ++i) {
            a.push_back(static_cast<T>(std::move(other.a[i])));
        }
    }
};

// deduktor do konstruktora
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

template <typename T, std::size_t N, typename U, std::size_t M>
struct std::common_type<poly<T, N>, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};


// funkcja const_poly
template <typename T, std::size_t N>
constexpr poly<poly<T, N>, 1> const_poly(poly<T, N> p) {
    poly<poly<T, N>, 1> result{};
    result[0] = p;
    return result;
}

// FUNKCJA CROSS
// TODO: declare and implement

#endif // POLY_H