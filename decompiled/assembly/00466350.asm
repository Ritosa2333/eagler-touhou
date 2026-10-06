
// block 00466350
00466350  push       ecx
00466351  push       ebp
00466352  push       edi
00466353  mov        edi, eax
00466355  xor        ebp, ebp
00466357  cmp        dword ptr [edi + 4], ebp
0046635a  jne        0x466365

// block 0046635c
0046635c  pop        edi
0046635d  mov        eax, 0x800401f0
00466362  pop        ebp
00466363  pop        ecx
00466364  ret        

// block 00466365
00466365  push       esi
00466366  call       0x466280

// block 0046636b
0046636b  mov        esi, eax
0046636d  cmp        esi, ebp
0046636f  jne        0x46637b

// block 00466371
00466371  pop        esi
00466372  pop        edi
00466373  mov        eax, 0x80004005
00466378  pop        ebp
00466379  pop        ecx
0046637a  ret        

// block 0046637b
0046637b  push       ebx
0046637c  lea        ebx, [esp + 0x10]
00466380  call       0x466220

// block 00466385
00466385  cmp        eax, ebp
00466387  jl         0x4663cf

// block 00466389
00466389  cmp        dword ptr [esp + 0x10], ebp
0046638d  je         0x4663a1

// block 0046638f
0046638f  push       ebp
00466390  push       esi
00466391  mov        ebx, edi
00466393  call       0x465fe0

// block 00466398
00466398  cmp        eax, ebp
0046639a  jl         0x4663cf

// block 0046639c
0046639c  call       0x4665c0

// block 004663a1
004663a1  xor        ecx, ecx
004663a3  mov        eax, edi
004663a5  mov        dword ptr [edi + 0x1c], ebp
004663a8  mov        dword ptr [edi + 0x14], ebp
004663ab  mov        dword ptr [edi + 0x18], ebp
004663ae  call       0x4663e0

// block 004663b3
004663b3  mov        eax, 1
004663b8  push       eax
004663b9  push       ebp
004663ba  mov        dword ptr [edi + 0x30], eax
004663bd  mov        dword ptr [edi + 0x20], ebp
004663c0  mov        dword ptr [edi + 0x24], eax
004663c3  mov        dword ptr [edi + 0x2c], ebp
004663c6  mov        ecx, dword ptr [esi]
004663c8  mov        edx, dword ptr [ecx + 0x30]
004663cb  push       ebp
004663cc  push       esi
004663cd  call       edx

// block 004663cf
004663cf  pop        ebx
004663d0  pop        esi
004663d1  pop        edi
004663d2  pop        ebp
004663d3  pop        ecx
004663d4  ret        
