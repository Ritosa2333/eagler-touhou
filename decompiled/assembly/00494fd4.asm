
// block 00494fd4
00494fd4  push       edx
00494fd5  sub        esp, 0x30
00494fd8  fstp       xword ptr [esp + 0x18]
00494fdc  fstp       xword ptr [esp]
00494fdf  mov        edx, 0
00494fe4  mov        eax, dword ptr [esp + 6]
00494fe8  test       eax, 0x7fff0000
00494fed  je         0x494ff9

// block 00494fef
00494fef  call       0x494dce

// block 00494ff4
00494ff4  add        esp, 0x30
00494ff7  pop        edx
00494ff8  ret        

// block 00494ff9
00494ff9  fld        xword ptr [esp]
00494ffc  fld        xword ptr [esp + 0x18]
00495000  mov        eax, dword ptr [esp]
00495003  or         eax, dword ptr [esp + 4]
00495007  je         0x495082

// block 00495009
00495009  fxch       st(1)
0049500b  fstp       xword ptr [esp + 0xc]
0049500f  fld        xword ptr [esp]
00495012  fxch       st(1)
00495014  or         edx, 2
00495017  fnstcw     word ptr [esp + 0x24]
0049501b  mov        eax, dword ptr [esp + 0x24]
0049501f  or         eax, 0x33f
00495024  mov        dword ptr [esp + 0x28], eax
00495028  fldcw      word ptr [esp + 0x28]
0049502c  mov        eax, dword ptr [esp + 0x20]
00495030  and        eax, 0x7fff
00495035  cmp        eax, 0x7fbe
0049503a  ja         0x495054

// block 0049503c
0049503c  or         edx, 1
0049503f  fmul       qword ptr [0x4b3624]
00495045  fstp       xword ptr [esp + 0x18]
00495049  fmul       qword ptr [0x4b3624]
0049504f  fstp       xword ptr [esp]
00495052  jmp        0x495074

// block 00495054
00495054  fnstcw     word ptr [esp + 0x24]
00495058  mov        eax, dword ptr [esp + 0x24]
0049505c  or         eax, 0x300
00495061  mov        dword ptr [esp + 0x28], eax
00495065  fldcw      word ptr [esp + 0x28]
00495069  fstp       st(0)
0049506b  fmul       qword ptr [0x4b3624]
00495071  fstp       xword ptr [esp]

// block 00495074
00495074  fldcw      word ptr [esp + 0x24]
00495078  call       0x494dce

// block 0049507d
0049507d  add        esp, 0x30
00495080  pop        edx
00495081  ret        

// block 00495082
00495082  fprem      
00495084  add        esp, 0x30
00495087  pop        edx
00495088  ret        
