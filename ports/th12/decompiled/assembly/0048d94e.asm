
// block 0048d94e
0048d94e  mov        edi, edi
0048d950  push       ebp
0048d951  mov        ebp, esp
0048d953  push       esi
0048d954  xor        esi, esi
0048d956  cmp        dword ptr [0x4b40dc], esi
0048d95c  jne        0x48d997

// block 0048d95e
0048d95e  cmp        dword ptr [ebp + 8], esi
0048d961  jne        0x48d982

// block 0048d963
0048d963  call       0x46e96f

// block 0048d968
0048d968  push       esi
0048d969  push       esi
0048d96a  push       esi
0048d96b  push       esi
0048d96c  push       esi
0048d96d  mov        dword ptr [eax], 0x16
0048d973  call       0x470f2d

// block 0048d978
0048d978  add        esp, 0x14
0048d97b  mov        eax, 0x7fffffff
0048d980  jmp        0x48d9a9

// block 0048d982
0048d982  cmp        dword ptr [ebp + 0xc], esi
0048d985  je         0x48d963

// block 0048d987
0048d987  cmp        dword ptr [ebp + 0x10], 0x7fffffff
0048d98e  ja         0x48d963

// block 0048d990
0048d990  pop        esi
0048d991  pop        ebp
0048d992  jmp        0x48edb0

// block 0048d997
0048d997  push       esi
0048d998  push       dword ptr [ebp + 0x10]
0048d99b  push       dword ptr [ebp + 0xc]
0048d99e  push       dword ptr [ebp + 8]
0048d9a1  call       0x48d85c

// block 0048d9a6
0048d9a6  add        esp, 0x10

// block 0048d9a9
0048d9a9  pop        esi
0048d9aa  pop        ebp
0048d9ab  ret        
