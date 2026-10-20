
// block 00494104
00494104  push       ebp
00494105  mov        ebp, esp
00494107  add        esp, -0xc
0049410a  push       ebx
0049410b  mov        ax, word ptr [ebp + 0xe]
0049410f  mov        bx, ax
00494112  and        ax, 0x7ff0
00494116  cmp        ax, 0x7ff0
0049411a  jne        0x49413a

// block 0049411c
0049411c  or         bx, 0x7fff
00494121  mov        word ptr [ebp - 2], bx
00494125  mov        eax, dword ptr [ebp + 0xc]
00494128  mov        ebx, dword ptr [ebp + 8]
0049412b  shld       eax, ebx, 0xb
0049412f  mov        dword ptr [ebp - 6], eax
00494132  mov        dword ptr [ebp - 0xa], ebx
00494135  fld        xword ptr [ebp - 0xa]
00494138  jmp        0x49413d

// block 0049413a
0049413a  fld        qword ptr [ebp + 8]

// block 0049413d
0049413d  pop        ebx
0049413e  leave      
0049413f  ret        
