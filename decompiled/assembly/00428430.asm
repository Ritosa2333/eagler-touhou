
// block 00428430
00428430  mov        eax, ecx
00428432  mov        ecx, dword ptr [0x4b44e8]
00428438  test       byte ptr [ecx + 0x60], 4
0042843c  je         0x428444

// block 0042843e
0042843e  mov        eax, 1
00428443  ret        

// block 00428444
00428444  jmp        0x4283a0
