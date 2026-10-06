
// block 0047e029
0047e029  mov        edi, edi
0047e02b  push       ebp
0047e02c  mov        ebp, esp
0047e02e  cmp        dword ptr [ebp + 8], 0
0047e032  jne        0x47e044

// block 0047e034
0047e034  xor        eax, eax
0047e036  pop        ebp
0047e037  ret        

// block 0047e038
0047e038  mov        al, byte ptr [ecx]
0047e03a  test       al, al
0047e03c  je         0x47e049

// block 0047e03e
0047e03e  cmp        al, byte ptr [edx]
0047e040  jne        0x47e049

// block 0047e042
0047e042  inc        ecx
0047e043  inc        edx

// block 0047e044
0047e044  dec        dword ptr [ebp + 8]
0047e047  jne        0x47e038

// block 0047e049
0047e049  movzx      eax, byte ptr [ecx]
0047e04c  movzx      ecx, byte ptr [edx]
0047e04f  sub        eax, ecx
0047e051  pop        ebp
0047e052  ret        
