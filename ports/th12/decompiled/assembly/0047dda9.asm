
// block 0047dda9
0047dda9  mov        edi, edi
0047ddab  push       ebp
0047ddac  mov        ebp, esp
0047ddae  mov        ecx, dword ptr [ecx]
0047ddb0  test       ecx, ecx
0047ddb2  jne        0x47ddbb

// block 0047ddb4
0047ddb4  mov        eax, dword ptr [ebp + 8]
0047ddb7  pop        ebp
0047ddb8  ret        8

// block 0047ddbb
0047ddbb  mov        eax, dword ptr [ecx]
0047ddbd  pop        ebp
0047ddbe  jmp        dword ptr [eax + 8]
