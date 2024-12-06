#ifndef POLY_H
#define POLY_H

#include <cstddef>
#include <type_traits>
#include <concepts>
#include <vector>


// Ze względu na użycie auto w definicji metody at nie byłem w stanie 
// zadeklarować jej poza klasą, więc zadeklarowałem ją wewnątrz klasy.
// no i to zmusilo mnie do wrzucenia tutaj odrobiny smieci xD.
// TODO: organizacja tego kodu

template <typename T, std::size_t N> 
class poly;

template <typename U>
struct is_poly : std::false_type{};

template <typename U, std::size_t M>
struct is_poly<poly<U, M>> : std::true_type{};

template<typename U>
inline constexpr bool is_poly_v = is_poly<U>::value;

template<typename X, typename A>
struct eval_type {
    using type = std::common_type_t<X, A>;
};

template<typename T, std::size_t N, typename U>
struct eval_type<poly<T, N>, U> {
    using type = poly<std::common_type_t<T, U>, N>;
};

template<typename T, std::size_t N, typename U>
struct eval_type<U, poly<T, N>> {
    using type = poly<std::common_type_t<T, U>, N>;
};


template<typename T, std::size_t N, typename U, std::size_t M>
struct eval_type<poly<T, N>, poly<U, M>> {
    using type = poly<std::common_type_t<T, U>, (N - 1) * (M - 1) + 1>;
};

template<typename X, typename A>
using eval_type_t = eval_type<X, A>::type;


template <typename T, std::size_t N = 0> 
class poly
{
public:

    // TODO: TO MOZE BYC CURSED ale naprawia kilka bledow kompilacji
    template<typename U, std::size_t M>
    friend class poly;

    // KONSTRUKTORY


    // Konstruktor bezargumentowy tworzy wielomian tożsamościowo równy zeru
    constexpr poly() : a() {}

    // Konstruktor kopiujący bądź przenoszący (jednoargumentowe), których argument jest odpowiednio typu const poly<U, M>& bądź poly<U, M>&&, gdzie M <= N, a typ U jest konwertowalny do typu T.

    template <typename U, std::size_t M>
    constexpr poly(poly<U, M>&& other) requires (N >= M) && (std::convertible_to<U, T>) {
        init(std::forward<poly<U, M>>(other));
    } 

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    template <typename U>
    constexpr poly(U other) requires std::convertible_to<U, T> : a{static_cast<T>(other)} {}

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów. 
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T. 
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.

    template <typename... U>
    constexpr poly(U&&... args) requires (sizeof...(args) >= 2) && (sizeof...(args) <= N) && (std::convertible_to<U, T> && ...) : a{} {
        T arr[] = {static_cast<T>(std::forward<U>(args))...};
        for (std::size_t i = 0; i < N; ++i) {
            a[i] = arr[i];
        }
    }

    // ^^^^^^^^^^^^
    // Należy zapoznać się z szablonem std::is_convertible i konceptem std::convertible_to

    // OPERATORY PRZYPISANIA
    // TODO: declare and implement
    template <typename U, std::size_t M>
    requires std::convertible_to<poly<U, M>, poly<T, N>>
    auto operator=(const poly<U, M>& other) const -> poly<T, N>& { // to raczej nie moze byc constexpr
        if (this != static_cast<decltype(this)>(&other)) {
            assign_elements(other);
        }
        return *this;
    }
    
    template <typename U, std::size_t M>
    requires std::convertible_to<poly<U, M>, poly<T, N>>
    constexpr auto operator=(poly<U, M>&& other) -> poly<T, N>& {
        if (this != static_cast<decltype(this)>(&other)) {
           assign_elements(std::move(other));
        }
        return *this;
    }

    // OPERATORY ARYTMETYCZNE
    
    // OPERATORY ARYTMETYCZNE
    template<typename U, std::size_t M>
    requires std::convertible_to<poly<U, M>, poly<T, N>>
    poly<T, N>& operator+=(const poly<U, M>& other)  { // TODO: wsm troche problem kiedy this == other (bo other jest const ref)
        for (size_t i = 0; i < M; ++i) {
            a[i] += other[i];
        }
        return *this;
    }

