
// block 00460cb7
00460cb7  push       edx
00460cb8  push       eax
00460cb9  mov        eax, dword ptr [ecx + 0xbc]
00460cbf  call       eax

// block 00460cc1
00460cc1  cmp        dword ptr [0x4cf278], 0
00460cc8  mov        dword ptr [0x4cee38], 2
00460cd2  je         0x460cff

// block 00460cd4
00460cd4  push       esi
00460cd5  mov        esi, dword ptr [0x4ce8cc]
00460cdb  call       0x45a3c0

// block 00460ce0
00460ce0  mov        eax, dword ptr [0x4ce8f0]
00460ce5  push       0
00460ce7  mov        dword ptr [0x4cf278], 0
00460cf1  mov        ecx, dword ptr [eax]
00460cf3  mov        edx, dword ptr [ecx + 0xe4]
00460cf9  push       0x1c
00460cfb  push       eax
00460cfc  call       edx

// block 00460cfe
00460cfe  pop        esi

// block 00460cff
00460cff  mov        eax, 4
00460d04  mov        edi, ebx
00460d06  call       0x4611d0

// block 00460d0b
00460d0b  pop        edi
00460d0c  pop        ebx
00460d0d  ret        
