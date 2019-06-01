#include <doctest/doctest.h>
#include <SVL/SVL.h>
#include <string>

#include <random>

// unions for the types to check
union F4Scalar {
  using vec_t = SVL::scalar::Vector4f;
  vec_t v;
  float s[4];
};
TYPE_TO_STRING(F4Scalar);

union F4SSE {
  using vec_t = SVL::sse::Vector4f;
  vec_t v;
  float s[4];
};
TYPE_TO_STRING(F4SSE);

union F4AVX2 {
  using vec_t = SVL::avx2::Vector4f;
  vec_t v;
  float s[4];
};
TYPE_TO_STRING(F4AVX2);

//union F4AVX512 {
//  using vec_t = SVL::avx512::Vector4f;
//  vec_t v;
//  float s[4];
//};
//TYPE_TO_STRING(F4AVX512);

union F8Scalar {
  using vec_t = SVL::scalar::Vector8f;
  vec_t v;
  float s[8];
};
TYPE_TO_STRING(F8Scalar);

union F8SSE {
  using vec_t = SVL::sse::Vector8f;
  vec_t v;
  float s[8];
};
TYPE_TO_STRING(F8SSE);

union F8AVX2 {
  using vec_t = SVL::avx2::Vector8f;
  vec_t v;
  float s[8];
};
TYPE_TO_STRING(F8AVX2);

//union F8AVX512 {
//  using vec_t = SVL::avx512::Vector8f;
//  vec_t v;
//  float s[8];
//};
//TYPE_TO_STRING(F8AVX512);

union F16Scalar {
  using vec_t = SVL::scalar::Vector16f;
  vec_t v;
  float s[16];
};
TYPE_TO_STRING(F16Scalar);

union F16SSE {
  using vec_t = SVL::sse::Vector16f;
  vec_t v;
  float s[16];
};
TYPE_TO_STRING(F16SSE);

union F16AVX2 {
  using vec_t = SVL::avx2::Vector16f;
  vec_t v;
  float s[16];
};
TYPE_TO_STRING(F16AVX2);

//union F16AVX512 {
//  using vec_t = SVL::avx512::Vector16f;
//  vec_t v;
//  float s[16];
//};
//TYPE_TO_STRING(F16AVX512);


float vs[64] = {64., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.,
  13., 14., 15., 16., 17., 18., 19., 20., 21., 22., 23., 24., 25., 26., 27., 28.,
  29., 30., 31., 32., 33., 34., 35., 36., 37., 38., 39., 40., 41., 42., 43., 44.,
  45., 46., 47., 48., 49., 50., 51., 52., 53., 54., 55., 56., 57., 58., 59., 60.,
  61., 62., 63.};

void PopulateVs(float* v) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(1, 100);
  SVL_FOR_RANGE(64) v[i] = dis(gen);
}

