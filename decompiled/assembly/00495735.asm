
// block 00495735
00495735  sub        esp, 8
00495738  fst        qword ptr [esp]
0049573b  mov        eax, dword ptr [esp + 4]
0049573f  add        esp, 8
00495742  and        eax, 0x7ff00000
00495747  jmp        0x49575d

// block 0049575d
0049575d  cmp        eax, 0x7ff00000
00495762  je         0x4957c3

// block 00495764
00495764  mov        ax, word ptr [esp]
00495768  cmp        ax, 0x27f
0049576c  je         0x495798

// block 0049576e
0049576e  and        ax, 0x20
00495772  jne        0x495795

// block 00495774
00495774  wait       
00495775  fnstsw     ax
00495777  and        ax, 0x20
0049577b  je         0x495795

// block 0049577d
0049577d  mov        eax, 8

// block 00495782
00495782  cmp        edx, 0x1d
00495785  je         0x49578e

// block 00495787
00495787  call       0x495617

// block 0049578c
0049578c  pop        edx
0049578d  ret        

// block 0049578e
0049578e  call       0x495600

// block 00495793
00495793  pop        edx
00495794  ret        

// block 00495795
00495795  fldcw      word ptr [esp]

// block 00495798
00495798  pop        edx
00495799  ret        

// block 004957c3
004957c3  fld        qword ptr [0x4a5df4]
004957c9  fxch       st(1)
004957cb  fscale     
004957cd  fstp       st(1)
004957cf  fld        st(0)
004957d1  fabs       
004957d3  fcomp      qword ptr [0x4a5de4]
004957d9  wait       
004957da  fnstsw     ax
004957dc  sahf       
004957dd  mov        eax, 3
004957e2  jbe        0x495782

// block 004957e4
004957e4  fmul       qword ptr [0x4a5e04]
004957ea  jmp        0x495782
