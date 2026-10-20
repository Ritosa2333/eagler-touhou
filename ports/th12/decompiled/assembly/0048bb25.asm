
// block 0048bb25
0048bb25  mov        edi, edi
0048bb27  push       ebp
0048bb28  mov        ebp, esp
0048bb2a  sub        esp, 0x10
0048bb2d  push       dword ptr [ebp + 0xc]
0048bb30  lea        ecx, [ebp - 0x10]
0048bb33  call       0x46d3b4

// block 0048bb38
0048bb38  mov        eax, dword ptr [ebp - 0x10]
0048bb3b  cmp        dword ptr [eax + 0xac], 1
0048bb42  jle        0x48bb57

// block 0048bb44
0048bb44  lea        eax, [ebp - 0x10]
0048bb47  push       eax
0048bb48  push       8
0048bb4a  push       dword ptr [ebp + 8]
0048bb4d  call       0x4758eb

// block 0048bb52
0048bb52  add        esp, 0xc
0048bb55  jmp        0x48bb67

// block 0048bb57
0048bb57  mov        eax, dword ptr [eax + 0xc8]
0048bb5d  mov        ecx, dword ptr [ebp + 8]
0048bb60  movzx      eax, word ptr [eax + ecx*2]
0048bb64  and        eax, 8

// block 0048bb67
0048bb67  cmp        byte ptr [ebp - 4], 0
0048bb6b  je         0x48bb74

// block 0048bb6d
0048bb6d  mov        ecx, dword ptr [ebp - 8]
0048bb70  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048bb74
0048bb74  leave      
0048bb75  ret        