TEST_SUITE_BEGIN("Vecf constructors");
TEST_CASE_TEMPLATE("Four floats", T, F4Scalar, F4SSE, F4AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  PopulateVs(vs);
  
  SUBCASE("No exceptions on construction/assignment") {
    // Construct
    CHECK_NOTHROW(V default_test);
    CHECK_NOTHROW(V array_test(vs));
    CHECK_NOTHROW(V broadcast_test(vs[1]));
    CHECK_NOTHROW(V value_test(vs[2], vs[3], vs[4], vs[5]));
    V copy_from(vs[6], vs[7], vs[8], vs[9]);
    CHECK_NOTHROW(V copy_test(copy_from));
    CHECK_NOTHROW(V move_test(V move_from));
    I test_i = { vs[0], vs[10], vs[20], vs[30] };
    CHECK_NOTHROW(V intrinsic_test(test_i));
    
    // Assign
    V assign_to;
    CHECK_NOTHROW(I intrinsic_cast(assign_to));
    CHECK_NOTHROW(assign_to = copy_from);     // copy
    CHECK_NOTHROW(assign_to = V(vs[10]));   // move
    CHECK_NOTHROW(assign_to = test_i);        // intrinsic copy
    CHECK_NOTHROW(assign_to = (I)copy_from); // intrinsic move
    CHECK_NOTHROW(assign_to = vs[63]);   // scalar assign

  }
  SUBCASE("Correctness of construction/assignment") {
    // Default constructor doesn't have any setting of values so no need to check
    T c{{0., 0., 0., 0.}};
    int i = 0, d = 0;
    // Setups
    SUBCASE("array construct") { i = 5; d = 1; c = T{V(vs + i)}; }
    SUBCASE("broadcast construct") { i = 17; d = 0; c = T{V(vs[i])}; }
    SUBCASE("value construct") {
      i = 21; d = 3; c = T{V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d])};
    }
    SUBCASE("copy construct") { i = 2; d = 0; V f(vs[i]); c = T{V(f)}; }
    SUBCASE("move construct") { i = 3; d = 0; c = T{V(V(vs[i]))}; }
    SUBCASE("intrinsic convert") {
      i = 5; d = 2; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c = T{V(f)};
    }
    SUBCASE("copy assign") {
      i = 3; d = 3; V f(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]); c.v = f;
    }
    SUBCASE("move assign") {
      i = 60; d = -3; c.v = V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]);
    }
    SUBCASE("intrinsic copy") {
      i = 15; d = -1; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c.v = f;
    }
    SUBCASE("intrinsic move") {
      i = 9; d = 9; c.v = I{vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]};
    }
    SUBCASE("scalar assign") {  i = 19; d = 0; c.v = vs[i]; }
    
    // Checking
    int j = i + 3 * d;
    REQUIRE((i < 64 && i >= 0 && j < 64 && j >= 0));
    CHECK(c.s[0] == vs[i + 0 * d]);
    CHECK(c.s[1] == vs[i + 1 * d]);
    CHECK(c.s[2] == vs[i + 2 * d]);
    CHECK(c.s[3] == vs[i + 3 * d]);
  }
}
TEST_CASE_TEMPLATE("Eight floats", T, F8Scalar, F8SSE, F8AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  PopulateVs(vs);
  
  SUBCASE("No exceptions on construction/assignment") {
    // Construct
    CHECK_NOTHROW(V default_test);
    CHECK_NOTHROW(V array_test(vs));
    CHECK_NOTHROW(V broadcast_test(vs[1]));
    CHECK_NOTHROW(V value_test(vs[2], vs[3], vs[4], vs[5],
                               vs[6], vs[7], vs[8], vs[9]));
    V copy_from(vs[6], vs[7], vs[8], vs[9], vs[9], vs[8], vs[7], vs[6]);
    CHECK_NOTHROW(V copy_test(copy_from));
    CHECK_NOTHROW(V move_test(V move_from));
    // Intrinsic types aren't cross compatible. Need to figure something out
    //    I test_i = {{vs[0], vs[10], vs[20], vs[30]}, {vs[40], vs[50], vs[60], vs[1]}};
    //    CHECK_NOTHROW(V intrinsic_test(test_i));
    
    // Assign
    V assign_to;
    CHECK_NOTHROW(I intrinsic_cast(assign_to));
    CHECK_NOTHROW(assign_to = copy_from);     // copy
    CHECK_NOTHROW(assign_to = V(vs[10]));   // move
                                            //    CHECK_NOTHROW(assign_to = test_i);        // intrinsic copy
    CHECK_NOTHROW(assign_to = (I)copy_from); // intrinsic move
    CHECK_NOTHROW(assign_to = vs[63]);   // scalar assign
  }
  SUBCASE("Correctness of construction/assignment") {
    // Default constructor doesn't have any setting of values so no need to check
    T c{{0., 0., 0., 0., 0., 0., 0., 0.}};
    int i = 0, d = 0;
    // Setups
    SUBCASE("array construct") { i = 5; d = 1; c = T{V(vs + i)}; }
    SUBCASE("broadcast construct") { i = 17; d = 0; c = T{V(vs[i])}; }
    SUBCASE("value construct") {
      i = 21; d = 3; c = T{V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                             vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d])};
    }
    SUBCASE("copy construct") { i = 2; d = 0; V f(vs[i]); c = T{V(f)}; }
    SUBCASE("move construct") { i = 3; d = 0; c = T{V(V(vs[i]))}; }
    //    SUBCASE("intrinsic convert") {
    //      i = 5; d = 2; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c = T{V(f)};
    //    }
    SUBCASE("copy assign") {
      i = 3; d = 3; V f(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                        vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d]); c.v = f;
    }
    SUBCASE("move assign") {
      i = 59; d = -3; c.v = V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                              vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d]);
    }
    //    SUBCASE("intrinsic copy") {
    //      i = 15; d = -1; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c.v = f;
    //    }
    //    SUBCASE("intrinsic move") {
    //      i = 9; d = 9; c.v = I{vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]};
    //    }
    SUBCASE("scalar assign") {  i = 34; d = 0; c.v = vs[i]; }
    
    // Checking
    int j = i + 7 * d;
    REQUIRE((i < 64 && i >= 0 && j < 64 && j >= 0));
    CHECK(c.s[0] == vs[i + 0 * d]);
    CHECK(c.s[1] == vs[i + 1 * d]);
    CHECK(c.s[2] == vs[i + 2 * d]);
    CHECK(c.s[3] == vs[i + 3 * d]);
    CHECK(c.s[4] == vs[i + 4 * d]);
    CHECK(c.s[5] == vs[i + 5 * d]);
    CHECK(c.s[6] == vs[i + 6 * d]);
    CHECK(c.s[7] == vs[i + 7 * d]);
  }
}
TEST_CASE_TEMPLATE("Sixteen floats", T, F16Scalar, F16SSE, F16AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  PopulateVs(vs);
  
  SUBCASE("No exceptions on construction/assignment") {
    // Construct
    CHECK_NOTHROW(V default_test);
    CHECK_NOTHROW(V array_test(vs));
    CHECK_NOTHROW(V broadcast_test(vs[1]));
    CHECK_NOTHROW(V value_test(vs[2], vs[3], vs[4], vs[5],
                               vs[6], vs[7], vs[8], vs[9],
                               vs[10], vs[11], vs[12], vs[13],
                               vs[14], vs[15], vs[16], vs[17]));
    V copy_from(vs[6], vs[7], vs[8], vs[9], vs[9], vs[8], vs[7], vs[6],
                vs[6], vs[7], vs[8], vs[9], vs[9], vs[8], vs[7], vs[6]);
    CHECK_NOTHROW(V copy_test(copy_from));
    CHECK_NOTHROW(V move_test(V move_from));
    // Intrinsic types aren't cross compatible. Need to figure something out
    //    I test_i = {{vs[0], vs[10], vs[20], vs[30]}, {vs[40], vs[50], vs[60], vs[1]}};
    //    CHECK_NOTHROW(V intrinsic_test(test_i));
    
    // Assign
    V assign_to;
    CHECK_NOTHROW(I intrinsic_cast(assign_to));
    CHECK_NOTHROW(assign_to = copy_from);     // copy
    CHECK_NOTHROW(assign_to = V(vs[10]));   // move
                                            //    CHECK_NOTHROW(assign_to = test_i);        // intrinsic copy
    CHECK_NOTHROW(assign_to = (I)copy_from); // intrinsic move
    CHECK_NOTHROW(assign_to = vs[63]);   // scalar assign
  }
  SUBCASE("Correctness of construction/assignment") {
    // Default constructor doesn't have any setting of values so no need to check
    T c{{0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}};
    int i = 0, d = 0;
    // Setups
    SUBCASE("array construct") { i = 32; d = 1; c = T{V(vs + i)}; }
    SUBCASE("broadcast construct") { i = 27; d = 0; c = T{V(vs[i])}; }
    SUBCASE("value construct") {
      i = 23; d = 2; c = T{V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                             vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d],
                             vs[i+8*d], vs[i+9*d], vs[i+10*d], vs[i+11*d],
                             vs[i+12*d], vs[i+13*d], vs[i+14*d], vs[i+15*d])};
    }
    SUBCASE("copy construct") { i = 2; d = 0; V f(vs[i]); c = T{V(f)}; }
    SUBCASE("move construct") { i = 3; d = 0; c = T{V(V(vs[i]))}; }
    //    SUBCASE("intrinsic convert") {
    //      i = 5; d = 2; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c = T{V(f)};
    //    }
    SUBCASE("copy assign") {
      i = 3; d = 3; V f(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                        vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d],
                        vs[i+8*d], vs[i+9*d], vs[i+10*d], vs[i+11*d],
                        vs[i+12*d], vs[i+13*d], vs[i+14*d], vs[i+15*d]); c.v = f;
    }
    SUBCASE("move assign") {
      i = 59; d = -3; c.v = V(vs[i], vs[i+d], vs[i+2*d], vs[i+3*d],
                              vs[i+4*d], vs[i+5*d], vs[i+6*d], vs[i+7*d],
                              vs[i+8*d], vs[i+9*d], vs[i+10*d], vs[i+11*d],
                              vs[i+12*d], vs[i+13*d], vs[i+14*d], vs[i+15*d]);
    }
    //    SUBCASE("intrinsic copy") {
    //      i = 15; d = -1; I f = {vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]}; c.v = f;
    //    }
    //    SUBCASE("intrinsic move") {
    //      i = 9; d = 9; c.v = I{vs[i], vs[i+d], vs[i+2*d], vs[i+3*d]};
    //    }
    SUBCASE("scalar assign") {  i = 34; d = 0; c.v = vs[i]; }
    
    // Checking
    int j = i + 15 * d;
    REQUIRE((i < 64 && i >= 0 && j < 64 && j >= 0));
    CHECK(c.s[0] == vs[i + 0 * d]);
    CHECK(c.s[1] == vs[i + 1 * d]);
    CHECK(c.s[2] == vs[i + 2 * d]);
    CHECK(c.s[3] == vs[i + 3 * d]);
    CHECK(c.s[4] == vs[i + 4 * d]);
    CHECK(c.s[5] == vs[i + 5 * d]);
    CHECK(c.s[6] == vs[i + 6 * d]);
    CHECK(c.s[7] == vs[i + 7 * d]);
    CHECK(c.s[8] == vs[i + 8 * d]);
    CHECK(c.s[9] == vs[i + 9 * d]);
    CHECK(c.s[10] == vs[i + 10 * d]);
    CHECK(c.s[11] == vs[i + 11 * d]);
    CHECK(c.s[12] == vs[i + 12 * d]);
    CHECK(c.s[13] == vs[i + 13 * d]);
    CHECK(c.s[14] == vs[i + 14 * d]);
    CHECK(c.s[15] == vs[i + 15 * d]);
  }
}
TEST_SUITE_END();

