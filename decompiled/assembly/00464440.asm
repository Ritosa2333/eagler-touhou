
// block 00464440
00464440  test       dword ptr [0x4cee78], 0x8000
0046444a  je         0x46445d

// block 0046444c
0046444c  push       0x4cf1e8
00464451  call       dword ptr [0x498088]

// block 00464457
00464457  inc        byte ptr [0x4cf222]

// block 0046445d
0046445d  mov        eax, 0x9630
00464462  xor        ax, word ptr [esi]
00464465  add        dword ptr [esi + 4], 2
00464469  mov        ecx, 0x6553
0046446e  sub        ax, cx
00464471  push       ebx
00464472  push       edi
00464473  movzx      edi, ax
00464476  mov        dx, di
00464479  shr        dx, 0xe
0046447d  lea        eax, [edi*4]
00464484  add        dx, ax
00464487  mov        ecx, 0x9630
0046448c  xor        dx, cx
0046448f  mov        eax, 0x6553
00464494  sub        dx, ax
00464497  movzx      ebx, dx
0046449a  mov        cx, bx
0046449d  shr        cx, 0xe
004644a1  lea        edx, [ebx*4]
004644a8  add        cx, dx
004644ab  mov        word ptr [esi], cx
004644ae  test       dword ptr [0x4cee78], 0x8000
004644b8  je         0x4644cb

// block 004644ba
004644ba  push       0x4cf1e8
004644bf  call       dword ptr [0x49808c]

// block 004644c5
004644c5  dec        byte ptr [0x4cf222]

// block 004644cb
004644cb  movzx      eax, di
004644ce  movzx      ecx, bx
004644d1  shl        eax, 0x10
004644d4  pop        edi
004644d5  or         eax, ecx
004644d7  pop        ebx
004644d8  ret        
