
// block 0047c13d
0047c13d  mov        edi, edi
0047c13f  push       ebp
0047c140  mov        ebp, esp
0047c142  mov        eax, dword ptr [ebp + 8]
0047c145  cmp        eax, 0x14
0047c148  jge        0x47c160

// block 0047c14a
0047c14a  add        eax, 0x10
0047c14d  push       eax
0047c14e  call       0x46ec9a

// block 0047c153
0047c153  mov        eax, dword ptr [ebp + 0xc]
0047c156  or         dword ptr [eax + 0xc], 0x8000
0047c15d  pop        ecx
0047c15e  pop        ebp
0047c15f  ret        

// block 0047c160
0047c160  mov        eax, dword ptr [ebp + 0xc]
0047c163  add        eax, 0x20
0047c166  push       eax
0047c167  call       dword ptr [0x498088]

// block 0047c16d
0047c16d  pop        ebp
0047c16e  ret        
