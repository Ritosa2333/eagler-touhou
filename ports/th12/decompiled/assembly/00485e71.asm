
// block 00485e71
00485e71  mov        edi, edi
00485e73  push       ebp
00485e74  mov        ebp, esp
00485e76  sub        esp, 0x7c
00485e79  mov        eax, dword ptr [0x4ad138]
00485e7e  xor        eax, ebp
00485e80  mov        dword ptr [ebp - 4], eax
00485e83  push       esi
00485e84  push       edi
00485e85  mov        edi, dword ptr [ebp + 8]
00485e88  call       0x475467

// block 00485e8d
00485e8d  mov        esi, eax
00485e8f  mov        edx, edi
00485e91  add        esi, 0x9c
00485e97  call       0x485b44

// block 00485e9c
00485e9c  mov        edi, eax
00485e9e  push       0x78
00485ea0  lea        eax, [ebp - 0x7c]
00485ea3  push       eax
00485ea4  mov        eax, dword ptr [esi + 0x10]
00485ea7  neg        eax
00485ea9  sbb        eax, eax
00485eab  and        eax, 0xfffff002
00485eb0  add        eax, 0x1001
00485eb5  push       eax
00485eb6  push       edi
00485eb7  call       dword ptr [0x4980e0]

// block 00485ebd
00485ebd  test       eax, eax
00485ebf  jne        0x485ec7

// block 00485ec1
00485ec1  and        dword ptr [esi + 8], eax
00485ec4  inc        eax
00485ec5  jmp        0x485f22

// block 00485ec7
00485ec7  lea        eax, [ebp - 0x7c]
00485eca  push       eax
00485ecb  push       dword ptr [esi]
00485ecd  call       0x46e3f8

// block 00485ed2
00485ed2  pop        ecx
00485ed3  pop        ecx
00485ed4  test       eax, eax
00485ed6  jne        0x485ee1

// block 00485ed8
00485ed8  cmp        dword ptr [esi + 0x10], eax
00485edb  jne        0x485f0d

// block 00485edd
00485edd  push       1
00485edf  jmp        0x485eff

// block 00485ee1
00485ee1  cmp        dword ptr [esi + 0x10], 0
00485ee5  jne        0x485f17

// block 00485ee7
00485ee7  cmp        dword ptr [esi + 0xc], 0
00485eeb  je         0x485f17

// block 00485eed
00485eed  lea        eax, [ebp - 0x7c]
00485ef0  push       eax
00485ef1  push       dword ptr [esi]
00485ef3  call       0x46e3f8

// block 00485ef8
00485ef8  pop        ecx
00485ef9  pop        ecx
00485efa  test       eax, eax
00485efc  jne        0x485f17

// block 00485efe
00485efe  push       eax

// block 00485eff
00485eff  push       edi
00485f00  mov        ecx, esi
00485f02  call       0x485c2b

// block 00485f07
00485f07  pop        ecx
00485f08  pop        ecx
00485f09  test       eax, eax
00485f0b  je         0x485f17

// block 00485f0d
00485f0d  or         dword ptr [esi + 8], 4
00485f11  mov        dword ptr [esi + 0x18], edi
00485f14  mov        dword ptr [esi + 0x1c], edi

// block 00485f17
00485f17  mov        eax, dword ptr [esi + 8]
00485f1a  shr        eax, 2
00485f1d  not        eax
00485f1f  and        eax, 1

// block 00485f22
00485f22  mov        ecx, dword ptr [ebp - 4]
00485f25  pop        edi
00485f26  xor        ecx, ebp
00485f28  pop        esi
00485f29  call       0x46c932

// block 00485f2e
00485f2e  leave      
00485f2f  ret        4