    template<typename U, std::size_t M>
    requires std::convertible_to<poly<U, M>, poly<T, N>>
    poly<T, N>& operator-=(const poly<U, M>& other)  { // TODO: wsm troche problem kiedy this == other (bo other jest const ref)
        for (size_t i = 0; i < M; ++i) {
            a[i] -= other[i];
        }
        return *this;
    }


    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator+=(const U& u)  { 
        a[0] += u;
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator-=(const U& u)  {
        a[0] -= u;
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator*=(const U& u)  {
        for (auto& x: a)
            x *= u;
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator+=(U&& u)  { 
        a[0] += std::forward<U>(u);
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator-=(U&& u)  {
        a[0] -= std::forward<U>(u);
        return *this;
    }

    template<typename U>
    requires std::convertible_to<U, T>
    poly<T, N>& operator*=(U&& u)  {
        for (auto& x: a)
            x *= std::forward<U>(u);
        return *this;
    }

    constexpr poly operator-() const {
        poly<T, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = -a[i];
        return result;
    }

    template<typename U, std::size_t M>
    constexpr auto operator+(const poly<U, M>& other) const 
    -> typename std::common_type_t<poly<T, N>, poly<U, M>> {
        typename std::common_type_t<poly<T, N>, poly<U, M>> result;
        for (size_t i = 0; i < std::max(N, M); ++i) {
            if (i < M)
                result[i] = result[i] + other[i];
            if (i < N)
                result[i] = result[i] + a[i];
        }
        
        return result;
    }   

    template<typename U, std::size_t M>
    constexpr auto operator-(const poly<U, M>& other) const 
    -> typename std::common_type_t<poly<T, N>, poly<U, M>> {
        typename std::common_type_t<poly<T, N>, poly<U, M>> result;
        for (size_t i = 0; i < std::max(N, M); ++i) {
            if (i < M)
                result[i] = result[i] - other[i];
            if (i < N)
                result[i] = result[i] + a[i];
        }
        
        return result;
    }  

    template<typename U, std::size_t M>
    constexpr auto operator*([[maybe_unused]]const poly<U, M>& other) const {
        return poly<T, 0>();
    } 

    template<typename U, std::size_t M>
    requires (N > 0 && M > 0)
    constexpr auto operator*(const poly<U, M>& other) const {
        poly<std::common_type_t<T, U>, N + M - 1> result; // TODO: co jesli N + M - 1 > SIZE_T_MAX?

        for (size_t i = 0; i < N; ++i) 
            for (size_t j = 0; j < M; ++j) {
                result[i + j] += a[i] * other[j];
            }

        return result;
    }


    template<typename U>
    constexpr auto operator-(const U& other) const 
    -> typename std::common_type_t<poly<T, N>, U>{
        typename std::common_type_t<poly<T, N>, U> result(*this);
        result[0] -= other;
        return result;
    }  

    template<typename U>
    constexpr auto operator+(const U& other) const
    -> typename std::common_type_t<poly<T, N>, U> {
        typename std::common_type_t<poly<T, N>, U> result = *this;
        result[0] += other;
        return result;
    }  

    template<typename U>
    constexpr auto operator*(const U& other) const 
    -> typename std::common_type_t<poly<T, N>, U> {
        typename std::common_type_t<poly<T, N>, U> result = *this;
        for (auto& x: result.a)
            x *= other;
        return result;
    }


    // OPERATOR INDEKSUJĄCY
    constexpr T& operator[](std::size_t i) {
        return a[i];
    }

    constexpr const T& operator[](std::size_t i) const {
        return a[i];
    }

    // METODA AT
    constexpr poly at() const{
        poly result = *this;
        return result;
    }

    template<typename U, typename... Args>
    requires(true) // TODO: muszę uzupełnić
    constexpr auto at(const U& first, Args&&... args) const;

    // II
    template<typename U, std::size_t K>
    auto at(const std::array<U, K>& arargs) {
        return std::apply([&](auto... args) {return at(args...);}, arargs);
    }

    // METODA SIZE
    constexpr std::size_t size() {
        return a.size();
    }

private:
    std::array<T, N> a;

    
    template<typename U, std::size_t M>
    constexpr void assign_elements(const poly<U, M>& other) {
        std::size_t i = 0;
        while (i < M) {
            a[i] = other[i++];
        }
        while (i < N) {
            a[i++] = 0;
        }
    }

    template<typename U, std::size_t M>
    constexpr void init(const poly<U, M>& other) requires (N >= M) {
        for (std::size_t i = 0; i < M; ++i) {
            a[i] = static_cast<T>(other[i]);
        }
    }

    template<typename U, std::size_t M>
    constexpr void init(poly<U, M>&& other) requires (N >= M) {
        for (std::size_t i = 0; i < M; ++i) {
            a[i] = static_cast<T>(std::move(other[i]));
        }
    }
};


// TODO: organize this later


template<typename T_From, std::size_t N_From, typename T_To, std::size_t N_To>
struct std::is_convertible<poly<T_From, N_From>, poly<T_To, N_To>> {
    static constexpr bool value = (std::is_convertible_v<T_From, T_To>) && (N_To >= N_From);
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

template<typename U, typename T, std::size_t N>
constexpr auto operator-(const U& x, const poly<T, N>& y) {
    return y - x;
}   
template<typename U, typename T, std::size_t N>
constexpr auto operator+(const U& x, const poly<T, N>& y) {
    return y + x;
}   
template<typename U, typename T, std::size_t N>
constexpr auto operator*(const U& x, const poly<T, N>& y) {
    return y * x;
}   

// funkcja const_poly
template <typename T, std::size_t N>
constexpr poly<poly<T, N>, 1> const_poly(poly<T, N> p) {
    poly<poly<T, N>, 1> result{};
    result[0] = p;
    return result;
}

// FUNKCJA CROSS

template <typename T, typename U, std::size_t M>
constexpr std::common_type<T, poly<U, M>> cross(const T& p, const poly<U, M>& q) requires (!is_poly_v<T>) {
    std::common_type<T, poly<U, M>> result = p * q; 
    return result;
}


template <typename T, std::size_t N, typename U, std::size_t M>
constexpr poly<std::common_type<T, poly<U, M>>, N> cross(const poly<T, N>& p, const poly<U, M>& q) {
    poly<std::common_type<T, poly<U, M>>, N> result;

    for (std::size_t i = 0; i < N; i++) {
        result[i] = cross(p[i], q);
    }

    return result;
}

#endif // POLY_H