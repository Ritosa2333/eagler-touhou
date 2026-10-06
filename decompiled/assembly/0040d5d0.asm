
// block 0040d5d0
0040d5d0  mov        eax, dword ptr [esi + 0x494]
0040d5d6  test       eax, eax
0040d5d8  je         0x40d5f0

// block 0040d5da
0040d5da  mov        edx, 2
0040d5df  mov        ecx, esi
0040d5e1  call       eax

// block 0040d5e3
0040d5e3  mov        eax, 2
0040d5e8  mov        word ptr [esi + 0x3c4], ax
0040d5ef  ret        

// block 0040d5f0
0040d5f0  mov        ecx, 2
0040d5f5  mov        word ptr [esi + 0x3c4], cx
0040d5fc  ret        
