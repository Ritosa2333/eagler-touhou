
// block 0047c16f
0047c16f  mov        edi, edi
0047c171  push       ebp
0047c172  mov        ebp, esp
0047c174  mov        eax, dword ptr [ebp + 8]
0047c177  mov        ecx, 0x4adbf0
0047c17c  cmp        eax, ecx
0047c17e  jb         0x47c19f

// block 0047c180
0047c180  cmp        eax, 0x4ade50
0047c185  ja         0x47c19f

// block 0047c187
0047c187  and        dword ptr [eax + 0xc], 0xffff7fff
0047c18e  sub        eax, ecx
0047c190  sar        eax, 5
0047c193  add        eax, 0x10
0047c196  push       eax
0047c197  call       0x46eba8

// block 0047c19c
0047c19c  pop        ecx
0047c19d  pop        ebp
0047c19e  ret        

// block 0047c19f
0047c19f  add        eax, 0x20
0047c1a2  push       eax
0047c1a3  call       dword ptr [0x49808c]

// block 0047c1a9
0047c1a9  pop        ebp
0047c1aa  ret        
