
// block 0041a6a0
0041a6a0  mov        eax, edx
0041a6a2  shl        eax, 4
0041a6a5  add        eax, esi
0041a6a7  mov        dword ptr [eax + 0x270c], ecx
0041a6ad  test       ecx, ecx
0041a6af  jl         0x41a6d5

// block 0041a6b1
0041a6b1  mov        ecx, dword ptr [esp + 4]
0041a6b5  add        edx, 0x271
0041a6bb  shl        edx, 4
0041a6be  mov        dword ptr [edx + esi], ecx
0041a6c1  mov        edx, dword ptr [esp + 8]
0041a6c5  mov        ecx, dword ptr [esp + 0xc]
0041a6c9  mov        dword ptr [eax + 0x2714], edx
0041a6cf  mov        dword ptr [eax + 0x2718], ecx

// block 0041a6d5
0041a6d5  ret        0xc
