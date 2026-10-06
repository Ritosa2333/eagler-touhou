
// block 00412c40
00412c40  and        dword ptr [esi + 0x60], 0xfffffffe
00412c44  push       0x7c
00412c46  push       0
00412c48  push       esi
00412c49  call       0x477420

// block 00412c4e
00412c4e  or         dword ptr [esi], 2
00412c51  add        esp, 0xc
00412c54  mov        dword ptr [0x4b43dc], esi
00412c5a  mov        eax, esi
00412c5c  ret        
