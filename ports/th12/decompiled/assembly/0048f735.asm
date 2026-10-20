
// block 0048f735
0048f735  mov        edi, edi
0048f737  push       ebp
0048f738  mov        ebp, esp
0048f73a  push       ecx
0048f73b  push       ecx
0048f73c  mov        eax, dword ptr [ebp + 0xc]
0048f73f  push       esi
0048f740  xor        esi, esi
0048f742  cmp        dword ptr [ebp + 8], 0x9001f
0048f749  jne        0x48f784

// block 0048f74b
0048f74b  cmp        eax, -1
0048f74e  jne        0x48f784

// block 0048f750
0048f750  wait       
0048f751  fnstcw     word ptr [ebp - 4]
0048f754  mov        ecx, dword ptr [ebp - 4]
0048f757  and        ecx, 0x1f3d
0048f75d  mov        edx, 0x23d
0048f762  cmp        cx, dx
0048f765  jne        0x48f784

// block 0048f767
0048f767  cmp        dword ptr [0x4d52dc], esi
0048f76d  je         0x48f7a7

// block 0048f76f
0048f76f  stmxcsr    dword ptr [ebp - 8]
0048f773  mov        ecx, dword ptr [ebp - 8]
0048f776  and        ecx, 0xfec0
0048f77c  cmp        ecx, 0x1e80
0048f782  je         0x48f7a7

// block 0048f784
0048f784  and        eax, 0xfff7ffff
0048f789  push       eax
0048f78a  push       dword ptr [ebp + 8]
0048f78d  push       esi
0048f78e  call       0x48e1cf

// block 0048f793
0048f793  add        esp, 0xc
0048f796  test       eax, eax
0048f798  je         0x48f7a7

// block 0048f79a
0048f79a  push       esi
0048f79b  push       esi
0048f79c  push       esi
0048f79d  push       esi
0048f79e  push       esi
0048f79f  call       0x470dc6

// block 0048f7a4
0048f7a4  add        esp, 0x14

// block 0048f7a7
0048f7a7  pop        esi
0048f7a8  leave      
0048f7a9  ret        
