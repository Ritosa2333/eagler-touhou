0041df70  55                                 push       ebp
0041df71  8b ec                              mov        ebp, esp
0041df73  51                                 push       ecx
0041df74  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041df77  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041df7a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041df7d  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041df80  89 54 81 38                        mov        dword ptr [ecx + eax*4 + 0x38], edx
0041df84  8b e5                              mov        esp, ebp
0041df86  5d                                 pop        ebp
0041df87  c2 08 00                           ret        8
