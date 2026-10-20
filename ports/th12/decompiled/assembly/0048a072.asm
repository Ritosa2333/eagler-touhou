
// block 0048a072
0048a072  mov        edi, edi
0048a074  push       ebp
0048a075  mov        ebp, esp
0048a077  sub        esp, 0x14
0048a07a  mov        ecx, dword ptr [ebp + 8]
0048a07d  movzx      edx, word ptr [ecx + 0xa]
0048a081  and        dword ptr [ebp - 8], 0
0048a085  push       ebx
0048a086  mov        ebx, dword ptr [ecx + 6]
0048a089  push       esi
0048a08a  mov        esi, dword ptr [ecx + 2]
0048a08d  movzx      ecx, word ptr [ecx]
0048a090  push       edi
0048a091  mov        edi, edx
0048a093  and        edx, 0x8000
0048a099  mov        eax, edx
0048a09b  shl        ecx, 0x10
0048a09e  mov        edx, 0x80000000
0048a0a3  and        edi, 0x7fff
0048a0a9  mov        dword ptr [ebp - 0x14], ebx
0048a0ac  mov        dword ptr [ebp - 0xc], ecx
0048a0af  test       edx, ecx
0048a0b1  je         0x48a112

// block 0048a0b3
0048a0b3  test       ecx, 0x7fffffff
0048a0b9  je         0x48a112

// block 0048a0bb
0048a0bb  lea        ecx, [esi + 1]
0048a0be  xor        ebx, ebx
0048a0c0  cmp        ecx, esi
0048a0c2  jb         0x48a0c9

// block 0048a0c4
0048a0c4  cmp        ecx, 1
0048a0c7  jae        0x48a0cc

// block 0048a0c9
0048a0c9  xor        ebx, ebx
0048a0cb  inc        ebx

// block 0048a0cc
0048a0cc  and        dword ptr [ebp + 8], 0
0048a0d0  mov        dword ptr [ebp - 0x10], ecx
0048a0d3  mov        ecx, ebx

// block 0048a0d5
0048a0d5  test       ecx, ecx
0048a0d7  je         0x48a10c

// block 0048a0d9
0048a0d9  mov        ecx, dword ptr [ebp + 8]
0048a0dc  and        dword ptr [ebp - 4], 0
0048a0e0  lea        ecx, [ebp + ecx*4 - 0x14]
0048a0e4  mov        esi, dword ptr [ecx]
0048a0e6  lea        ebx, [esi + 1]
0048a0e9  cmp        ebx, esi
0048a0eb  jb         0x48a0f2

// block 0048a0ed
0048a0ed  cmp        ebx, 1
0048a0f0  jae        0x48a0f9

// block 0048a0f2
0048a0f2  mov        dword ptr [ebp - 4], 1

// block 0048a0f9
0048a0f9  dec        dword ptr [ebp + 8]
0048a0fc  mov        dword ptr [ecx], ebx
0048a0fe  mov        ecx, dword ptr [ebp - 4]
0048a101  jns        0x48a0d5

// block 0048a103
0048a103  test       ecx, ecx
0048a105  je         0x48a10c

// block 0048a107
0048a107  mov        ebx, edx
0048a109  inc        edi
0048a10a  jmp        0x48a10f

// block 0048a10c
0048a10c  mov        ebx, dword ptr [ebp - 0x14]

// block 0048a10f
0048a10f  mov        esi, dword ptr [ebp - 0x10]

// block 0048a112
0048a112  mov        ecx, 0x7fff
0048a117  cmp        di, cx
0048a11a  jne        0x48a123

// block 0048a11c
0048a11c  mov        dword ptr [ebp - 8], 1

// block 0048a123
0048a123  mov        ecx, dword ptr [ebp + 0xc]
0048a126  or         eax, edi
0048a128  pop        edi
0048a129  mov        dword ptr [ecx], esi
0048a12b  pop        esi
0048a12c  mov        dword ptr [ecx + 4], ebx
0048a12f  mov        word ptr [ecx + 8], ax
0048a133  mov        eax, dword ptr [ebp - 8]
0048a136  pop        ebx
0048a137  leave      
0048a138  ret        
