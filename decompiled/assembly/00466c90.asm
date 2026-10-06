
// block 00466c90
00466c90  cmp        dword ptr [esi + 0x7c], 0
00466c94  je         0x466cd3

// block 00466c96
00466c96  mov        edx, dword ptr [esi + 0x80]
00466c9c  mov        ecx, dword ptr [esi + 0x90]
00466ca2  mov        dword ptr [esi + 0x84], edx
00466ca8  mov        eax, dword ptr [ecx + 0x1c]
00466cab  test       eax, eax
00466cad  jle        0x466cb5

// block 00466caf
00466caf  mov        dword ptr [esi + 0x88], eax

// block 00466cb5
00466cb5  test       bl, bl
00466cb7  je         0x466d6c

// block 00466cbd
00466cbd  mov        ecx, dword ptr [ecx + 0x18]
00466cc0  test       ecx, ecx
00466cc2  jle        0x466d6c

// block 00466cc8
00466cc8  add        ecx, edx
00466cca  mov        dword ptr [esi + 0x84], ecx
00466cd0  xor        eax, eax
00466cd2  ret        

// block 00466cd3
00466cd3  mov        eax, dword ptr [esi + 0x8c]
00466cd9  test       eax, eax
00466cdb  jne        0x466ce3

// block 00466cdd
00466cdd  mov        eax, 0x800401f0
00466ce2  ret        

// block 00466ce3
00466ce3  test       bl, bl
00466ce5  je         0x466d1d

// block 00466ce7
00466ce7  mov        ecx, dword ptr [esi + 0x90]
00466ced  mov        edx, dword ptr [ecx + 0x18]
00466cf0  test       edx, edx
00466cf2  jle        0x466d1d

// block 00466cf4
00466cf4  mov        ecx, dword ptr [ecx + 0x10]
00466cf7  push       0
00466cf9  add        ecx, edx
00466cfb  add        ecx, dword ptr [0x4d4760]
00466d01  push       0
00466d03  push       ecx
00466d04  push       eax
00466d05  call       dword ptr [0x4980a0]

// block 00466d0b
00466d0b  mov        eax, dword ptr [esi + 0x90]
00466d11  mov        edx, dword ptr [eax + 0x1c]
00466d14  sub        edx, dword ptr [eax + 0x18]
00466d17  xor        eax, eax
00466d19  mov        dword ptr [esi + 8], edx
00466d1c  ret        

// block 00466d1d
00466d1d  push       0
00466d1f  push       0
00466d21  test       edi, edi
00466d23  jne        0x466d4b

// block 00466d25
00466d25  mov        ecx, dword ptr [esi + 0x90]
00466d2b  mov        edx, dword ptr [ecx + 0x10]
00466d2e  add        edx, dword ptr [0x4d4760]
00466d34  push       edx
00466d35  push       eax
00466d36  call       dword ptr [0x4980a0]

// block 00466d3c
00466d3c  mov        eax, dword ptr [esi + 0x90]
00466d42  mov        ecx, dword ptr [eax + 0x1c]
00466d45  mov        dword ptr [esi + 8], ecx
00466d48  xor        eax, eax
00466d4a  ret        

// block 00466d4b
00466d4b  mov        edx, dword ptr [0x4d4760]
00466d51  add        edx, edi
00466d53  push       edx
00466d54  push       eax
00466d55  call       dword ptr [0x4980a0]

// block 00466d5b
00466d5b  mov        eax, dword ptr [esi + 0x90]
00466d61  mov        ecx, dword ptr [eax + 0x1c]
00466d64  add        ecx, dword ptr [eax + 0x10]
00466d67  sub        ecx, edi
00466d69  mov        dword ptr [esi + 8], ecx

// block 00466d6c
00466d6c  xor        eax, eax
00466d6e  ret        
