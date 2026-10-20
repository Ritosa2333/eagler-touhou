
// block 00454d10
00454d10  push       ebp
00454d11  mov        ebp, esp
00454d13  and        esp, 0xfffffff8
00454d16  push       ecx
00454d17  push       ebx
00454d18  mov        ebx, dword ptr [ebp + 0xc]
00454d1b  push       esi
00454d1c  push       edi
00454d1d  mov        edi, ecx
00454d1f  mov        eax, dword ptr [edi + 0x11c]
00454d25  cmp        dword ptr [eax + ebx*4], 0
00454d29  je         0x454dd3

// block 00454d2f
00454d2f  cmp        dword ptr [edi + 0x124], 0
00454d36  jne        0x454dd3

// block 00454d3c
00454d3c  mov        esi, dword ptr [ebp + 8]
00454d3f  call       0x402520

// block 00454d44
00454d44  fldz       
00454d46  mov        word ptr [esi + 0x3ea], bx
00454d4d  mov        cx, word ptr [edi]
00454d50  and        dword ptr [esi + 0x47c], 0xfffff3ff
00454d5a  mov        word ptr [esi + 0x3e6], cx
00454d61  mov        dword ptr [esi + 0x3f8], edi
00454d67  mov        edx, dword ptr [edi + 0x11c]
00454d6d  mov        eax, dword ptr [edx + ebx*4]
00454d70  mov        dword ptr [esi + 0x3ec], eax
00454d76  mov        dword ptr [esi + 0x3f0], eax
00454d7c  mov        eax, dword ptr [esi + 0x78]
00454d7f  test       al, 1
00454d81  jne        0x454da1

// block 00454d83
00454d83  or         eax, 1
00454d86  fst        dword ptr [esi + 0x70]
00454d89  mov        dword ptr [esi + 0x6c], 0
00454d90  mov        dword ptr [esi + 0x68], 0xfff0bdc1
00454d97  mov        dword ptr [esi + 0x74], 0x4b2ed0
00454d9e  mov        dword ptr [esi + 0x78], eax

// block 00454da1
00454da1  fstp       dword ptr [esi + 0x70]
00454da4  mov        dword ptr [esi + 0x6c], 0
00454dab  mov        dword ptr [esi + 0x68], 0xffffffff
00454db2  and        dword ptr [esi + 0x47c], 0xfffffffe
00454db9  push       esi
00454dba  call       0x455630

// block 00454dbf
00454dbf  mov        eax, dword ptr [0x4ce8cc]
00454dc4  inc        dword ptr [eax + 0xa0]
00454dca  pop        edi
00454dcb  pop        esi
00454dcc  pop        ebx
00454dcd  mov        esp, ebp
00454dcf  pop        ebp
00454dd0  ret        8

// block 00454dd3
00454dd3  mov        eax, dword ptr [ebp + 8]
00454dd6  push       0x4b4
00454ddb  push       0
00454ddd  push       eax
00454dde  call       0x477420

// block 00454de3
00454de3  add        esp, 0xc
00454de6  pop        edi
00454de7  pop        esi
00454de8  pop        ebx
00454de9  mov        esp, ebp
00454deb  pop        ebp
00454dec  ret        8
