// Przeanalizuj poniższy kod. Odpowiedz na pytania.
// W jakich sytuacjach wywoływane są poszczególne konstruktory?
// Kiedy wywoływane są operatory przypisania i konwersji?
// W jakiej kolejności wywoływane są konstruktory obiektów składowych klasy?
// W jakiej kolejności wywoływane są destruktory?
// Kiedy konstruowane są wartości tymczasowe?
// Czym rożni się przekazywanie argumentów przez wartość i przez referencję?
// Jak funkcja zwraca wartość będącą obiektem?
// Co sie dzieje, gdy stała jest przekazywana przez const&?
// Dlaczego operator konwersji jest const, a operator przypisania nie?
// Dlaczego operator przypisania zwraca wartość typu Integer&, a nie const Integer&?
// Czy metoda operator* w klasie Rational powinna zwraca wartość typu
// const Rational, czy Rational?

#include <iostream>
#include <typeinfo>
#include <utility>

template <class T>
class Integer {
public:
  Integer() : internal(0) {
    std::cout << *this << "::konstruktor_bezparametrowy()" << std::endl;
  }
  Integer(const Integer<T>& that) : internal(that.internal) {
    std::cout << *this << "::konstruktor_kopiujący(" << that << ")" << std::endl;
  }
  Integer(Integer<T>&& that) : internal(std::move(that.internal)) {
    std::cout << *this << "::konstruktor_przenoszący(" << that << ")" << std::endl;
  }
  // Sprawdź, co się stanie, gdy poniższy konstruktor zostanie zadeklarowany tak:
  // explicit Integer(T i) : internal(i) {
  Integer(T i) : internal(i) {
    std::cout << *this << "::konstruktor_konwertujący(" << i << ")" << std::endl;
  }
  ~Integer() {
    std::cout << *this << "::destruktor()" << std::endl;
  }
  Integer& operator=(const Integer& that) {
    std::cout << *this << "::operator_przypisania_kopiujący(" << that << ")" << std::endl;
    internal = that.internal;
    return *this;
  }
  Integer& operator=(Integer&& that) {
    std::cout << *this << "::operator_przypisania_przenoszący(" << that << ")" << std::endl;
    internal = std::move(that.internal);
    return *this;
  }
  // Sprawdź, co sie zmienia, gdy poniższy operator zostanie zadeklarowany tak:
  // explicit operator int() const {
  operator int() const {
    std::cout << *this << "::operator int()" << std::endl;
    return internal;
  }
  Integer& operator*=(const Integer& that) {
    std::cout << *this << "::operator*=(" << that << ")" << std::endl;
    internal *= that.internal;
    return *this;
  }
private:
  T internal;
  template <class T1>
  friend std::ostream& operator<<(std::ostream&, const Integer<T1>&);
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Integer<T>& i) {
  os << "{"
    << typeid(i).name()
    << "(" << i.internal
    << ")@"
    << static_cast<const void*>(&i) << "}";
  return os;
}

using Int = Integer<int>;
using Unsigned = Integer<unsigned>;

class Rational {
public:
  Rational(Int n, Unsigned d) : numerator(n), denominator(d) {}
  Rational& operator*=(const Rational& that) {
    // To jest badziewie. Trzeba skracać przez NWD.
    numerator *= that.numerator;
    denominator *= that.denominator;
    return *this;
  }
private:
  Int numerator;
  Unsigned denominator;
};

// Zwracanie wartości typu const Rational powoduje niekompilowanie się
// wyrażenia a * b = c, dla obiektów a, b, c typu Rational, podobnie jak dla
// typów wbudowanych. Wtedy jednak w wyrażeniu c = a * b nie może zostać użyty
// przenoszący operator przypisania. Dla klasy, której wszystkie składowe są
// typów wbudowanych, nie ma to znaczenia, ponieważ wtedy przenoszenie jest
// kopiowaniem. Gdyby jednak składową klasy Rational był jakiś kontener lub
// wskaźnik, to lepszym rozwiązaniem byłoby opuszczenie const w deklaracji typu
// wynikowego.
const Rational operator*(const Rational& a, const Rational& b) {
  return std::move(Rational(a) *= b);
}
// Ten operator można też zaimplementować tak:
// const Rational operator*(Rational a, const Rational& b) {
//   return std::move(a *= b);
// }

Int foo(Int i) {
  std::cout << "Wywołano foo(" << i << ")." << std::endl;
  return i;
}

void foo(const Int& i, const Int& j) {
  std::cout << "Wywołano foo(" << i << ", " << j << ")." << std::endl;
}

Int foo(void) {
  std::cout << "Wywołano foo(void)." << std::endl;
  return Int();
}

int main() {
  {
    std::cout << "\nWykonuje się konstruktor bezparametrowy i kopiujący." << std::endl;
    Int i, j(i);
    std::cout << "\nPrzy opuszczaniu zasięgu wykonują się destruktory." << std::endl;
  }
  {
    std::cout << "\nWykonuje się konwersja typu." << std::endl;
    Int i('3');
    std::cout << "\nTu też wykonuje się konwersja typu." << std::endl;
    Int j = 4;
    std::cout << "\nA tu wykonuje się operator przypisania." << std::endl;
    i = j;
    std::cout << "\nWykonuje się ponowna konwersja typu." << std::endl;
    [[maybe_unused]] int k = j;
    std::cout << "\nWykonuje się jeszcze jedna konwersja typu." << std::endl;
    [[maybe_unused]] char c = j;
    std::cout << "\nPrzekazywanie wartości wywołuje konstruktor kopiujący lub "
            "przenoszący (i potem destruktor)." << std::endl;
    foo(j);
    std::cout << "\nPrzekazywanie argumentów przez referencję." << std::endl;
    foo(i, j);
    std::cout << "\nZwracanie wartości przez funkcję." << std::endl;
    i = foo();
    std::cout << "\nPrzy opuszczaniu zasiegu wykonują sie destruktory." << std::endl;
  }
  {
    std::cout << "\nI wszystko na raz." << std::endl;
    Int i;
    [[maybe_unused]] int j = i = foo(7);
  }
  {
    std::cout << "\nTworzymy ułamki 2/3 i 5/7." << std::endl;
    Rational a(2, 3), b(5, 7);
    std::cout << "\nMnożymy je." << std::endl;
    a *= b;
    std::cout << "\nA teraz znów mnożymy, ale wynik w nowym obiekcie." << std::endl;
    Rational c = a * b;
    std::cout << "\nOperator przypisania dla ułamków." << std::endl;
    a = c;
    std::cout << "\nI znów destrukcja." << std::endl;
  }
}
