
// block 0047c1ab
0047c1ab  mov        edi, edi
0047c1ad  push       ebp
0047c1ae  mov        ebp, esp
0047c1b0  mov        ecx, dword ptr [ebp + 8]
0047c1b3  cmp        ecx, 0x14
0047c1b6  mov        eax, dword ptr [ebp + 0xc]
0047c1b9  jge        0x47c1ce

// block 0047c1bb
0047c1bb  and        dword ptr [eax + 0xc], 0xffff7fff
0047c1c2  add        ecx, 0x10
0047c1c5  push       ecx
0047c1c6  call       0x46eba8

// block 0047c1cb
0047c1cb  pop        ecx
0047c1cc  pop        ebp
0047c1cd  ret        

// block 0047c1ce
0047c1ce  add        eax, 0x20
0047c1d1  push       eax
0047c1d2  call       dword ptr [0x49808c]

// block 0047c1d8
0047c1d8  pop        ebp
0047c1d9  ret        
