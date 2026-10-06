
// block 0047c925
0047c925  test       byte ptr [ecx + 0xc], 0x40
0047c929  je         0x47c931

// block 0047c92b
0047c92b  cmp        dword ptr [ecx + 8], 0
0047c92f  je         0x47c955

// block 0047c931
0047c931  dec        dword ptr [ecx + 4]
0047c934  js         0x47c941

// block 0047c936
0047c936  mov        edx, dword ptr [ecx]
0047c938  mov        byte ptr [edx], al
0047c93a  inc        dword ptr [ecx]
0047c93c  movzx      eax, al
0047c93f  jmp        0x47c94d

// block 0047c941
0047c941  movsx      eax, al
0047c944  push       ecx
0047c945  push       eax
0047c946  call       0x46ffdb

// block 0047c94b
0047c94b  pop        ecx
0047c94c  pop        ecx

// block 0047c94d
0047c94d  cmp        eax, -1
0047c950  jne        0x47c955

// block 0047c952
0047c952  or         dword ptr [esi], eax
0047c954  ret        

// block 0047c955
0047c955  inc        dword ptr [esi]
0047c957  ret        
