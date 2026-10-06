
// block 004831b2
004831b2  mov        edi, edi
004831b4  push       ebp
004831b5  mov        ebp, esp
004831b7  mov        ecx, dword ptr [ebp + 8]
004831ba  push       esi
004831bb  xor        esi, esi
004831bd  cmp        ecx, esi
004831bf  jl         0x4831df

// block 004831c1
004831c1  cmp        ecx, 2
004831c4  jle        0x4831d2

// block 004831c6
004831c6  cmp        ecx, 3
004831c9  jne        0x4831df

// block 004831cb
004831cb  mov        eax, dword ptr [0x4b38d8]
004831d0  jmp        0x4831fa

// block 004831d2
004831d2  mov        eax, dword ptr [0x4b38d8]
004831d7  mov        dword ptr [0x4b38d8], ecx
004831dd  jmp        0x4831fa

// block 004831df
004831df  call       0x46e96f

// block 004831e4
004831e4  push       esi
004831e5  push       esi
004831e6  push       esi
004831e7  push       esi
004831e8  push       esi
004831e9  mov        dword ptr [eax], 0x16
004831ef  call       0x470f2d

// block 004831f4
004831f4  add        esp, 0x14
004831f7  or         eax, 0xffffffff

// block 004831fa
004831fa  pop        esi
004831fb  pop        ebp
004831fc  ret        
