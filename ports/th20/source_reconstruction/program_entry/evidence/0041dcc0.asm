; draw counter setter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041dcc0  55                                 push       ebp
0041dcc1  8b ec                              mov        ebp, esp
0041dcc3  51                                 push       ecx
0041dcc4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dcc7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dcca  8a 4d 08                           mov        cl, byte ptr [ebp + 8]
0041dccd  88 48 70                           mov        byte ptr [eax + 0x70], cl
0041dcd0  8b e5                              mov        esp, ebp
0041dcd2  5d                                 pop        ebp
0041dcd3  c2 04 00                           ret        4
