
// block 004356d0
004356d0  xor        eax, eax
004356d2  mov        ecx, edx

// block 004356d4
004356d4  cmp        dword ptr [ecx], 0
004356d7  jl         0x4356e7

// block 004356d9
004356d9  inc        eax
004356da  add        ecx, 0x28
004356dd  cmp        eax, 4
004356e0  jb         0x4356d4

// block 004356e2
004356e2  xor        eax, eax
004356e4  ret        0x18

// block 004356e7
004356e7  mov        ecx, dword ptr [esp + 4]
004356eb  lea        eax, [eax + eax*4]
004356ee  lea        eax, [edx + eax*8]
004356f1  mov        edx, dword ptr [esp + 8]
004356f5  mov        dword ptr [eax], ecx
004356f7  mov        ecx, dword ptr [esp + 0xc]
004356fb  mov        dword ptr [eax + 4], edx
004356fe  mov        edx, dword ptr [esp + 0x10]
00435702  mov        dword ptr [eax + 0x18], ecx
00435705  mov        ecx, dword ptr [esp + 0x14]
00435709  mov        dword ptr [eax + 0x1c], edx
0043570c  mov        edx, dword ptr [esp + 0x18]
00435710  mov        dword ptr [eax + 8], 0x20
00435717  mov        dword ptr [eax + 0xc], 0x10
0043571e  mov        dword ptr [eax + 0x10], 0x180
00435725  mov        dword ptr [eax + 0x14], 0x1c0
0043572c  mov        dword ptr [eax + 0x20], ecx
0043572f  mov        dword ptr [eax + 0x24], edx
00435732  xor        eax, eax
00435734  ret        0x18
