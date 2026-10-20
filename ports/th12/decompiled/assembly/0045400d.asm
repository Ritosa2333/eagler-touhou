
// block 0045400d
0045400d  mov        eax, dword ptr [0x4d4754]
00454012  cmp        eax, ebx
00454014  je         0x454282

// block 0045401a
0045401a  mov        ecx, dword ptr [0x4d477c]
00454020  call       0x4663e0

// block 00454025
00454025  jmp        0x454282

// block 00454282
00454282  xor        ebx, ebx
00454284  xor        eax, eax
00454286  lea        edx, [ebp + 0x10c]
0045428c  lea        esp, [esp]

// block 00454290
00454290  cmp        dword ptr [ebp], ebx
00454293  je         0x4542b2

// block 00454295
00454295  mov        esi, edx
00454297  mov        edi, ebp
00454299  inc        eax
0045429a  mov        ecx, 0x43
0045429f  add        ebp, 0x10c
004542a5  add        edx, 0x10c
004542ab  cmp        eax, 0x1f

// block 004542ae
004542ae  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004542b0
004542b0  jl         0x454290

// block 004542b2
004542b2  cmp        dword ptr [esp + 0x10], ebx
004542b6  jne        0x453ff5

// block 004542bc
004542bc  jmp        0x4543a4
