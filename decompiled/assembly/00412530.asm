
// block 00412530
00412530  test       ecx, ecx
00412532  je         0x412551

// block 00412534
00412534  mov        eax, dword ptr [0x4b43dc]
00412539  mov        eax, dword ptr [eax + 0x68]
0041253c  test       eax, eax
0041253e  je         0x412551

// block 00412540
00412540  mov        edx, dword ptr [eax]
00412542  cmp        dword ptr [edx + 0x27b4], ecx
00412548  je         0x412554

// block 0041254a
0041254a  mov        eax, dword ptr [eax + 4]
0041254d  test       eax, eax
0041254f  jne        0x412540

// block 00412551
00412551  xor        eax, eax
00412553  ret        

// block 00412554
00412554  mov        eax, edx
00412556  ret        
