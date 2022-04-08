#pragma once

// Copyright (c) 2021-present The Alive2 Authors.
// Distributed under the MIT license that can be found in the LICENSE file.
// Version: April 7, 2022

#include <array>
#include <immintrin.h>
#include <iostream>
#include <stdexcept>
#include <variant>

#include "randomizer.h"
#include "x86Intrin.h"

template <typename type> constexpr unsigned bitSize() {
  if constexpr (std::is_same_v<type, __m512i>)
    return 512;
  else if constexpr (std::is_same_v<type, __m256i>)
    return 256;
  else if constexpr (std::is_same_v<type, __m128i>)
    return 128;
  else if constexpr (std::is_same_v<type, int32_t>)
    return 32;
  else
    throw std::invalid_argument(
        "bitSize: Must be a type of __m128i, __m256i, __m512i, or int32_t");
}

// Function that prints a vector with a specified bitwidth
template <unsigned bitwidth, typename type>
void constexpr printVec(type input) {
  std::variant<int64_t *, int32_t *, int16_t *, int8_t *> vals;
  if constexpr (bitwidth == 64)
    vals = (int64_t *)&input;
  else if constexpr (bitwidth == 32)
    vals = (int32_t *)&input;
  else if constexpr (bitwidth == 16)
    vals = (int16_t *)&input;
  else if constexpr (bitwidth == 8)
    vals = (int8_t *)&input;
  else
    throw std::invalid_argument(
        "Bitwidth must be powers of 2 between 2 and 64");

  constexpr unsigned vectorBitSize = bitSize<type>();

  // Cast is required because int8_t is synonymous to signed char
  std::cout << "< ";
  for (unsigned i = 0; i < vectorBitSize / bitwidth; ++i)
    std::visit(
        [&](auto &&arg) { std::cout << static_cast<int64_t>(arg[i]) << " "; },
        vals);
  std::cout << ">\n";
}
// Function that prints a value mismatch from two operands
template <unsigned bitwidthOp1, unsigned bitwidthOp2, unsigned bitwidthRet,
          typename typeOp1, typename typeOp2, typename typeRet>
void constexpr printError(typeOp1 input1, typeOp2 input2, typeRet output1,
                          typeRet output2) {
  std::cout << "========================\n";
  std::cout << "\nOperand 1:\n";
  printVec<bitwidthOp1>(input1);
  std::cout << "\nOperand 2:\n";
  printVec<bitwidthOp2>(input2);
  std::cout << "\nOutput 1(src):\n";
  printVec<bitwidthRet>(output1);
  std::cout << "\nOutput 2(intrinsic call):\n";
  printVec<bitwidthRet>(output2);
  std::cout << "\n========================\n\n";
}
// Function that compares two vectors for equality
template <unsigned bitwidth, typename type>
bool constexpr areEqual(type input1, type input2) {
  std::variant<int64_t *, int32_t *, int16_t *, int8_t *> vals1;
  std::variant<int64_t *, int32_t *, int16_t *, int8_t *> vals2;
  if constexpr (bitwidth == 64) {
    vals1 = (int64_t *)&input1;
    vals2 = (int64_t *)&input2;
  } else if constexpr (bitwidth == 32) {
    vals1 = (int32_t *)&input1;
    vals2 = (int32_t *)&input2;
  } else if constexpr (bitwidth == 16) {
    vals1 = (int16_t *)&input1;
    vals2 = (int16_t *)&input2;
  } else if constexpr (bitwidth == 8) {
    vals1 = (int8_t *)&input1;
    vals2 = (int8_t *)&input2;
  } else
    throw std::invalid_argument(
        "Bitwidth must be powers of 2 between 2 and 64");

  constexpr unsigned vectorBitSize = bitSize<type>();
  if constexpr (vectorBitSize != bitSize<type>())
    throw std::invalid_argument("Vectors can't be equal - differing sizes!");

  for (unsigned i = 0; i < vectorBitSize / bitwidth; ++i) {
    std::variant<int64_t, int32_t, int16_t, int8_t> val1, val2;
    std::visit(
        [&](auto &&arg1, auto &&arg2) {
          val1 = arg1[i];
          val2 = arg2[i];
        },
        vals1, vals2);

    if (val1 != val2)
      return false;
  }

  return true;
}

