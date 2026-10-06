
// block 00454df0
00454df0  push       ebp
00454df1  mov        ebp, dword ptr [esp + 8]
00454df5  push       esi
00454df6  mov        esi, eax
00454df8  mov        eax, dword ptr [edi + 0x11c]
00454dfe  cmp        dword ptr [eax + ebp*4], 0
00454e02  je         0x454ebf

// block 00454e08
00454e08  cmp        dword ptr [edi + 0x124], 0
00454e0f  jne        0x454ebf

// block 00454e15
00454e15  call       0x402520

// block 00454e1a
00454e1a  fldz       
00454e1c  mov        ecx, dword ptr [ebx]
00454e1e  mov        dword ptr [esi + 0x424], ecx
00454e24  mov        edx, dword ptr [ebx + 4]
00454e27  mov        dword ptr [esi + 0x428], edx
00454e2d  mov        eax, dword ptr [ebx + 8]
00454e30  mov        dword ptr [esi + 0x42c], eax
00454e36  mov        word ptr [esi + 0x3ea], bp
00454e3d  mov        cx, word ptr [edi]
00454e40  and        dword ptr [esi + 0x47c], 0xfffff3ff
00454e4a  mov        word ptr [esi + 0x3e6], cx
00454e51  mov        dword ptr [esi + 0x3f8], edi
00454e57  mov        edx, dword ptr [edi + 0x11c]
00454e5d  mov        eax, dword ptr [edx + ebp*4]
00454e60  mov        dword ptr [esi + 0x3ec], eax
00454e66  mov        dword ptr [esi + 0x3f0], eax
00454e6c  mov        eax, dword ptr [esi + 0x78]
00454e6f  test       al, 1
00454e71  jne        0x454e91

// block 00454e73
00454e73  or         eax, 1
00454e76  fst        dword ptr [esi + 0x70]
00454e79  mov        dword ptr [esi + 0x6c], 0
00454e80  mov        dword ptr [esi + 0x68], 0xfff0bdc1
00454e87  mov        dword ptr [esi + 0x74], 0x4b2ed0
00454e8e  mov        dword ptr [esi + 0x78], eax

// block 00454e91
00454e91  fstp       dword ptr [esi + 0x70]
00454e94  mov        dword ptr [esi + 0x6c], 0
00454e9b  mov        dword ptr [esi + 0x68], 0xffffffff
00454ea2  and        dword ptr [esi + 0x47c], 0xfffffffe
00454ea9  push       esi
00454eaa  call       0x455630

// block 00454eaf
00454eaf  mov        eax, dword ptr [0x4ce8cc]
00454eb4  inc        dword ptr [eax + 0xa0]
00454eba  pop        esi
00454ebb  pop        ebp
00454ebc  ret        4

// block 00454ebf
00454ebf  push       0x4b4
00454ec4  push       0
00454ec6  push       esi
00454ec7  call       0x477420

// block 00454ecc
00454ecc  add        esp, 0xc
00454ecf  pop        esi
00454ed0  pop        ebp
00454ed1  ret        4
