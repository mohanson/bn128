#include <bn128_mont.hpp>
#include <intx/intx.hpp>

using namespace bn128;

int test_invmod() {
  constexpr uint256 x_case[8] = {
      h256("0x0000000000000000000000000000000000000000000000000000000000000001"),
      h256("0x00000000000000000000000012341234abcd0000000000000000000000000001"),
      h256("0x0000000000000000000000000000000000000000000fffffffffffffffffffff"),
      h256("0x0123456789a00000000000000000000000000000000000000000000000000001"),
      h256("0x0000000000000000000000000000000000000000000000000000000000000007"),
      h256("0x30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd07"),
      h256("0x00644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd47"),
      h256("0x30644e72e131a029b82222228181585d97816a916871ca8d3c208c16d87cfd47"),
  };
  for (int i = 0; i < 8; i++) {
    uint256 a = x_case[i];
    uint256 r = _mulmod(a, _invmod(a, FIELD_MODULUS), FIELD_MODULUS);
    if (r != 1) {
      return 1;
    }
  }
  return 0;
}

int test_powmod() {
  uint256 r = _powmod(2, 10, FIELD_MODULUS);
  if (r != 1024) {
    return 1;
  }
  return 0;
}

int test_constexpr() {
  // R * R_PRIME % FIELD_MODULUS == 1
  if (uint512(1, 0) * uint512(R_PRIME) % uint512(FIELD_MODULUS) != 1) {
    return 1;
  }
  // -FIELD_MODULUS * FIELD_MODULUS_PRIME % R == 1
  if ((uint512(1, 0) - uint512(FIELD_MODULUS)) * uint512(FIELD_MODULUS_PRIME) % uint512(1, 0) != 1) {
    return 1;
  }
  if (mont_encode(0) != FQ_ZERO) {
    return 1;
  }
  if (mont_encode(1) != FQ_ONE) {
    return 1;
  }
  if (mont_encode(3) != G1_COEFF_B) {
    return 1;
  }
  // G2_COEFF_B0 = mont_encode(0x2b149d40ceb8aaae81be18991be06ac3b5b4c5e559dbefa33267e6dc24a138e5)
  if (mont_encode(h256("0x2b149d40ceb8aaae81be18991be06ac3b5b4c5e559dbefa33267e6dc24a138e5")) != G2_COEFF_B.c0.c0) {
    return 1;
  }
  // G2_COEFF_B1 = mont_encode(0x009713b03af0fed4cd2cafadeed8fdf4a74fa084e52d1852e4a2bd0685c315d2)
  if (mont_encode(h256("0x009713b03af0fed4cd2cafadeed8fdf4a74fa084e52d1852e4a2bd0685c315d2")) != G2_COEFF_B.c1.c0) {
    return 1;
  }

  return 0;
}

int test_mont_encode_decode() {
  // Taking from https://github.com/paritytech/bn
  uint256 a = mont_encode(1);
  if (a != h256("0xe0a77c19a07df2f666ea36f7879462c0a78eb28f5c70b3dd35d438dc58f0d9d")) {
    return 1;
  }
  if (mont_decode(a) != 1) {
    return 1;
  }

  uint256 b = mont_encode(42);
  if (b != h256("0x0903f860b6f71bd22a638bbbb1d55ed69dc595e76d525985dbc68430439c5c6e")) {
    return 1;
  }
  if (mont_decode(b) != 42) {
    return 1;
  }

  uint256 c = FIELD_MODULUS / 2 - 17;
  uint256 d = mont_encode(c);
  if (d != h256("0x14707fbbcf072f27f529534d0bfd1a000a03b6d2f156954ed7d2e44ca56802cb")) {
    return 1;
  }
  if (mont_decode(d) != c) {
    return 1;
  }

  return 0;
}

int test_fq_inv() {
  FQ a = FQ(mont_encode(10));
  FQ b = a.inv();
  // a = 0x2ba010aa41eb77868fb1d6edb1ba0cfd39b65a76c8e2db4fc9638b5c069c8d94
  // b = 0x0febf04f6c8facc474bcbedb7fb2ee53ae7fb77c84b60ab08d93172ea1809a25
  if (b != FQ(h256("0x0febf04f6c8facc474bcbedb7fb2ee53ae7fb77c84b60ab08d93172ea1809a25"))) {
    return 1;
  }
  return 0;
}

