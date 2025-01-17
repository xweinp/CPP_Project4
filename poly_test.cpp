#include "poly.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <array>
#include <cassert>
#include <utility>
#include <ostream>
#include <string>
#include <sstream>
#include <type_traits>

namespace
{
  template <typename T, std::size_t N>
  constexpr bool operator==(const poly<T, N>& a, const poly<T, N>& b)
  {
    for (std::size_t i = 0; i < N; ++i)
      if (!(a[i] == b[i]))
        return false;
    return true;
  }

  template <std::size_t n, typename T, std::size_t N>
  constexpr auto pow(const poly<T, N>& a)
  {
    if constexpr (n == 1)
      return a;
    else
      return a * pow<n - 1>(a);
  }

  template <typename T, std::size_t N>
  std::stringstream& operator<<(std::stringstream& ss, const poly<T, N>& p) {
    ss << '{';
    for (std::size_t i = 0; i < N; i++) {
      if (i)
      ss << ',';
      ss << p[i];
    }
    ss << '}';
    return ss;
  }

  template <typename T>
  std::string as_string(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
  }

  template <typename T, std::size_t N>
  std::ostream& operator<<(std::ostream& os, const poly<T, N>& p) {
    return os << as_string(p);
  }

  template <typename T>
  struct weak_wrapper
  {
    T a;
    weak_wrapper() {}
    weak_wrapper(T a) : a(a) {}
    weak_wrapper& operator-=(weak_wrapper b) {a -= b.a; return *this;}
    weak_wrapper& operator+=(weak_wrapper b) {a += b.a; return *this;}
    weak_wrapper& operator*=(weak_wrapper b) {a *= b.a; return *this;}
    template <typename U>
    friend weak_wrapper<U> operator*(weak_wrapper<U> a, weak_wrapper<U> b) {return a.a * b.a;}
  };

  template <typename T>
  struct wrapper
  {
    T a;

    static int& copies()
    {
      static int count = 0;
      return count;
    }

    void copy(const wrapper& o)
    {
      copies()++;
      a = o.a;
    }

    void move(wrapper&& o)
    {
      a = o.a;
      o.a = -1;
    }

    wrapper() {}

    wrapper(T a) : a(a) {}
    wrapper(const wrapper& o) {copy(o);}
    wrapper(wrapper&& o) {move(std::move(o));}
    wrapper& operator=(const wrapper& o) {copy(o); return *this;}
    wrapper& operator=(wrapper&& o) {move(std::move(o)); return *this;}

    template <typename U>
    friend wrapper<U> operator+(const wrapper<U>& a, const wrapper<U>& b)
    {
      T c = a;
      c += b;
      return c;
    }
    template <typename U>
    friend wrapper<U> operator-(const wrapper<U>& a, const wrapper<U>& b)
    {
      T c = a;
      c -= b;
      return c;
    }
    template <typename U>
    friend wrapper<U> operator-(const wrapper<U>& a)
    {
      return T(-a.a);
    }
    template <typename U>
    friend wrapper<U> operator*(const wrapper<U>& a, const wrapper<U>& b) {return a.a * b.a;}
    wrapper& operator-=(const wrapper& b) {a -= b.a; return *this;}
    wrapper& operator+=(const wrapper& b) {a += b.a; return *this;}
    wrapper& operator*=(const wrapper& b) {a *= b.a; return *this;}
  };
}

namespace std
{
  template <>
  struct common_type<wrapper<int>, wrapper<unsigned>>
  {
    using type = wrapper<long>;
  };
  template <>
  struct common_type<wrapper<unsigned>, wrapper<int>>
  {
    using type = wrapper<long>;
  };
}

int main()
{
#if TEST_NUM == 101
  // poly(), N = 0
  poly<int> p;
  assert(p.size() == 0);
  poly<poly<int>> q;
  assert(q.size() == 0);
  // poly(), poly(U&&), poly(U&&, ...) ze współczynników
  poly<int, 3> a;
  assert(a[0] == 0);
  assert(a[1] == 0);
  assert(a[2] == 0);
  poly<int, 3> b(1);
  assert(b[0] == 1);
  assert(b[1] == 0);
  assert(b[2] == 0);
  poly<int, 3> c(1, 2);
  assert(c[0] == 1);
  assert(c[1] == 2);
  assert(c[2] == 0);
  poly<int, 3> d(1, 2, 3);
  assert(d[0] == 1);
  assert(d[1] == 2);
  assert(d[2] == 3);
  // Współczynniki też są poly.
  poly u(a, b, c, d);
  assert(u.size() == 4);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 3; j++)
      assert(u[i][j] == (j < i ? j + 1 : 0));
  u[0][0] = -1;
  u[1] = d;
  poly v(c, u);
  assert(v.size() == 2);
  assert(v[0].size() == 4);
  assert(v[0][0].size() == 3);
  std::string sv = "{{{1,0,0},{2,0,0},{0,0,0},{0,0,0}},{{-1,0,0},{1,2,3},{1,2,0},{1,2,3}}}";
  assert(as_string(v) == sv);
  // poly(const poly&)
  poly x(v);
  assert(as_string(x) == sv);
  // poly(poly&&)
  poly y(std::move(x));
  assert(as_string(y) == sv);
  x[0][0][0] = 0;
  y[0][0][0] = 0;
  assert(v[0][0][0] == 1);
