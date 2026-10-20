
// block 00493efe
00493efe  push       ebp
00493eff  mov        ebp, esp
00493f01  add        esp, 0xfffffd30
00493f07  push       ebx
00493f08  wait       
00493f09  fnstcw     word ptr [ebp - 0xa4]
00493f0f  cmp        dword ptr [0x4b37a0], 0
00493f16  je         0x493f33

// block 00493f18
00493f18  call       0x494140

// block 00493f1d
00493f1d  or         byte ptr [ebp - 0x2c8], 1
00493f24  and        byte ptr [ebp - 0x2c8], 0xfd
00493f2b  call       0x493f8a

// block 00493f30
00493f30  pop        ebx
00493f31  leave      
00493f32  ret        

// block 00493f33
00493f33  fst        qword ptr [ebp - 0x86]
00493f39  jmp        0x493f18
