
// block 004236f0
004236f0  push       ebx
004236f1  push       esi
004236f2  push       edi
004236f3  mov        edi, eax
004236f5  add        edi, 0x7c
004236f8  mov        ebx, 8
004236fd  lea        ecx, [ecx]

// block 00423700
00423700  mov        eax, dword ptr [edi - 0x60]
00423703  test       eax, eax
00423705  je         0x42371b

// block 00423707
00423707  mov        esi, dword ptr [eax + 4]
0042370a  mov        eax, dword ptr [eax]
0042370c  push       eax
0042370d  call       0x46ca4f

// block 00423712
00423712  add        esp, 4
00423715  mov        eax, esi
00423717  test       esi, esi
00423719  jne        0x423707

// block 0042371b
0042371b  mov        eax, dword ptr [edi]
0042371d  test       eax, eax
0042371f  je         0x423735

// block 00423721
00423721  mov        ecx, dword ptr [eax]
00423723  mov        esi, dword ptr [eax + 4]
00423726  push       ecx
00423727  call       0x46ca4f

// block 0042372c
0042372c  add        esp, 4
0042372f  mov        eax, esi
00423731  test       esi, esi
00423733  jne        0x423721

// block 00423735
00423735  add        edi, 0xc
00423738  sub        ebx, 1
0042373b  jne        0x423700

// block 0042373d
0042373d  pop        edi
0042373e  pop        esi
0042373f  pop        ebx
00423740  ret        
