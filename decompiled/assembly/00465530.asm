
// block 00465530
00465530  test       eax, eax
00465532  je         0x46553f

// block 00465534
00465534  cmp        dword ptr [eax], ecx
00465536  je         0x465541

// block 00465538
00465538  mov        eax, dword ptr [eax + 4]
0046553b  test       eax, eax
0046553d  jne        0x465534

// block 0046553f
0046553f  xor        eax, eax

// block 00465541
00465541  ret        
