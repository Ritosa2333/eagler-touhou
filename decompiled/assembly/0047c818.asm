
// block 0047c818
0047c818  mov        edi, edi
0047c81a  push       ebp
0047c81b  mov        ebp, esp
0047c81d  push       0x107
0047c822  push       dword ptr [ebp + 8]
0047c825  call       0x48d5ae

// block 0047c82a
0047c82a  pop        ecx
0047c82b  pop        ecx
0047c82c  test       eax, eax
0047c82e  jne        0x47c839

// block 0047c830
0047c830  cmp        word ptr [ebp + 8], 0x5f
0047c835  je         0x47c839

// block 0047c837
0047c837  pop        ebp
0047c838  ret        

// block 0047c839
0047c839  xor        eax, eax
0047c83b  inc        eax
0047c83c  pop        ebp
0047c83d  ret        
