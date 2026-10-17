004162f0  55                                 push       ebp
004162f1  8b ec                              mov        ebp, esp
004162f3  83 ec 0c                           sub        esp, 0xc
004162f6  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004162f9  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004162fc  50                                 push       eax
004162fd  e8 7e 4f ff ff                     call       0x40b280
00416302  83 c4 04                           add        esp, 4
00416305  89 45 f8                           mov        dword ptr [ebp - 8], eax
00416308  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041630b  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0041630e  89 55 f4                           mov        dword ptr [ebp - 0xc], edx
00416311  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
00416314  50                                 push       eax
00416315  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00416318  51                                 push       ecx
00416319  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041631c  e8 bf ce ff ff                     call       0x4131e0
00416321  90                                 nop        
00416322  8b e5                              mov        esp, ebp
00416324  5d                                 pop        ebp
00416325  c2 04 00                           ret        4
