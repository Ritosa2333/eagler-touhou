
// block 00408580
00408580  push       ebx
00408581  push       ebp
00408582  mov        ebp, dword ptr [esp + 0xc]
00408586  push       esi
00408587  push       edi
00408588  mov        edi, dword ptr [ebp + 0x500]
0040858e  mov        esi, 8

// block 00408593
00408593  call       0x408030

// block 00408598
00408598  add        edi, 0xb4
0040859e  sub        esi, 1
004085a1  jne        0x408593

// block 004085a3
004085a3  mov        eax, dword ptr [ebp + 0x48]
004085a6  push       eax
004085a7  lea        ebx, [esi + 1]
004085aa  call       0x461970

// block 004085af
004085af  mov        eax, dword ptr [ebp + 0x500]
004085b5  xor        edi, edi
004085b7  cmp        eax, edi
004085b9  je         0x4085ca

// block 004085bb
004085bb  push       eax
004085bc  call       0x46c941

// block 004085c1
004085c1  add        esp, 4
004085c4  mov        dword ptr [ebp + 0x500], edi

// block 004085ca
004085ca  push       0x44
004085cc  call       0x46c9ea

// block 004085d1
004085d1  mov        esi, eax
004085d3  add        esp, 4
004085d6  cmp        esi, edi
004085d8  je         0x4085fe

// block 004085da
004085da  and        dword ptr [esi + 0x40], 0xfffffffe
004085de  push       0x44
004085e0  push       edi
004085e1  push       esi
004085e2  call       0x477420

// block 004085e7
004085e7  or         dword ptr [esi], 2
004085ea  add        esp, 0xc
004085ed  push       0x46
004085ef  push       edi
004085f0  push       6
004085f2  push       6
004085f4  push       8
004085f6  push       1
004085f8  push       esi
004085f9  call       0x452a60

// block 004085fe
004085fe  mov        dword ptr [ebp + 0x3c], edi
00408601  pop        edi
00408602  pop        esi
00408603  pop        ebp
00408604  xor        eax, eax
00408606  pop        ebx
00408607  ret        4
