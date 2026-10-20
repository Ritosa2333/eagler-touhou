
// block 0048ba9f
0048ba9f  mov        edi, edi
0048baa1  push       ebp
0048baa2  mov        ebp, esp
0048baa4  sub        esp, 0x10
0048baa7  push       dword ptr [ebp + 0xc]
0048baaa  lea        ecx, [ebp - 0x10]
0048baad  call       0x46d3b4

// block 0048bab2
0048bab2  mov        eax, dword ptr [ebp - 0x10]
0048bab5  cmp        dword ptr [eax + 0xac], 1
0048babc  jle        0x48bad4

// block 0048babe
0048babe  lea        eax, [ebp - 0x10]
0048bac1  push       eax
0048bac2  push       0x80
0048bac7  push       dword ptr [ebp + 8]
0048baca  call       0x4758eb

// block 0048bacf
0048bacf  add        esp, 0xc
0048bad2  jmp        0x48bae6

// block 0048bad4
0048bad4  mov        eax, dword ptr [eax + 0xc8]
0048bada  mov        ecx, dword ptr [ebp + 8]
0048badd  movzx      eax, word ptr [eax + ecx*2]
0048bae1  and        eax, 0x80

// block 0048bae6
0048bae6  cmp        byte ptr [ebp - 4], 0
0048baea  je         0x48baf3

// block 0048baec
0048baec  mov        ecx, dword ptr [ebp - 8]
0048baef  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048baf3
0048baf3  leave      
0048baf4  ret        
