
// block 0041c890
0041c890  mov        eax, dword ptr [esi]
0041c892  mov        edx, dword ptr [0x4ce8cc]
0041c898  push       eax
0041c899  call       0x461920

// block 0041c89e
0041c89e  test       eax, eax
0041c8a0  jne        0x41c8a4

// block 0041c8a2
0041c8a2  mov        dword ptr [esi], eax

// block 0041c8a4
0041c8a4  xor        ecx, ecx
0041c8a6  test       eax, eax
0041c8a8  setne      cl
0041c8ab  mov        eax, ecx
0041c8ad  ret        
