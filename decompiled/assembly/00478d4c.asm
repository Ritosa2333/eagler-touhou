
// block 00478d4c
00478d4c  mov        edi, edi
00478d4e  push       ebp
00478d4f  mov        ebp, esp
00478d51  mov        eax, dword ptr [esi]
00478d53  cmp        dword ptr [ebp + 8], eax
00478d56  jne        0x478d9e

// block 00478d58
00478d58  mov        ecx, dword ptr [edi]
00478d5a  push       2
00478d5c  push       eax
00478d5d  cmp        ecx, dword ptr [ebp + 0xc]
00478d60  jne        0x478d8d

// block 00478d62
00478d62  call       0x477813

// block 00478d67
00478d67  pop        ecx
00478d68  pop        ecx
00478d69  mov        dword ptr [edi], eax
00478d6b  test       eax, eax
00478d6d  jne        0x478d73

// block 00478d6f
00478d6f  xor        eax, eax
00478d71  pop        ebp
00478d72  ret        

// block 00478d73
00478d73  mov        eax, dword ptr [ebp + 0x10]
00478d76  mov        dword ptr [eax], 1
00478d7c  push       dword ptr [esi]
00478d7e  push       dword ptr [ebp + 0xc]
00478d81  push       dword ptr [edi]
00478d83  call       0x47a330

// block 00478d88
00478d88  add        esp, 0xc
00478d8b  jmp        0x478d9c

// block 00478d8d
00478d8d  push       ecx
00478d8e  call       0x4778ad

// block 00478d93
00478d93  add        esp, 0xc
00478d96  test       eax, eax
00478d98  je         0x478d6f

// block 00478d9a
00478d9a  mov        dword ptr [edi], eax

// block 00478d9c
00478d9c  shl        dword ptr [esi], 1

// block 00478d9e
00478d9e  xor        eax, eax
00478da0  inc        eax
00478da1  pop        ebp
00478da2  ret        
