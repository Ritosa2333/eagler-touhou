
// block 00454ad0
00454ad0  mov        eax, dword ptr [esi]
00454ad2  test       eax, eax
00454ad4  je         0x454ae4

// block 00454ad6
00454ad6  mov        ecx, dword ptr [eax]
00454ad8  mov        edx, dword ptr [ecx + 8]
00454adb  push       eax
00454adc  call       edx

// block 00454ade
00454ade  mov        dword ptr [esi], 0

// block 00454ae4
00454ae4  ret        
