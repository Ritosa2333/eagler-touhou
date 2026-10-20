
// block 0044d550
0044d550  mov        eax, dword ptr [esi]
0044d552  test       eax, eax
0044d554  je         0x44d563

// block 0044d556
0044d556  push       eax
0044d557  call       dword ptr [0x498030]

// block 0044d55d
0044d55d  mov        dword ptr [esi], 0

// block 0044d563
0044d563  ret        