template <unsigned bitwidth, typename type>
type constexpr vectorRandomizer(type input) {
  constexpr int vectorBitSize = bitSize<type>();

  std::variant<std::array<int64_t, vectorBitSize / 64>,
               std::array<int32_t, vectorBitSize / 32>,
               std::array<int16_t, vectorBitSize / 16>,
               std::array<int8_t, vectorBitSize / 8>>
      vals;
  if constexpr (bitwidth == 64)
    vals = std::array<int64_t, vectorBitSize / 64>();
  else if constexpr (bitwidth == 32)
    vals = std::array<int32_t, vectorBitSize / 32>();
  else if constexpr (bitwidth == 16)
    vals = std::array<int16_t, vectorBitSize / 16>();
  else if constexpr (bitwidth == 8)
    vals = std::array<int8_t, vectorBitSize / 8>();
  else
    throw std::invalid_argument(
        "vectorRandomizer: Bitwidth must be powers of 2 between 2 and 64");

  const int32_t pathToTake = Randomizer::randInt<32, 7, int32_t>();

  for (unsigned i = 0; i < vectorBitSize / bitwidth; ++i) {
    if (pathToTake == 0) // Full range of integers, no upper bound
      std::visit(
          [&](auto &&arg) {
            arg[i] = Randomizer::randInt<bitwidth, 0, int64_t>();
          },
          vals);              // It works, but int64_t shouldn't be here
    else if (pathToTake == 1) // Upper bound of 65 for shifts, 75% zeroes
      std::visit(
          [&](auto &&arg) {
            if (Randomizer::randInt<32, 4, int32_t>() != 0)
              arg[i] = 0;
            else
              arg[i] = Randomizer::randInt<bitwidth, 65, int64_t>();
          },
          vals);
    else if (pathToTake ==
             2) // Upper bound of 33 for smaller shifts, 75% zeroes
      std::visit(
          [&](auto &&arg) {
            if (Randomizer::randInt<32, 4, int32_t>() != 0)
              arg[i] = 0;
            else
              arg[i] = Randomizer::randInt<bitwidth, 33, int64_t>();
          },
          vals);
    else if (pathToTake == 3) // Upper bound of 17 for shifts, 75% zeroes
      std::visit(
          [&](auto &&arg) {
            if (Randomizer::randInt<32, 4, int32_t>() != 0)
              arg[i] = 0;
            else
              arg[i] = Randomizer::randInt<bitwidth, 17, int64_t>();
          },
          vals);
    else if (pathToTake == 4) // Upper bound of 9 for shuffles, 75% zeroes
      std::visit(
          [&](auto &&arg) {
            if (Randomizer::randInt<32, 4, int32_t>() != 0)
              arg[i] = 0;
            else
              arg[i] = Randomizer::randInt<bitwidth, 9, int64_t>();
          },
          vals);
    else if (pathToTake ==
             5) // Upper bound of 5 for smaller shuffles, 75% zeroes
      std::visit(
          [&](auto &&arg) {
            if (Randomizer::randInt<32, 4, int32_t>() != 0)
              arg[i] = 0;
            else
              arg[i] = Randomizer::randInt<bitwidth, 5, int64_t>();
          },
          vals);
    else if (pathToTake == 6) // Pre-determined interesting values
      std::visit(
          [&](auto &&arg) {
            arg[i] = Randomizer::randIntInteresting<int64_t>();
          },
          vals);
  }

  type returnVector;

  if constexpr (vectorBitSize == 512) {
    if constexpr (bitwidth == 64) {
      returnVector = _mm512_setr_epi64(
          std::get<0>(vals)[0], std::get<0>(vals)[1], std::get<0>(vals)[2],
          std::get<0>(vals)[3], std::get<0>(vals)[4], std::get<0>(vals)[5],
          std::get<0>(vals)[6], std::get<0>(vals)[7]);
    } else if constexpr (bitwidth == 32) {
      returnVector = _mm512_setr_epi32(
          std::get<1>(vals)[0], std::get<1>(vals)[1], std::get<1>(vals)[2],
          std::get<1>(vals)[3], std::get<1>(vals)[4], std::get<1>(vals)[5],
          std::get<1>(vals)[6], std::get<1>(vals)[7], std::get<1>(vals)[8],
          std::get<1>(vals)[9], std::get<1>(vals)[10], std::get<1>(vals)[11],
          std::get<1>(vals)[12], std::get<1>(vals)[13], std::get<1>(vals)[14],
          std::get<1>(vals)[15]);
    } else if constexpr (bitwidth == 16) {
      returnVector = _mm512_set_epi16(
          std::get<2>(vals)[31], std::get<2>(vals)[30], std::get<2>(vals)[29],
          std::get<2>(vals)[28], std::get<2>(vals)[27], std::get<2>(vals)[26],
          std::get<2>(vals)[25], std::get<2>(vals)[24], std::get<2>(vals)[23],
          std::get<2>(vals)[22], std::get<2>(vals)[21], std::get<2>(vals)[20],
          std::get<2>(vals)[19], std::get<2>(vals)[18], std::get<2>(vals)[17],
          std::get<2>(vals)[16], std::get<2>(vals)[15], std::get<2>(vals)[14],
          std::get<2>(vals)[13], std::get<2>(vals)[12], std::get<2>(vals)[11],
          std::get<2>(vals)[10], std::get<2>(vals)[9], std::get<2>(vals)[8],
          std::get<2>(vals)[7], std::get<2>(vals)[6], std::get<2>(vals)[5],
          std::get<2>(vals)[4], std::get<2>(vals)[3], std::get<2>(vals)[2],
          std::get<2>(vals)[1], std::get<2>(vals)[0]);

    } else if constexpr (bitwidth == 8) {
      returnVector = _mm512_set_epi8(
          std::get<3>(vals)[63], std::get<3>(vals)[62], std::get<3>(vals)[61],
          std::get<3>(vals)[60], std::get<3>(vals)[59], std::get<3>(vals)[58],
          std::get<3>(vals)[57], std::get<3>(vals)[56], std::get<3>(vals)[55],
          std::get<3>(vals)[54], std::get<3>(vals)[53], std::get<3>(vals)[52],
          std::get<3>(vals)[51], std::get<3>(vals)[50], std::get<3>(vals)[49],
          std::get<3>(vals)[48], std::get<3>(vals)[47], std::get<3>(vals)[46],
          std::get<3>(vals)[45], std::get<3>(vals)[44], std::get<3>(vals)[43],
          std::get<3>(vals)[42], std::get<3>(vals)[41], std::get<3>(vals)[40],
          std::get<3>(vals)[39], std::get<3>(vals)[38], std::get<3>(vals)[37],
          std::get<3>(vals)[36], std::get<3>(vals)[35], std::get<3>(vals)[34],
          std::get<3>(vals)[33], std::get<3>(vals)[32], std::get<3>(vals)[31],
          std::get<3>(vals)[30], std::get<3>(vals)[29], std::get<3>(vals)[28],
          std::get<3>(vals)[27], std::get<3>(vals)[26], std::get<3>(vals)[25],
          std::get<3>(vals)[24], std::get<3>(vals)[23], std::get<3>(vals)[22],
          std::get<3>(vals)[21], std::get<3>(vals)[20], std::get<3>(vals)[19],
          std::get<3>(vals)[18], std::get<3>(vals)[17], std::get<3>(vals)[16],
          std::get<3>(vals)[15], std::get<3>(vals)[14], std::get<3>(vals)[13],
          std::get<3>(vals)[12], std::get<3>(vals)[11], std::get<3>(vals)[10],
          std::get<3>(vals)[9], std::get<3>(vals)[8], std::get<3>(vals)[7],
          std::get<3>(vals)[6], std::get<3>(vals)[5], std::get<3>(vals)[4],
          std::get<3>(vals)[3], std::get<3>(vals)[2], std::get<3>(vals)[1],
          std::get<3>(vals)[0]);
    }
  } else if constexpr (vectorBitSize == 256) {
    if constexpr (bitwidth == 64) {
      returnVector =
          _mm256_setr_epi64x(std::get<0>(vals)[0], std::get<0>(vals)[1],
                             std::get<0>(vals)[2], std::get<0>(vals)[3]);
    } else if constexpr (bitwidth == 32) {
      returnVector = _mm256_setr_epi32(
          std::get<1>(vals)[0], std::get<1>(vals)[1], std::get<1>(vals)[2],
          std::get<1>(vals)[3], std::get<1>(vals)[4], std::get<1>(vals)[5],
          std::get<1>(vals)[6], std::get<1>(vals)[7]);
    } else if constexpr (bitwidth == 16) {
      returnVector = _mm256_setr_epi16(
          std::get<2>(vals)[0], std::get<2>(vals)[1], std::get<2>(vals)[2],
          std::get<2>(vals)[3], std::get<2>(vals)[4], std::get<2>(vals)[5],
          std::get<2>(vals)[6], std::get<2>(vals)[7], std::get<2>(vals)[8],
          std::get<2>(vals)[9], std::get<2>(vals)[10], std::get<2>(vals)[11],
          std::get<2>(vals)[12], std::get<2>(vals)[13], std::get<2>(vals)[14],
          std::get<2>(vals)[15]);

    } else if constexpr (bitwidth == 8) {
      returnVector = _mm256_setr_epi8(
          std::get<3>(vals)[0], std::get<3>(vals)[1], std::get<3>(vals)[2],
          std::get<3>(vals)[3], std::get<3>(vals)[4], std::get<3>(vals)[5],
          std::get<3>(vals)[6], std::get<3>(vals)[7], std::get<3>(vals)[8],
          std::get<3>(vals)[9], std::get<3>(vals)[10], std::get<3>(vals)[11],
          std::get<3>(vals)[12], std::get<3>(vals)[13], std::get<3>(vals)[14],
          std::get<3>(vals)[15], std::get<3>(vals)[16], std::get<3>(vals)[17],
          std::get<3>(vals)[18], std::get<3>(vals)[19], std::get<3>(vals)[20],
          std::get<3>(vals)[21], std::get<3>(vals)[22], std::get<3>(vals)[23],
          std::get<3>(vals)[24], std::get<3>(vals)[25], std::get<3>(vals)[26],
          std::get<3>(vals)[27], std::get<3>(vals)[28], std::get<3>(vals)[29],
          std::get<3>(vals)[30], std::get<3>(vals)[31]);
    }
  } else if constexpr (vectorBitSize == 128) {
    if constexpr (bitwidth == 64) {
      returnVector =
          _mm_set_epi64x(std::get<0>(vals)[1],
                         std::get<0>(vals)[0]); // For some reason only epi64x
                                                // doesn't have a reverse load
    } else if constexpr (bitwidth == 32) {
      returnVector = _mm_setr_epi32(std::get<1>(vals)[0], std::get<1>(vals)[1],
                                    std::get<1>(vals)[2], std::get<1>(vals)[3]);
    } else if constexpr (bitwidth == 16) {
      returnVector = _mm_setr_epi16(std::get<2>(vals)[0], std::get<2>(vals)[1],
                                    std::get<2>(vals)[2], std::get<2>(vals)[3],
                                    std::get<2>(vals)[4], std::get<2>(vals)[5],
                                    std::get<2>(vals)[6], std::get<2>(vals)[7]);

    } else if constexpr (bitwidth == 8) {
      returnVector = _mm_setr_epi8(
          std::get<3>(vals)[0], std::get<3>(vals)[1], std::get<3>(vals)[2],
          std::get<3>(vals)[3], std::get<3>(vals)[4], std::get<3>(vals)[5],
          std::get<3>(vals)[6], std::get<3>(vals)[7], std::get<3>(vals)[8],
          std::get<3>(vals)[9], std::get<3>(vals)[10], std::get<3>(vals)[11],
          std::get<3>(vals)[12], std::get<3>(vals)[13], std::get<3>(vals)[14],
          std::get<3>(vals)[15]);
    }
  } else
    throw std::invalid_argument(
        "vectorRandomizer: vectorwidth must be either 512, 256 or 128 bits");
  return returnVector;
}

