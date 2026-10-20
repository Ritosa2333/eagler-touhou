
// block 004644e0
004644e0  test       dword ptr [0x4cee78], 0x8000
004644ea  je         0x4644fd

// block 004644ec
004644ec  push       0x4cf1e8
004644f1  call       dword ptr [0x498088]

// block 004644f7
004644f7  inc        byte ptr [0x4cf222]

// block 004644fd
004644fd  mov        eax, 0x9630
00464502  xor        ax, word ptr [esi]
00464505  add        dword ptr [esi + 4], 2
00464509  mov        ecx, 0x6553
0046450e  sub        ax, cx
00464511  push       ebx
00464512  push       edi
00464513  movzx      edi, ax
00464516  mov        dx, di
00464519  shr        dx, 0xe
0046451d  lea        eax, [edi*4]
00464524  add        dx, ax
00464527  mov        ecx, 0x9630
0046452c  xor        dx, cx
0046452f  mov        eax, 0x6553
00464534  sub        dx, ax
00464537  movzx      ebx, dx
0046453a  mov        cx, bx
0046453d  shr        cx, 0xe
00464541  lea        edx, [ebx*4]
00464548  add        cx, dx
0046454b  mov        word ptr [esi], cx
0046454e  test       dword ptr [0x4cee78], 0x8000
00464558  je         0x46456b

// block 0046455a
0046455a  push       0x4cf1e8
0046455f  call       dword ptr [0x49808c]

// block 00464565
00464565  dec        byte ptr [0x4cf222]

// block 0046456b
0046456b  movzx      eax, di
0046456e  movzx      ecx, bx
00464571  shl        eax, 0x10
00464574  pop        edi
00464575  or         eax, ecx
00464577  pop        ebx
00464578  ret        
