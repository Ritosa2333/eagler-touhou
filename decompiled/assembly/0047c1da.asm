
// block 0047c1da
0047c1da  mov        edi, edi
0047c1dc  push       ebp
0047c1dd  mov        ebp, esp
0047c1df  mov        eax, dword ptr [ebp + 8]
0047c1e2  push       esi
0047c1e3  xor        esi, esi
0047c1e5  cmp        eax, esi
0047c1e7  jne        0x47c206

// block 0047c1e9
0047c1e9  call       0x46e96f

// block 0047c1ee
0047c1ee  push       esi
0047c1ef  push       esi
0047c1f0  push       esi
0047c1f1  push       esi
0047c1f2  push       esi
0047c1f3  mov        dword ptr [eax], 0x16
0047c1f9  call       0x470f2d

// block 0047c1fe
0047c1fe  add        esp, 0x14
0047c201  or         eax, 0xffffffff
0047c204  jmp        0x47c209

// block 0047c206
0047c206  mov        eax, dword ptr [eax + 0x10]

// block 0047c209
0047c209  pop        esi
0047c20a  pop        ebp
0047c20b  ret        
