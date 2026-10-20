
// block 0044c130
0044c130  sub        esp, 0x108
0044c136  mov        eax, dword ptr [0x4ad138]
0044c13b  xor        eax, esp
0044c13d  mov        dword ptr [esp + 0x104], eax
0044c144  push       ebx
0044c145  push       esi
0044c146  push       edi
0044c147  mov        esi, ecx
0044c149  push       0x2f
0044c14b  push       esi
0044c14c  call       0x46d1a0

// block 0044c151
0044c151  add        esp, 8
0044c154  test       eax, eax
0044c156  je         0x44c15b

// block 0044c158
0044c158  inc        eax
0044c159  jmp        0x44c15d

// block 0044c15b
0044c15b  mov        eax, esi

// block 0044c15d
0044c15d  sub        eax, esi
0044c15f  dec        eax
0044c160  lea        edx, [esp + 0xc]
0044c164  mov        edi, eax
0044c166  mov        eax, esi
0044c168  sub        edx, esi
0044c16a  lea        ebx, [ebx]

// block 0044c170
0044c170  mov        cl, byte ptr [eax]
0044c172  mov        byte ptr [edx + eax], cl
0044c175  inc        eax
0044c176  test       cl, cl
0044c178  jne        0x44c170

// block 0044c17a
0044c17a  test       edi, edi
0044c17c  jl         0x44c193

// block 0044c17e
0044c17e  mov        ecx, dword ptr [0x4a2324]
0044c184  mov        dl, byte ptr [0x4a2328]
0044c18a  lea        eax, [esp + edi + 0xc]
0044c18e  mov        dword ptr [eax], ecx
0044c190  mov        byte ptr [eax + 4], dl

// block 0044c193
0044c193  mov        edi, dword ptr [0x4b4538]
0044c199  xor        esi, esi
0044c19b  mov        eax, 0x4cf2b0
0044c1a0  test       edi, edi
0044c1a2  jle        0x44c1e1

// block 0044c1a4
0044c1a4  mov        ecx, dword ptr [eax + 8]
0044c1a7  lea        edx, [esp + 0xc]
0044c1ab  jmp        0x44c1b0

// block 0044c1b0
0044c1b0  mov        bl, byte ptr [ecx]
0044c1b2  cmp        bl, byte ptr [edx]
0044c1b4  jne        0x44c1d0

// block 0044c1b6
0044c1b6  test       bl, bl
0044c1b8  je         0x44c1cc

// block 0044c1ba
0044c1ba  mov        bl, byte ptr [ecx + 1]
0044c1bd  cmp        bl, byte ptr [edx + 1]
0044c1c0  jne        0x44c1d0

// block 0044c1c2
0044c1c2  add        ecx, 2
0044c1c5  add        edx, 2
0044c1c8  test       bl, bl
0044c1ca  jne        0x44c1b0

// block 0044c1cc
0044c1cc  xor        ecx, ecx
0044c1ce  jmp        0x44c1d5

// block 0044c1d0
0044c1d0  sbb        ecx, ecx
0044c1d2  sbb        ecx, -1

// block 0044c1d5
0044c1d5  test       ecx, ecx
0044c1d7  je         0x44c1e3

// block 0044c1d9
0044c1d9  inc        esi
0044c1da  add        eax, 0x10
0044c1dd  cmp        esi, edi
0044c1df  jl         0x44c1a4

// block 0044c1e1
0044c1e1  xor        eax, eax

// block 0044c1e3
0044c1e3  mov        ecx, dword ptr [esp + 0x110]
0044c1ea  pop        edi
0044c1eb  pop        esi
0044c1ec  pop        ebx
0044c1ed  xor        ecx, esp
0044c1ef  call       0x46c932

// block 0044c1f4
0044c1f4  add        esp, 0x108
0044c1fa  ret        
