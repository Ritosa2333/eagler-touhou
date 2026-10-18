0041dff0  55                                 push       ebp
0041dff1  8b ec                              mov        ebp, esp
0041dff3  51                                 push       ecx
0041dff4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041dff7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041dffa  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041dffd  89 48 5c                           mov        dword ptr [eax + 0x5c], ecx
0041e000  8b e5                              mov        esp, ebp
0041e002  5d                                 pop        ebp
0041e003  c2 04 00                           ret        4