int test_fq_neg() {
  FQ a = FQ(mont_encode(42));
  FQ b = -a;
  // a = 0x0903f860b6f71bd22a638bbbb1d55ed69dc595e76d525985dbc68430439c5c6e
  // b = 0x276056122a3a84578decb9facfabf986f9bbd4a9fb1f7107605a07e694e0a0d9
  if (b != FQ(h256("0x276056122a3a84578decb9facfabf986f9bbd4a9fb1f7107605a07e694e0a0d9"))) {
    return 1;
  }
  return 0;
}

int test_fq2_mul() {
  FQ2 a = FQ2(FQ(h256("0x0010b52d9fe70d08c967a97deeb9eb186da14c608196f376d63ca9589ca5990e")),
              FQ(h256("0x2f682d1f7dda8678b0d017978b3067b74807a5d49d2a41739659c6600a8bf018")));
  FQ2 b = FQ2(FQ(h256("0x19015293267c8307ffb557fd4ad6052cc22e04f121f21da65bbe2a733c22c53d")),
              FQ(h256("0x0e2bf3144f8ca0808b1dfce33c2240c641ff2f7b8f2ca8c185201c5edcc67040")));
  FQ2 c = a * b;
  FQ2 r = FQ2(FQ(h256("0x1a458f1555acd5430609a64acd087c155541125d671ced7f65dcb5a4e48e7d6d")),
              FQ(h256("0x08a9a40e08f6e1ee5c2997247f30e5d49235ef40a999734c3ae55e9726f7f1c8")));
  if (c != r) {
    return 1;
  }
  return 0;
}

int test_fq2_inv() {
  FQ2 a = FQ2(FQ(h256("0x0010b52d9fe70d08c967a97deeb9eb186da14c608196f376d63ca9589ca5990e")),
              FQ(h256("0x2f682d1f7dda8678b0d017978b3067b74807a5d49d2a41739659c6600a8bf018")));
  FQ2 b = a.inv();
  FQ2 r = FQ2(FQ(h256("0x2e99d10b04272627f24f9f0c41e928004b3bdc31880830c2ff2bbe19546ec5d1")),
              FQ(h256("0x2b5c2767a083b12aae7fe0d7b0682b5813f1a5566bc3e345b985aa42dd595820")));
  if (b != r) {
    return 1;
  }
  return 0;
}

int test_fq2_squared() {
  FQ2 a = FQ2(FQ(h256("0x0020b52d9fe70d08c967a97deeb9eb186da14c608196f376d63ca9589ca5970f")),
              FQ(h256("0x2b782d1f7dda8678b0d017978b3067b74807a5d49d2a41739659c6600a8bf015")));
  FQ2 b = a.squared();
  FQ2 c = a * a;
  if (b != c) {
    return 1;
  }
  FQ2 d = FQ2(10, 20);
  FQ2 e = d.squared();
  FQ2 f = d * d;
  if (e != f) {
    return 1;
  }
  return 0;
}

int test_fq6_inv() {
  FQ6 a = FQ6(FQ2(h256("0x1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed"),
                  h256("0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2")),
              FQ2(h256("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa"),
                  h256("0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b")),
              FQ2(h256("0x047d62b471a91cfa91fb08d4344792a0355cc0471784adb20e9a99bf11286133"),
                  h256("0x19c7a2f5e6eedf85ec5adff3e0c2028a52b78e15a2e081513a5e5c6309cf7cc2")));
  FQ6 b = a.inv();
  FQ6 c = FQ6(FQ2(h256("0x0d5804da621fee188d4d3175eaa049b655203f524bf4fc530ffc42d7f4d7c07b"),
                  h256("0x07874ef0b6d425106deee51f93ea945fa531451e9ecdd39fdc5d89f19262006c")),
              FQ2(h256("0x2a5e34d590461f51d853b6b6b01fc6cbdba22c3a4208d22eaae6076b4c1333ff"),
                  h256("0x076f32da39aab611469d7d4544a7df049686b51f8e737fca55c94585b4589021")),
              FQ2(h256("0x204254b4bd4e8d3a42d52806cf82352d89e92f532018efc71bae9146ba998d2b"),
                  h256("0x2350e0848205f48d00eeac6d57bc144b14f99883e8a39d601e500d03bdc099c3")));
  if (b != c) {
    return 1;
  }
  return 0;
}

