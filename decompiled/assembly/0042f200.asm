
// block 0042f200
0042f200  sub        esp, 0x10
0042f203  cmp        dword ptr [0x4cea94], 0
0042f20a  je         0x42f284

// block 0042f20c
0042f20c  push       esi
0042f20d  mov        esi, dword ptr [0x4ce8cc]
0042f213  call       0x45a3c0

// block 0042f218
0042f218  mov        edx, dword ptr [0x4cea98]
0042f21e  mov        eax, dword ptr [0x4ce8f0]
0042f223  mov        ecx, dword ptr [eax]
0042f225  push       edx
0042f226  push       0
0042f228  push       eax
0042f229  mov        eax, dword ptr [ecx + 0x94]
0042f22f  call       eax

// block 0042f231
0042f231  fld1       
0042f233  mov        eax, dword ptr [0x4cede8]
0042f238  mov        edx, dword ptr [0x4cedf0]
0042f23e  mov        ecx, dword ptr [0x4cedec]
0042f244  add        edx, eax
0042f246  mov        dword ptr [esp + 4], eax
0042f24a  mov        eax, dword ptr [0x4cedf4]
0042f24f  add        eax, ecx
0042f251  mov        dword ptr [esp + 0x10], eax
0042f255  mov        eax, dword ptr [0x4ce8f0]
0042f25a  push       0
0042f25c  mov        dword ptr [esp + 0x10], edx
0042f260  mov        edx, dword ptr [0x4cf2a8]
0042f266  mov        dword ptr [esp + 0xc], ecx
0042f26a  mov        ecx, dword ptr [eax]
0042f26c  push       ecx
0042f26d  fstp       dword ptr [esp]
0042f270  push       edx
0042f271  push       3
0042f273  lea        edx, [esp + 0x14]
0042f277  push       edx
0042f278  push       1
0042f27a  push       eax
0042f27b  mov        eax, dword ptr [ecx + 0xac]
0042f281  call       eax

// block 0042f283
0042f283  pop        esi

// block 0042f284
0042f284  mov        eax, 1
0042f289  add        esp, 0x10
0042f28c  ret        
