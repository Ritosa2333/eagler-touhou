
// block 00421350
00421350  sub        esp, 8
00421353  push       esi
00421354  mov        esi, dword ptr [0x4b43e4]
0042135a  mov        ecx, dword ptr [esi + 0x6d44]
00421360  push       0
00421362  push       0x55
00421364  lea        eax, [esp + 0x10]
00421368  push       eax
00421369  push       ecx
0042136a  mov        eax, 0x17
0042136f  xor        ecx, ecx
00421371  call       0x4615a0

// block 00421376
00421376  mov        edx, dword ptr [esp + 8]
0042137a  mov        dword ptr [esi + 0x6cac], edx
00421380  mov        ecx, dword ptr [0x4b0cb0]
00421386  imul       ecx, ecx, 0xf4240
0042138c  mov        eax, 0x66666667
00421391  imul       ecx
00421393  mov        dword ptr [esi + 0x6d48], ecx
00421399  mov        eax, dword ptr [0x4b0c44]
0042139e  sar        edx, 2
004213a1  mov        ecx, edx
004213a3  shr        ecx, 0x1f
004213a6  add        ecx, edx
004213a8  add        eax, ecx
004213aa  cmp        eax, 0x3b9aca00
004213af  mov        dword ptr [0x4b0c44], eax
004213b4  jl         0x4213c0

// block 004213b6
004213b6  mov        dword ptr [0x4b0c44], 0x3b9ac9ff

// block 004213c0
004213c0  or         dword ptr [esi + 0x6d18], 0x200
004213ca  fldz       
004213cc  mov        eax, dword ptr [esi + 0x6d2c]
004213d2  test       al, 1
004213d4  jne        0x421403

// block 004213d6
004213d6  or         eax, 1
004213d9  fst        dword ptr [esi + 0x6d24]
004213df  mov        dword ptr [esi + 0x6d20], 0
004213e9  mov        dword ptr [esi + 0x6d1c], 0xfff0bdc1
004213f3  mov        dword ptr [esi + 0x6d28], 0x4b2ed0
004213fd  mov        dword ptr [esi + 0x6d2c], eax

// block 00421403
00421403  fstp       dword ptr [esi + 0x6d24]
00421409  mov        dword ptr [esi + 0x6d20], 0
00421413  mov        dword ptr [esi + 0x6d1c], 0xffffffff
0042141d  pop        esi
0042141e  add        esp, 8
00421421  ret        
