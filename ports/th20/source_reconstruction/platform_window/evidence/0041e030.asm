0041e030  55                                 push       ebp
0041e031  8b ec                              mov        ebp, esp
0041e033  51                                 push       ecx
0041e034  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041e037  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041e03a  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041e03d  89 88 8c 20 00 00                  mov        dword ptr [eax + 0x208c], ecx
0041e043  8b e5                              mov        esp, ebp
0041e045  5d                                 pop        ebp
0041e046  c2 04 00                           ret        4
