#ifndef POLY_H
#define POLY_H

#include <cstddef>

template <typename T, std::size_t N = 0> 
class poly
{
public:

    // KONSTRUKTORY

    // Konstruktor bezargumentowy tworzy wielomian tożsamościowo równy zeru
    poly(); // TODO: implement 

    // Konstruktor kopiujący bądź przenoszący (jednoargumentowe), których argument jest odpowiednio typu const poly<U, M>& bądź poly<U, M>&&, gdzie M <= N, a typ U jest konwertowalny do typu T.
    poly(const poly<U, M>&); // TODO: implement 
    poly(const poly<U, M>&&); // TODO: implement 

    // Konstruktor konwertujący (jednoargumentowy) o argumencie typu konwertowalnego do typu T tworzy wielomian rozmiaru 1.
    // TODO: declare and implement 

    // Konstruktor wieloargumentowy (dwa lub więcej argumentów) tworzy wielomian o współczynnikach takich jak wartości kolejnych argumentów. 
    // Liczba argumentów powinna być nie większa niż rozmiar wielomianu N, a typ każdego argumentu powinien być r-referencją do typu konwertowalnego do typu T. 
    // Wymagamy użycia „perfect forwarding”, patrz std::forward.
    // TODO: declare and implement 

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
};


// FUNKCJA CROSS
// TODO: declare and implement


#endif // POLY_H