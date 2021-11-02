; TEST-ARGS: -disable-undef-input
; disabled undef inputs to accelerate the verification

declare <2 x i32> @llvm.x86.mmx.padd.d(<2 x i32>, <2 x i32>)

define <2 x i32> @src(<2 x i32> %v, <2 x i32> %v2) {
  %1 = call <2 x i32> @llvm.x86.mmx.padd.d(<2 x i32> %v, <2 x i32> %v2)
  ret <2 x i32> %1
}

define <2 x i32> @tgt(<2 x i32> %v, <2 x i32> %v2) {
  %tmp = add <2 x i32> %v, %v2
  ret <2 x i32> %tmp
}
