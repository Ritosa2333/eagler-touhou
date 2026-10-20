
// block 0043f180
0043f180  mov        eax, dword ptr [0x4b451c]
0043f185  mov        cl, 0x10
0043f187  test       byte ptr [eax + 0x1e9d0], cl
0043f18d  jne        0x43f1ba

// block 0043f18f
0043f18f  test       byte ptr [eax + 0x1e9d1], cl
0043f195  jne        0x43f1ba

// block 0043f197
0043f197  test       byte ptr [eax + 0x1e9d2], cl
0043f19d  jne        0x43f1ba

// block 0043f19f
0043f19f  test       byte ptr [eax + 0x1e9d3], cl
0043f1a5  jne        0x43f1ba

// block 0043f1a7
0043f1a7  test       byte ptr [eax + 0x1e9d4], cl
0043f1ad  jne        0x43f1ba

// block 0043f1af
0043f1af  test       byte ptr [eax + 0x1e9d5], cl
0043f1b5  jne        0x43f1ba

// block 0043f1b7
0043f1b7  xor        eax, eax
0043f1b9  ret        

// block 0043f1ba
0043f1ba  mov        eax, 1
0043f1bf  ret        
