
// block 00491d80
00491d80  mov        edi, edi
00491d82  push       ebp
00491d83  mov        ebp, esp
00491d85  call       0x475467

// block 00491d8a
00491d8a  mov        eax, dword ptr [eax + 0x98]
00491d90  jmp        0x491d9c

// block 00491d92
00491d92  mov        ecx, dword ptr [eax]
00491d94  cmp        ecx, dword ptr [ebp + 8]
00491d97  je         0x491da3

// block 00491d99
00491d99  mov        eax, dword ptr [eax + 4]

// block 00491d9c
00491d9c  test       eax, eax
00491d9e  jne        0x491d92

// block 00491da0
00491da0  inc        eax
00491da1  pop        ebp
00491da2  ret        

// block 00491da3
00491da3  xor        eax, eax
00491da5  pop        ebp
00491da6  ret        
