#ifndef BN128_H_
#define BN128_H_

#include <intx/intx.hpp>

namespace bn128 {

// Maybe there is a better way to implement this macro, but this is enough for
// now.
#ifdef __riscv
#undef assert
#define assert(x)                                                              \
  if (!x)                                                                      \
  exit(2)
#endif

using uint256 = intx::uint256;

inline bool eq2(const uint256 x[2], const uint256 y[2]) {
  return x[0] == y[0] && x[1] == y[1];
}

inline bool eq12(const uint256 x[12], const uint256 y[12]) {
  return x[0] == y[0] && x[1] == y[1] && x[2] == y[2] && x[3] == y[3] &&
         x[4] == y[4] && x[5] == y[5] && x[6] == y[6] && x[7] == y[7] &&
         x[8] == y[8] && x[9] == y[9] && x[10] == y[10] && x[11] == y[11];
}

inline void cp2(const uint256 x[2], uint256 r[2]) {
  r[0] = x[0];
  r[1] = x[1];
}

inline void cp12(const uint256 x[12], uint256 r[12]) {
  r[0] = x[0];
  r[1] = x[1];
  r[2] = x[2];
  r[3] = x[3];
  r[4] = x[4];
  r[5] = x[5];
  r[6] = x[6];
  r[7] = x[7];
  r[8] = x[8];
  r[9] = x[9];
  r[10] = x[10];
  r[11] = x[11];
}

inline void cp13(const uint256 x[13], uint256 r[13]) {
  cp12(x, r);
  r[12] = x[12];
}

// The prime modulus of the field.
constexpr uint256 FIELD_MODULUS = intx::from_string<uint256>(
    "0x30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47");

inline uint256 _addmod(const uint256 &x, const uint256 &y, const uint256 &n) {
  return intx::addmod(x, y, n);
}

inline uint256 _submod(const uint256 &x, const uint256 &y, const uint256 &n) {
  return _addmod(x, n - y, n);
}

inline uint256 _negmod(const uint256 &x, const uint256 &n) {
  return _submod(n, x, n);
}

inline uint256 _mulmod(const uint256 &x, const uint256 &y, const uint256 &n) {
  return intx::mulmod(x, y, n);
}

// Extended euclidean algorithm to find modular inverses for integers.
inline uint256 _invmod(const uint256 &x, const uint256 &n) {
  uint256 t = 0;
  uint256 newt = 1;
  uint256 r = n;
  uint256 newr = x;
  while (newr) {
    uint256 q = r / newr;
    uint256 oldt = t;
    t = newt;
    newt = _submod(oldt, _mulmod(q, newt, n), n);
    uint256 oldr = r;
    r = newr;
    newr = oldr - q * newr;
  }
  return t;
}

inline uint256 _divmod(const uint256 &x, const uint256 &y, const uint256 &n) {
  return _mulmod(x, _invmod(y, FIELD_MODULUS), FIELD_MODULUS);
}

inline uint256 _powmod(const uint256 &x, const uint256 &y, const uint256 &n) {
  if (y == 0) {
    return 1;
  } else if (y == 1) {
    return x;
  } else if (y & 1) {
    return _mulmod(_powmod(_mulmod(x, x, n), y >> 1, n), x, n);
  } else {
    return _powmod(_mulmod(x, x, n), y >> 1, n);
  }
}

inline uint256 fq_add(const uint256 &x, const uint256 &y) {
  return _addmod(x, y, FIELD_MODULUS);
}

inline uint256 fq_sub(const uint256 &x, const uint256 &y) {
  return _submod(x, y, FIELD_MODULUS);
}

inline uint256 fq_neg(const uint256 &x) { return _negmod(x, FIELD_MODULUS); }

inline uint256 fq_mul(const uint256 &x, const uint256 &y) {
  return _mulmod(x, y, FIELD_MODULUS);
}

inline uint256 fq_inv(const uint256 &x) { return _invmod(x, FIELD_MODULUS); }

inline uint256 fq_div(const uint256 &x, const uint256 &y) {
  return _divmod(x, y, FIELD_MODULUS);
}

inline uint256 fq_pow(const uint256 &x, const uint256 &y) {
  return _powmod(x, y, FIELD_MODULUS);
}

// The quadratic extension field.
constexpr uint256 FQ2_ONE[2] = {1, 0};
constexpr uint256 FQ2_ZERO[2] = {0, 0};

void fq2_add(const uint256 x[2], const uint256 y[2], uint256 r[2]) {
  uint256 a = fq_add(x[0], y[0]);
  uint256 b = fq_add(x[1], y[1]);
  r[0] = a;
  r[1] = b;
}

void fq2_sub(const uint256 x[2], const uint256 y[2], uint256 r[2]) {
  uint256 a = fq_sub(x[0], y[0]);
  uint256 b = fq_sub(x[1], y[1]);
  r[0] = a;
  r[1] = b;
}

void fq2_neg(const uint256 x[2], uint256 r[2]) {
  uint256 a = fq_neg(x[0]);
  uint256 b = fq_neg(x[1]);
  r[0] = a;
  r[1] = b;
}

void fq2_mul(const uint256 x[2], const uint256 y[2], uint256 r[2]) {
  uint256 a = fq_sub(fq_mul(x[0], y[0]), fq_mul(x[1], y[1]));
  uint256 b = fq_add(fq_mul(x[0], y[1]), fq_mul(x[1], y[0]));
  r[0] = a;
  r[1] = b;
}

void fq2_muc(const uint256 x[2], const uint256 &c, uint256 r[2]) {
  uint256 a = fq_mul(x[0], c);
  uint256 b = fq_mul(x[1], c);
  r[0] = a;
  r[1] = b;
}

void fq2_inv(const uint256 x[2], uint256 r[2]) {
  uint256 i = fq_inv(fq_add(fq_mul(x[0], x[0]), fq_mul(x[1], x[1])));
  uint256 a = fq_mul(x[0], i);
  uint256 b = fq_sub(FIELD_MODULUS, fq_mul(x[1], i));
  r[0] = a;
  r[1] = b;
}

void fq2_div(const uint256 x[2], const uint256 y[2], uint256 r[2]) {
  uint256 t[2];
  fq2_inv(y, t);
  fq2_mul(x, t, r);
}

void fq2_dic(const uint256 x[2], const uint256 &c, uint256 r[2]) {
  uint256 a = fq_div(x[0], c);
  uint256 b = fq_div(x[1], c);
  r[0] = a;
  r[1] = b;
}

void fq2_pow(const uint256 x[2], const uint256 &y, uint256 r[2]) {
  if (y == 0) {
    cp2(FQ2_ONE, r);
  } else if (y == 1) {
    cp2(x, r);
  } else if (y & 1) {
    uint256 t[2];
    fq2_mul(x, x, t);
    fq2_pow(t, y >> 1, t);
    fq2_mul(x, t, r);
  } else {
    fq2_mul(x, x, r);
    fq2_pow(r, y >> 1, r);
  }
}

// The 12th-degree extension field.
constexpr uint256 FQ12_ONE[12] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
constexpr uint256 FQ12_ZERO[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// The modulus of the polynomial in this representation of FQ12.
constexpr uint256 FQ12_MODULUS_COEFFS[12] = {
    82, 0, 0, 0, 0, 0, FIELD_MODULUS - 18, 0, 0, 0, 0, 0};

void fq12_add(const uint256 x[12], const uint256 y[12], uint256 r[12]) {
  for (int i = 0; i < 12; i++) {
    r[i] = fq_add(x[i], y[i]);
  }
}

void fq12_sub(const uint256 x[12], const uint256 y[12], uint256 r[12]) {
  for (int i = 0; i < 12; i++) {
    r[i] = fq_sub(x[i], y[i]);
  }
}

void fq12_neg(const uint256 x[12], uint256 r[12]) {
  for (int i = 0; i < 12; i++) {
    r[i] = fq_neg(x[i]);
  }
}

void fq12_mul(const uint256 x[12], const uint256 y[12], uint256 r[12]) {
  uint256 b[23] = {0};
  uint256 eli;
  uint256 elj;
  uint256 top;
  for (int i = 0; i < 12; i++) {
    eli = x[i];
    for (int j = 0; j < 12; j++) {
      elj = y[j];
      b[i + j] = fq_add(b[i + j], fq_mul(eli, elj));
    }
  }
  for (int exp = 10; exp > -1; exp--) {
    top = b[exp + 12];
    b[exp] = fq_sub(b[exp], fq_mul(top, FQ12_MODULUS_COEFFS[0]));
    b[exp + 6] = fq_sub(b[exp + 6], fq_mul(top, FQ12_MODULUS_COEFFS[6]));
  }
  cp12(b, r);
}

void fq12_muc(const uint256 x[12], const uint256 &c, uint256 r[12]) {
  for (int i = 0; i < 12; i++) {
    r[i] = fq_mul(x[i], c);
  }
}

// Utility methods for polynomial math.
int _deg(const uint256 p[13]) {
  int d = 12;
  while (p[d] == 0 && d != 0) {
    d--;
  }
  return d;
}

void _poly_rounded_div(const uint256 a[13], const uint256 b[13],
                       uint256 r[13]) {
  int dega = _deg(a);
  int degb = _deg(b);
  uint256 t[13] = {};
  cp13(a, t);
  uint256 o[13] = {0};
  for (int i = dega - degb; i > -1; i--) {
    o[i] = fq_add(o[i], fq_div(t[degb + i], b[degb]));
    for (int c = 0; c < degb + 1; c++) {
      t[c + i] = fq_sub(t[c + i], o[c]);
    }
  }
  int n = _deg(o) + 1;
  for (int i = 0; i < n; i++) {
    r[i] = o[i];
  }
}

void fq12_inv(const uint256 x[12], uint256 r[12]) {
  const int degree = 12;
  uint256 lm[degree + 1] = {lm[0] = 1, 0};
  uint256 hm[degree + 1] = {0};
  uint256 low[degree + 1] = {};
  for (int i = 0; i < degree; i++) {
    low[i] = x[i];
  }
  low[degree] = 0;
  uint256 high[degree + 1] = {};
  for (int i = 0; i < degree; i++) {
    high[i] = FQ12_MODULUS_COEFFS[i];
  }
  high[degree] = 1;

  uint256 temp[degree + 1] = {};
  uint256 nm[degree + 1] = {};
  uint256 news[degree + 1] = {};
  while (_deg(low) != 0) {
    for (int i = 0; i < degree + 1; i++) {
      temp[i] = 0;
      nm[i] = hm[i];
      news[i] = high[i];
    }
    _poly_rounded_div(high, low, temp);
    for (int i = 0; i < degree + 1; i++) {
      for (int j = 0; j < degree + 1 - i; j++) {
        nm[i + j] = fq_sub(nm[i + j], fq_mul(lm[i], temp[j]));
        news[i + j] = fq_sub(news[i + j], fq_mul(low[i], temp[j]));
      }
    }
    for (int i = 0; i < degree + 1; i++) {
      hm[i] = lm[i];
      lm[i] = nm[i];
      high[i] = low[i];
      low[i] = news[i];
    }
  }
  for (int i = 0; i < degree; i++) {
    r[i] = fq_div(lm[i], low[0]);
  }
}

void fq12_div(const uint256 x[12], const uint256 y[12], uint256 r[12]) {
  uint256 t[12];
  fq12_inv(y, t);
  fq12_mul(x, t, r);
}

void fq12_dic(const uint256 x[12], const uint256 &c, uint256 r[12]) {
  for (int i = 0; i < 12; i++) {
    r[i] = fq_div(x[i], c);
  }
}

void fq12_pow(const uint256 x[12], const uint256 &y, uint256 r[12]) {
  if (y == 0) {
    r[0] = 1;
    for (int i = 1; i < 12; i++) {
      r[i] = 0;
    }
  } else if (y == 1) {
    for (int i = 0; i < 12; i++) {
      r[i] = x[i];
    }
  } else if (y % 2 == 0) {
    uint256 t[12];
    fq12_mul(x, x, t);
    fq12_pow(t, y >> 1, r);
  } else {
    uint256 t[12];
    fq12_mul(x, x, t);
    fq12_pow(t, y >> 1, r);
    fq12_mul(x, r, r);
  }
}

constexpr uint256 CURVE_ORDER = intx::from_string<uint256>(
    "0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001");
// Curve is y**2 = x**3 + 3
constexpr uint256 B = 3;
// Twisted curve over FQ**2
constexpr uint256 B2[2] = {
    intx::from_string<uint256>(
        "0x2b149d40ceb8aaae81be18991be06ac3b5b4c5e559dbefa33267e6dc24a138e5"),
    intx::from_string<uint256>(
        "0x009713b03af0fed4cd2cafadeed8fdf4a74fa084e52d1852e4a2bd0685c315d2"),
};
// Extension curve over FQ**12; same b value as over FQ
constexpr uint256 B12[12] = {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Generator for curve over FQ
constexpr uint256 G1[2] = {1, 2};
// Generator for twisted curve over FQ2
constexpr uint256 G2[2][2] = {
    {
        intx::from_string<uint256>("0x1800deef121f1e76426a00665e5c4479674322d4f"
                                   "75edadd46debd5cd992f6ed"),
        intx::from_string<uint256>("0x198e9393920d483a7260bfb731fb5d25f1aa49333"
                                   "5a9e71297e485b7aef312c2"),
    },
    {
        intx::from_string<uint256>("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690"
                                   "c43d37b4ce6cc0166fa7daa"),
        intx::from_string<uint256>("0x090689d0585ff075ec9e99ad690c3395bc4b31337"
                                   "0b38ef355acdadcd122975b"),
    }};

// "Twist" a point in E(FQ2) into a point in E(FQ12)
constexpr uint256 W[12] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

constexpr uint256 G12[2][12] = {
    {0, 0,
     intx::from_string<uint256>(
         "0x23f336fd559fb538d6949f86240cb7f7ddcda4df1e9eaff81c78c659ed78407e"),
     0, 0, 0, 0, 0,
     intx::from_string<uint256>(
         "0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2"),
     0, 0, 0},
    {0, 0, 0,
     intx::from_string<uint256>(
         "0x2256233882903a1969b895d4df602107743001bce6d76207c214326bbdbd2605"),
     0, 0, 0, 0, 0,
     intx::from_string<uint256>(
         "0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b"),
     0, 0},
};

namespace g1 {

constexpr uint256 INF[2] = {-1, -1};

// Check if a point is the point at infinity
inline bool is_inf(const uint256 pt[2]) { return eq2(pt, INF); }

// Check that a point is on the curve defined by y**2 == x**3 + b
bool is_on_curve(const uint256 pt[2]) {
  if (is_inf(pt)) {
    return true;
  }
  uint256 l = fq_mul(pt[1], pt[1]);
  uint256 r = fq_add(fq_mul(fq_mul(pt[0], pt[0]), pt[0]), B);
  return l == r;
}

void doubl2(const uint256 pt[2], uint256 r[2]) {
  uint256 x = pt[0];
  uint256 y = pt[1];
  uint256 l = fq_div(fq_mul(fq_mul(x, x), 3), fq_mul(y, 2));
  uint256 newx = fq_sub(fq_mul(l, l), fq_mul(x, 2));
  uint256 newy =
      fq_sub(fq_add(fq_mul(fq_sub(FIELD_MODULUS, l), newx), fq_mul(l, x)), y);
  r[0] = newx;
  r[1] = newy;
}

// Elliptic curve addition
void add(const uint256 p1[2], const uint256 p2[2], uint256 r[2]) {
  if (is_inf(p1)) {
    r[0] = p2[0];
    r[1] = p2[1];
  } else if (is_inf(p2)) {
    r[0] = p1[0];
    r[1] = p1[1];
  } else if (p2[0] == p1[0] && p2[1] == p1[1]) {
    doubl2(p1, r);
  } else if (p2[0] == p1[0]) {
    r[0] = INF[0];
    r[1] = INF[1];
  } else {
    uint256 l = fq_div(fq_sub(p2[1], p1[1]), fq_sub(p2[0], p1[0]));
    uint256 newx = fq_sub(fq_sub(fq_mul(l, l), p1[0]), p2[0]);
    uint256 newy =
        fq_sub(fq_add(fq_mul(fq_neg(l), newx), fq_mul(l, p1[0])), p1[1]);
    r[0] = newx;
    r[1] = newy;
    return;
  }
}

// Elliptic curve point multiplication.
void mul(const uint256 pt[2], const uint256 &n, uint256 r[2]) {
  if (n == 0) {
    r[0] = INF[0];
    r[1] = INF[1];
  } else if (n == 1) {
    r[0] = pt[0];
    r[1] = pt[1];
  } else if (n % 2 == 0) {
    uint256 t[2];
    doubl2(pt, t);
    mul(t, n >> 1, r);
  } else {
    uint256 t1[2];
    uint256 t2[2];
    doubl2(pt, t1);
    mul(t1, n >> 1, t2);
    add(t2, pt, r);
  }
}

} // namespace g1

namespace g2 {

constexpr uint256 INF[2][2] = {{-1, -1}, {-1, -1}};

// Check if a point is the point at infinity
inline bool is_inf(const uint256 pt[2][2]) {
  return eq2(pt[0], INF[0]) && eq2(pt[1], INF[1]);
}

// Check that a point is on the curve defined by y**2 == x**3 + b.
bool is_on_curve(const uint256 pt[2][2]) {
  if (is_inf(pt)) {
    return true;
  }
  uint256 t[2][2];
  fq2_mul(pt[1], pt[1], t[0]);
  fq2_mul(pt[0], pt[0], t[1]);
  fq2_mul(pt[0], t[1], t[1]);
  fq2_add(t[1], B2, t[1]);
  return eq2(t[0], t[1]);
}

void doubl2(const uint256 pt[2][2], uint256 r[2][2]) {
  uint256 t[2];
  uint256 newx[2];
  uint256 newy[2];
  uint256 l[2];

  fq2_mul(pt[0], pt[0], l);
  l[0] = fq_mul(3, l[0]);
  l[1] = fq_mul(3, l[1]);
  t[0] = fq_mul(2, pt[1][0]);
  t[1] = fq_mul(2, pt[1][1]);
  fq2_div(l, t, l);

  fq2_mul(l, l, newx);
  t[0] = fq_mul(2, pt[0][0]);
  t[1] = fq_mul(2, pt[0][1]);
  fq2_sub(newx, t, newx);

  fq2_mul(l, pt[0], t);
  fq2_sub(t, pt[1], t);
  fq2_neg(l, newy);
  fq2_mul(newy, newx, newy);
  fq2_add(newy, t, newy);

  r[0][0] = newx[0];
  r[0][1] = newx[1];
  r[1][0] = newy[0];
  r[1][1] = newy[1];
}

void add(const uint256 p1[2][2], const uint256 p2[2][2], uint256 r[2][2]) {
  if (is_inf(p1)) {
    r[0][0] = p2[0][0];
    r[0][1] = p2[0][1];
    r[1][0] = p2[1][0];
    r[1][1] = p2[1][1];
  } else if (is_inf(p2)) {
    r[0][0] = p1[0][0];
    r[0][1] = p1[0][1];
    r[1][0] = p1[1][0];
    r[1][1] = p1[1][1];
  } else if (eq2(p2[0], p1[0]) && eq2(p2[1], p1[1])) {
    doubl2(p1, r);
  } else if (eq2(p2[0], p1[0])) {
    r[0][0] = INF[0][0];
    r[0][1] = INF[0][1];
    r[1][0] = INF[1][0];
    r[1][1] = INF[1][1];
  } else {
    uint256 t[2];
    uint256 l[2];
    uint256 newx[2];
    uint256 newy[2];
    fq2_sub(p2[1], p1[1], l);
    fq2_sub(p2[0], p1[0], t);
    fq2_div(l, t, l);
    fq2_mul(l, l, newx);
    fq2_sub(newx, p1[0], newx);
    fq2_sub(newx, p2[0], newx);
    fq2_neg(l, newy);
    fq2_mul(newy, newx, newy);
    fq2_mul(l, p1[0], t);
    fq2_add(newy, t, newy);
    fq2_sub(newy, p1[1], newy);
    r[0][0] = newx[0];
    r[0][1] = newx[1];
    r[1][0] = newy[0];
    r[1][1] = newy[1];
  }
}

// Elliptic curve point multiplication.
void mul(const uint256 pt[2][2], const uint256 &n, uint256 r[2][2]) {
  if (n == 0) {
    r[0][0] = INF[0][0];
    r[0][1] = INF[0][1];
    r[1][0] = INF[1][0];
    r[1][1] = INF[1][1];
  } else if (n == 1) {
    r[0][0] = pt[0][0];
    r[0][1] = pt[0][1];
    r[1][0] = pt[1][0];
    r[1][1] = pt[1][1];
  } else if (n % 2 == 0) {
    uint256 t[2][2];
    doubl2(pt, t);
    mul(t, n >> 1, r);
  } else {
    uint256 t[2][2][2];
    doubl2(pt, t[0]);
    mul(t[0], n >> 1, t[1]);
    add(t[1], pt, r);
  }
}

// Elliptic curve point multiplication.
void twist(const uint256 pt[2][2], uint256 r[2][12]) {
  if (is_inf(pt)) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = -1;
      r[1][i] = -1;
    }
    return;
  }
  uint256 xcoeffs[2] = {fq_sub(pt[0][0], fq_mul(pt[0][1], 9)), pt[0][1]};
  uint256 ycoeffs[2] = {fq_sub(pt[1][0], fq_mul(pt[1][1], 9)), pt[1][1]};
  uint256 nx[12] = {xcoeffs[0], 0, 0, 0, 0, 0, xcoeffs[1], 0, 0, 0, 0, 0};
  uint256 ny[12] = {ycoeffs[0], 0, 0, 0, 0, 0, ycoeffs[1], 0, 0, 0, 0, 0};

  uint256 t[2][12];
  fq12_mul(W, W, t[0]);
  fq12_mul(nx, t[0], t[1]);
  for (int i = 0; i < 12; i++) {
    r[0][i] = t[1][i];
  }
  fq12_pow(W, 3, t[0]);
  fq12_mul(ny, t[0], t[1]);
  for (int i = 0; i < 12; i++) {
    r[1][i] = t[1][i];
  }
}

} // namespace g2

namespace g12 {

constexpr uint256 INF[2][12] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

// Check if a point is the point at infinity
inline bool is_inf(const uint256 pt[2][12]) {
  return eq12(pt[0], INF[0]) && eq12(pt[1], INF[1]);
}

// // Check that a point is on the curve defined by y**2 == x**3 + b.
bool is_on_curve(const uint256 pt[2][12]) {
  if (is_inf(pt)) {
    return true;
  }
  uint256 t[2][12];
  fq12_mul(pt[1], pt[1], t[0]);
  fq12_mul(pt[0], pt[0], t[1]);
  fq12_mul(pt[0], t[1], t[1]);
  fq12_add(t[1], B12, t[1]);
  return eq12(t[0], t[1]);
}

void doubl2(const uint256 pt[2][12], uint256 r[2][12]) {
  uint256 t[12];
  uint256 newx[12];
  uint256 newy[12];
  uint256 l[12];

  fq12_mul(pt[0], pt[0], l);
  for (int i = 0; i < 12; i++) {
    l[i] = fq_mul(3, l[i]);
  }
  for (int i = 0; i < 12; i++) {
    t[i] = fq_mul(2, pt[1][i]);
  }
  fq12_div(l, t, l);

  fq12_mul(l, l, newx);
  for (int i = 0; i < 12; i++) {
    t[i] = fq_mul(2, pt[0][i]);
  }
  fq12_sub(newx, t, newx);

  fq12_mul(l, pt[0], t);
  fq12_sub(t, pt[1], t);
  fq12_neg(l, newy);
  fq12_mul(newy, newx, newy);
  fq12_add(newy, t, newy);

  for (int i = 0; i < 12; i++) {
    r[0][i] = newx[i];
    r[1][i] = newy[i];
  }
}

void add(const uint256 p1[2][12], const uint256 p2[2][12], uint256 r[2][12]) {
  if (is_inf(p1)) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = p2[0][i];
      r[1][i] = p2[1][i];
    }
  } else if (is_inf(p2)) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = p1[0][i];
      r[1][i] = p1[1][i];
    }
  } else if (eq12(p2[0], p1[0]) && eq12(p2[1], p1[1])) {
    doubl2(p1, r);
  } else if (eq12(p2[0], p1[0])) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = INF[0][i];
      r[1][i] = INF[1][i];
    }
  } else {
    uint256 t[2][12];
    uint256 l[12];
    uint256 newx[12];
    uint256 newy[12];
    fq12_sub(p2[1], p1[1], t[0]);
    fq12_sub(p2[0], p1[0], t[1]);
    fq12_div(t[0], t[1], l);
    fq12_mul(l, l, newx);
    fq12_sub(newx, p1[0], newx);
    fq12_sub(newx, p2[0], newx);
    fq12_neg(l, newy);
    fq12_mul(newy, newx, newy);
    fq12_mul(l, p1[0], t[0]);
    fq12_add(newy, t[0], newy);
    fq12_sub(newy, p1[1], newy);
    for (int i = 0; i < 12; i++) {
      r[0][i] = newx[i];
      r[1][i] = newy[i];
    }
  }
}