TEST_SUITE_BEGIN("Vecf load/store");
TEST_CASE_TEMPLATE("Four floats", T, F4Scalar, F4SSE, F4AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  T check{{-1., -1., -1., -1.}};
  T expected{{-1., -1., -1., -1.}};
  std::string subcases[3];
  PopulateVs(vs);
  
  // Loading
  SUBCASE("Load from array") {
    subcases[0] = "Load from array ";
    check.v.load(vs + 17);
    expected.v = V(vs[17], vs[18], vs[19], vs[20]);
  }
  SUBCASE("Load upto N values") {
    subcases[0] = "Load upto N values ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load(); expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load(vs[1]); expected.v = V(vs[1], 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load(vs[2], vs[12]);
      expected.v = V(vs[2], vs[12], 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load(vs[3], vs[13], vs[23]);
      expected.v = V(vs[3], vs[13], vs[23], 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load(vs[4], vs[14], vs[24], vs[34]);
      expected.v = V(vs[4], vs[14], vs[24], vs[34]); }
  }
  SUBCASE("Load N values from an array") {
    subcases[0] = "Load N values from an array ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load_partial(vs + 17, 0);
      expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load_partial(vs + 27, 1);
      expected.v = V(vs[27], 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load_partial(vs + 37, 2);
      expected.v = V(vs[37], vs[38], 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load_partial(vs + 47, 3);
      expected.v = V(vs[47], vs[48], vs[49], 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load_partial(vs + 57, 4);
      expected.v = V(vs[57], vs[58], vs[59], vs[60]);
    }
  }
  SUBCASE("Broadcast N") {
    subcases[0] = "Broadcast N ";
    check.v.broadcast(vs[43]);
    expected.v = V(vs[43]);
  }
  
  // Storing
  SUBCASE("Store in an array") {
    subcases[0] = "Store in an array ";
    V from(vs + 54);
    from.store(check.s);
    expected.v = V(vs[54], vs[55], vs[56], vs[57]);
  }
  SUBCASE("Store N values in an array") {
    subcases[0] = "Store N values in an array ";
    V from(vs + 21);
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      from.store_partial(check.s, 0);
      expected.v = V(-1., -1., -1., -1.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      from.store_partial(check.s, 1);
      expected.v = V(vs[21], -1., -1., -1.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      from.store_partial(check.s, 2);
      expected.v = V(vs[21], vs[22], -1., -1.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      from.store_partial(check.s, 3);
      expected.v = V(vs[21], vs[22], vs[23], -1.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      from.store_partial(check.s, 4);
      expected.v = V(vs[21], vs[22], vs[23], vs[24]);
    }
  }
  
  SVL_FOR_RANGE(V::step) {
    CAPTURE(subcases[0] << subcases[1] << subcases[2] << " Check = " << i);
    CHECK(check.s[i] == expected.s[i]);
  }
}
TEST_CASE_TEMPLATE("Eight floats", T, F8Scalar, F8SSE, F8AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  T check{{-1., -1., -1., -1., -1., -1., -1., -1.}};
  T expected{{-1., -1., -1., -1., -1., -1., -1., -1.}};
  std::string subcases[3];
  PopulateVs(vs);
  
  // Loading
  SUBCASE("Load from array") {
    subcases[0] = "Load from array ";
    check.v.load(vs + 17);
    expected.v = V(vs[17], vs[18], vs[19], vs[20], vs[21], vs[22], vs[23], vs[24]);
  }
  SUBCASE("Load upto N values") {
    subcases[0] = "Load upto N values ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load(); expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load(vs[1]);
      expected.v = V(vs[1], 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load(vs[1], vs[2]);
      expected.v = V(vs[1], vs[2], 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load(vs[1], vs[2], vs[3]);
      expected.v = V(vs[1], vs[2], vs[3], 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load(vs[1], vs[2], vs[3], vs[4]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], 0., 0., 0., 0.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], 0., 0., 0.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], 0., 0.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], 0.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34]);
    }
  }
  SUBCASE("Load N values from an array") {
    subcases[0] = "Load N values from an array ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load_partial(vs + 17, 0);
      expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load_partial(vs + 27, 1);
      expected.v = V(vs[27], 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load_partial(vs + 37, 2);
      expected.v = V(vs[37], vs[38], 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load_partial(vs + 47, 3);
      expected.v = V(vs[47], vs[48], vs[49], 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load_partial(vs + 57, 4);
      expected.v = V(vs[57], vs[58], vs[59], vs[60], 0., 0., 0., 0.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      check.v.load_partial(vs + 6, 5);
      expected.v = V(vs[6], vs[7], vs[8], vs[9], vs[10], 0., 0., 0.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      check.v.load_partial(vs + 16, 6);
      expected.v = V(vs[16], vs[17], vs[18], vs[19], vs[20], vs[21], 0., 0.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      check.v.load_partial(vs + 26, 7);
      expected.v = V(vs[26], vs[27], vs[28], vs[29], vs[30], vs[31], vs[32], 0.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      check.v.load_partial(vs + 36, 8);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43]);
    }
  }
  SUBCASE("Broadcast N") {
    subcases[0] = "Broadcast N ";
    check.v.broadcast(vs[43]);
    expected.v = V(vs[43]);
  }
  
  // Storing
  SUBCASE("Store in an array") {
    subcases[0] = "Store in an array ";
    V from(vs + 54);
    from.store(check.s);
    expected.v = V(vs[54], vs[55], vs[56], vs[57], vs[58], vs[59], vs[60], vs[61]);
  }
  SUBCASE("Store N values in an array") {
    subcases[0] = "Store N values in an array ";
    V from(vs + 21);
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      from.store_partial(check.s, 0);
      expected.v = V(-1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      from.store_partial(check.s, 1);
      expected.v = V(vs[21], -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      from.store_partial(check.s, 2);
      expected.v = V(vs[21], vs[22], -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      from.store_partial(check.s, 3);
      expected.v = V(vs[21], vs[22], vs[23], -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      from.store_partial(check.s, 4);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], -1., -1., -1., -1.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      from.store_partial(check.s, 5);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], -1., -1., -1.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      from.store_partial(check.s, 6);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], -1., -1.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      from.store_partial(check.s, 7);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], -1.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      from.store_partial(check.s, 8);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28]);
    }
  }
  
  SVL_FOR_RANGE(V::step) {
    CAPTURE(subcases[0] << subcases[1] << subcases[2] << " Check = " << i);
    CHECK(check.s[i] == expected.s[i]);
  }
}
TEST_CASE_TEMPLATE("Sixteen floats", T, F16Scalar, F16SSE, F16AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  T check{{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.}};
  T expected{{-1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1., -1.}};
  std::string subcases[2];
  PopulateVs(vs);
  
  // Loading
  SUBCASE("Load from array") {
    subcases[0] = "Load from array ";
    check.v.load(vs + 17);
    expected.v = V(vs[17], vs[18], vs[19], vs[20], vs[21], vs[22], vs[23], vs[24],
                   vs[25], vs[26], vs[27], vs[28], vs[29], vs[30], vs[31], vs[32]);
  }
  SUBCASE("Load upto N values") {
    subcases[0] = "Load upto N values ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load(); expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load(vs[1]);
      expected.v = V(vs[1], 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load(vs[1], vs[2]);
      expected.v = V(vs[1], vs[2], 0., 0., 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load(vs[1], vs[2], vs[3]);
      expected.v = V(vs[1], vs[2], vs[3], 0., 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load(vs[1], vs[2], vs[3], vs[4]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 9") {
      subcases[1] = "N = 9";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 10") {
      subcases[1] = "N = 10";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 11") {
      subcases[1] = "N = 11";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 12") {
      subcases[1] = "N = 12";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11], vs[12]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], vs[12], 0., 0., 0., 0.);
    }
    SUBCASE("N = 13") {
      subcases[1] = "N = 13";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11], vs[12], vs[13]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], vs[12], vs[13], 0., 0., 0.);
    }
    SUBCASE("N = 14") {
      subcases[1] = "N = 14";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11], vs[12], vs[13], vs[14]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], vs[12], vs[13], vs[14], 0., 0.);
    }
    SUBCASE("N = 15") {
      subcases[1] = "N = 15";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11], vs[12], vs[13], vs[14], vs[15]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], vs[12], vs[13], vs[14], vs[15], 0.);
    }
    SUBCASE("N = 16") {
      subcases[1] = "N = 16";
      check.v.load(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                   vs[9], vs[10], vs[11], vs[12], vs[13], vs[14], vs[15], vs[16]);
      expected.v = V(vs[1], vs[2], vs[3], vs[4], vs[5], vs[60], vs[61], vs[34],
                     vs[9], vs[10], vs[11], vs[12], vs[13], vs[14], vs[15], vs[16]);
    }
  }
  SUBCASE("Load N values from an array") {
    subcases[0] = "Load N values from an array ";
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      check.v.load_partial(vs + 17, 0);
      expected.v = V(0.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      check.v.load_partial(vs + 27, 1);
      expected.v = V(vs[27], 0., 0., 0., 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      check.v.load_partial(vs + 37, 2);
      expected.v = V(vs[37], vs[38], 0., 0., 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      check.v.load_partial(vs + 47, 3);
      expected.v = V(vs[47], vs[48], vs[49], 0., 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      check.v.load_partial(vs + 57, 4);
      expected.v = V(vs[57], vs[58], vs[59], vs[60], 0., 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      check.v.load_partial(vs + 6, 5);
      expected.v = V(vs[6], vs[7], vs[8], vs[9], vs[10], 0., 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      check.v.load_partial(vs + 16, 6);
      expected.v = V(vs[16], vs[17], vs[18], vs[19], vs[20], vs[21], 0., 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      check.v.load_partial(vs + 26, 7);
      expected.v = V(vs[26], vs[27], vs[28], vs[29], vs[30], vs[31], vs[32], 0.,
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      check.v.load_partial(vs + 36, 8);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     0., 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 9") {
      subcases[1] = "N = 9";
      check.v.load_partial(vs + 36, 9);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], 0., 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 10") {
      subcases[1] = "N = 10";
      check.v.load_partial(vs + 36, 10);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], 0., 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 11") {
      subcases[1] = "N = 11";
      check.v.load_partial(vs + 36, 11);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], 0., 0., 0., 0., 0.);
    }
    SUBCASE("N = 12") {
      subcases[1] = "N = 12";
      check.v.load_partial(vs + 36, 12);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], vs[47], 0., 0., 0., 0.);
    }
    SUBCASE("N = 13") {
      subcases[1] = "N = 13";
      check.v.load_partial(vs + 36, 13);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], vs[47], vs[48], 0., 0., 0.);
    }
    SUBCASE("N = 14") {
      subcases[1] = "N = 14";
      check.v.load_partial(vs + 36, 14);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], vs[47], vs[48], vs[49], 0., 0.);
    }
    SUBCASE("N = 15") {
      subcases[1] = "N = 15";
      check.v.load_partial(vs + 36, 15);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], vs[47], vs[48], vs[49], vs[50], 0.);
    }
    SUBCASE("N = 16") {
      subcases[1] = "N = 16";
      check.v.load_partial(vs + 36, 16);
      expected.v = V(vs[36], vs[37], vs[38], vs[39], vs[40], vs[41], vs[42], vs[43],
                     vs[44], vs[45], vs[46], vs[47], vs[48], vs[49], vs[50], vs[51]);
    }
  }
  SUBCASE("Broadcast N") {
    subcases[0] = "Broadcast N ";
    check.v.broadcast(vs[43]);
    expected.v = V(vs[43]);
  }
  
  // Storing
  SUBCASE("Store in an array") {
    subcases[0] = "Store in an array ";
    V from(vs + 34);
    from.store(check.s);
    expected.v = V(vs + 34);
  }
  SUBCASE("Store N values in an array") {
    subcases[0] = "Store N values in an array ";
    V from(vs + 21);
    SUBCASE("N = 0") {
      subcases[1] = "N = 0";
      from.store_partial(check.s, 0);
      expected.v = V(-1., -1., -1., -1., -1., -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 1") {
      subcases[1] = "N = 1";
      from.store_partial(check.s, 1);
      expected.v = V(vs[21], -1., -1., -1., -1., -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 2") {
      subcases[1] = "N = 2";
      from.store_partial(check.s, 2);
      expected.v = V(vs[21], vs[22], -1., -1., -1., -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 3") {
      subcases[1] = "N = 3";
      from.store_partial(check.s, 3);
      expected.v = V(vs[21], vs[22], vs[23], -1., -1., -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 4") {
      subcases[1] = "N = 4";
      from.store_partial(check.s, 4);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], -1., -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 5") {
      subcases[1] = "N = 5";
      from.store_partial(check.s, 5);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], -1., -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 6") {
      subcases[1] = "N = 6";
      from.store_partial(check.s, 6);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], -1., -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 7") {
      subcases[1] = "N = 7";
      from.store_partial(check.s, 7);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], -1.,
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 8") {
      subcases[1] = "N = 8";
      from.store_partial(check.s, 8);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     -1., -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 9") {
      subcases[1] = "N = 9";
      from.store_partial(check.s, 9);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], -1., -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 10") {
      subcases[1] = "N = 10";
      from.store_partial(check.s, 10);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], -1., -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 11") {
      subcases[1] = "N = 11";
      from.store_partial(check.s, 11);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], -1., -1., -1., -1., -1.);
    }
    SUBCASE("N = 12") {
      subcases[1] = "N = 12";
      from.store_partial(check.s, 12);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], vs[32], -1., -1., -1., -1.);
    }
    SUBCASE("N = 13") {
      subcases[1] = "N = 13";
      from.store_partial(check.s, 13);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], vs[32], vs[33], -1., -1., -1.);
    }
    SUBCASE("N = 14") {
      subcases[1] = "N = 14";
      from.store_partial(check.s, 14);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], vs[32], vs[33], vs[34], -1., -1.);
    }
    SUBCASE("N = 15") {
      subcases[1] = "N = 15";
      from.store_partial(check.s, 15);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], vs[32], vs[33], vs[34], vs[35], -1.);
    }
    SUBCASE("N = 16") {
      subcases[1] = "N = 16";
      from.store_partial(check.s, 16);
      expected.v = V(vs[21], vs[22], vs[23], vs[24], vs[25], vs[26], vs[27], vs[28],
                     vs[29], vs[30], vs[31], vs[32], vs[33], vs[34], vs[35], vs[36]);
    }
  }
  
  SVL_FOR_RANGE(V::step) {
    CAPTURE(subcases[0] << subcases[1] << " Check = " << i);
    CHECK(check.s[i] == expected.s[i]);
  }
}
TEST_SUITE_END();

