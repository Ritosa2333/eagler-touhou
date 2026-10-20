
// block 00495749
00495749  sub        esp, 8
0049574c  fst        qword ptr [esp]
0049574f  mov        eax, dword ptr [esp + 4]
00495753  add        esp, 8
00495756  and        eax, 0x7ff00000
0049575b  je         0x49579a

// block 0049579a
0049579a  fld        qword ptr [0x4a5dfc]
004957a0  fxch       st(1)
004957a2  fscale     
004957a4  fstp       st(1)
004957a6  fld        st(0)
004957a8  fabs       
004957aa  fcomp      qword ptr [0x4a5dec]
004957b0  wait       
004957b1  fnstsw     ax
004957b3  sahf       
004957b4  mov        eax, 4
004957b9  jae        0x495782

// block 004957bb
004957bb  fmul       qword ptr [0x4a5e0c]
004957c1  jmp        0x495782
