
// block 00452350
00452350  sub        esp, 0x10
00452353  fldz       
00452355  push       ebx
00452356  fst        dword ptr [esp + 4]
0045235a  push       esi
0045235b  mov        esi, dword ptr [0x4ce8cc]
00452361  fstp       dword ptr [esp + 0xc]
00452365  fld        dword ptr [0x4a3ec4]
0045236b  push       edi
0045236c  fstp       dword ptr [esp + 0x14]
00452370  mov        edi, ecx
00452372  fld        dword ptr [0x4a3ec0]
00452378  fstp       dword ptr [esp + 0x18]
0045237c  call       0x45a3c0

// block 00452381
00452381  xor        eax, eax
00452383  mov        dword ptr [0x4ce9c4], eax
00452388  mov        dword ptr [0x4ce9c8], eax
0045238d  mov        eax, dword ptr [0x4ce8f0]
00452392  mov        dword ptr [0x4ce9cc], 0x280
0045239c  mov        dword ptr [0x4ce9d0], 0x1e0
004523a6  mov        ecx, dword ptr [eax]
004523a8  mov        edx, dword ptr [ecx + 0xbc]
004523ae  push       0x4ce9c4
004523b3  push       eax
004523b4  call       edx

// block 004523b6
004523b6  mov        ebx, dword ptr [edi + 0x18]
004523b9  shl        ebx, 0x18
004523bc  or         ebx, dword ptr [edi + 0x20]
004523bf  lea        edi, [esp + 0xc]
004523c3  call       0x451f30

// block 004523c8
004523c8  pop        edi
004523c9  pop        esi
004523ca  mov        eax, 1
004523cf  pop        ebx
004523d0  add        esp, 0x10
004523d3  ret        
