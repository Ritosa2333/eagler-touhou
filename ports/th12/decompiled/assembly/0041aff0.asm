
// block 0041aff0
0041aff0  sub        esp, 0x50
0041aff3  push       ebp
0041aff4  push       esi
0041aff5  push       edi
0041aff6  push       0x50
0041aff8  lea        eax, [esp + 0x10]
0041affc  push       0
0041affe  push       eax
0041afff  call       0x477420

// block 0041b004
0041b004  mov        ebp, dword ptr [0x4b43c8]
0041b00a  mov        esi, dword ptr [esp + 0x6c]
0041b00e  mov        eax, 0xa
0041b013  add        esp, 0xc
0041b016  add        ebp, 0x64
0041b019  add        esi, 0x23c
0041b01f  mov        ecx, 0xc
0041b024  lea        edi, [esp + 0x2c]
0041b028  mov        dword ptr [esp + 0x20], eax
0041b02c  mov        dword ptr [esp + 0x18], eax
0041b030  mov        dword ptr [esp + 0x1c], 0xfffffffe

// block 0041b038
0041b038  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0041b03a
0041b03a  mov        esi, 0x7d0
0041b03f  lea        edi, [eax + 0x3d]

// block 0041b042
0041b042  test       byte ptr [ebp], 1
0041b046  je         0x41b094

// block 0041b048
0041b048  cmp        word ptr [ebp + 0x532], 1
0041b050  jne        0x41b094

// block 0041b052
0041b052  movsx      ecx, word ptr [ebp + 0x9f4]
0041b059  imul       ecx, ecx, 0xd0
0041b05f  cmp        dword ptr [ecx + 0x4af280], edi
0041b065  jne        0x41b094

// block 0041b067
0041b067  mov        edx, dword ptr [ebp + 0x4bc]
0041b06d  mov        eax, dword ptr [ebp + 0x4c0]
0041b073  mov        ecx, dword ptr [ebp + 0x4c4]
0041b079  mov        dword ptr [esp + 0xc], edx
0041b07d  lea        edx, [esp + 0xc]
0041b081  push       edx
0041b082  push       0x49fc14
0041b087  mov        dword ptr [esp + 0x18], eax
0041b08b  mov        dword ptr [esp + 0x1c], ecx
0041b08f  call       0x412990

// block 0041b094
0041b094  add        ebp, 0x9f8
0041b09a  sub        esi, 1
0041b09d  jne        0x41b042

// block 0041b09f
0041b09f  pop        edi
0041b0a0  pop        esi
0041b0a1  xor        eax, eax
0041b0a3  pop        ebp
0041b0a4  add        esp, 0x50
0041b0a7  ret        4
