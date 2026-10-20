
// block 004339c0
004339c0  fldz       
004339c2  mov        dword ptr [esi + 4], 0x13
004339c9  mov        eax, dword ptr [esi + 0x20]
004339cc  xor        ecx, ecx
004339ce  test       al, 1
004339d0  jne        0x4339ec

// block 004339d2
004339d2  or         eax, 1
004339d5  fst        dword ptr [esi + 0x18]
004339d8  mov        dword ptr [esi + 0x14], ecx
004339db  mov        dword ptr [esi + 0x10], 0xfff0bdc1
004339e2  mov        dword ptr [esi + 0x1c], 0x4b2ed0
004339e9  mov        dword ptr [esi + 0x20], eax

// block 004339ec
004339ec  fstp       dword ptr [esi + 0x18]
004339ef  push       ebx
004339f0  mov        dword ptr [esi + 0x14], ecx
004339f3  mov        dword ptr [esi + 0x10], 0xffffffff
004339fa  mov        eax, dword ptr [esi + 0x1ec]
00433a00  push       eax
00433a01  mov        ebx, 1
00433a06  call       0x461970

// block 00433a0b
00433a0b  mov        ecx, dword ptr [esi + 0x1e8]
00433a11  push       ecx
00433a12  call       0x461970

// block 00433a17
00433a17  fld        dword ptr [esi + 0x2d8]
00433a1d  fstp       dword ptr [0x4b2ed0]
00433a23  pop        ebx
00433a24  ret        
