#ifndef POLY_H
#define POLY_H

#include <cstddef>
#include <type_traits>

template <typename T, std::size_t N = 0> 
class poly
{
public:

    // KONSTRUKTORY


    // Konstruktor bezargumentowy tworzy wielomian tożsamościowo równy zeru
    constexpr poly() {
        poly({0});
    }

    // Konstruktor kopiujący bądź przenoszący (jednoargumentowe), których argument jest odpowiednio typu const poly<U, M>& bądź poly<U, M>&&, gdzie M <= N, a typ U jest konwertowalny do typu T.
    
    // tego lepiej nie dawać z tego co Pawlewicz mówił, bo wywalają się błędy
    // taka sytuacja powoduje, że lvalue będzie przekierowane do constructora poly(poly<U, M>&& other)
    // a następnie dzięki init i std::forward nie będzie już wywalało błędów i skonstruuje się odpowiedni obiekt

    /*template <typename U, typename M>
    constexpr poly(const poly<U, M>&) requires std::convertible_to<U, T> {
        
    } */
    
    template <typename U, typename M>
    constexpr poly(poly<U, M>&& other) requires std::convertible_to<U, T> {
        init(std::forward<poly<U, M>>(other));
    } 

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    template <typename U>
    constexpr poly(U other) requires std::convertible_to<U, T> : a(1, static_cast<T>(other)) {}

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów. 
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T. 
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.
    // TODO: declare and implement 

    template <typename... U>
    constexpr poly(U&&... args) requires (sizeof...(args) <= N) && (std::convertible_to<U, T> && ...) 
        : a{std::forward<U>(args)...} {
    } // FIXME: jeszcze nieprzemyślałem tego

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
    vector<T> a;

    constexpr poly(std::initializer_list<T> init_list) : a(init_list) {}

    template<typename U, typename M>
    constexpr init(const poly<U, M>& other) {
        a = std::vector<T>(other.a.size());
        for (std::size_t i = 0; i < other.a.size(); ++i) {
            a[i] = static_cast<T>(other.a[i]);
        }
    }

    template<typename U, typename M>
    constexpr init(poly<U, M>&& other) {
        a = std::vector<T>(other.a.size());
        for (std::size_t i = 0; i < other.a.size(); ++i) {
            a[i] = static_cast<T>(std::move(other.a[i]));
        }
        //other.a.clear();
    }
};


// FUNKCJA CROSS
// TODO: declare and implement


#endif // POLY_H