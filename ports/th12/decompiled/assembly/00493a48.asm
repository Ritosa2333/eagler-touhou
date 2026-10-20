
// block 00493a48
00493a48  push       edx
00493a49  wait       
00493a4a  fnstcw     word ptr [esp]
00493a4d  je         0x493a9f

// block 00493a4f
00493a4f  cmp        word ptr [esp], 0x27f
00493a55  je         0x493a5d

// block 00493a57
00493a57  fldcw      word ptr [0x4a5dd8]

// block 00493a5d
00493a5d  fcos       
00493a5f  wait       
00493a60  fnstsw     ax
00493a62  sahf       
00493a63  jp         0x493a82

// block 00493a65
00493a65  cmp        dword ptr [0x4b4110], 0
00493a6c  jne        0x4956fe

// block 00493a72
00493a72  mov        edx, 0x12
00493a77  lea        ecx, [0x4b35b0]
00493a7d  jmp        0x49570b

// block 00493a82
00493a82  fld        xword ptr [0x4a5dda]
00493a88  fxch       st(1)

// block 00493a8a
00493a8a  fprem1     
00493a8c  wait       
00493a8d  fnstsw     ax
00493a8f  sahf       
00493a90  jp         0x493a8a

// block 00493a92
00493a92  fstp       st(1)
00493a94  fcos       
00493a96  jmp        0x493a65

// block 00493a98
00493a98  call       0x49568c

// block 00493a9d
00493a9d  jmp        0x493aba

// block 00493a9f
00493a9f  test       eax, 0xfffff
00493aa4  jne        0x493a98

// block 00493aa6
00493aa6  cmp        dword ptr [esp + 8], 0
00493aab  jne        0x493a98

// block 00493aad
00493aad  fstp       st(0)
00493aaf  fld        xword ptr [0x4b35c0]
00493ab5  mov        eax, 1

// block 00493aba
00493aba  cmp        dword ptr [0x4b4110], 0
00493ac1  jne        0x4956fe

// block 00493ac7
00493ac7  mov        edx, 0x12
00493acc  lea        ecx, [0x4b35b0]
00493ad2  call       0x495617

// block 00493ad7
00493ad7  pop        edx
00493ad8  ret        