// Elliptic curve point multiplication.
void mul(const uint256 pt[2][12], const uint256 &n, uint256 r[2][12]) {
  if (n == 0) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = INF[0][i];
      r[1][i] = INF[1][i];
    }
  } else if (n == 1) {
    for (int i = 0; i < 12; i++) {
      r[0][i] = pt[0][i];
      r[1][i] = pt[1][i];
    }
  } else if (n % 2 == 0) {
    uint256 t[2][12];
    doubl2(pt, t);
    mul(t, n >> 1, r);
  } else {
    uint256 t[2][2][12];
    doubl2(pt, t[0]);
    mul(t[0], n >> 1, t[1]);
    add(t[1], pt, r);
  }
}

} // namespace g12

constexpr uint256 ATE_LOOP_COUNT =
    intx::from_string<uint256>("0x19d797039be763ba8");
constexpr int LOG_ATE_LOOP_COUNT = 63;

// Create a function representing the line between P1 and P2, and evaluate it at
// T
uint256 linefunc(const uint256 p1[2], const uint256 p2[2],
                 const uint256 pt[2]) {
  // No points-at-infinity allowed, sorry
  assert(!(g1::is_inf(p1) || g1::is_inf(p2) || g1::is_inf(pt)));
  uint256 x1 = p1[0], y1 = p1[1];
  uint256 x2 = p2[0], y2 = p2[1];
  uint256 xt = pt[0], yt = pt[1];
  if (x1 != x2) {
    uint256 m = fq_div(fq_sub(y2, y1), fq_sub(x2, x1));
    return fq_sub(fq_mul(m, fq_sub(xt, x1)), fq_sub(yt, y1));
  } else if (y1 == y2) {
    uint256 m = fq_div(fq_mul(3, fq_mul(x1, x1)), fq_mul(2, y1));
    return fq_sub(fq_mul(m, fq_sub(xt, x1)), fq_sub(yt, y1));
  } else {
    return fq_sub(xt, x1);
  }
}

