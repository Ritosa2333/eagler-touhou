
// block 004055d0
004055d0  push       ecx
004055d1  mov        ecx, dword ptr [0x4b43c0]
004055d7  mov        eax, dword ptr [ecx + 0x1c]
004055da  cmp        dword ptr [eax], 0
004055dd  jl         0x40563f

// block 004055df
004055df  push       edi

// block 004055e0
004055e0  cmp        word ptr [eax + 4], 0x10
004055e5  jne        0x4055ec

// block 004055e7
004055e7  cmp        dword ptr [eax + 8], edx
004055ea  je         0x4055fa

// block 004055ec
004055ec  movsx      edi, word ptr [eax + 6]
004055f0  add        eax, edi
004055f2  cmp        dword ptr [eax], 0
004055f5  jge        0x4055e0

// block 004055f7
004055f7  pop        edi
004055f8  pop        ecx
004055f9  ret        

// block 004055fa
004055fa  movsx      edx, word ptr [eax + 6]
004055fe  add        eax, edx
00405600  mov        dword ptr [ecx + 0x4c], eax
00405603  mov        edx, dword ptr [eax]
00405605  mov        eax, dword ptr [ecx + 0x48]
00405608  mov        dword ptr [esp + 4], edx
0040560c  test       al, 1
0040560e  jne        0x405630

// block 00405610
00405610  fldz       
00405612  or         eax, 1
00405615  fstp       dword ptr [ecx + 0x40]
00405618  mov        dword ptr [ecx + 0x3c], 0
0040561f  mov        dword ptr [ecx + 0x38], 0xfff0bdc1
00405626  mov        dword ptr [ecx + 0x44], 0x4b2ed0
0040562d  mov        dword ptr [ecx + 0x48], eax

// block 00405630
00405630  fild       dword ptr [esp + 4]
00405634  mov        dword ptr [ecx + 0x3c], edx
00405637  dec        edx
00405638  mov        dword ptr [ecx + 0x38], edx
0040563b  fstp       dword ptr [ecx + 0x40]
0040563e  pop        edi

// block 0040563f
0040563f  pop        ecx
00405640  ret        
