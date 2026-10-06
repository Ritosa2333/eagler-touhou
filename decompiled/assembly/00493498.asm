
// block 00493498
00493498  push       edx
00493499  wait       
0049349a  fnstcw     word ptr [esp]
0049349d  je         0x49350c

// block 0049349f
0049349f  cmp        word ptr [esp], 0x27f
004934a5  je         0x4934ac

// block 004934a7
004934a7  call       0x495675

// block 004934ac
004934ac  cmp        eax, 0x3ff00000
004934b1  jae        0x4934e0

// block 004934b3
004934b3  fld1       
004934b5  fadd       st(1)
004934b7  fld1       
004934b9  fsub       st(2)
004934bb  fmulp      st(1)
004934bd  fsqrt      
004934bf  fxch       st(1)
004934c1  fpatan     

// block 004934c3
004934c3  cmp        dword ptr [0x4b4110], 0
004934ca  jne        0x4956fe

// block 004934d0
004934d0  mov        edx, 0xd
004934d5  lea        ecx, [0x4b3560]
004934db  jmp        0x49570b

// block 004934e0
004934e0  ja         0x49351a

// block 004934e2
004934e2  mov        eax, dword ptr [esp + 0xc]
004934e6  mov        ecx, eax
004934e8  and        eax, 0xfffff
004934ed  or         eax, dword ptr [esp + 8]
004934f1  jne        0x49351a

// block 004934f3
004934f3  and        ecx, 0x80000000
004934f9  fstp       st(0)
004934fb  je         0x493501

// block 004934fd
004934fd  fldpi      
004934ff  jmp        0x4934c3

// block 00493501
00493501  fldz       
00493503  jmp        0x4934c3

// block 00493505
00493505  call       0x49568c

// block 0049350a
0049350a  jmp        0x493527

// block 0049350c
0049350c  test       eax, 0xfffff
00493511  jne        0x493505

// block 00493513
00493513  cmp        dword ptr [esp + 8], 0
00493518  jne        0x493505

// block 0049351a
0049351a  fstp       st(0)
0049351c  fld        xword ptr [0x4b35c0]
00493522  mov        eax, 1

// block 00493527
00493527  cmp        dword ptr [0x4b4110], 0
0049352e  jne        0x4956fe

// block 00493534
00493534  mov        edx, 0xd
00493539  lea        ecx, [0x4b3560]
0049353f  call       0x495617

// block 00493544
00493544  pop        edx
00493545  ret        

// block 004956fe
004956fe  cmp        word ptr [esp], 0x27f
00495704  je         0x495709

// block 00495706
00495706  fldcw      word ptr [esp]

// block 00495709
00495709  pop        edx
0049570a  ret        

// block 0049570b
0049570b  mov        ax, word ptr [esp]
0049570f  cmp        ax, 0x27f
00495713  je         0x495733

// block 00495715
00495715  and        ax, 0x20
00495719  je         0x495730

// block 0049571b
0049571b  wait       
0049571c  fnstsw     ax
0049571e  and        ax, 0x20
00495722  je         0x495730

// block 00495724
00495724  mov        eax, 8
00495729  call       0x495617

// block 0049572e
0049572e  pop        edx
0049572f  ret        

// block 00495730
00495730  fldcw      word ptr [esp]

// block 00495733
00495733  pop        edx
00495734  ret        
