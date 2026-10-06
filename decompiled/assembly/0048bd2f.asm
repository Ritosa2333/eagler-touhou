
// block 0048bd2f
0048bd2f  mov        edi, edi
0048bd31  push       ebp
0048bd32  mov        ebp, esp
0048bd34  sub        esp, 0x10
0048bd37  push       dword ptr [ebp + 0xc]
0048bd3a  lea        ecx, [ebp - 0x10]
0048bd3d  call       0x46d3b4

// block 0048bd42
0048bd42  mov        eax, dword ptr [ebp - 0x10]
0048bd45  cmp        dword ptr [eax + 0xac], 1
0048bd4c  jle        0x48bd64

// block 0048bd4e
0048bd4e  lea        eax, [ebp - 0x10]
0048bd51  push       eax
0048bd52  push       0x117
0048bd57  push       dword ptr [ebp + 8]
0048bd5a  call       0x4758eb

// block 0048bd5f
0048bd5f  add        esp, 0xc
0048bd62  jmp        0x48bd76

// block 0048bd64
0048bd64  mov        eax, dword ptr [eax + 0xc8]
0048bd6a  mov        ecx, dword ptr [ebp + 8]
0048bd6d  movzx      eax, word ptr [eax + ecx*2]
0048bd71  and        eax, 0x117

// block 0048bd76
0048bd76  cmp        byte ptr [ebp - 4], 0
0048bd7a  je         0x48bd83

// block 0048bd7c
0048bd7c  mov        ecx, dword ptr [ebp - 8]
0048bd7f  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048bd83
0048bd83  leave      
0048bd84  ret        
