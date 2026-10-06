
// block 0044f4b0
0044f4b0  push       ebx
0044f4b1  mov        ebx, dword ptr [0x4ce8cc]
0044f4b7  push       esi
0044f4b8  push       edi
0044f4b9  mov        esi, ebx
0044f4bb  mov        edi, 4

// block 0044f4c0
0044f4c0  cmp        dword ptr [esi], 0
0044f4c3  jl         0x44f4d4

// block 0044f4c5
0044f4c5  mov        eax, esi
0044f4c7  mov        ecx, ebx
0044f4c9  call       0x4609d0

// block 0044f4ce
0044f4ce  mov        dword ptr [esi], 0xffffffff

// block 0044f4d4
0044f4d4  add        esi, 0x28
0044f4d7  sub        edi, 1
0044f4da  jne        0x44f4c0

// block 0044f4dc
0044f4dc  pop        edi
0044f4dd  pop        esi
0044f4de  pop        ebx
0044f4df  ret        
