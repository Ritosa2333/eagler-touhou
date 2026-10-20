
// block 0044dc20
0044dc20  mov        eax, dword ptr [0x4b0e58]
0044dc25  mov        ecx, dword ptr [0x4b0e48]
0044dc2b  imul       eax, dword ptr [esp + 4]
0044dc30  sub        ecx, 0x15
0044dc33  mov        edx, dword ptr [0x4b0e68]
0044dc39  push       esi
0044dc3a  je         0x44dca2

// block 0044dc3c
0044dc3c  sub        ecx, 4
0044dc3f  je         0x44dc66

// block 0044dc41
0044dc41  sub        ecx, 1
0044dc44  je         0x44dc4c

// block 0044dc46
0044dc46  xor        al, al
0044dc48  pop        esi
0044dc49  ret        4

// block 0044dc4c
0044dc4c  mov        ecx, 1
0044dc51  cmp        eax, ecx
0044dc53  jle        0x44dcbf

// block 0044dc55
0044dc55  xor        byte ptr [ecx + edx], 0xf0
0044dc59  add        ecx, 2
0044dc5c  cmp        ecx, eax
0044dc5e  jl         0x44dc55

// block 0044dc60
0044dc60  mov        al, 1
0044dc62  pop        esi
0044dc63  ret        4

// block 0044dc66
0044dc66  test       eax, eax
0044dc68  jle        0x44dcbf

// block 0044dc6a
0044dc6a  lea        esi, [eax - 1]
0044dc6d  shr        esi, 1
0044dc6f  inc        esi

// block 0044dc70
0044dc70  movzx      ecx, word ptr [edx]
0044dc73  mov        eax, ecx
0044dc75  not        eax
0044dc77  xor        eax, ecx
0044dc79  and        eax, 0x7fff
0044dc7e  not        ecx
0044dc80  xor        eax, ecx
0044dc82  mov        word ptr [edx], ax
0044dc85  test       eax, 0x8000
0044dc8a  jne        0x44dc94

// block 0044dc8c
0044dc8c  and        eax, 0x8000
0044dc91  mov        word ptr [edx], ax

// block 0044dc94
0044dc94  add        edx, 2
0044dc97  sub        esi, 1
0044dc9a  jne        0x44dc70

// block 0044dc9c
0044dc9c  mov        al, 1
0044dc9e  pop        esi
0044dc9f  ret        4

// block 0044dca2
0044dca2  mov        esi, 3
0044dca7  cmp        eax, esi
0044dca9  jle        0x44dcbf

// block 0044dcab
0044dcab  jmp        0x44dcb0

// block 0044dcb0
0044dcb0  mov        cl, byte ptr [esi + edx]
0044dcb3  not        cl
0044dcb5  mov        byte ptr [esi + edx], cl
0044dcb8  add        esi, 4
0044dcbb  cmp        esi, eax
0044dcbd  jl         0x44dcb0

// block 0044dcbf
0044dcbf  mov        al, 1
0044dcc1  pop        esi
0044dcc2  ret        4
