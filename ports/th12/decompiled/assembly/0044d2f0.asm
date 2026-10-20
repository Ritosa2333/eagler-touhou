
// block 0044d2f0
0044d2f0  mov        eax, dword ptr [esp + 4]
0044d2f4  lea        edx, [eax + 1]

// block 0044d2f7
0044d2f7  mov        cl, byte ptr [eax]
0044d2f9  inc        eax
0044d2fa  test       cl, cl
0044d2fc  jne        0x44d2f7

// block 0044d2fe
0044d2fe  sub        eax, edx
0044d300  ret        
