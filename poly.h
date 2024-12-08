#ifndef POLY_H
#define POLY_H

#include <cstddef>
#include <type_traits>
#include <concepts>
#include <array>
#include <functional>

// deklaracja poly
template <typename T, std::size_t N> // TODO: czy nie przeszkadza nam, że tutaj N != 0?
class poly;

// template do sprawdzania czy typ jest wielomianem

namespace detail
{

    template <typename U>
    struct is_poly : std::false_type
    {
    };

    template <typename U, std::size_t M>
    struct is_poly<poly<U, M>> : std::true_type
    {
    };

    template <typename U>
    inline constexpr bool is_poly_v = is_poly<U>::value;

}

// TODO: da się zbić niektóre requires do wspólnych template'ów (może niektóre warto)
// TODO: są trochę pomieszane is_convertible z convertible:to, tak samo z
// TODO: czy może istnić poly z N = 0? jeśli tak to trzeba go wyifować w operatorach arytmetycznych
// TODO: niektore ify zapewne moge zmienc na if constexpr
// TODO: ogarnąć co jest z tym konstruktorem kopiującym i przenoszącym
// TODO: dokończyć cross
// TODO: idk czy napewno definicje operatorów mogą być wyrzucone poza klasę
//       chciałem wyrzucić wszystkie metody dłuższe niż kilka linijek
//       vscode cos podkresla, ale kod sie kompiluje

template <typename T, std::size_t N = 0>
class poly
{
public:
    // Aby można było swobodnie używać zmiennych prywatnych przy używaniu common_type
    template <typename U, std::size_t M>
    friend class poly;

    // KONSTRUKTORY

    // Konstruktor bezargumentowy tworzy wielomian tożsamościowo równy zeru
    constexpr poly() : a() {}

    // Konstruktor kopiujący bądź przenoszący (jednoargumentowe), których argument jest odpowiednio typu const poly<U, M>& bądź poly<U, M>&&, gdzie M <= N, a typ U jest konwertowalny do typu T.

    template <typename U, std::size_t M>
    constexpr poly(const poly<U, M> &other)
        requires (N >= M) && (std::convertible_to<U, T>)
    {
        for (std::size_t i = 0; i < M; i++)
        {
            a[i] = other[i];
        }
    }

    template <typename U, std::size_t M>
    constexpr poly(poly<U, M> &&other)
        requires(N >= M) && (std::convertible_to<U, T>)
    {
        init(std::forward<poly<U, M>>(other));
    }

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    template <typename U>
    constexpr poly(U other)
        requires std::convertible_to<U, T>
        : a{static_cast<T>(other)}
    {
    }

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów.
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T.
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.

    template <typename... U>
    constexpr poly(U &&...args)
        requires (sizeof...(args) >= 2) && (sizeof...(args) <= N)
        : a{}
    {
        T arr[] = {static_cast<T>(std::forward<U>(args))...};
        for (std::size_t i = 0; i < sizeof...(args); i++)
        {
            a[i] = arr[i];
        }
    }

    // ^^^^^^^^^^^^
    // Należy zapoznać się z szablonem std::is_convertible i konceptem std::convertible_to

    // OPERATORY PRZYPISANIA
    // TODO: declare and implement
    template <typename U, std::size_t M>
        requires (std::is_convertible_v<poly<U, M>, poly<T, N>>)
    constexpr auto operator=(const poly<U, M> &other) -> poly<T, N> &
    {
        if (!is_same_object(other))
            assign_elements(other);
        return *this;
    }

    template <typename U, std::size_t M>
        requires (std::is_convertible_v<poly<U, M>, poly<T, N>>)
    constexpr auto operator=(poly<U, M> &&other) -> poly<T, N> &
    {
        if (!is_same_object(other))
            assign_elements(std::move(other));
        return *this;
    }

    // OPERATORY ARYTMETYCZNE

    // +=
    template <typename U, std::size_t M>
        requires (std::is_convertible_v<poly<U, M>, poly<T, N>>)
    constexpr poly<T, N> &operator+=(const poly<U, M> &other)
    {
        for (size_t i = 0; i < M; ++i)
            a[i] += other[i];
        return *this;
    }

    template <typename U>
        requires (std::is_convertible_v<U, T>)
    constexpr poly<T, N> &operator+=(const U &other)
    {
        a[0] += other;
        return *this;
    }

    // -=
    template <typename U, std::size_t M>
        requires (std::is_convertible_v<poly<U, M>, poly<T, N>>)
    constexpr poly<T, N> &operator-=(const poly<U, M> &other) 
    {
        for (size_t i = 0; i < M; ++i)
            a[i] -= other[i];
        return *this;
    }

    template <typename U>
        requires (std::is_convertible_v<U, T>)
    constexpr poly<T, N> &operator-=(const U &other)
    {
        a[0] -= other;
        return *this;
    }

    // *=
    template <typename U>
        requires (std::is_convertible_v<U, T>)
    constexpr poly<T, N> &operator*=(const U &other)
    {
        for (auto &x : a)
            x *= other;
        return *this;
    }

