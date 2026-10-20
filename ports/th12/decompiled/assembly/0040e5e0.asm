
// block 0040e5e0
0040e5e0  push       ebx
0040e5e1  push       esi
0040e5e2  push       edi
0040e5e3  mov        edi, dword ptr [0x4b43cc]
0040e5e9  mov        ebx, 1
0040e5ee  test       byte ptr [edi + 0x7c], bl
0040e5f1  je         0x40e71d

// block 0040e5f7
0040e5f7  mov        eax, dword ptr [0x4b43c0]
0040e5fc  or         dword ptr [eax + 0x35bc], ebx
0040e602  mov        eax, dword ptr [edi + 0x14]
0040e605  push       eax
0040e606  call       0x461970

// block 0040e60b
0040e60b  mov        ecx, dword ptr [edi + 0x18]
0040e60e  push       ecx
0040e60f  call       0x461970

// block 0040e614
0040e614  mov        edx, dword ptr [edi + 0x1c]
0040e617  push       edx
0040e618  call       0x461970

// block 0040e61d
0040e61d  and        dword ptr [edi + 0x7c], 0xfffffffe
0040e621  mov        eax, dword ptr [edi + 0x10]
0040e624  push       eax
0040e625  call       0x461a70

// block 0040e62a
0040e62a  xor        esi, esi
0040e62c  mov        dword ptr [edi + 0x10], esi
0040e62f  and        dword ptr [edi + 0x7c], 0xffffffdf
0040e633  mov        ecx, dword ptr [edi + 0x20]
0040e636  push       ecx
0040e637  call       0x461a70

// block 0040e63c
0040e63c  mov        dword ptr [edi + 0x20], esi
0040e63f  test       byte ptr [edi + 0x7c], 2
0040e643  je         0x40e70f

// block 0040e649
0040e649  mov        ecx, dword ptr [edi + 0x80]
0040e64f  mov        eax, 0x66666667
0040e654  imul       ecx
0040e656  mov        eax, dword ptr [0x4b0c44]
0040e65b  sar        edx, 2
0040e65e  mov        ecx, edx
0040e660  shr        ecx, 0x1f
0040e663  add        ecx, edx
0040e665  add        eax, ecx
0040e667  cmp        eax, 0x3b9aca00
0040e66c  mov        dword ptr [0x4b0c44], eax
0040e671  jl         0x40e67d

// block 0040e673
0040e673  mov        dword ptr [0x4b0c44], 0x3b9ac9ff

// block 0040e67d
0040e67d  mov        edx, dword ptr [edi + 0x80]
0040e683  mov        esi, dword ptr [0x4b43e4]
0040e689  push       edx
0040e68a  xor        eax, eax
0040e68c  call       0x420f90

// block 0040e691
0040e691  mov        eax, dword ptr [0x4b4518]
0040e696  cmp        dword ptr [eax + 0x10], ebx
0040e699  je         0x40e702

// block 0040e69b
0040e69b  mov        edx, dword ptr [0x4b0c90]
0040e6a1  mov        ecx, dword ptr [0x4b0c94]
0040e6a7  mov        eax, dword ptr [edi + 0x78]
0040e6aa  lea        ecx, [ecx + edx*2]
0040e6ad  mov        edx, dword ptr [0x4b451c]
0040e6b3  imul       ecx, ecx, 0x45f4
0040e6b9  lea        eax, [eax + eax*8]
0040e6bc  add        ecx, edx
0040e6be  shl        eax, 4
0040e6c1  lea        eax, [eax + ecx + 0x66c]
0040e6c8  mov        ecx, dword ptr [eax + 0x80]
0040e6ce  cmp        ecx, 0x1869f
0040e6d4  jge        0x40e6dd

// block 0040e6d6
0040e6d6  inc        ecx
0040e6d7  mov        dword ptr [eax + 0x80], ecx

// block 0040e6dd
0040e6dd  mov        edi, dword ptr [edi + 0x78]
0040e6e0  lea        ecx, [edi + edi*8]
0040e6e3  shl        ecx, 4
0040e6e6  lea        eax, [ecx + edx + 0x1aa24]
0040e6ed  mov        ecx, dword ptr [eax + 0x80]
0040e6f3  cmp        ecx, 0x1869f
0040e6f9  jge        0x40e702

// block 0040e6fb
0040e6fb  inc        ecx
0040e6fc  mov        dword ptr [eax + 0x80], ecx

// block 0040e702
0040e702  mov        edx, 0x30
0040e707  pop        edi
0040e708  pop        esi
0040e709  pop        ebx
0040e70a  jmp        0x453d90

// block 0040e70f
0040e70f  push       esi
0040e710  mov        esi, dword ptr [0x4b43e4]
0040e716  mov        eax, ebx
0040e718  call       0x420f90

// block 0040e71d
0040e71d  pop        edi
0040e71e  pop        esi
0040e71f  pop        ebx
0040e720  ret        
