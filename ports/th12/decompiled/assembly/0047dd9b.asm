
// block 0047dd9b
0047dd9b  mov        ecx, dword ptr [ecx]
0047dd9d  test       ecx, ecx
0047dd9f  jne        0x47dda4

// block 0047dda1
0047dda1  xor        al, al
0047dda3  ret        

// block 0047dda4
0047dda4  mov        eax, dword ptr [ecx]
0047dda6  jmp        dword ptr [eax + 4]
