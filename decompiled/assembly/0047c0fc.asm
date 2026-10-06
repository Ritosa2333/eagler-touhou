
// block 0047c0fc
0047c0fc  mov        edi, edi
0047c0fe  push       ebp
0047c0ff  mov        ebp, esp
0047c101  push       esi
0047c102  mov        esi, dword ptr [ebp + 8]
0047c105  mov        eax, 0x4adbf0
0047c10a  cmp        esi, eax
0047c10c  jb         0x47c130

// block 0047c10e
0047c10e  cmp        esi, 0x4ade50
0047c114  ja         0x47c130

// block 0047c116
0047c116  mov        ecx, esi
0047c118  sub        ecx, eax
0047c11a  sar        ecx, 5
0047c11d  add        ecx, 0x10
0047c120  push       ecx
0047c121  call       0x46ec9a

// block 0047c126
0047c126  or         dword ptr [esi + 0xc], 0x8000
0047c12d  pop        ecx
0047c12e  jmp        0x47c13a

// block 0047c130
0047c130  add        esi, 0x20
0047c133  push       esi
0047c134  call       dword ptr [0x498088]

// block 0047c13a
0047c13a  pop        esi
0047c13b  pop        ebp
0047c13c  ret        
