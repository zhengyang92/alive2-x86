
define <64 x i8> @tgt() {
entry:
  ret <64 x i8> <i8 57, i8 28, i8 66, i8 70, i8 32, i8 113, i8 114, i8 58, i8 109, i8 9, i8 54, i8 57, i8 101, i8 13, i8 60, i8 37, i8 48, i8 66, i8 115, i8 20, i8 60, i8 35, i8 77, i8 41, i8 -115, i8 112, i8 105, i8 26, i8 43, i8 81, i8 41, i8 117, i8 15, i8 88, i8 29, i8 56, i8 13, i8 116, i8 109, i8 -126, i8 38, i8 93, i8 93, i8 101, i8 33, i8 35, i8 31, i8 57, i8 97, i8 34, i8 104, i8 -124, i8 76, i8 121, i8 69, i8 92, i8 32, i8 86, i8 43, i8 112, i8 89, i8 109, i8 81, i8 9>
}
define <64 x i8> @src() {
entry:
  %calltmp = call <64 x i8> @llvm.x86.avx512.pavg.b.512(<64 x i8> <i8 79, i8 52, i8 101, i8 -124, i8 34, i8 -45, i8 -59, i8 104, i8 -63, i8 7, i8 75, i8 94, i8 -94, i8 7, i8 116, i8 67, i8 73, i8 126, i8 -55, i8 5, i8 115, i8 40, i8 -124, i8 69, i8 -4, i8 -40, i8 -48, i8 30, i8 81, i8 -97, i8 73, i8 -34, i8 19, i8 -82, i8 35, i8 86, i8 8, i8 -37, i8 -39, i8 -5, i8 51, i8 -81, i8 -84, i8 -85, i8 43, i8 41, i8 40, i8 113, i8 -67, i8 45, i8 -54, i8 -29, i8 -125, i8 -53, i8 -124, i8 -101, i8 49, i8 -121, i8 81, i8 -50, i8 -109, i8 -48, i8 -101, i8 11>, <64 x i8> <i8 34, i8 3, i8 30, i8 7, i8 29, i8 15, i8 30, i8 11, i8 25, i8 10, i8 32, i8 19, i8 39, i8 19, i8 4, i8 7, i8 22, i8 6, i8 29, i8 35, i8 5, i8 29, i8 21, i8 12, i8 30, i8 7, i8 1, i8 22, i8 4, i8 2, i8 8, i8 11, i8 11, i8 1, i8 23, i8 26, i8 18, i8 12, i8 1, i8 8, i8 25, i8 10, i8 14, i8 30, i8 22, i8 29, i8 22, i8 1, i8 4, i8 22, i8 5, i8 36, i8 21, i8 39, i8 5, i8 29, i8 14, i8 37, i8 5, i8 17, i8 31, i8 10, i8 6, i8 6>)
    ret <64 x i8> %calltmp
}
declare  <64 x i8> @llvm.x86.avx512.pavg.b.512 (<64 x i8>, <64 x i8>)