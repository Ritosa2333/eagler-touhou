
// block 0048d5ae
0048d5ae  mov        edi, edi
0048d5b0  push       ebp
0048d5b1  mov        ebp, esp
0048d5b3  push       ecx
0048d5b4  mov        eax, 0xffff
0048d5b9  cmp        word ptr [ebp + 8], ax
0048d5bd  jne        0x48d5c3

// block 0048d5bf
0048d5bf  xor        eax, eax
0048d5c1  leave      
0048d5c2  ret        

// block 0048d5c3
0048d5c3  mov        eax, 0x100
0048d5c8  cmp        word ptr [ebp + 8], ax
0048d5cc  jae        0x48d5e4

// block 0048d5ce
0048d5ce  movzx      eax, word ptr [ebp + 8]
0048d5d2  mov        ecx, dword ptr [0x4adea4]
0048d5d8  movzx      eax, word ptr [ecx + eax*2]
0048d5dc  movzx      ecx, word ptr [ebp + 0xc]
0048d5e0  and        eax, ecx
0048d5e2  leave      
0048d5e3  ret        

// block 0048d5e4
0048d5e4  cmp        dword ptr [0x4b40dc], 0
0048d5eb  jne        0x48d612

// block 0048d5ed
0048d5ed  push       dword ptr [0x4ad9f4]
0048d5f3  lea        eax, [ebp - 4]
0048d5f6  push       dword ptr [0x4ad9e4]
0048d5fc  push       eax
0048d5fd  push       1
0048d5ff  lea        eax, [ebp + 8]
0048d602  push       eax
0048d603  push       1
0048d605  push       0x4adac0
0048d60a  call       0x48ed72

// block 0048d60f
0048d60f  add        esp, 0x1c

// block 0048d612
0048d612  push       0
0048d614  push       dword ptr [ebp + 0xc]
0048d617  push       dword ptr [ebp + 8]
0048d61a  call       0x48d524

// block 0048d61f
0048d61f  add        esp, 0xc
0048d622  leave      
0048d623  ret        
