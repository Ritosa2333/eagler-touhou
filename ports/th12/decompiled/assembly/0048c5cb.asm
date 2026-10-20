
// block 0048c5cb
0048c5cb  mov        edi, edi
0048c5cd  push       ebp
0048c5ce  mov        ebp, esp
0048c5d0  sub        esp, 0x10
0048c5d3  push       dword ptr [ebp + 8]
0048c5d6  lea        ecx, [ebp - 0x10]
0048c5d9  call       0x46d3b4

// block 0048c5de
0048c5de  movzx      eax, byte ptr [ebp + 0xc]
0048c5e2  mov        ecx, dword ptr [ebp - 0xc]
0048c5e5  mov        dl, byte ptr [ebp + 0x14]
0048c5e8  test       byte ptr [ecx + eax + 0x1d], dl
0048c5ec  jne        0x48c60c

// block 0048c5ee
0048c5ee  cmp        dword ptr [ebp + 0x10], 0
0048c5f2  je         0x48c606

// block 0048c5f4
0048c5f4  mov        ecx, dword ptr [ebp - 0x10]
0048c5f7  mov        ecx, dword ptr [ecx + 0xc8]
0048c5fd  movzx      eax, word ptr [ecx + eax*2]
0048c601  and        eax, dword ptr [ebp + 0x10]
0048c604  jmp        0x48c608

// block 0048c606
0048c606  xor        eax, eax

// block 0048c608
0048c608  test       eax, eax
0048c60a  je         0x48c60f

// block 0048c60c
0048c60c  xor        eax, eax
0048c60e  inc        eax

// block 0048c60f
0048c60f  cmp        byte ptr [ebp - 4], 0
0048c613  je         0x48c61c

// block 0048c615
0048c615  mov        ecx, dword ptr [ebp - 8]
0048c618  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048c61c
0048c61c  leave      
0048c61d  ret        