    // unary-
    constexpr poly<T, N> operator-() const
    {
        poly<T, N> res;
        for (size_t i = 0; i < N; ++i)
            res[i] = -a[i];
        return res;
    }

    // OPERATOR INDEKSUJĄCY
    constexpr T &operator[](std::size_t i)
    {
        return a[i];
    }

    constexpr const T &operator[](std::size_t i) const
    {
        return a[i];
    }

    // METODA AT
    // Bez argumentów
    constexpr poly at() const
    {
        poly res = *this;
        return res;
    }
    // Kiedy T też jest wielomianem
    template <typename U, typename... Args>
    constexpr auto at(const U &first, Args &&...args) const
        requires(detail::is_poly_v<T>)
    {
        return calc_at<U, 0, Args...>(first, std::forward<Args>(args)...);
    }
    // Kiedy T nie jest już wielomianem
    template <typename U, typename... Args>
    constexpr auto at(const U &first, [[maybe_unused]] Args &&...args) const
        requires(!detail::is_poly_v<T>)
    {
        return calc_at<U, 0>(first);
    }
    // Wersja dla std::array
    template <typename U, std::size_t K>
    constexpr auto at(const std::array<U, K> &arargs) const
    {
        return std::apply([&](auto... args)
                          { return at(args...); }, arargs);
    }

    // METODA SIZE
    constexpr std::size_t size() const
    {
        return a.size();
    }

private:
    std::array<T, N> a;

    template <typename U, std::size_t M>
    constexpr void assign_elements(const poly<U, M> &other)
    {
        std::size_t i = 0;
        while (i < M)
        {
            a[i] = other[i];
            ++i;
        }
        while (i < N)
        {
            a[i] = 0;
            ++i;
        }
    }

    template <typename U, std::size_t M>
    constexpr void init(const poly<U, M> &other)
        requires(N >= M)
    {
        for (std::size_t i = 0; i < M; ++i)
            a[i] = static_cast<T>(other[i]);
    }

    template <typename U, std::size_t M>
    constexpr void init(poly<U, M> &&other)
        requires(N >= M)
    {
        for (std::size_t i = 0; i < M; ++i)
            a[i] = static_cast<T>(std::move(other[i]));
    }

    // pomocnicze funkcje do at()
    // dla T, które nie są już wielomianami
    template <typename U, std::size_t I>
        requires(!detail::is_poly_v<T>)
    constexpr auto calc_at(const U &first) const
    {
        if constexpr (I == N - 1)
            return a[I];
        else
            return (first * calc_at<U, I + 1>(first)) + a[I];
    }
    // dla T, które są wielomianami rekurencyjnie wywołuję at()
    template <typename U, std::size_t I, typename... Args>
        requires(detail::is_poly_v<T>)
    constexpr auto calc_at(const U &first, Args &&...args) const
    {
        auto son_res = a[I].at(std::forward<Args>(args)...);
        if constexpr (I == N - 1)
            return son_res;
        else
            return (first * calc_at<U, I + 1>(first, std::forward<Args>(args)...)) + son_res;
    }

    template <typename U>
    constexpr bool is_same_object(const U &other) const
    {
        return (static_cast<const void*>(this) != static_cast<const void*>(&other));
    }
};

// deduktor do konstruktora
template <typename... U>
poly(U &&...) -> poly<std::common_type_t<U...>, sizeof...(U)>;

// COMMON TYPE
// reguły konwersji