int test_fq6_squared() {
  FQ6 a = FQ6(FQ2(h256("0x1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed"),
                  h256("0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2")),
              FQ2(h256("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa"),
                  h256("0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b")),
              FQ2(h256("0x047d62b471a91cfa91fb08d4344792a0355cc0471784adb20e9a99bf11286133"),
                  h256("0x19c7a2f5e6eedf85ec5adff3e0c2028a52b78e15a2e081513a5e5c6309cf7cc2")));
  FQ6 b = a.squared();
  FQ6 c = FQ6(FQ2(h256("0x2c1aa3c13cf206dd8e756e7a426504343c756ca4e891dd1a5b6124a4b95118ca"),
                  h256("0x006eaa3f0885d6c6e94bec92fcac3f66d591ed269a575b6acf66d2e42adb2181")),
              FQ2(h256("0x074720caa92f241f97477120e2a6e77ddf97f5c7333deb99559470dbb8bf8ca8"),
                  h256("0x2b223b0bb9a271cc03a8b3971a892452cadbd19bb34b87a8db6429642cbb6304")),
              FQ2(h256("0x2ad0378b5e1bb4ed1a58fe5d816d0578968d557ee449edae8190d0521a35379f"),
                  h256("0x27756649b1a8aaff4c6886512d93b45e618ace5a2abf06df0f5ab6eb2287c9ac")));
  if (b != c) {
    return 1;
  }
  return 0;
}

int test_g2_jacobian_affine_conv() {
  FQ a00 = FQ(mont_encode(h256("0x1ecfd2dff2aad18798b64bdb0c2b50c9d73e6c05619e04cbf5b448fd98726880")));
  FQ a01 = FQ(mont_encode(h256("0x0e16c8d96362720af0916592be1b839a26f5e6b710f3ede0d8840d9a70eaf97f")));
  FQ a10 = FQ(mont_encode(h256("0x2aa778acda9e7d4925c60ad84c12fb3b4f2b9539d5699934b0e6fdd10cc2c0e1")));
  FQ a11 = FQ(mont_encode(h256("0x1e8f2c1f441fed039bb46d6bfb91236cf7ba240c75080cedbe40e049c46b26be")));
  G2Affine a = G2Affine{x : FQ2(a00, a01), y : FQ2(a10, a11)};
  G2 b = a.into();
  if (b.x.c0 != FQ(h256("0x180b9347a1a7d0a8fdaf0bced1c1762460053210c288b08a98bf18dbac6f2e8f"))) {
    return 1;
  }
  if (b.x.c1 != FQ(h256("0x20695a8c8acfdc037e47d786b0615dd17f3f17b3f63bb0ad9fd68d54d7bbf4ed"))) {
    return 1;
  }
  if (b.y.c0 != FQ(h256("0x29dad7762754564b72ac95ba67b28db571932de27a4d188d7704dd18f28cfcbe"))) {
    return 1;
  }
  if (b.y.c1 != FQ(h256("0x2c5d04ea5da8d50c233667e8e1086c089a67f531e25789bfbaeb22635e0a5f73"))) {
    return 1;
  }
  if (b.z != FQ2_ONE) {
    return 1;
  }
  G2Affine c = b.affine();
  if (a.x != c.x || a.y != c.y) {
    return 1;
  }
  return 0;
}

int test_g2_double() {
  G2Affine a = G2Affine{
    x : FQ2(FQ(mont_encode(h256("0x1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed"))),
            FQ(mont_encode(h256("0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2")))),
    y : FQ2(FQ(mont_encode(h256("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa"))),
            FQ(mont_encode(h256("0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b")))),
  };
  G2 b = a.into();
  G2 c = b.doubl2();
  if (c.x.c0 != FQ(h256("0x190178401674fd1a6ce2b0feabb43601f71985df6e1d9ae392c552517113f04c"))) {
    return 1;
  }
  if (c.x.c1 != FQ(h256("0x22c5dc5d9fe664ed64074fdc1d7eba9d5be249895ca71ac4192c5eacbcc23fc5"))) {
    return 1;
  }
  if (c.y.c0 != FQ(h256("0x16b5ca02a81579a0b7c93f9ab34a255d29737922388c9abf69faf1822d82e9d2"))) {
    return 1;
  }
  if (c.y.c1 != FQ(h256("0x036e1261fdadb4eead4952efe416e4ba9a5d9892c8fce6864a14ebd2219f1b7"))) {
    return 1;
  }
  if (c.z.c0 != FQ(h256("0x2196af647c0ae3e446ebef0dc4b5a3a0657e3a9e82cb6c63871b6924385ad6a5"))) {
    return 1;
  }
  if (c.z.c1 != FQ(h256("0x1b4941cd27fac904abf26b7c666a20edb8aff24464fa7976c812b6ad8e30addc"))) {
    return 1;
  }
  return 0;
}

