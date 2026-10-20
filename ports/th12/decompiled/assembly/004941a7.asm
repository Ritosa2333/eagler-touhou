
// block 004941a7
004941a7  cmp        byte ptr [edx + 0xe], 5
004941ab  jne        0x4941be

// block 004941ad
004941ad  mov        bx, word ptr [ebp - 0xa4]
004941b4  or         bh, 2
004941b7  and        bh, 0xfe
004941ba  mov        bl, 0x3f
004941bc  jmp        0x4941c2

// block 004941be
004941be  mov        bx, 0x133f

// block 004941c2
004941c2  mov        word ptr [ebp - 0xa2], bx
004941c9  fldcw      word ptr [ebp - 0xa2]
004941cf  mov        ebx, 0x4b35dc
004941d4  fxam       
004941d6  mov        dword ptr [ebp - 0x94], edx
004941dc  wait       
004941dd  fnstsw     word ptr [ebp - 0xa0]
004941e3  mov        byte ptr [ebp - 0x90], 0
004941ea  fxch       st(1)
004941ec  mov        cl, byte ptr [ebp - 0x9f]
004941f2  fxam       
004941f4  wait       
004941f5  fnstsw     word ptr [ebp - 0xa0]
004941fb  fxch       st(1)
004941fd  mov        ch, byte ptr [ebp - 0x9f]
00494203  shl        ch, 1
00494205  sar        ch, 1
00494207  rol        ch, 1
00494209  mov        al, ch
0049420b  and        al, 0xf
0049420d  xlatb      
0049420e  mov        ah, al
00494210  shl        cl, 1
00494212  sar        cl, 1
00494214  rol        cl, 1
00494216  mov        al, cl
00494218  and        al, 0xf
0049421a  xlatb      
0049421b  shl        ah, 1
0049421d  shl        ah, 1
0049421f  or         al, ah
00494221  movsx      eax, al
00494224  and        ecx, 0x404
0049422a  mov        ebx, edx
0049422c  add        ebx, eax
0049422e  add        ebx, 0x10
00494231  jmp        dword ptr [ebx]