#endif

#if TEST_NUM == 102
  // poly(const poly<U, M>&)
  poly z(poly(3.14, 2.7), -1.1);
  std::string sz = "{{3,2,0},{-1,0,0},{0,0,0}}";
  assert(as_string(poly<poly<int, 3>, 3>(z)) == sz);
  // poly(poly<U, M>&&)
  poly za(z);
  assert(as_string(poly<poly<int, 3>, 3>(std::move(za))) == sz);
  // const_poly
  assert(as_string(const_poly(poly<poly<int, 3>, 3>(z))) == '{' + sz + '}');
#endif

#if TEST_NUM == 103
  // Nie powinno się kompilować, bo sizeof...(args) > N.
  [[maybe_unused]] poly<int, 3> err(1, 2, 3, 4);
#endif

#if TEST_NUM == 104
  // Nie powinno się kompilować, bo się nie mieści w drugim wymiarze.
  poly<poly<int, 2>, 2> ok(poly<int, 2>(3, 2), poly<int, 2>(-1, 1));
  [[maybe_unused]] poly<poly<int, 2>, 1> err(ok);
#endif

#if TEST_NUM == 105
  // Nie powinno się kompilować, bo się nie mieści w drugim wymiarze.
  poly<poly<int, 2>, 2> ok(poly<int, 2>(3, 2), poly<int, 2>(-1, 1));
  [[maybe_unused]] poly<poly<int, 2>, 1> err(std::move(ok));
#endif

#if TEST_NUM == 201
  // testy na operator=()
  // operator=(const poly<U, M>&)
  poly a(poly(1, 2), 3, poly(4, 5, 6), poly(7));
  poly b(1.0, 2.0, poly(3.0, 4.0, 5.0));
  auto aa = a;
  a = b;
  assert(a == poly(1, 2, poly(3, 4, 5), 0));
  assert((a = 1LL) == poly(poly(1, 0, 0), 0, 0, 0));
  // operator=(poly<U, M>&&)
  a = std::move(aa);
  assert(a == poly(poly(1, 2), 3, poly(4, 5, 6), poly(7)));
  assert((a = std::move(b)) == poly(1, 2, poly(3, 4, 5), 0));
#endif

#if TEST_NUM == 202
  // testy na arytmetykę – jedna zmienna
  poly a(-1, 1);
  poly b(2, 3, -4, 2);
  poly c(2.5, -1.1);
  poly d(-1.7, 3.8, 3.2, 1.3);
  poly aa = a;
  poly bb = b;
  bb += a;
  bb -= std::move(aa);
  assert(b == bb);
  bb *= 2;
  bb *= 0.5;
  assert(b == bb);
  bb += c;
  bb -= c;
  assert(bb == poly(1, 2, -4, 2));
  bb += d;
  bb -= d;
  assert(bb == poly(1, 1, -3, 1));
  poly cc = c;
  poly dd = d;
  dd += c;
  dd -= c;
  assert(d == dd);
  dd *= -2;
  dd *= -0.5;
  assert(d == dd);
  dd -= c;
  dd += std::move(cc);
  assert(as_string(d) == as_string(dd));
  ((dd += a) += b) *= 2;
  ((dd *= 0.5) -= b) -= a;
  assert(as_string(d) == as_string(dd));
  assert(a - 3 == poly(-4, 1));
  assert(2 + b == poly(4, 3, -4, 2));
  assert(a + b == poly(1, 4, -4, 2));
  assert(a - b == poly(-3, -2, 4, -2));
  assert(b - a == -(a - b));
  assert(a * 3 == poly(-3, 3));
  assert(-2 * b == poly(-4, -6, 8, -4));
  assert(a * b == poly(-2, -1, 7, -6, 2));
  assert(as_string(2 + (c + b) * (a - d) - (-b)) == "{7.15,-8.27,-26.52,2.67,4.73,-1.2,-2.6}");
