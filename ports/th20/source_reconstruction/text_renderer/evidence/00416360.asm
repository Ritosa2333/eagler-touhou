00416360  55                                 push       ebp
00416361  8b ec                              mov        ebp, esp
00416363  51                                 push       ecx
00416364  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00416367  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041636a  83 b8 14 01 00 00 00               cmp        dword ptr [eax + 0x114], 0
00416371  0f 84 99 00 00 00                  je         0x416410
00416377  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041637a  8b 91 18 01 00 00                  mov        edx, dword ptr [ecx + 0x118]
00416380  52                                 push       edx
00416381  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00416384  8b 88 14 01 00 00                  mov        ecx, dword ptr [eax + 0x114]
0041638a  51                                 push       ecx
0041638b  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00416391  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
00416394  8b 82 14 01 00 00                  mov        eax, dword ptr [edx + 0x114]
0041639a  50                                 push       eax
0041639b  ff 15 3c c0 56 00                  call       dword ptr [0x56c03c] ; GDI32.dll!DeleteDC
004163a1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004163a4  8b 91 1c 01 00 00                  mov        edx, dword ptr [ecx + 0x11c]
004163aa  52                                 push       edx
004163ab  ff 15 38 c0 56 00                  call       dword ptr [0x56c038] ; GDI32.dll!DeleteObject
004163b1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004163b4  c7 80 00 01 00 00 ff ff ff ff      mov        dword ptr [eax + 0x100], 0xffffffff
004163be  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004163c1  c7 81 04 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x104], 0
004163cb  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004163ce  c7 82 08 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x108], 0
004163d8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004163db  c7 80 14 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x114], 0
004163e5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004163e8  c7 81 1c 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x11c], 0
004163f2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004163f5  c7 82 18 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x118], 0
004163ff  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
00416402  c7 80 20 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x120], 0
0041640c  b0 01                              mov        al, 1
0041640e  eb 02                              jmp        0x416412
00416410  32 c0                              xor        al, al
00416412  8b e5                              mov        esp, ebp
00416414  5d                                 pop        ebp
00416415  c3                                 ret        
