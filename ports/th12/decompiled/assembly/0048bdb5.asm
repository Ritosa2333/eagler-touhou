
// block 0048bdb5
0048bdb5  mov        edi, edi
0048bdb7  push       ebp
0048bdb8  mov        ebp, esp
0048bdba  sub        esp, 0x10
0048bdbd  push       dword ptr [ebp + 0xc]
0048bdc0  lea        ecx, [ebp - 0x10]
0048bdc3  call       0x46d3b4

// block 0048bdc8
0048bdc8  mov        eax, dword ptr [ebp - 0x10]
0048bdcb  cmp        dword ptr [eax + 0xac], 1
0048bdd2  jle        0x48bde7

// block 0048bdd4
0048bdd4  lea        eax, [ebp - 0x10]
0048bdd7  push       eax
0048bdd8  push       0x20
0048bdda  push       dword ptr [ebp + 8]
0048bddd  call       0x4758eb

// block 0048bde2
0048bde2  add        esp, 0xc
0048bde5  jmp        0x48bdf7

// block 0048bde7
0048bde7  mov        eax, dword ptr [eax + 0xc8]
0048bded  mov        ecx, dword ptr [ebp + 8]
0048bdf0  movzx      eax, word ptr [eax + ecx*2]
0048bdf4  and        eax, 0x20

// block 0048bdf7
0048bdf7  cmp        byte ptr [ebp - 4], 0
0048bdfb  je         0x48be04

// block 0048bdfd
0048bdfd  mov        ecx, dword ptr [ebp - 8]
0048be00  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048be04
0048be04  leave      
0048be05  ret        