#endif

#if TEST_NUM == 203
  // testy na arytmetykę – dwie zmienne
  poly a = poly(-1, poly(0, 2, -2), poly(1, 3));
  poly b = poly(-5, poly(1, -2, 0, 1));
  assert((a + b + 1) * (poly(0, 1) + a - b) - 4 * a * b ==
    poly(-40, poly(8, 12, -38, 13), poly(19, 53, 22, -25, 0, 8, -1),
    poly(-3, 3, 32, -16, -12), poly(1, 6, 9)));
  assert((a + b + 1) * (poly(0, 1.0) + a - b) - 4 * a * b ==
    poly(-40, poly(8, 12.0, -38, 13), poly(19, 53, 22, -25, 0, 8, -1),
    poly(-3, 3, 32, -16, -12), poly(1, 6, 9)));
  poly c = a * 2.0 + b;
  c += poly(0, 3);
  c -= b;
  c *= 0.5;
  assert(c + -poly(0, 1.5) == a + (c - c));
#endif

#if TEST_NUM == 204
  // testy na arytmetykę – trzy zmienne
  poly x(0, 1);
  poly y(const_poly(x));
  poly z(const_poly(y));
  assert(pow<3>(-x + y + z) + pow<3>(x - y + z) + pow<3>(x + y - z)
    == pow<3>(x + y + z) - 24 * x * y * z);
  assert(pow<2>(x + y + z)
    == pow<2>(z) + pow<2>(y) + pow<2>(x) + 2 * (z * y + y * x + x * z));
#endif

#if TEST_NUM == 205
  poly a(poly(1, 2), 3, poly(4, 5, 6), poly(7));
  poly b(1, 2, poly(3, 4, 5));
  a = b;
  // Nie powinno się kompilować, bo a nie mieści się w b.
  b = a;
#endif

#if TEST_NUM == 206
  poly a(poly(1, 2), 3, poly(4, 5, 6), poly(7));
  poly b(1, 2, 3, poly(4, 5));
  // Nie powinno się kompilować, bo a nie mieści się w b ze względu na drugi wymiar.
  b = a;
#endif

#if TEST_NUM == 207
  poly a(poly(1, 2), 3, poly(4, 5, 6), poly(7));
  poly b(1, 2, poly(3, 4, 5));
  a = b;
  // Nie powinno się kompilować, bo a nie mieści się w b.
  b = std::move(a);
#endif

#if TEST_NUM == 208
  poly a(poly(1, 2), 3, poly(4, 5, 6), poly(7));
  poly b(1, 2, 3, poly(4, 5));
  // Nie powinno się kompilować, bo a nie mieści się w b ze względu na drugi wymiar.
  b = std::move(a);
#endif

#if TEST_NUM == 209
  poly a(-1, 1);
  poly b(2, 3, -4, 2);
  // Nie powinno się kompilować, bo b nie mieści się w a.
  a += b;
#endif

#if TEST_NUM == 210
  poly c(2.5, -1.1);
  poly d(-1.7, 3.8, 3.2, 1.3);
  // Nie powinno się kompilować, bo d nie mieści się w c.
  c -= std::move(d);
#endif

#if TEST_NUM == 211
  poly a(-1, 1);
  poly b(2, 3, -4, 2);
  // Nie powinno się kompilować, bo a jest wielomianem.
  b *= a;
#endif

#if TEST_NUM == 212
  poly a = poly(-1, poly(0, 2, -2), poly(1, 3));
  poly b = poly(-5, poly(1, -2, 0, 1));
  // Nie powinno się kompilować.
  a += b;
#endif

#if TEST_NUM == 213
  poly a = poly(-1, poly(0, 2, -2), poly(1, 3));
  poly b = poly(-5, poly(1, -2, 0, 1));
  // Nie powinno się kompilować.
  b -= std::move(a);
#endif

