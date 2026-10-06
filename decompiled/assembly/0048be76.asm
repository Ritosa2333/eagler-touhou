
// block 0048be76
0048be76  mov        edi, edi
0048be78  push       ebp
0048be79  mov        ebp, esp
0048be7b  push       dword ptr [ebp + 8]
0048be7e  call       0x48b8f2

// block 0048be83
0048be83  pop        ecx
0048be84  test       eax, eax
0048be86  jne        0x48be90

// block 0048be88
0048be88  cmp        dword ptr [ebp + 8], 0x5f
0048be8c  je         0x48be90

// block 0048be8e
0048be8e  pop        ebp
0048be8f  ret        

// block 0048be90
0048be90  xor        eax, eax
0048be92  inc        eax
0048be93  pop        ebp
0048be94  ret        
