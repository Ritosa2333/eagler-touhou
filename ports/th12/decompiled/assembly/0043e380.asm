
// block 0043e380
0043e380  cmp        dword ptr [esi + 0x34], 0
0043e384  je         0x43e3d2

// block 0043e386
0043e386  push       edi
0043e387  xor        edi, edi
0043e389  cmp        dword ptr [esi + 0x38], edi
0043e38c  jle        0x43e3b3

// block 0043e38e
0043e38e  mov        edi, edi

// block 0043e390
0043e390  mov        eax, dword ptr [esi + 0x34]
0043e393  mov        eax, dword ptr [eax + edi*4]
0043e396  test       eax, eax
0043e398  je         0x43e3ad

// block 0043e39a
0043e39a  push       eax
0043e39b  call       0x46c941

// block 0043e3a0
0043e3a0  mov        ecx, dword ptr [esi + 0x34]
0043e3a3  add        esp, 4
0043e3a6  mov        dword ptr [ecx + edi*4], 0

// block 0043e3ad
0043e3ad  inc        edi
0043e3ae  cmp        edi, dword ptr [esi + 0x38]
0043e3b1  jl         0x43e390

// block 0043e3b3
0043e3b3  mov        eax, dword ptr [esi + 0x34]
0043e3b6  pop        edi
0043e3b7  test       eax, eax
0043e3b9  je         0x43e3cb

// block 0043e3bb
0043e3bb  push       eax
0043e3bc  call       0x46c941

// block 0043e3c1
0043e3c1  add        esp, 4
0043e3c4  mov        dword ptr [esi + 0x34], 0

// block 0043e3cb
0043e3cb  mov        dword ptr [esi + 0x34], 0

// block 0043e3d2
0043e3d2  ret        
