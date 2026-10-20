
// block 00460f27
00460f27  push       edx
00460f28  push       eax
00460f29  mov        eax, dword ptr [ecx + 0xbc]
00460f2f  call       eax

// block 00460f31
00460f31  mov        eax, 0x1e
00460f36  mov        edi, esi
00460f38  mov        dword ptr [0x4cee38], 1
00460f42  call       0x4611d0

// block 00460f47
00460f47  pop        edi
00460f48  pop        esi
00460f49  ret        
