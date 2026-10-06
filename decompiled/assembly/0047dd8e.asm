
// block 0047dd8e
0047dd8e  mov        ecx, dword ptr [ecx]
0047dd90  test       ecx, ecx
0047dd92  jne        0x47dd97

// block 0047dd94
0047dd94  xor        eax, eax
0047dd96  ret        

// block 0047dd97
0047dd97  mov        eax, dword ptr [ecx]
0047dd99  jmp        dword ptr [eax]
