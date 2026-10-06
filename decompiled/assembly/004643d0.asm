
// block 004643d0
004643d0  test       dword ptr [0x4cee78], 0x8000
004643da  je         0x4643ed

// block 004643dc
004643dc  push       0x4cf1e8
004643e1  call       dword ptr [0x498088]

// block 004643e7
004643e7  inc        byte ptr [0x4cf222]

// block 004643ed
004643ed  inc        dword ptr [esi + 4]
004643f0  mov        eax, 0x9630
004643f5  xor        ax, word ptr [esi]
004643f8  mov        ecx, 0x6553
004643fd  sub        ax, cx
00464400  movzx      eax, ax
00464403  mov        dx, ax
00464406  add        eax, eax
00464408  shr        dx, 0xe
0046440c  add        eax, eax
0046440e  add        dx, ax
00464411  mov        word ptr [esi], dx
00464414  test       dword ptr [0x4cee78], 0x8000
0046441e  je         0x464435

// block 00464420
00464420  push       0x4cf1e8
00464425  call       dword ptr [0x49808c]

// block 0046442b
0046442b  dec        byte ptr [0x4cf222]
00464431  mov        ax, word ptr [esi]
00464434  ret        

// block 00464435
00464435  mov        ax, dx
00464438  ret        
