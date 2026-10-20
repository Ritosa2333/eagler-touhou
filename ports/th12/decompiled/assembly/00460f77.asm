
// block 00460f77
00460f77  push       edx
00460f78  push       eax
00460f79  mov        eax, dword ptr [ecx + 0xbc]
00460f7f  call       eax

// block 00460f81
00460f81  mov        eax, 0x1f
00460f86  mov        edi, esi
00460f88  mov        dword ptr [0x4cee38], 0
00460f92  call       0x4611d0

// block 00460f97
00460f97  mov        edi, 0x4cec04
00460f9c  mov        esi, eax
00460f9e  mov        dword ptr [0x4cee34], edi
00460fa4  call       0x430910

// block 00460fa9
00460fa9  mov        edx, dword ptr [0x4cee34]
00460faf  mov        eax, dword ptr [0x4ce8f0]
00460fb4  mov        ecx, dword ptr [eax]
00460fb6  add        edx, 0xcc
00460fbc  push       edx
00460fbd  push       eax
00460fbe  mov        eax, dword ptr [ecx + 0xbc]
00460fc4  call       eax

// block 00460fc6
00460fc6  pop        edi
00460fc7  mov        eax, esi
00460fc9  mov        dword ptr [0x4cee38], 1
00460fd3  pop        esi
00460fd4  ret        
