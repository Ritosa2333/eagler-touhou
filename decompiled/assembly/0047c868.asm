
// block 0047c868
0047c868  mov        edi, edi
0047c86a  push       ebp
0047c86b  mov        ebp, esp
0047c86d  push       0x103
0047c872  push       dword ptr [ebp + 8]
0047c875  call       0x48d5ae

// block 0047c87a
0047c87a  pop        ecx
0047c87b  pop        ecx
0047c87c  test       eax, eax
0047c87e  jne        0x47c889

// block 0047c880
0047c880  cmp        word ptr [ebp + 8], 0x5f
0047c885  je         0x47c889

// block 0047c887
0047c887  pop        ebp
0047c888  ret        

// block 0047c889
0047c889  xor        eax, eax
0047c88b  inc        eax
0047c88c  pop        ebp
0047c88d  ret        
