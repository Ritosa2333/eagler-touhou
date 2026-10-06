
// block 0043ef40
0043ef40  fldz       
0043ef42  mov        dword ptr [eax + 0x24], ecx
0043ef45  mov        ecx, dword ptr [eax + 0x2c4]
0043ef4b  xor        edx, edx
0043ef4d  test       cl, 1
0043ef50  jne        0x43ef7b

// block 0043ef52
0043ef52  or         ecx, 1
0043ef55  fst        dword ptr [eax + 0x2bc]
0043ef5b  mov        dword ptr [eax + 0x2b8], edx
0043ef61  mov        dword ptr [eax + 0x2b4], 0xfff0bdc1
0043ef6b  mov        dword ptr [eax + 0x2c0], 0x4b2ed0
0043ef75  mov        dword ptr [eax + 0x2c4], ecx

// block 0043ef7b
0043ef7b  fstp       dword ptr [eax + 0x2bc]
0043ef81  mov        dword ptr [eax + 0x2b8], edx
0043ef87  mov        dword ptr [eax + 0x2b4], 0xffffffff
0043ef91  ret        
