
// block 0048b89c
0048b89c  mov        edi, edi
0048b89e  push       ebp
0048b89f  mov        ebp, esp
0048b8a1  sub        esp, 0x10
0048b8a4  push       dword ptr [ebp + 0xc]
0048b8a7  lea        ecx, [ebp - 0x10]
0048b8aa  call       0x46d3b4

// block 0048b8af
0048b8af  mov        eax, dword ptr [ebp - 0x10]
0048b8b2  cmp        dword ptr [eax + 0xac], 1
0048b8b9  jle        0x48b8d1

// block 0048b8bb
0048b8bb  lea        eax, [ebp - 0x10]
0048b8be  push       eax
0048b8bf  push       0x103
0048b8c4  push       dword ptr [ebp + 8]
0048b8c7  call       0x4758eb

// block 0048b8cc
0048b8cc  add        esp, 0xc
0048b8cf  jmp        0x48b8e3

// block 0048b8d1
0048b8d1  mov        eax, dword ptr [eax + 0xc8]
0048b8d7  mov        ecx, dword ptr [ebp + 8]
0048b8da  movzx      eax, word ptr [eax + ecx*2]
0048b8de  and        eax, 0x103

// block 0048b8e3
0048b8e3  cmp        byte ptr [ebp - 4], 0
0048b8e7  je         0x48b8f0

// block 0048b8e9
0048b8e9  mov        ecx, dword ptr [ebp - 8]
0048b8ec  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048b8f0
0048b8f0  leave      
0048b8f1  ret        
