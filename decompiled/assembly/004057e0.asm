
// block 004057e0
004057e0  push       esi
004057e1  push       edi
004057e2  mov        edi, dword ptr [0x4b43bc]
004057e8  push       0x44
004057ea  call       0x46c9ea

// block 004057ef
004057ef  mov        esi, eax
004057f1  add        esp, 4
004057f4  test       esi, esi
004057f6  je         0x40581e

// block 004057f8
004057f8  and        dword ptr [esi + 0x40], 0xfffffffe
004057fc  push       0x44
004057fe  push       0
00405800  push       esi
00405801  call       0x477420

// block 00405806
00405806  or         dword ptr [esi], 2
00405809  add        esp, 0xc
0040580c  push       0xb
0040580e  push       0
00405810  push       0
00405812  push       0
00405814  push       0x1e
00405816  push       2
00405818  push       esi
00405819  call       0x452a60

// block 0040581e
0040581e  mov        eax, dword ptr [edi + 0x35d0]
00405824  test       al, 1
00405826  jne        0x405857

// block 00405828
00405828  fldz       
0040582a  or         eax, 1
0040582d  fstp       dword ptr [edi + 0x35c8]
00405833  mov        dword ptr [edi + 0x35c4], 0
0040583d  mov        dword ptr [edi + 0x35c0], 0xfff0bdc1
00405847  mov        dword ptr [edi + 0x35cc], 0x4b2ed0
00405851  mov        dword ptr [edi + 0x35d0], eax

// block 00405857
00405857  fld        dword ptr [0x4a3e14]
0040585d  mov        dword ptr [edi + 0x35c4], 0x1e
00405867  fstp       dword ptr [edi + 0x35c8]
0040586d  mov        dword ptr [edi + 0x35c0], 0x1d
00405877  or         dword ptr [edi + 0x35bc], 2
0040587e  pop        edi
0040587f  pop        esi
00405880  ret        