// Create a function representing the line between P1 and P2, and evaluate it at
// T
void linefunc12(const uint256 p1[2][12], const uint256 p2[2][12],
                const uint256 pt[2][12], uint256 r[12]) {
  // No points-at-infinity allowed, sorry
  assert(!(g12::is_inf(p1) || g12::is_inf(p2) || g12::is_inf(pt)));
  uint256 tmp[3][12] = {};
  if (!eq12(p1[0], p2[0])) {
    fq12_sub(p2[1], p1[1], tmp[0]);
    fq12_sub(p2[0], p1[0], tmp[1]);
    fq12_div(tmp[0], tmp[1], tmp[2]);
    fq12_sub(pt[0], p1[0], tmp[1]);
    fq12_mul(tmp[2], tmp[1], tmp[0]);
    fq12_sub(pt[1], p1[1], tmp[1]);
    fq12_sub(tmp[0], tmp[1], r);
  } else if (eq12(p1[1], p2[1])) {
    fq12_mul(p1[0], p1[0], tmp[0]);
    for (int i = 0; i < 12; i++) {
      tmp[0][i] = fq_mul(3, tmp[0][i]);
    }
    for (int i = 0; i < 12; i++) {
      tmp[1][i] = fq_mul(2, p1[1][i]);
    }
    fq12_div(tmp[0], tmp[1], tmp[2]);
    fq12_sub(pt[0], p1[0], tmp[1]);
    fq12_mul(tmp[2], tmp[1], tmp[0]);
    fq12_sub(pt[1], p1[1], tmp[1]);
    fq12_sub(tmp[0], tmp[1], r);
  } else {
    fq12_sub(pt[0], p1[0], r);
  }
}

