
// block 00406ce0
00406ce0  push       ebx
00406ce1  push       esi
00406ce2  mov        esi, eax
00406ce4  mov        eax, dword ptr [esi + 0x3c]
00406ce7  xor        ebx, ebx
00406ce9  cmp        eax, ebx
00406ceb  je         0x406da6

// block 00406cf1
00406cf1  sub        eax, 1
00406cf4  push       edi
00406cf5  jne        0x406d9d

// block 00406cfb
00406cfb  mov        eax, dword ptr [0x4b0c94]
00406d00  mov        ecx, dword ptr [0x4b0c90]
00406d06  lea        eax, [eax + ecx*2]
00406d09  cmp        eax, 5
00406d0c  ja         0x406d9d

// block 00406d12
00406d12  jmp        dword ptr [eax*4 + 0x406db0]

// block 00406d19
00406d19  mov        edi, esi
00406d1b  call       0x46a970

// block 00406d20
00406d20  test       eax, eax
00406d22  je         0x406d9d

// block 00406d24
00406d24  pop        edi
00406d25  mov        dword ptr [esi + 0x3c], ebx
00406d28  pop        esi
00406d29  mov        eax, 1
00406d2e  pop        ebx
00406d2f  ret        

// block 00406d30
00406d30  push       esi
00406d31  call       0x4082e0

// block 00406d36
00406d36  test       eax, eax
00406d38  je         0x406d9d

// block 00406d3a
00406d3a  pop        edi
00406d3b  mov        dword ptr [esi + 0x3c], ebx
00406d3e  pop        esi
00406d3f  mov        eax, 1
00406d44  pop        ebx
00406d45  ret        

// block 00406d46
00406d46  push       esi
00406d47  call       0x4072d0

// block 00406d4c
00406d4c  test       eax, eax
00406d4e  je         0x406d9d

// block 00406d50
00406d50  pop        edi
00406d51  mov        dword ptr [esi + 0x3c], ebx
00406d54  pop        esi
00406d55  mov        eax, 1
00406d5a  pop        ebx
00406d5b  ret        

// block 00406d5c
00406d5c  call       0x407950

// block 00406d61
00406d61  test       eax, eax
00406d63  je         0x406d9d

// block 00406d65
00406d65  pop        edi
00406d66  mov        dword ptr [esi + 0x3c], ebx
00406d69  pop        esi
00406d6a  mov        eax, 1
00406d6f  pop        ebx
00406d70  ret        

// block 00406d71
00406d71  call       0x408b90

// block 00406d76
00406d76  test       eax, eax
00406d78  je         0x406d9d

// block 00406d7a
00406d7a  pop        edi
00406d7b  mov        dword ptr [esi + 0x3c], ebx
00406d7e  pop        esi
00406d7f  mov        eax, 1
00406d84  pop        ebx
00406d85  ret        

// block 00406d86
00406d86  mov        edi, esi
00406d88  call       0x408f00

// block 00406d8d
00406d8d  test       eax, eax
00406d8f  je         0x406d9d

// block 00406d91
00406d91  pop        edi
00406d92  mov        dword ptr [esi + 0x3c], ebx
00406d95  pop        esi
00406d96  mov        eax, 1
00406d9b  pop        ebx
00406d9c  ret        

// block 00406d9d
00406d9d  add        esi, 0x14
00406da0  call       0x464a80

// block 00406da5
00406da5  pop        edi

// block 00406da6
00406da6  pop        esi
00406da7  mov        eax, 1
00406dac  pop        ebx
00406dad  ret        
