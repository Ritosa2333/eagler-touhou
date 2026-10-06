
// block 00469610
00469610  mov        ecx, dword ptr [eax + 0x1000]
00469616  push       esi
00469617  lea        esi, [ecx + 4]
0046961a  cmp        esi, 0x1000
00469620  jl         0x469629

// block 00469622
00469622  or         eax, 0xffffffff
00469625  pop        esi
00469626  ret        4

// block 00469629
00469629  mov        esi, 4
0046962e  test       dl, dl
00469630  je         0x46963b

// block 00469632
00469632  mov        byte ptr [ecx + eax], dl
00469635  add        dword ptr [eax + 0x1000], esi

// block 0046963b
0046963b  mov        ecx, dword ptr [eax + 0x1000]
00469641  mov        edx, dword ptr [esp + 8]
00469645  mov        edx, dword ptr [edx]
00469647  add        ecx, eax
00469649  mov        dword ptr [ecx], edx
0046964b  add        dword ptr [eax + 0x1000], esi
00469651  xor        eax, eax
00469653  pop        esi
00469654  ret        4