template <typename T, typename U, std::size_t N>
struct std::common_type<poly<T, N>, poly<U, N>>
{
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, std::size_t N, typename U, std::size_t M>
struct std::common_type<const poly<T, N> &, poly<U, M>>
{
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};

template <typename T, std::size_t N, typename U, std::size_t M>
struct std::common_type<poly<T, N>, const poly<U, M> &>
{
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};

template <typename T, std::size_t N, typename U, std::size_t M>
struct std::common_type<poly<T, N>, poly<U, M>>
{
    using type = poly<std::common_type_t<T, U>, std::max(N, M)>;
};

template <typename T, std::size_t N, typename U>
    requires(!detail::is_poly_v<U> && std::convertible_to<U, T>)
struct std::common_type<poly<T, N>, U>
{
    using type = poly<std::common_type_t<T, U>, N>;
};

template <typename T, std::size_t N, typename U>
    requires(!detail::is_poly_v<U> && std::convertible_to<U, T>)
struct std::common_type<U, poly<T, N>>
{
    using type = poly<std::common_type_t<T, U>, N>;
};

// OPERATORY ARYTMETYCZNE

// +

// Tylko lewy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && std::is_convertible_v<U, T>)
constexpr auto operator+(const poly<T, N> &x, const U &y)
{
    std::common_type_t<poly<T, N>, U> res = x;
    res[0] = x[0] + y;
    return res;
}

// Tylko prawy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && std::is_convertible_v<U, T>)
constexpr auto operator+(const U &y, const poly<T, N> &x)
{
    return x + y;
}
// Oba argumenty to wielomiany
template <typename T, typename U>
    requires(detail::is_poly_v<T> && detail::is_poly_v<U> && (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>))
constexpr auto operator+(const T &x, const U &y)
{
    std::common_type_t<T, U> res;
    size_t both = std::min(x.size(), y.size());
    size_t i = 0;
    while (i < both)
    {
        res[i] = x[i] + y[i];
        ++i;
    }
    while (i < x.size())
    {
        res[i] = x[i];
        ++i;
    }
    while (i < y.size())
    {
        res[i] = y[i];
        ++i;
    }
    return res;
}

// -
// Tylko lewy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && std::is_convertible_v<U, T>)
constexpr auto operator-(const poly<T, N> &x, const U &y)
{
    std::common_type_t<poly<T, N>, U> res = x;
    res[0] = x[0] - y;
    return res;
}
// Tylko prawy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && std::is_convertible_v<U, T>)
constexpr auto operator-(const U &y, const poly<T, N> &x)
{
    std::common_type_t<poly<T, N>, U> res = -x;
    res[0] = y - x[0];
    return res;
}
// Oba argumenty to wielomiany
template <typename T, typename U>
    requires(detail::is_poly_v<T> && detail::is_poly_v<U> && (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>))
constexpr auto operator-(const T &x, const U &y)
{
    std::common_type_t<T, U> res;
    size_t both = std::min(x.size(), y.size());
    size_t i = 0;
    while (i < both)
    {
        res[i] = x[i] - y[i];
        ++i;
    }
    while (i < x.size())
    {
        res[i] = x[i];
        ++i;
    }
    while (i < y.size())
    {
        res[i] = -y[i];
        ++i;
    }
    return res;
}

// *
// Tylko lewy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>))
constexpr auto operator*(const poly<T, N> &x, const U &y)
{
    poly<std::common_type_t<T, U>, N> res;
    for (size_t i = 0; i < N; ++i)
        res[i] = x[i] * y;
    return res;
}
// Tylko prawy argument to wielomian
template <typename T, std::size_t N, typename U>
    requires((!detail::is_poly_v<U>) && (std::is_convertible_v<U, T> || std::is_convertible_v<T, U>))
constexpr auto operator*(const U &y, const poly<T, N> &x)
{
    return x * y;
}
// Oba argumenty to wielomiany
template <typename T, std::size_t N, typename U, std::size_t M>
    requires ((std::is_convertible_v<U, T> || std::is_convertible_v<T, U>) && (N > 0 && M > 0))
constexpr auto operator*(const poly<T, N> &x, const poly<U, M> &y)
{
    poly<decltype(x[0] * y[0]), N + M - 1> res;
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < M; ++j)
            res[i + j] = res[i + j] + (x[i] * y[j]);

    return res;
}
template <typename T, std::size_t N, typename U, std::size_t M>
    requires ((std::is_convertible_v<U, T> || std::is_convertible_v<T, U>) && (N == 0 || M == 0))
constexpr auto operator*([[maybe_unused]] const poly<T, N> &x, [[maybe_unused]] const poly<U, M> &y)
{
        return poly<std::common_type_t<T, U>, 0>{};
}

template <typename T_From, std::size_t N_From, typename T_To, std::size_t N_To>
struct std::is_convertible<poly<T_From, N_From>, poly<T_To, N_To>>
{
    static constexpr bool value = (std::is_convertible_v<T_From, T_To>) && (N_To >= N_From);
};

// CONST POLY

template <typename T, std::size_t N>
constexpr poly<poly<T, N>, 1> const_poly(poly<T, N> p)
{
    poly<poly<T, N>, 1> res{};
    res[0] = p;
    return res;
}

// CROSS TYPE
template <typename T, typename U>
struct cross_type
{
    using type = std::common_type_t<T, U>;
};

template <typename T, size_t N, typename U, size_t M>
struct cross_type<poly<T, N>, poly<U, M>>
{
    using type = poly<typename cross_type<T, poly<U, M>>::type, N>;
};

template <typename T, typename U, size_t M>
struct cross_type<T, poly<U, M>>
{
    using type = poly<std::common_type_t<T, U>, M>;
};

// FUNKCJA CROSS

template <typename T, typename U, std::size_t M>
constexpr auto cross(const T &p, const poly<U, M> &q)
    requires(!detail::is_poly_v<T>)
{
    typename cross_type<T, poly<U, M>>::type result = p * q;
    return result;
}

template <typename T, std::size_t N, typename U, std::size_t M>
constexpr auto cross(const poly<T, N> &p, const poly<U, M> &q)
    requires(detail::is_poly_v<poly<T, N>> && detail::is_poly_v<poly<U, M>>)
{
    typename cross_type<poly<T, N>, poly<U, M>>::type result;
    for (std::size_t i = 0; i < N; i++)
    {
        result[i] = cross(p[i], q);
    }

    return result;
}

#endif // POLY_H