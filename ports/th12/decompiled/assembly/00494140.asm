
// block 00494140
00494140  cmp        byte ptr [edx + 0xe], 5
00494144  jne        0x494157

// block 00494146
00494146  mov        bx, word ptr [ebp - 0xa4]
0049414d  or         bh, 2
00494150  and        bh, 0xfe
00494153  mov        bl, 0x3f
00494155  jmp        0x49415b

// block 00494157
00494157  mov        bx, 0x133f

// block 0049415b
0049415b  mov        word ptr [ebp - 0xa2], bx
00494162  fldcw      word ptr [ebp - 0xa2]
00494168  mov        ebx, 0x4b35dc
0049416d  fxam       
0049416f  mov        dword ptr [ebp - 0x94], edx
00494175  wait       
00494176  fnstsw     word ptr [ebp - 0xa0]
0049417c  mov        byte ptr [ebp - 0x90], 0
00494183  wait       
00494184  mov        cl, byte ptr [ebp - 0x9f]
0049418a  shl        cl, 1
0049418c  sar        cl, 1
0049418e  rol        cl, 1
00494190  mov        al, cl
00494192  and        al, 0xf
00494194  xlatb      
00494195  movsx      eax, al
00494198  and        ecx, 0x404
0049419e  mov        ebx, edx
004941a0  add        ebx, eax
004941a2  add        ebx, 0x10
004941a5  jmp        dword ptr [ebx]
