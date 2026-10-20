
// block 00461d40
00461d40  mov        ecx, dword ptr [eax]
00461d42  mov        edx, dword ptr [0x4ce8cc]
00461d48  push       ecx
00461d49  call       0x461920

// block 00461d4e
00461d4e  test       eax, eax
00461d50  je         0x461d7f

// block 00461d52
00461d52  mov        edx, 2
00461d57  or         dword ptr [eax + 0x47c], edx
00461d5d  cmp        dword ptr [eax + 0x18], 0
00461d61  jne        0x461d7f

// block 00461d63
00461d63  mov        eax, dword ptr [eax + 0x14]
00461d66  test       eax, eax
00461d68  je         0x461d7f

// block 00461d6a
00461d6a  lea        ebx, [ebx]

// block 00461d70
00461d70  mov        ecx, dword ptr [eax]
00461d72  or         dword ptr [ecx + 0x47c], edx
00461d78  mov        eax, dword ptr [eax + 4]
00461d7b  test       eax, eax
00461d7d  jne        0x461d70

// block 00461d7f
00461d7f  ret        
