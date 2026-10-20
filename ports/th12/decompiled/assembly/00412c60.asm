
// block 00412c60
00412c60  mov        eax, dword ptr [0x4b43c8]
00412c65  mov        ecx, dword ptr [eax + 0x4debdc]
00412c6b  push       ebp
00412c6c  push       esi
00412c6d  push       0x1098
00412c72  mov        dword ptr [edi + 0x40], ecx
00412c75  call       0x46c9ea

// block 00412c7a
00412c7a  mov        esi, eax
00412c7c  xor        ebp, ebp
00412c7e  add        esp, 4
00412c81  cmp        esi, ebp
00412c83  je         0x412caa

// block 00412c85
00412c85  push       0x1098
00412c8a  push       ebp
00412c8b  push       esi
00412c8c  mov        dword ptr [esi + 0x1090], ebp
00412c92  mov        dword ptr [esi + 0x1094], ebp
00412c98  call       0x477420

// block 00412c9d
00412c9d  add        esp, 0xc
00412ca0  mov        dword ptr [esi], 0x49fc6c
00412ca6  mov        ecx, esi
00412ca8  jmp        0x412cac

// block 00412caa
00412caa  xor        ecx, ecx

// block 00412cac
00412cac  mov        eax, dword ptr [esp + 0xc]
00412cb0  mov        dword ptr [edi + 0x64], ecx
00412cb3  mov        edx, dword ptr [ecx]
00412cb5  mov        edx, dword ptr [edx + 8]
00412cb8  push       eax
00412cb9  call       edx

// block 00412cbb
00412cbb  push       0x24
00412cbd  call       0x46c9ea

// block 00412cc2
00412cc2  add        esp, 4
00412cc5  cmp        eax, ebp
00412cc7  je         0x412ce5

// block 00412cc9
00412cc9  and        dword ptr [eax + 4], 0xfffffffe
00412ccd  mov        dword ptr [eax + 8], ebp
00412cd0  mov        dword ptr [eax + 0xc], ebp
00412cd3  mov        dword ptr [eax + 0x10], ebp
00412cd6  mov        dword ptr [eax], ebp
00412cd8  mov        dword ptr [eax + 0x14], eax
00412cdb  mov        dword ptr [eax + 0x18], ebp
00412cde  mov        dword ptr [eax + 0x1c], ebp
00412ce1  mov        esi, eax
00412ce3  jmp        0x412ce7

// block 00412ce5
00412ce5  xor        esi, esi

// block 00412ce7
00412ce7  mov        eax, dword ptr [esi + 4]
00412cea  and        eax, 0xfffffffd
00412ced  push       ebx
00412cee  or         eax, 1
00412cf1  mov        ebx, 0x12
00412cf6  mov        dword ptr [esi + 8], 0x413210
00412cfd  mov        dword ptr [esi + 0xc], ebp
00412d00  mov        dword ptr [esi + 0x10], ebp
00412d03  mov        dword ptr [esi + 0x20], edi
00412d06  mov        dword ptr [esi + 4], eax
00412d09  call       0x462380

// block 00412d0e
00412d0e  push       0x24
00412d10  mov        dword ptr [edi + 8], esi
00412d13  call       0x46c9ea

// block 00412d18
00412d18  add        esp, 4
00412d1b  cmp        eax, ebp
00412d1d  je         0x412d3b

// block 00412d1f
00412d1f  and        dword ptr [eax + 4], 0xfffffffe
00412d23  mov        dword ptr [eax + 8], ebp
00412d26  mov        dword ptr [eax + 0xc], ebp
00412d29  mov        dword ptr [eax + 0x10], ebp
00412d2c  mov        dword ptr [eax], ebp
00412d2e  mov        dword ptr [eax + 0x14], eax
00412d31  mov        dword ptr [eax + 0x18], ebp
00412d34  mov        dword ptr [eax + 0x1c], ebp
00412d37  mov        esi, eax
00412d39  jmp        0x412d3d

// block 00412d3b
00412d3b  xor        esi, esi

// block 00412d3d
00412d3d  mov        ecx, dword ptr [esi + 4]
00412d40  and        ecx, 0xfffffffd
00412d43  or         ecx, 1
00412d46  mov        ebx, 0x15
00412d4b  mov        dword ptr [esi + 8], 0x413220
00412d52  mov        dword ptr [esi + 0xc], ebp
00412d55  mov        dword ptr [esi + 0x10], ebp
00412d58  mov        dword ptr [esi + 0x20], edi
00412d5b  mov        dword ptr [esi + 4], ecx
00412d5e  call       0x462420

// block 00412d63
00412d63  fldz       
00412d65  mov        dword ptr [edi + 0xc], esi
00412d68  mov        eax, dword ptr [edi + 0x60]
00412d6b  pop        ebx
00412d6c  test       al, 1
00412d6e  jne        0x412d8a

// block 00412d70
00412d70  or         eax, 1
00412d73  fst        dword ptr [edi + 0x58]
00412d76  mov        dword ptr [edi + 0x54], ebp
00412d79  mov        dword ptr [edi + 0x50], 0xfff0bdc1
00412d80  mov        dword ptr [edi + 0x5c], 0x4b2ed0
00412d87  mov        dword ptr [edi + 0x60], eax

// block 00412d8a
00412d8a  pop        esi
00412d8b  fstp       dword ptr [edi + 0x58]
00412d8e  mov        dword ptr [edi + 0x54], ebp
00412d91  mov        dword ptr [edi + 0x50], 0xffffffff
00412d98  xor        eax, eax
00412d9a  pop        ebp
00412d9b  ret        4