template <int bitSize>
std::variant<__m128i, __m256i, __m512i, int32_t> constexpr returnVectorType() {
  std::variant<__m128i, __m256i, __m512i, int32_t> vectors;
  if constexpr (bitSize == 128) {
    vectors = _mm_setzero_si128();
    return vectors;
  } else if constexpr (bitSize == 256) {
    vectors = _mm256_setzero_si256();
    return vectors;
  } else if constexpr (bitSize == 512) {
    vectors = _mm512_setzero_epi32();
    return vectors;
  } else if constexpr (bitSize == 32) {
    constexpr int32_t val = 0;
    vectors = val;
    return vectors;
  } else
    throw std::invalid_argument(
        "returnVectorType must take in either 128, 256, or 512");
}

template <int bitSize> int constexpr vectorTypeIndex() {
  if constexpr (bitSize == 128)
    return 0;
  else if constexpr (bitSize == 256)
    return 1;
  else if constexpr (bitSize == 512)
    return 2;
  else if constexpr (bitSize == 32)
    return 3;
  else
    throw std::invalid_argument(
        "vectorTypeIndex must take in either 128, 256, or 512");
}

int32_t constexpr integerRandomizer() {
  constexpr int bitwidth = 32;
  const int32_t pathToTake = Randomizer::randInt<32, 6, int32_t>();

  if (pathToTake == 0)
    return Randomizer::randInt<bitwidth, 0, int32_t>();
  else if (pathToTake == 1)
    return Randomizer::randInt<bitwidth, 33, int32_t>();
  else if (pathToTake == 2)
    return Randomizer::randInt<bitwidth, 17, int32_t>();
  else if (pathToTake == 3)
    return Randomizer::randInt<bitwidth, 9, int32_t>();
  else if (pathToTake == 4)
    return Randomizer::randInt<bitwidth, 5, int32_t>();
  else
    return Randomizer::randIntInteresting<int32_t>();
}
