
// block 00430720
00430720  push       ecx
00430721  push       esi
00430722  mov        esi, eax
00430724  cmp        dword ptr [esi + 0x93c], 0
0043072b  jne        0x430836

// block 00430731
00430731  mov        ecx, dword ptr [esi + 0x58c]
00430737  push       0
00430739  push       0
0043073b  lea        eax, [esp + 0xc]
0043073f  push       eax
00430740  push       ecx
00430741  mov        eax, 0x17
00430746  xor        ecx, ecx
00430748  call       0x4615a0

// block 0043074d
0043074d  mov        edx, dword ptr [esp + 4]
00430751  push       0
00430753  push       1
00430755  lea        eax, [esp + 0xc]
00430759  mov        dword ptr [0x4b44fc], edx
0043075f  mov        ecx, dword ptr [esi + 0x58c]
00430765  push       eax
00430766  push       ecx
00430767  mov        eax, 0x17
0043076c  xor        ecx, ecx
0043076e  call       0x4615a0

// block 00430773
00430773  mov        edx, dword ptr [esp + 4]
00430777  push       0
00430779  push       2
0043077b  lea        eax, [esp + 0xc]
0043077f  mov        dword ptr [0x4b4500], edx
00430785  mov        ecx, dword ptr [esi + 0x58c]
0043078b  push       eax
0043078c  push       ecx
0043078d  mov        eax, 0x17
00430792  xor        ecx, ecx
00430794  call       0x4615a0

// block 00430799
00430799  mov        edx, dword ptr [esp + 4]
0043079d  mov        dword ptr [0x4b4504], edx
004307a3  mov        dword ptr [esi + 0x93c], 1
004307ad  mov        eax, dword ptr [0x4b44fc]
004307b2  mov        esi, dword ptr [0x4ce8cc]
004307b8  push       eax
004307b9  mov        edx, esi
004307bb  call       0x461920

// block 004307c0
004307c0  test       eax, eax
004307c2  je         0x4307de

// block 004307c4
004307c4  mov        ecx, dword ptr [edi]
004307c6  mov        dword ptr [eax + 0x430], ecx
004307cc  mov        edx, dword ptr [edi + 4]
004307cf  mov        dword ptr [eax + 0x434], edx
004307d5  mov        ecx, dword ptr [edi + 8]
004307d8  mov        dword ptr [eax + 0x438], ecx

// block 004307de
004307de  mov        edx, dword ptr [0x4b4500]
004307e4  push       edx
004307e5  mov        edx, esi
004307e7  call       0x461920

// block 004307ec
004307ec  test       eax, eax
004307ee  je         0x43080a

// block 004307f0
004307f0  mov        ecx, dword ptr [edi]
004307f2  mov        dword ptr [eax + 0x430], ecx
004307f8  mov        edx, dword ptr [edi + 4]
004307fb  mov        dword ptr [eax + 0x434], edx
00430801  mov        ecx, dword ptr [edi + 8]
00430804  mov        dword ptr [eax + 0x438], ecx

// block 0043080a
0043080a  mov        edx, dword ptr [0x4b4504]
00430810  push       edx
00430811  mov        edx, esi
00430813  call       0x461920

// block 00430818
00430818  test       eax, eax
0043081a  je         0x430836

// block 0043081c
0043081c  mov        ecx, dword ptr [edi]
0043081e  mov        dword ptr [eax + 0x430], ecx
00430824  mov        edx, dword ptr [edi + 4]
00430827  mov        dword ptr [eax + 0x434], edx
0043082d  mov        ecx, dword ptr [edi + 8]
00430830  mov        dword ptr [eax + 0x438], ecx

// block 00430836
00430836  pop        esi
00430837  pop        ecx
00430838  ret        
