
// block 00464900
00464900  mov        ecx, dword ptr [eax + 0x8c]
00464906  mov        edx, dword ptr [eax]
00464908  mov        dword ptr [eax + ecx*4 + 0xc], edx
0046490c  mov        ecx, dword ptr [eax + 0x8c]
00464912  mov        edx, dword ptr [eax + 8]
00464915  mov        dword ptr [eax + ecx*4 + 0x4c], edx
00464919  inc        dword ptr [eax + 0x8c]
0046491f  cmp        dword ptr [eax + 0x8c], 0x10
00464926  mov        dword ptr [eax + 0xd4], 0
00464930  jl         0x46493c

// block 00464932
00464932  mov        dword ptr [eax + 0x8c], 0xf

// block 0046493c
0046493c  ret        
