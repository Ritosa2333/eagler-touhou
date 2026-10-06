
// block 0042e8b0
0042e8b0  push       edi
0042e8b1  xor        edi, edi
0042e8b3  lea        ecx, [esi + 0x30]
0042e8b6  mov        dword ptr [esi + 0x10], 0x4a3738
0042e8bd  mov        dword ptr [esi + 0x14], edi
0042e8c0  mov        dword ptr [esi + 0x18], edi
0042e8c3  mov        dword ptr [esi + 0x1c], edi
0042e8c6  mov        dword ptr [esi + 0x20], edi
0042e8c9  call       0x4027e0

// block 0042e8ce
0042e8ce  push       0x4f8
0042e8d3  push       edi
0042e8d4  push       esi
0042e8d5  call       0x477420

// block 0042e8da
0042e8da  or         dword ptr [esi], 2
0042e8dd  add        esp, 0xc
0042e8e0  mov        dword ptr [0x4b44f8], esi
0042e8e6  mov        eax, esi
0042e8e8  pop        edi
0042e8e9  ret        