#if TEST_NUM == 301
  poly a(-2.0, 1, -2, 1);
  assert(a.at(1.5) == -1.625);
  assert(a.at(1.3) == a.at(std::array{1.3}));
  poly b(poly(-1.0, 2.0), poly(-3.5, 4.5, 1.0), poly(0, 0, 0, 1.0));
  assert(b.at(-2) == poly(6.0, -7, -2, 4));
  assert(b.at(0.5, -2) == -11.25);
  assert(b.at(7) == b.at(std::array{7}));
  assert(b.at(5, -3) == b.at(std::array{5, -3}));
  assert(b.at() == b);
  assert(b.at(0.5, -2, 3) == b.at(0.5, -2));
  assert(b.at(poly(1)) == poly(-4.5, 6.5, 1, 1));
  assert(a.at(poly(0, 1)) == a);
  assert(b.at(poly(1, 0, 0, 1)) == poly(-4.5, 6.5, 1, -2.5, 4.5, 1, 2, 0, 0, 1));
  poly x(0, 1);
  poly y(const_poly(x));
  poly z(const_poly(y));
  poly s(pow<3>(x + y + z));
  assert(s.at(s, -s, s) == decltype(s.at(s, -s, s))(pow<3>(s)));
#endif

#if TEST_NUM == 302
  poly a(-2, 1, -2, 1);
  // Nie powinno się kompilować.
  [[maybe_unused]] auto y = a.at("string");
#endif

#if TEST_NUM == 401
  // Funkcja cross powinna też działać na argumentach niebędących wielomianami,
  // ale to wymaganie nie zostało sformułowane ani w treści zadania,
  // ani w przykładzie użycia.
  // assert(cross(3, 4) == 12);
  // assert(cross(3.0, poly(1, 2, 3)) == poly(3.0, 6.0, 9.0));
  // assert(cross(poly(poly(1, 2), poly(3, 4), 5), 2.0)
  //   == poly(poly(2.0, 4.0), poly(6.0, 8.0), 10.0));
  poly x(0, 1);
  poly y(const_poly(x));
  poly z(const_poly(y));
  poly w(const_poly(z));
  assert(cross(x, y) == x * z);
  assert(cross(y, x) == y * z);
  assert(cross(y, y) == y * w);
#endif

#if TEST_NUM == 402
  // Nie powinno się kompilować.
  [[maybe_unused]] auto p = cross(2, std::string("text"));
#endif

#if TEST_NUM == 501
  // Ten test jest w pliku poly_test_extern.cc.
#endif

#if TEST_NUM == 502
  // constexpr z przykładu
  constexpr auto p = poly(2, 1);
  static_assert(std::is_same_v<decltype(p), const poly<int, 2>>);
  static_assert(p[0] == 2);
  static_assert(p.at(1) == 3);
  static_assert(p.at(0.5) == 2.5);
  static_assert(p.at(p) == poly(4, 1));

  static_assert(std::is_same_v<std::common_type_t<double, poly<int, 2>>, poly<double, 2>>);
  static_assert(std::is_same_v<std::common_type_t<poly<int, 2>, poly<double, 2>>, poly<double, 2>>);

  constexpr auto q = poly(poly(1.0, 2.0), 3.0, 4.0);
  static_assert(std::is_same_v<decltype(q), const poly<poly<double, 2>, 3>>);
  static_assert(q[0][0] == 1.0);
  static_assert(q[0][1] == 2.0);
  static_assert(q[1][0] == 3.0);
  static_assert(q[1][1] == 0.0);
  static_assert(q[2][0] == 4.0);
  static_assert(q[2][1] == 0.0);
  static_assert(q.at() == q);
  static_assert(q.at(2.0, 3.0) == 29.0);
  static_assert(q.at(2.0, 3.0, 4.0) == 29.0);
  static_assert(q.at(2.0) == poly(23.0, 2.0));
  static_assert(q.at(2.0).at(3.0) == q.at(2.0, 3.0));
  static_assert(q.at(p) == poly(23.0, 21.0, 4.0, 0.0));
  static_assert(q.at(-1, p) == poly(6.0, 2.0));
  static_assert(q.at(std::array{2.0, 3.0}) == 29.0);

  static_assert(std::is_same_v<decltype(p + q), poly<poly<double, 2>, 3>>);
  static_assert(p + q == poly(poly(3.0, 2.0), 4.0, 4.0));
  static_assert(-q == poly(poly(-1.0, -2.0), -3.0, -4.0));
  static_assert(p - q == poly(poly(1.0, -2.0), -2.0, -4.0));
  static_assert(q - p == poly(poly(-1.0, 2.0), 2.0, 4.0));
  static_assert(std::is_same_v<decltype(p * q), poly<poly<double, 2>, 4>>);
  static_assert(p * q == poly(poly(2.0, 4.0), poly(7.0, 2.0), 11.0, 4.0));
  static_assert(cross(p, q) == poly(2.0 * q, q));
  static_assert(cross(q, p) == poly(poly(p, 2.0 * p), const_poly(3.0 * p), const_poly(4.0 * p)));

  constexpr auto pp(p);
  static_assert(p == pp);
  constexpr auto qq(q);
  static_assert(q == qq);
