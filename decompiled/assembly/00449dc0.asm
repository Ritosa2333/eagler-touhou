
// block 00449dc0
00449dc0  cmp        byte ptr [ecx], 0xa
00449dc3  mov        eax, ecx
00449dc5  je         0x449ddb

// block 00449dc7
00449dc7  cmp        byte ptr [eax], 0xd
00449dca  je         0x449ddb

// block 00449dcc
00449dcc  mov        edx, dword ptr [edi]
00449dce  test       edx, edx
00449dd0  je         0x449e16

// block 00449dd2
00449dd2  inc        eax
00449dd3  dec        edx
00449dd4  cmp        byte ptr [eax], 0xa
00449dd7  mov        dword ptr [edi], edx
00449dd9  jne        0x449dc7

// block 00449ddb
00449ddb  push       ebx
00449ddc  mov        ebx, dword ptr [edi]
00449dde  test       ebx, ebx
00449de0  je         0x449e15

// block 00449de2
00449de2  push       esi
00449de3  mov        esi, dword ptr [esp + 0xc]
00449de7  mov        byte ptr [eax], 0
00449dea  sub        esi, ecx
00449dec  lea        esp, [esp]

// block 00449df0
00449df0  mov        dl, byte ptr [ecx]
00449df2  mov        byte ptr [esi + ecx], dl
00449df5  inc        ecx
00449df6  test       dl, dl
00449df8  jne        0x449df0

// block 00449dfa
00449dfa  inc        eax
00449dfb  lea        ecx, [ebx - 1]
00449dfe  pop        esi

// block 00449dff
00449dff  mov        dl, byte ptr [eax]
00449e01  mov        dword ptr [edi], ecx
00449e03  cmp        dl, 0xa
00449e06  je         0x449e0d

// block 00449e08
00449e08  cmp        dl, 0xd
00449e0b  jne        0x449e15

// block 00449e0d
00449e0d  test       ecx, ecx
00449e0f  je         0x449e15

// block 00449e11
00449e11  inc        eax
00449e12  dec        ecx
00449e13  jmp        0x449dff

// block 00449e15
00449e15  pop        ebx

// block 00449e16
00449e16  ret        4