int test_g2_add() {
  G2Affine a = G2Affine{
    x : FQ2(FQ(mont_encode(h256("0x1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed"))),
            FQ(mont_encode(h256("0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2")))),
    y : FQ2(FQ(mont_encode(h256("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa"))),
            FQ(mont_encode(h256("0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b")))),
  };
  G2 b = a.into();
  G2 c = b + G2_ONE;
  if (c.x.c0 != FQ(h256("0x190178401674fd1a6ce2b0feabb43601f71985df6e1d9ae392c552517113f04c"))) {
    return 1;
  }
  if (c.x.c1 != FQ(h256("0x22c5dc5d9fe664ed64074fdc1d7eba9d5be249895ca71ac4192c5eacbcc23fc5"))) {
    return 1;
  }
  if (c.y.c0 != FQ(h256("0x16b5ca02a81579a0b7c93f9ab34a255d29737922388c9abf69faf1822d82e9d2"))) {
    return 1;
  }
  if (c.y.c1 != FQ(h256("0x36e1261fdadb4eead4952efe416e4ba9a5d9892c8fce6864a14ebd2219f1b7"))) {
    return 1;
  }
  if (c.z.c0 != FQ(h256("0x2196af647c0ae3e446ebef0dc4b5a3a0657e3a9e82cb6c63871b6924385ad6a5"))) {
    return 1;
  }
  if (c.z.c1 != FQ(h256("0x1b4941cd27fac904abf26b7c666a20edb8aff24464fa7976c812b6ad8e30addc"))) {
    return 1;
  }

  G2 d = b + G2_ZERO;
  if (b != d) {
    return 1;
  }

  G2 e = b + b + b;
  if (e.x.c0 != FQ(h256("0x178ccafd45302ad9a9cc2c2619f354a546e17d9ef882176268874584435a174d"))) {
    return 1;
  }
  if (e.x.c1 != FQ(h256("0x22bd6dd71372cf61c0a45ba029d8286d2078e56e4dfb707c3be17d3082bd6313"))) {
    return 1;
  }
  if (e.y.c0 != FQ(h256("0x07f6555d95103d4a9ad6e833c966af819141b80fb9f0c736ba3155bb745fd473"))) {
    return 1;
  }
  if (e.y.c1 != FQ(h256("0x25472a0de9dc5cdbc176677542c925c01484160af28eb09b802aa2eb3a34c188"))) {
    return 1;
  }
  if (e.z.c0 != FQ(h256("0x0bc2acc98cf51700e0ece64c8e8888cd4fbee1ca0edf4a361eb498df3106e29f"))) {
    return 1;
  }
  if (e.z.c1 != FQ(h256("0x1ba16ff98d805609ceff8f1a56f25372514642a88c62b59b52558fb3a5b0397c"))) {
    return 1;
  }

  return 0;
}

