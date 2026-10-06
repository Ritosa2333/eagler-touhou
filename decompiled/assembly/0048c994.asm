
// block 0048c994
0048c994  push       ebx
0048c995  push       ecx
0048c996  mov        ebx, 0x4ae304
0048c99b  jmp        0x48c9a8

// block 0048c9a8
0048c9a8  mov        dword ptr [ebx + 8], ecx
0048c9ab  mov        dword ptr [ebx + 4], eax
0048c9ae  mov        dword ptr [ebx + 0xc], ebp
0048c9b1  push       ebp
0048c9b2  push       ecx
0048c9b3  push       eax
0048c9b4  pop        eax
0048c9b5  pop        ecx
0048c9b6  pop        ebp
0048c9b7  pop        ecx
0048c9b8  pop        ebx
0048c9b9  ret        4
