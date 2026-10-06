
// block 0041b490
0041b490  mov        ecx, dword ptr [0x4ceaa4]
0041b496  mov        eax, dword ptr [ecx + 0x494]
0041b49c  push       esi
0041b49d  mov        esi, ecx
0041b49f  test       eax, eax
0041b4a1  je         0x41b4aa

// block 0041b4a3
0041b4a3  mov        edx, 2
0041b4a8  call       eax

// block 0041b4aa
0041b4aa  mov        eax, 2
0041b4af  mov        word ptr [esi + 0x3c4], ax
0041b4b6  mov        ecx, dword ptr [0x4ceaa4]
0041b4bc  push       ecx
0041b4bd  call       0x455630

// block 0041b4c2
0041b4c2  xor        eax, eax
0041b4c4  pop        esi
0041b4c5  ret        