int test_g2_mul() {
  // Taking from
  // https://github.com/xxuejie/benchmarking-wasm-ewasm-evm/blob/checkpoint/evmrace/ckbvm/bn256g2_test.cpp
  G2Affine a = G2Affine{
    x : FQ2(FQ(mont_encode(h256("0x1800deef121f1e76426a00665e5c4479674322d4f75edadd46debd5cd992f6ed"))),
            FQ(mont_encode(h256("0x198e9393920d483a7260bfb731fb5d25f1aa493335a9e71297e485b7aef312c2")))),
    y : FQ2(FQ(mont_encode(h256("0x12c85ea5db8c6deb4aab71808dcb408fe3d1e7690c43d37b4ce6cc0166fa7daa"))),
            FQ(mont_encode(h256("0x090689d0585ff075ec9e99ad690c3395bc4b313370b38ef355acdadcd122975b")))),
  };
  G2 b = a.into();
  G2 c = b.mul(0x2dddefa19);
  if (c.x.c0 != FQ(h256("0x1dafb20da22cc10ef4736831e8a0f1419a9753c43d8400f2949cb68a2d381719"))) {
    return 1;
  }
  if (c.x.c1 != FQ(h256("0x047d62b471a91cfa91fb08d4344792a0355cc0471784adb20e9a99bf11286133"))) {
    return 1;
  }
  if (c.y.c0 != FQ(h256("0x1b5694e95d08d04325f5ebb31dd1889a402e9e76de68700071cd0506326f8ec3"))) {
    return 1;
  }
  if (c.y.c1 != FQ(h256("0x19c7a2f5e6eedf85ec5adff3e0c2028a52b78e15a2e081513a5e5c6309cf7cc2"))) {
    return 1;
  }
  if (c.z.c0 != FQ(h256("0x1415a498e2d14c0157076be278709572d52e168d72ac62082e53aac308d50cbe"))) {
    return 1;
  }
  if (c.z.c1 != FQ(h256("0x1363bae5bce3ce99dbcc3a7419295dcb54434a925f3933a7342aa8d9077925a3"))) {
    return 1;
  }
  G2Affine d = c.affine();
  if (mont_decode(d.x.c0.c0) != h256("0x23997083c2c4409869ee3546806a544c8c16bc46cc88598c4e1c853eb81d45b0")) {
    return 1;
  }
  if (mont_decode(d.x.c1.c0) != h256("0x1142585a23028cbe57783f890d1a2f6837049fce43c9b3b5e8e14c40a43c617a")) {
    return 1;
  }
  if (mont_decode(d.y.c0.c0) != h256("0x215a23c8a96e1ca11d52cf6e2d6ada4ed01ee7e09b06dbc7f3315e7e6e73b919")) {
    return 1;
  }
  if (mont_decode(d.y.c1.c0) != h256("0x0edac9f3a977530e28d4a385e614bcb7a8f9c3c3cb65707c1b90b5ea86174512")) {
    return 1;
  }

  return 0;
}

