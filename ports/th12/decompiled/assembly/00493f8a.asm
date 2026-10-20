
// block 00493f8a
00493f8a  cmp        dword ptr [0x4b4110], 0
00493f91  jne        0x493fe2

// block 00493f93
00493f93  fst        qword ptr [ebp - 0x2d0]
00493f99  mov        al, byte ptr [ebp - 0x90]
00493f9f  or         al, al
00493fa1  je         0x493fbd

// block 00493fa3
00493fa3  cmp        al, 0xff
00493fa5  je         0x494002

// block 00493fa7
00493fa7  cmp        al, 0xfe
00493fa9  je         0x493fea

// block 00493fab
00493fab  or         al, al
00493fad  je         0x493fe2

// block 00493faf
00493faf  movsx      eax, al
00493fb2  mov        dword ptr [ebp - 0x8e], eax
00493fb8  jmp        0x49406f

// block 00493fbd
00493fbd  mov        ax, word ptr [ebp - 0xa4]
00493fc4  and        ax, 0x20
00493fc8  jne        0x493fe2

// block 00493fca
00493fca  wait       
00493fcb  fnstsw     ax
00493fcd  and        ax, 0x20
00493fd1  je         0x493fe2

// block 00493fd3
00493fd3  mov        dword ptr [ebp - 0x8e], 8
00493fdd  jmp        0x49406f

// block 00493fe2
00493fe2  fldcw      word ptr [ebp - 0xa4]
00493fe8  wait       
00493fe9  ret        

// block 00493fea
00493fea  mov        ax, word ptr [ebp - 0x2ca]
00493ff1  and        ax, 0x7ff0
00493ff5  or         ax, ax
00493ff8  je         0x494015

// block 00493ffa
00493ffa  cmp        ax, 0x7ff0
00493ffe  je         0x494043

// block 00494000
00494000  jmp        0x493fbd

// block 00494002
00494002  mov        ax, word ptr [ebp - 0x2ca]
00494009  and        ax, 0x7ff0
0049400d  cmp        ax, 0x7ff0
00494011  je         0x494043

// block 00494013
00494013  jmp        0x493fbd

// block 00494015
00494015  mov        dword ptr [ebp - 0x8e], 4
0049401f  fld        qword ptr [0x4a4598]
00494025  fxch       st(1)
00494027  fscale     
00494029  fstp       st(1)
0049402b  fld        st(0)
0049402d  fabs       
0049402f  fcomp      qword ptr [0x4a4588]
00494035  wait       
00494036  fnstsw     ax
00494038  sahf       
00494039  jae        0x49406f

// block 0049403b
0049403b  fmul       qword ptr [0x4a45a8]
00494041  jmp        0x49406f

// block 00494043
00494043  mov        dword ptr [ebp - 0x8e], 3
0049404d  fld        qword ptr [0x4a4590]
00494053  fxch       st(1)
00494055  fscale     
00494057  fstp       st(1)
00494059  fld        st(0)
0049405b  fabs       
0049405d  fcomp      qword ptr [0x4a4580]
00494063  wait       
00494064  fnstsw     ax
00494066  sahf       
00494067  jbe        0x49406f

// block 00494069
00494069  fmul       qword ptr [0x4a45a0]

// block 0049406f
0049406f  push       esi
00494070  push       edi
00494071  mov        ebx, dword ptr [ebp - 0x94]
00494077  inc        ebx
00494078  mov        dword ptr [ebp - 0x8a], ebx
0049407e  test       byte ptr [ebp - 0x2c8], 1
00494085  jne        0x4940a1

// block 00494087
00494087  cld        
00494088  lea        esi, [ebp + 8]
0049408b  lea        edi, [ebp - 0x86]
00494091  movsd      dword ptr es:[edi], dword ptr [esi]
00494092  movsd      dword ptr es:[edi], dword ptr [esi]
00494093  cmp        byte ptr [ebx + 0xc], 1
00494097  je         0x4940a1

// block 00494099
00494099  lea        esi, [ebp + 0x10]
0049409c  lea        edi, [ebp - 0x7e]
0049409f  movsd      dword ptr es:[edi], dword ptr [esi]
004940a0  movsd      dword ptr es:[edi], dword ptr [esi]

// block 004940a1
004940a1  fstp       qword ptr [ebp - 0x76]
004940a4  lea        eax, [ebp - 0x8e]
004940aa  lea        ebx, [ebp - 0xa4]
004940b0  push       ebx
004940b1  push       eax
004940b2  mov        ebx, dword ptr [ebp - 0x94]
004940b8  mov        al, byte ptr [ebx + 0xe]
004940bb  movsx      eax, al
004940be  push       eax
004940bf  call       0x496bb6

// block 004940c4
004940c4  add        esp, 0xc
004940c7  pop        edi
004940c8  pop        esi
004940c9  fld        qword ptr [ebp - 0x76]
004940cc  jmp        0x493fe2
