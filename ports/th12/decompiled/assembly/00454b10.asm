
// block 00454b10
00454b10  mov        dword ptr [ecx + 4], 0xffffffff
00454b17  mov        eax, 0x4ae5a0
00454b1c  lea        esp, [esp]

// block 00454b20
00454b20  cmp        dword ptr [eax], edx
00454b22  je         0x454b29

// block 00454b24
00454b24  add        eax, 0x14
00454b27  jne        0x454b20

// block 00454b29
00454b29  mov        dword ptr [ecx + 0xc], edx
00454b2c  mov        dword ptr [ecx + 8], eax
00454b2f  ret        