int test_alt_bn128_add() {
  // Taking from
  // https://github.com/ethereum/go-ethereum/blob/master/core/vm/testdata/precompiles/bn256Add.json
  uint256 a[2];
  uint256 b[2];
  uint256 r[2];

  a[0] = h256("0x18b18acfb4c2c30276db5411368e7185b311dd124691610c5d3b74034e093dc9");
  a[1] = h256("0x063c909c4720840cb5134cb9f59fa749755796819658d32efc0d288198f37266");
  b[0] = h256("0x07c2b7f58a84bd6145f00c9c2bc0bb1a187f20ff2c92963a88019e7c6a014eed");
  b[1] = h256("0x06614e20c147e940f2d70da3f74c9a17df361706a4485c742bd6788478fa17d7");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x2243525c5efd4b9c3d3c45ac0ca3fe4dd85e830a4ce6b65fa1eeaee202839703")) {
    return 1;
  }
  if (r[1] != h256("0x301d1d33be6da8e509df21cc35964723180eed7532537db9ae5e7d48f195c915")) {
    return 1;
  }

  a[0] = h256("0x2243525c5efd4b9c3d3c45ac0ca3fe4dd85e830a4ce6b65fa1eeaee202839703");
  a[1] = h256("0x301d1d33be6da8e509df21cc35964723180eed7532537db9ae5e7d48f195c915");
  b[0] = h256("0x18b18acfb4c2c30276db5411368e7185b311dd124691610c5d3b74034e093dc9");
  b[1] = h256("0x063c909c4720840cb5134cb9f59fa749755796819658d32efc0d288198f37266");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x2bd3e6d0f3b142924f5ca7b49ce5b9d54c4703d7ae5648e61d02268b1a0a9fb7")) {
    return 1;
  }
  if (r[1] != h256("0x21611ce0a6af85915e2f1d70300909ce2e49dfad4a4619c8390cae66cefdb204")) {
    return 1;
  }

  a[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  a[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  b[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  b[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x0000000000000000000000000000000000000000000000000000000000000000")) {
    return 1;
  }
  if (r[1] != h256("0x0000000000000000000000000000000000000000000000000000000000000000")) {
    return 1;
  }

  a[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  a[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  b[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  b[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000002");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x0000000000000000000000000000000000000000000000000000000000000001")) {
    return 1;
  }
  if (r[1] != h256("0x0000000000000000000000000000000000000000000000000000000000000002")) {
    return 1;
  }

  a[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  a[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000002");
  b[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  b[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000000");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x0000000000000000000000000000000000000000000000000000000000000001")) {
    return 1;
  }
  if (r[1] != h256("0x0000000000000000000000000000000000000000000000000000000000000002")) {
    return 1;
  }

  a[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  a[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000002");
  b[0] = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  b[1] = h256("0x0000000000000000000000000000000000000000000000000000000000000002");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x030644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd3")) {
    return 1;
  }
  if (r[1] != h256("0x15ed738c0e0a7c92e7845f96b2ae9c0a68a6a449e3538fc7ff3ebf7a5a18a2c4")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  b[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x15bf2bb17880144b5d1cd2b1f46eff9d617bffd1ca57c37fb5a49bd84e53cf66")) {
    return 1;
  }
  if (r[1] != h256("0x049c797f9ce0d17083deb32b5e36f2ea2a212ee036598dd7624c168993d1355f")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  b[1] = h256("0x2e83f8d734803fc370eba25ed1f6b8768bd6d83887b87165fc2434fe11a830cb");
  alt_bn128_add(a, b, r);
  if (r[0] != h256("0x0000000000000000000000000000000000000000000000000000000000000000")) {
    return 1;
  }
  if (r[1] != h256("0x0000000000000000000000000000000000000000000000000000000000000000")) {
    return 1;
  }

  return 0;
}

int test_alt_bn128_mul() {
  // Taking from
  // https://github.com/ethereum/go-ethereum/blob/master/core/vm/testdata/precompiles/bn256ScalarMul.json
  uint256 a[2];
  uint256 b;
  uint256 r[2];

  a[0] = h256("0x2bd3e6d0f3b142924f5ca7b49ce5b9d54c4703d7ae5648e61d02268b1a0a9fb7");
  a[1] = h256("0x21611ce0a6af85915e2f1d70300909ce2e49dfad4a4619c8390cae66cefdb204");
  b = h256("0x00000000000000000000000000000000000000000000000011138ce750fa15c2");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x070a8d6a982153cae4be29d434e8faef8a47b274a053f5a4ee2a6c9c13c31e5c")) {
    return 1;
  }
  if (r[1] != h256("0x031b8ce914eba3a9ffb989f9cdd5b0f01943074bf4f0f315690ec3cec6981afc")) {
    return 1;
  }

  a[0] = h256("0x070a8d6a982153cae4be29d434e8faef8a47b274a053f5a4ee2a6c9c13c31e5c");
  a[1] = h256("0x031b8ce914eba3a9ffb989f9cdd5b0f01943074bf4f0f315690ec3cec6981afc");
  b = h256("0x30644e72e131a029b85045b68181585d97816a916871ca8d3c208c16d87cfd46");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x025a6f4181d2b4ea8b724290ffb40156eb0adb514c688556eb79cdea0752c2bb")) {
    return 1;
  }
  if (r[1] != h256("0x2eff3f31dea215f1eb86023a133a996eb6300b44da664d64251d05381bb8a02e")) {
    return 1;
  }

  a[0] = h256("0x025a6f4181d2b4ea8b724290ffb40156eb0adb514c688556eb79cdea0752c2bb");
  a[1] = h256("0x2eff3f31dea215f1eb86023a133a996eb6300b44da664d64251d05381bb8a02e");
  b = h256("0x183227397098d014dc2822db40c0ac2ecbc0b548b438e5469e10460b6c3e7ea3");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x14789d0d4a730b354403b5fac948113739e276c23e0258d8596ee72f9cd9d323")) {
    return 1;
  }
  if (r[1] != h256("0x0af18a63153e0ec25ff9f2951dd3fa90ed0197bfef6e2a1a62b5095b9d2b4a27")) {
    return 1;
  }

  a[0] = h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3");
  a[1] = h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6");
  b = h256("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x2cde5879ba6f13c0b5aa4ef627f159a3347df9722efce88a9afbb20b763b4c41")) {
    return 1;
  }
  if (r[1] != h256("0x1aa7e43076f6aee272755a7f9b84832e71559ba0d2e0b17d5f9f01755e5b0d11")) {
    return 1;
  }

  a[0] = h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3");
  a[1] = h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6");
  b = h256("0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3")) {
    return 1;
  }
  if (r[1] != h256("0x163511ddc1c3f25d396745388200081287b3fd1472d8339d5fecb2eae0830451")) {
    return 1;
  }

  a[0] = h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3");
  a[1] = h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6");
  b = h256("0x0000000000000000000000000000000100000000000000000000000000000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x1051acb0700ec6d42a88215852d582efbaef31529b6fcbc3277b5c1b300f5cf0")) {
    return 1;
  }
  if (r[1] != h256("0x135b2394bb45ab04b8bd7611bd2dfe1de6a4e6e2ccea1ea1955f577cd66af85b")) {
    return 1;
  }

  a[0] = h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3");
  a[1] = h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000009");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x1dbad7d39dbc56379f78fac1bca147dc8e66de1b9d183c7b167351bfe0aeab74")) {
    return 1;
  }
  if (r[1] != h256("0x2cd757d51289cd8dbd0acf9e673ad67d0f0a89f912af47ed1be53664f5692575")) {
    return 1;
  }

  a[0] = h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3");
  a[1] = h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x1a87b0584ce92f4593d161480614f2989035225609f08058ccfa3d0f940febe3")) {
    return 1;
  }
  if (r[1] != h256("0x1a2f3c951f6dadcc7ee9007dff81504b0fcd6d7cf59996efdc33d92bf7f9f8f6")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b = h256("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x29e587aadd7c06722aabba753017c093f70ba7eb1f1c0104ec0564e7e3e21f60")) {
    return 1;
  }
  if (r[1] != h256("0x22b1143f6a41008e7755c71c3d00b6b915d386de21783ef590486d8afa8453b1")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b = h256("0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9")) {
    return 1;
  }
  if (r[1] != h256("0x2e83f8d734803fc370eba25ed1f6b8768bd6d83887b87165fc2434fe11a830cb")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b = h256("0x0000000000000000000000000000000100000000000000000000000000000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x221a3577763877920d0d14a91cd59b9479f83b87a653bb41f82a3f6f120cea7c")) {
    return 1;
  }
  if (r[1] != h256("0x2752c7f64cdd7f0e494bff7b60419f242210f2026ed2ec70f89f78a4c56a1f15")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000009");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x228e687a379ba154554040f8821f4e41ee2be287c201aa9c3bc02c9dd12f1e69")) {
    return 1;
  }
  if (r[1] != h256("0x1e0fd6ee672d04cfd924ed8fdc7ba5f2d06c53c1edc30f65f2af5a5b97f0a76a")) {
    return 1;
  }

  a[0] = h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9");
  a[1] = h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x17c139df0efee0f766bc0204762b774362e4ded88953a39ce849a8a7fa163fa9")) {
    return 1;
  }
  if (r[1] != h256("0x01e0559bacb160664764a357af8a9fe70baa9258e0b959273ffc5718c6d4cc7c")) {
    return 1;
  }

  a[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  a[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  b = h256("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x00a1a234d08efaa2616607e31eca1980128b00b415c845ff25bba3afcb81dc00")) {
    return 1;
  }
  if (r[1] != h256("0x242077290ed33906aeb8e42fd98c41bcb9057ba03421af3f2d08cfc441186024")) {
    return 1;
  }

  a[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  a[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  b = h256("0x30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869")) {
    return 1;
  }
  if (r[1] != h256("0x2929ee761a352600f54921df9bf472e66217e7bb0cee9032e00acc86b3c8bfaf")) {
    return 1;
  }

  a[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  a[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  b = h256("0x0000000000000000000000000000000100000000000000000000000000000000");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x1071b63011e8c222c5a771dfa03c2e11aac9666dd097f2c620852c3951a4376a")) {
    return 1;
  }
  if (r[1] != h256("0x2f46fe2f73e1cf310a168d56baa5575a8319389d7bfa6b29ee2d908305791434")) {
    return 1;
  }

  a[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  a[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000009");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x19f75b9dd68c080a688774a6213f131e3052bd353a304a189d7a2ee367e3c258")) {
    return 1;
  }
  if (r[1] != h256("0x2612f545fb9fc89fde80fd81c68fc7dcb27fea5fc124eeda69433cf5c46d2d7f")) {
    return 1;
  }

  a[0] = h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869");
  a[1] = h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98");
  b = h256("0x0000000000000000000000000000000000000000000000000000000000000001");
  alt_bn128_mul(a, b, r);
  if (r[0] != h256("0x039730ea8dff1254c0fee9c0ea777d29a9c710b7e616683f194f18c43b43b869")) {
    return 1;
  }
  if (r[1] != h256("0x073a5ffcc6fc7a28c30723d6e58ce577356982d65b833a5a5c15bf9024b43d98")) {
    return 1;
  }

  return 0;
}

int test_alt_bn128_pairing() {
  FQ a00 = FQ(mont_encode(h256("0x2276cf730cf493cd95d64677bbb75fc42db72513a4c1e387b476d056f80aa75f")));
  FQ a01 = FQ(mont_encode(h256("0x1213d2149b006137fcfb23036606f848d638d576a120ca981b5b1a5f9300b3ee")));
  FQ a10 = FQ(mont_encode(h256("0x096df1f82dff337dd5972e32a8ad43e28a78a96a823ef1cd4debe12b6552ea5f")));
  FQ a11 = FQ(mont_encode(h256("0x21ee6226d31426322afcda621464d0611d226783262e21bb3bc86b537e986237")));
  FQ b0 = FQ(mont_encode(h256("0x2eca0c7238bf16e83e7a1e6c5d49540685ff51380f309842a98561558019fc02")));
  FQ b1 = FQ(mont_encode(h256("0x03d3260361bb8451de5ff5ecd17f010ff22f5c31cdf184e9020b06fa5997db84")));
  G2Affine a = G2Affine{x : FQ2(a00, a01), y : FQ2(a10, a11)};
  G1Affine b = G1Affine{x : b0, y : b1};
  G2Precomp c = a.precompute();
  FQ12 d = c.miller_loop(b).final_exponentiation();

  if (d.c0.c0.c0 != FQ(h256("0x0f4eabc79f4207cae5c25efed5dd895b483c6f02ae7169a2465a1b0c5d7e87f5"))) {
    return 1;
  }
  if (d.c0.c0.c1 != FQ(h256("0x1ea573771832738906d7e744788de7aa58c04f1b67485fe4556f89c18606946d"))) {
    return 1;
  }
  if (d.c0.c1.c0 != FQ(h256("0x225653f8808a23eda89b43521c123a0482c5209467b0b8a98820ef1d0f0f6ff6"))) {
    return 1;
  }
  if (d.c0.c1.c1 != FQ(h256("0x2470c518434776631bd2e00036fa72262b2b31880ae191cde3bb08911e764eef"))) {
    return 1;
  }
  if (d.c0.c2.c0 != FQ(h256("0x0d520d77a697c64ebaea6b4f901409aab541f7a95e4218c02c177b71eb13505c"))) {
    return 1;
  }
  if (d.c0.c2.c1 != FQ(h256("0x28594977c319352c6c7558065cdb68a5297cbed2820ca7c316171ac93c99fc80"))) {
    return 1;
  }
  if (d.c1.c0.c0 != FQ(h256("0x21bc12ad2febc4c7c2b691208deea520c189dc7c693c5a344c0573d3d863f174"))) {
    return 1;
  }
  if (d.c1.c0.c1 != FQ(h256("0x0ef0acb2435e8a50fd26d98d6933668dd4325b8e2ebc79b66316f3a1d2640d19"))) {
    return 1;
  }
  if (d.c1.c1.c0 != FQ(h256("0x09e7b2ab23b2f61ac30c80dd44883abc0da9a50a3fb2216a1c594dca7e4859e8"))) {
    return 1;
  }
  if (d.c1.c1.c1 != FQ(h256("0x2c191a033b0c28682f64cd2b0da005812dbf182b278d17a0eae786deaa0749b3"))) {
    return 1;
  }
  if (d.c1.c2.c0 != FQ(h256("0x27f00e6cd47e6cde40df2e14a4d3e83bc5bd714c7fade1979e4ed2675806a283"))) {
    return 1;
  }
  if (d.c1.c2.c1 != FQ(h256("0x1f8aad6f81013a83ebd3ced4b5ed0da2651932067f8d24f7c9b084c9ce3b799c"))) {
    return 1;
  }

  return 0;
}

int main() {
  if (test_invmod())
    return 1;
  if (test_powmod())
    return 1;
  if (test_constexpr())
    return 1;
  if (test_mont_encode_decode())
    return 1;
  if (test_fq_inv())
    return 1;
  if (test_fq_neg())
    return 1;
  if (test_fq2_mul())
    return 1;
  if (test_fq2_inv())
    return 1;
  if (test_fq2_squared())
    return 1;
  if (test_fq6_inv())
    return 1;
  if (test_fq6_squared())
    return 1;
  if (test_g2_jacobian_affine_conv())
    return 1;
  if (test_g2_double())
    return 1;
  if (test_g2_add())
    return 1;
  if (test_g2_mul())
    return 1;
  if (test_alt_bn128_add())
    return 1;
  if (test_alt_bn128_mul())
    return 1;
  if (test_alt_bn128_pairing())
    return 1;
  return 0;
}
