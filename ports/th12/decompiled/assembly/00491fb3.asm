
// block 00491fb3
00491fb3  mov        edi, edi
00491fb5  push       ebp
00491fb6  mov        ebp, esp
00491fb8  push       esi
00491fb9  push       edi
00491fba  mov        edi, dword ptr [ebp + 8]
00491fbd  mov        eax, dword ptr [edi + 4]
00491fc0  test       eax, eax
00491fc2  je         0x49200b

// block 00491fc4
00491fc4  lea        edx, [eax + 8]
00491fc7  cmp        byte ptr [edx], 0
00491fca  je         0x49200b

// block 00491fcc
00491fcc  mov        esi, dword ptr [ebp + 0xc]
00491fcf  mov        ecx, dword ptr [esi + 4]
00491fd2  cmp        eax, ecx
00491fd4  je         0x491fea

// block 00491fd6
00491fd6  add        ecx, 8
00491fd9  push       ecx
00491fda  push       edx
00491fdb  call       0x472ac0

// block 00491fe0
00491fe0  pop        ecx
00491fe1  pop        ecx
00491fe2  test       eax, eax
00491fe4  je         0x491fea

// block 00491fe6
00491fe6  xor        eax, eax
00491fe8  jmp        0x49200e

// block 00491fea
00491fea  test       byte ptr [esi], 2
00491fed  je         0x491ff4

// block 00491fef
00491fef  test       byte ptr [edi], 8
00491ff2  je         0x491fe6

// block 00491ff4
00491ff4  mov        eax, dword ptr [ebp + 0x10]
00491ff7  mov        eax, dword ptr [eax]
00491ff9  test       al, 1
00491ffb  je         0x492002

// block 00491ffd
00491ffd  test       byte ptr [edi], 1
00492000  je         0x491fe6

// block 00492002
00492002  test       al, 2
00492004  je         0x49200b

// block 00492006
00492006  test       byte ptr [edi], 2
00492009  je         0x491fe6

// block 0049200b
0049200b  xor        eax, eax
0049200d  inc        eax

// block 0049200e
0049200e  pop        edi
0049200f  pop        esi
00492010  pop        ebp
00492011  ret        
