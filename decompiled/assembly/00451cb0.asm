
// block 00451cb0
00451cb0  mov        eax, dword ptr [esp + 4]
00451cb4  mov        eax, dword ptr [eax + 0x18]
00451cb7  sub        esp, 0x18
00451cba  test       al, 3
00451cbc  je         0x451d11

// block 00451cbe
00451cbe  mov        ecx, dword ptr [0x4ce8c8]
00451cc4  mov        dword ptr [esp + 8], eax
00451cc8  mov        eax, dword ptr [ecx*4 + 0x4ce90c]
00451ccf  lea        ecx, [esp]
00451cd2  push       ecx
00451cd3  mov        dword ptr [esp + 4], 0x18
00451cdb  mov        dword ptr [esp + 8], 0x10
00451ce3  mov        dword ptr [esp + 0x10], 2
00451ceb  mov        dword ptr [esp + 0x14], 0xfffffc18
00451cf3  mov        dword ptr [esp + 0x18], 0x3e8
00451cfb  mov        edx, dword ptr [eax]
00451cfd  mov        edx, dword ptr [edx + 0x18]
00451d00  push       4
00451d02  push       eax
00451d03  call       edx

// block 00451d05
00451d05  test       eax, eax
00451d07  jge        0x451d11

// block 00451d09
00451d09  xor        eax, eax
00451d0b  add        esp, 0x18
00451d0e  ret        8

// block 00451d11
00451d11  mov        eax, 1
00451d16  add        esp, 0x18
00451d19  ret        8