#endif

#if TEST_NUM == 503
  // kilka dodatkowych testów na constexpr
  constexpr poly x(0, 1);
  constexpr poly cube(pow<3>(x));
  constexpr poly y(const_poly(x));
  constexpr poly z(const_poly(y));
  static_assert(pow<3>(x + y + z) + cube.at(x) + cube.at(y) + cube.at(z)
    == cube.at(x + y) + cube.at(y + z) + cube.at(z + x) + 6 * x * y * z);
#endif

#if TEST_NUM == 504
  constexpr poly<int> zero;
  static_assert(zero.size() == 0);
  constexpr poly<poly<poly<int, 1>, 0>, 1> zero_3;
  static_assert(zero_3.size() == 0 || zero_3[0].size() == 0);
  constexpr auto p = zero_3 * const_poly(poly(3, poly(2, 1)));
  static_assert(p.size() == 0 || (p.size() == 1 && p[0].size() == 0));
  constexpr poly a = poly(3.0, -1.0, 2.0);
  constexpr poly b = const_poly(poly(a, 1, -a));
  static_assert(zero + a == a);
  static_assert(zero_3 - b == -b);
  constexpr auto z0 = zero.at(0);
  assert(as_string(z0) == "0" || as_string(z0) == "{}");
  constexpr auto z3 = zero_3.at(0, 0, 0);
  assert(as_string(z3) == "0" || as_string(z3) == "{}" || as_string(z3) == "{0}");
#endif

#if TEST_NUM == 505
  poly<int, 3> a;
  poly<int, 3> b(1);
  poly<int, 3> c(1, 2);
  poly<int, 3> d(1, 2, 3);
  poly u(a, b, c, d);
  const poly v(c, u);
  assert(v.size() == 2);
  assert(v[0].size() == 4);
  assert(v[0][0].size() == 3);
  poly x(v);
  assert(x.size() == 2);
  assert(x[0].size() == 4);
  assert(x[0][0].size() == 3);
  const poly zc(poly(3.14, 2.7), -1.1);
  assert(as_string(poly<poly<int, 3>, 3>(zc)) == "{{3,2,0},{-1,0,0},{0,0,0}}");
#endif

#if TEST_NUM == 506
  const poly v(1);
  // Nie powinno się kompilować, bo operator[] const.
  v[0] = 0;
#endif

#if TEST_NUM == 507
  poly<int, 3> a;
  poly<int, 3> b(1);
  poly<int, 3> c(1, 2);
  poly<int, 3> d(1, 2, 3);
  poly u(a, b, c, d);
  const poly v(c, u);
  // Nie powinno się kompilować, bo operator[] const.
  v[0][1][0] = 0;
#endif

#if TEST_NUM == 601
  using t = wrapper<int>;
  auto pp = poly(t(0), 1);
  auto p = std::move(pp);
  assert(t::copies() == 0);
  t one(1);
  auto qq = poly(one);
  auto q(std::move(qq));
  assert(t::copies() == 1);
  p += q;
  p -= q;
  assert(t::copies() == 1);
  p = q;
  assert(t::copies() == 2);
  p *= 2;
  p *= t(2);
  assert(t::copies() == 2);
  poly<poly<t, 2>, 2> p_copy(p);
  assert(t::copies() == 4);
  poly<poly<t, 2>, 2> p_move(std::move(p));
  assert(t::copies() == 4);

  using common_t = std::common_type_t<poly<poly<wrapper<int>, 2>, 1>, poly<wrapper<unsigned>, 2>>;
  static_assert(std::is_same_v<common_t, poly<poly<wrapper<long>, 2>, 2>>);
#endif

#if TEST_NUM == 602
  // Nie powinno się kompilować.
  using t = weak_wrapper<int>;
  [[maybe_unused]] auto y = t(2) + t(3);
#endif

#if TEST_NUM == 603
  // Nie powinno się kompilować.
  using t = weak_wrapper<int>;
  [[maybe_unused]] auto y = t(2) - t(3);
#endif

#if TEST_NUM == 604
  // Nie powinno się kompilować: wrapper<int> niekonwertowalny do int.
  using t = wrapper<int>;
  poly p(t(1), 2);
  poly q(0, 0);
  q += p;
#endif
}
