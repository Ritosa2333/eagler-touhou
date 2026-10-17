; proven side-effect-free constant-zero function
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00412540  55                                 push       ebp
00412541  8b ec                              mov        ebp, esp
00412543  51                                 push       ecx
00412544  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00412547  33 c0                              xor        eax, eax
00412549  8b e5                              mov        esp, ebp
0041254b  5d                                 pop        ebp
0041254c  c3                                 ret
