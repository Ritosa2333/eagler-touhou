
// block 0043cb70
0043cb70  push       0x18b0
0043cb75  push       0
0043cb77  push       esi
0043cb78  call       0x477420

// block 0043cb7d
0043cb7d  lea        eax, [esi + 0x151c]
0043cb83  mov        dword ptr [esi + 0x18a0], eax
0043cb89  mov        dword ptr [esi + 0x1518], esi
0043cb8f  add        esp, 0xc
0043cb92  mov        dword ptr [esi + 0x18a4], esi
0043cb98  mov        dword ptr [esi + 0x18a8], 0
0043cba2  mov        dword ptr [esi + 0x18ac], 0
0043cbac  mov        eax, esi
0043cbae  ret        
