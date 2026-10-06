
// block 00454b30
00454b30  push       0x52a0
00454b35  push       0
00454b37  push       0x4cf4e8
00454b3c  call       0x477420

// block 00454b41
00454b41  add        esp, 0xc
00454b44  xor        edx, edx
00454b46  mov        eax, 0x4d0e70
00454b4b  jmp        0x454b50

// block 00454b50
00454b50  mov        dword ptr [eax + 4], 0xffffffff
00454b57  mov        ecx, 0x4ae5a0
00454b5c  lea        esp, [esp]

// block 00454b60
00454b60  cmp        dword ptr [ecx], edx
00454b62  je         0x454b69

// block 00454b64
00454b64  add        ecx, 0x14
00454b67  jne        0x454b60

// block 00454b69
00454b69  mov        dword ptr [eax + 0xc], edx
00454b6c  mov        dword ptr [eax + 8], ecx
00454b6f  add        eax, 0x18
00454b72  inc        edx
00454b73  cmp        eax, 0x4d1410
00454b78  jl         0x454b50

// block 00454b7a
00454b7a  mov        eax, 0x4cf4e8
00454b7f  ret        
