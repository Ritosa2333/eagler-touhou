
// block 00493918
00493918  push       edx
00493919  wait       
0049391a  fnstcw     word ptr [esp]
0049391d  je         0x49396f

// block 0049391f
0049391f  cmp        word ptr [esp], 0x27f
00493925  je         0x49392d

// block 00493927
00493927  fldcw      word ptr [0x4a5dd8]

// block 0049392d
0049392d  fsin       
0049392f  wait       
00493930  fnstsw     ax
00493932  sahf       
00493933  jp         0x493952

// block 00493935
00493935  cmp        dword ptr [0x4b4110], 0
0049393c  jne        0x4956fe

// block 00493942
00493942  mov        edx, 0x1e
00493947  lea        ecx, [0x4b35a0]
0049394d  jmp        0x49570b

// block 00493952
00493952  fld        xword ptr [0x4a5dda]
00493958  fxch       st(1)

// block 0049395a
0049395a  fprem1     
0049395c  wait       
0049395d  fnstsw     ax
0049395f  sahf       
00493960  jp         0x49395a

// block 00493962
00493962  fstp       st(1)
00493964  fsin       
00493966  jmp        0x493935

// block 00493968
00493968  call       0x49568c

// block 0049396d
0049396d  jmp        0x49398a

// block 0049396f
0049396f  test       eax, 0xfffff
00493974  jne        0x493968

// block 00493976
00493976  cmp        dword ptr [esp + 8], 0
0049397b  jne        0x493968

// block 0049397d
0049397d  fstp       st(0)
0049397f  fld        xword ptr [0x4b35c0]
00493985  mov        eax, 1

// block 0049398a
0049398a  cmp        dword ptr [0x4b4110], 0
00493991  jne        0x4956fe

// block 00493997
00493997  mov        edx, 0x1e
0049399c  lea        ecx, [0x4b35a0]
004939a2  call       0x495617

// block 004939a7
004939a7  pop        edx
004939a8  ret        
