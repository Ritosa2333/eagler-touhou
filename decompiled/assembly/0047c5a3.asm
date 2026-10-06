
// block 0047c5a3
0047c5a3  mov        edi, edi
0047c5a5  push       ebp
0047c5a6  mov        ebp, esp
0047c5a8  sub        esp, 0x10
0047c5ab  push       dword ptr [ebp + 0xc]
0047c5ae  lea        ecx, [ebp - 0x10]
0047c5b1  call       0x46d3b4

// block 0047c5b6
0047c5b6  movzx      eax, byte ptr [ebp + 8]
0047c5ba  mov        ecx, dword ptr [ebp - 0x10]
0047c5bd  mov        ecx, dword ptr [ecx + 0xc8]
0047c5c3  movzx      eax, word ptr [ecx + eax*2]
0047c5c7  and        eax, 0x8000
0047c5cc  cmp        byte ptr [ebp - 4], 0
0047c5d0  je         0x47c5d9

// block 0047c5d2
0047c5d2  mov        ecx, dword ptr [ebp - 8]
0047c5d5  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0047c5d9
0047c5d9  leave      
0047c5da  ret        
