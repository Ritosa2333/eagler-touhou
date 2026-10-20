
// block 0048e343
0048e343  mov        edi, edi
0048e345  push       ebp
0048e346  mov        ebp, esp
0048e348  push       ecx
0048e349  push       ecx
0048e34a  push       ebx
0048e34b  push       esi
0048e34c  xor        esi, esi
0048e34e  push       edi
0048e34f  mov        edi, dword ptr [0x4b3d88]
0048e355  mov        dword ptr [ebp - 4], esi
0048e358  mov        eax, dword ptr [edi]
0048e35a  cmp        eax, esi
0048e35c  je         0x48e3c3

// block 0048e35e
0048e35e  mov        ebx, dword ptr [0x498134]

// block 0048e364
0048e364  push       esi
0048e365  push       esi
0048e366  push       esi
0048e367  push       esi
0048e368  push       -1
0048e36a  push       eax
0048e36b  push       esi
0048e36c  push       esi
0048e36d  call       ebx

// block 0048e36f
0048e36f  mov        dword ptr [ebp - 8], eax
0048e372  cmp        eax, esi
0048e374  je         0x48e3ca

// block 0048e376
0048e376  push       1
0048e378  push       eax
0048e379  call       0x477813

// block 0048e37e
0048e37e  pop        ecx
0048e37f  pop        ecx
0048e380  mov        dword ptr [ebp - 4], eax
0048e383  cmp        eax, esi
0048e385  je         0x48e3ca

// block 0048e387
0048e387  push       esi
0048e388  push       esi
0048e389  push       dword ptr [ebp - 8]
0048e38c  push       eax
0048e38d  push       -1
0048e38f  push       dword ptr [edi]
0048e391  push       esi
0048e392  push       esi
0048e393  call       ebx

// block 0048e395
0048e395  test       eax, eax
0048e397  je         0x48e3cf

// block 0048e399
0048e399  lea        eax, [ebp - 4]
0048e39c  push       esi
0048e39d  push       eax
0048e39e  call       0x490d7b

// block 0048e3a3
0048e3a3  pop        ecx
0048e3a4  pop        ecx
0048e3a5  test       eax, eax
0048e3a7  jge        0x48e3ba

// block 0048e3a9
0048e3a9  cmp        dword ptr [ebp - 4], esi
0048e3ac  je         0x48e3ba

// block 0048e3ae
0048e3ae  push       dword ptr [ebp - 4]
0048e3b1  call       0x46c941

// block 0048e3b6
0048e3b6  pop        ecx
0048e3b7  mov        dword ptr [ebp - 4], esi

// block 0048e3ba
0048e3ba  add        edi, 4
0048e3bd  mov        eax, dword ptr [edi]
0048e3bf  cmp        eax, esi
0048e3c1  jne        0x48e364

// block 0048e3c3
0048e3c3  xor        eax, eax

// block 0048e3c5
0048e3c5  pop        edi
0048e3c6  pop        esi
0048e3c7  pop        ebx
0048e3c8  leave      
0048e3c9  ret        

// block 0048e3ca
0048e3ca  or         eax, 0xffffffff
0048e3cd  jmp        0x48e3c5

// block 0048e3cf
0048e3cf  push       dword ptr [ebp - 4]
0048e3d2  call       0x46c941

// block 0048e3d7
0048e3d7  pop        ecx
0048e3d8  jmp        0x48e3ca
