
// block 00421690
00421690  push       ebx
00421691  push       esi
00421692  push       edi
00421693  mov        edi, dword ptr [0x4b43e4]
00421699  mov        esi, dword ptr [edi + 0x6788]
0042169f  xor        ebx, ebx
004216a1  inc        esi
004216a2  cmp        dword ptr [esp + 0x10], ebx
004216a6  mov        eax, esi
004216a8  setne      bl
004216ab  imul       eax, eax, 0x4b4
004216b1  mov        edx, dword ptr [eax + edi + 0x58b0]
004216b8  lea        eax, [eax + edi + 0x54b8]
004216bf  lea        ebx, [ebx*4 + 0x3b]
004216c6  test       edx, edx
004216c8  je         0x4216d7

// block 004216ca
004216ca  mov        ecx, dword ptr [0x4b0c4c]
004216d0  add        ecx, ebx
004216d2  call       0x454b80

// block 004216d7
004216d7  inc        esi
004216d8  cmp        esi, 4
004216db  jl         0x4216e2

// block 004216dd
004216dd  mov        esi, 1

// block 004216e2
004216e2  mov        edx, esi
004216e4  imul       edx, edx, 0x4b4
004216ea  lea        eax, [edx + edi + 0x54b8]
004216f1  mov        edx, dword ptr [eax + 0x3f8]
004216f7  test       edx, edx
004216f9  je         0x421708

// block 004216fb
004216fb  mov        ecx, dword ptr [0x4b0c50]
00421701  add        ecx, ebx
00421703  call       0x454b80

// block 00421708
00421708  inc        esi
00421709  cmp        esi, 4
0042170c  jl         0x421713

// block 0042170e
0042170e  mov        esi, 1

// block 00421713
00421713  imul       esi, esi, 0x4b4
00421719  mov        edx, dword ptr [esi + edi + 0x58b0]
00421720  lea        eax, [esi + edi + 0x54b8]
00421727  test       edx, edx
00421729  je         0x421738

// block 0042172b
0042172b  mov        ecx, dword ptr [0x4b0c54]
00421731  add        ecx, ebx
00421733  call       0x454b80

// block 00421738
00421738  pop        edi
00421739  pop        esi
0042173a  pop        ebx
0042173b  ret        4
