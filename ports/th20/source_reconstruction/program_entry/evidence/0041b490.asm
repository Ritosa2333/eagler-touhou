; restore system settings
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041b490  55                                 push       ebp
0041b491  8b ec                              mov        ebp, esp
0041b493  51                                 push       ecx
0041b494  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041b497  6a 02                              push       2
0041b499  6a 00                              push       0
0041b49b  0f b6 05 e1 87 5b 00               movzx      eax, byte ptr [0x5b87e1]
0041b4a2  50                                 push       eax
0041b4a3  6a 11                              push       0x11
0041b4a5  ff 15 84 c2 56 00                  call       dword ptr [0x56c284] ; USER32.dll!SystemParametersInfoW
0041b4ab  6a 02                              push       2
0041b4ad  6a 00                              push       0
0041b4af  0f b6 0d e2 87 5b 00               movzx      ecx, byte ptr [0x5b87e2]
0041b4b6  51                                 push       ecx
0041b4b7  6a 55                              push       0x55
0041b4b9  ff 15 84 c2 56 00                  call       dword ptr [0x56c284] ; USER32.dll!SystemParametersInfoW
0041b4bf  6a 02                              push       2
0041b4c1  6a 00                              push       0
0041b4c3  0f b6 15 e3 87 5b 00               movzx      edx, byte ptr [0x5b87e3]
0041b4ca  52                                 push       edx
0041b4cb  6a 56                              push       0x56
0041b4cd  ff 15 84 c2 56 00                  call       dword ptr [0x56c284] ; USER32.dll!SystemParametersInfoW
0041b4d3  6a 01                              push       1
0041b4d5  6a 00                              push       0
0041b4d7  e8 7e 51 12 00                     call       0x54065a
0041b4dc  90                                 nop
0041b4dd  8b e5                              mov        esp, ebp
0041b4df  5d                                 pop        ebp
0041b4e0  c3                                 ret
