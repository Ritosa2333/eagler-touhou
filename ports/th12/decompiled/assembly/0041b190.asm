
// block 0041b190
0041b190  mov        ecx, dword ptr [0x4ceaa4]
0041b196  mov        eax, dword ptr [ecx + 0x494]
0041b19c  push       esi
0041b19d  mov        esi, ecx
0041b19f  test       eax, eax
0041b1a1  je         0x41b1aa

// block 0041b1a3
0041b1a3  mov        edx, 3
0041b1a8  call       eax

// block 0041b1aa
0041b1aa  mov        eax, 3
0041b1af  mov        word ptr [esi + 0x3c4], ax
0041b1b6  xor        eax, eax
0041b1b8  pop        esi
0041b1b9  ret        
