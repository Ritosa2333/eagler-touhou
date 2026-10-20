
// block 0047c20c
0047c20c  mov        edi, edi
0047c20e  push       esi
0047c20f  push       edi
0047c210  xor        edi, edi

// block 0047c212
0047c212  lea        esi, [edi + 0x4ade70]
0047c218  push       dword ptr [esi]
0047c21a  call       0x475163

// block 0047c21f
0047c21f  add        edi, 4
0047c222  pop        ecx
0047c223  mov        dword ptr [esi], eax
0047c225  cmp        edi, 0x28
0047c228  jb         0x47c212

// block 0047c22a
0047c22a  pop        edi
0047c22b  pop        esi
0047c22c  ret        