void final_exponentiate(const uint256 x[12], const intx::uint<4096> &y,
                        uint256 r[12]) {
  if (y == 0) {
    r[0] = 1;
    for (int i = 1; i < 12; i++) {
      r[i] = 0;
    }
  } else if (y == 1) {
    for (int i = 0; i < 12; i++) {
      r[i] = x[i];
    }
  } else if (y % 2 == 0) {
    uint256 t[12];
    fq12_mul(x, x, t);
    final_exponentiate(t, y >> 1, r);
  } else {
    uint256 t[12];
    fq12_mul(x, x, t);
    final_exponentiate(t, y >> 1, r);
    fq12_mul(x, r, r);
  }
}

// Main miller loop
void miller_loop(const uint256 q[2][12], const uint256 p[2][12],
                 uint256 r[12]) {
  if (g12::is_inf(q) || g12::is_inf(p)) {
    cp12(FQ12_ONE, r);
    return;
  }
  uint256 t[2][12] = {};
  uint256 R[2][12] = {};
  cp12(q[0], R[0]);
  cp12(q[1], R[1]);
  uint256 f[12] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 63; i > -1; i--) {
    linefunc12(R, R, p, t[0]);
    fq12_mul(t[0], f, t[1]);
    fq12_mul(t[1], f, t[0]);
    cp12(t[0], f);
    g12::doubl2(R, t);
    cp12(t[0], R[0]);
    cp12(t[1], R[1]);
    if ((ATE_LOOP_COUNT & fq_pow(2, i)) != 0) {
      linefunc12(R, q, p, t[0]);
      fq12_mul(t[0], f, t[1]);
      cp12(t[1], f);
      g12::add(R, q, t);
      cp12(t[0], R[0]);
      cp12(t[1], R[1]);
    }
  }
  g12::mul(q, ATE_LOOP_COUNT, t);
  assert(eq12(R[0], t[0]) && eq12(R[1], t[1]));

  uint256 Q1[2][12] = {};
  fq12_pow(q[0], FIELD_MODULUS, Q1[0]);
  fq12_pow(q[1], FIELD_MODULUS, Q1[1]);
  assert(g12::is_on_curve(Q1));
  uint256 nQ2[2][12] = {};
  fq12_pow(Q1[0], FIELD_MODULUS, nQ2[0]);
  fq12_neg(Q1[1], t[0]);
  fq12_pow(t[0], FIELD_MODULUS, nQ2[1]);
  assert(g12::is_on_curve(nQ2));

  linefunc12(R, Q1, p, t[0]);
  fq12_mul(f, t[0], t[1]);
  cp12(t[1], f);
  g12::add(R, Q1, t);
  cp12(t[0], R[0]);
  cp12(t[1], R[1]);
  linefunc12(R, nQ2, p, t[0]);
  fq12_mul(f, t[0], t[1]);
  cp12(t[1], f);

  intx::uint<4096> n =
      (intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} *
           intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} *
           intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} *
           intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} *
           intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} *
           intx::uint<4096>{FIELD_MODULUS} * intx::uint<4096>{FIELD_MODULUS} -
       1) /
      intx::uint<4096>{CURVE_ORDER};
  final_exponentiate(f, n, r);
}

// Pairing computation
void pairing(const uint256 q[2][2], const uint256 p[2], uint256 r[12]) {
  assert(g2::is_on_curve(q));
  assert(g1::is_on_curve(p));
  uint256 twist_q[2][12];
  g2::twist(q, twist_q);
  uint256 fq12_p[2][12] = {{p[0], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {p[1], 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  miller_loop(twist_q, fq12_p, r);
}

} // namespace bn128

#endif /* BN128_H_ */