TEST_CASE_TEMPLATE("Vecf access single value", T, F4Scalar, F4SSE, F4AVX2,
                   F8Scalar, F8SSE, F8AVX2, F16Scalar, F16SSE, F16AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  PopulateVs(vs);
  
  // Check access
  V to_check(vs+4);
  SVL_FOR_RANGE(V::step) {
    CAPTURE(i);
    CHECK(to_check[i] == vs[4 + i]);
    CHECK(to_check.access(i) == vs[4 + i]);
  }
  
  // Check assign
  SVL_FOR_RANGE(V::step) {
    CAPTURE(i);
    CHECK(to_check.assign(vs[60 - i], i)[i] == vs[60 - i]);
  }
}

TEST_CASE_TEMPLATE("Vecf mathematics", T, F4Scalar, F4SSE, F4AVX2,
                   F8Scalar, F8SSE, F8AVX2, F16Scalar, F16SSE, F16AVX2) {
  using V = typename T::vec_t;
  using I = typename V::intrinsic_t;
  
  std::string subcases[3];
  float expected_values[16] = {0.};
  
  V to_check;
  SUBCASE("Addition") {
    subcases[0] = "Addition ";
    float tmp[32] = { 0.f,  1.f,  2.f,  3.f,  4.f,  5.f,  6.f,  7.f,
                      8.f,  9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f,
                     16.f, 17.f, 18.f, 19.f, 20.f, 21.f, 22.f, 23.f,
                     24.f, 25.f, 26.f, 27.f, 28.f, 29.f, 30.f, 31.f};
    SUBCASE("v + v") {
      float tmp2[16] = {16.f, 18.f, 20.f, 22.f, 24.f, 26.f, 28.f, 30.f,
                        32.f, 34.f, 36.f, 38.f, 40.f, 42.f, 44.f, 46.f};
      subcases[1] = "v + v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j), b(tmp + j + 16);
          to_check = a + b;
          memcpy(expected_values, tmp2 + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("v + s") {
      subcases[1] = "v + s ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j);
          float b = 2.f;
          to_check = a + b;
          memcpy(expected_values, tmp + j + 2, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("s + v") {
      subcases[1] = "s + v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j);
          float b = 2.f;
          to_check = b + a;
          memcpy(expected_values, tmp + j + 2, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("v += v") {
      float tmp2[16] = {16.f, 18.f, 20.f, 22.f, 24.f, 26.f, 28.f, 30.f,
        32.f, 34.f, 36.f, 38.f, 40.f, 42.f, 44.f, 46.f};
      subcases[1] = "v += v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j);
          V b(tmp + j + 16);
          to_check += b;
          memcpy(expected_values, tmp2 + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("v += s") {
      subcases[1] = "v += s ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j);
          float b = 2.f;
          to_check += b;
          memcpy(expected_values, tmp + j + 2, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("++v") {
      subcases[1] = "++v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j);
          V prefix = ++to_check;
          memcpy(expected_values, tmp + j + 1, V::step * sizeof(float));
          SVL_FOR_RANGE(V::step) {
            CAPTURE(subcases[0] << subcases[1] << "Prefix return Check " << i);
            CHECK(prefix[i] == expected_values[i]);
          }
        }
      }
    }
    SUBCASE("v++") {
      subcases[1] = "v++ ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j);
          V postfix = to_check++;
          memcpy(expected_values, tmp + j, V::step * sizeof(float));
          SVL_FOR_RANGE(V::step) {
            CAPTURE(subcases[0] << subcases[1] << "Postfix return Check " << i);
            CHECK(postfix[i] == expected_values[i]);
          }
          memcpy(expected_values, tmp + j + 1, V::step * sizeof(float));
        }
      }
    }
  }
  SUBCASE("Subtraction") {
    subcases[0] = "Subtraction ";
    float tmp[32] = { 0.f,  1.f,  2.f,  3.f,  4.f,  5.f,  6.f,  7.f,
      8.f,  9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f,
      16.f, 17.f, 18.f, 19.f, 20.f, 21.f, 22.f, 23.f,
      24.f, 25.f, 26.f, 27.f, 28.f, 29.f, 30.f, 31.f};
    SUBCASE("v - v") {
      float tmp2[16] = {-16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f,
                        -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f};
      subcases[1] = "v - v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j), b(tmp + j + 16);
          to_check = a - b;
          memcpy(expected_values, tmp2 + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("v - s") {
      subcases[1] = "v - s ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j + 2);
          float b = 2.f;
          to_check = a - b;
          memcpy(expected_values, tmp + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("s - v") {
      subcases[1] = "s - v ";
      for (unsigned j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          V a(tmp + j);
          float b = 31.f;
          to_check = b - a;
          for (unsigned k = 0; k < V::step; ++k)
            expected_values[k] = tmp[31 - j - k];
        }
      }
    }
    SUBCASE("v -= v") {
      float tmp2[16] = {-16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f,
        -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f, -16.f};
      subcases[1] = "v -= v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j);
          V b(tmp + j + 16);
          to_check -= b;
          memcpy(expected_values, tmp2 + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("v -= s") {
      subcases[1] = "v -= s ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j + 2);
          float b = 2.f;
          to_check -= b;
          memcpy(expected_values, tmp + j, V::step * sizeof(float));
        }
      }
    }
    SUBCASE("--v") {
      subcases[1] = "--v ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j + 1);
          V prefix = --to_check;
          memcpy(expected_values, tmp + j, V::step * sizeof(float));
          SVL_FOR_RANGE(V::step) {
            CAPTURE(subcases[0] << subcases[1] << "Prefix return Check " << i);
            CHECK(prefix[i] == expected_values[i]);
          }
        }
      }
    }
    SUBCASE("v--") {
      subcases[1] = "v-- ";
      for (int j = 0; j < 16; j += V::step) {
        SUBCASE("begin") {
          subcases[2] = "begin at " + std::to_string(j) + " ";
          to_check.load(tmp + j + 1);
          V postfix = to_check--;
          memcpy(expected_values, tmp + j + 1, V::step * sizeof(float));
          SVL_FOR_RANGE(V::step) {
            CAPTURE(subcases[0] << subcases[1] << "Postfix return Check " << i);
            CHECK(postfix[i] == expected_values[i]);
          }
          memcpy(expected_values, tmp + j, V::step * sizeof(float));
        }
      }
    }
  }
  
  // CHecking
  SVL_FOR_RANGE(V::step) {
    CAPTURE(subcases[0] << subcases[1] << subcases[2] << "Check " << i);
    CHECK(to_check[i] == expected_values[i]);
  }
  
}

