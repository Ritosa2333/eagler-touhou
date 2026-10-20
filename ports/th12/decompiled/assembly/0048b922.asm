
// block 0048b922
0048b922  mov        edi, edi
0048b924  push       ebp
0048b925  mov        ebp, esp
0048b927  sub        esp, 0x10
0048b92a  push       dword ptr [ebp + 0xc]
0048b92d  lea        ecx, [ebp - 0x10]
0048b930  call       0x46d3b4

// block 0048b935
0048b935  mov        eax, dword ptr [ebp - 0x10]
0048b938  cmp        dword ptr [eax + 0xac], 1
0048b93f  jle        0x48b954

// block 0048b941
0048b941  lea        eax, [ebp - 0x10]
0048b944  push       eax
0048b945  push       1
0048b947  push       dword ptr [ebp + 8]
0048b94a  call       0x4758eb

// block 0048b94f
0048b94f  add        esp, 0xc
0048b952  jmp        0x48b964

// block 0048b954
0048b954  mov        eax, dword ptr [eax + 0xc8]
0048b95a  mov        ecx, dword ptr [ebp + 8]
0048b95d  movzx      eax, word ptr [eax + ecx*2]
0048b961  and        eax, 1

// block 0048b964
0048b964  cmp        byte ptr [ebp - 4], 0
0048b968  je         0x48b971

// block 0048b96a
0048b96a  mov        ecx, dword ptr [ebp - 8]
0048b96d  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048b971
0048b971  leave      
0048b972  ret        
