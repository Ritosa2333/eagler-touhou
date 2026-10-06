
// block 00493f3b
00493f3b  push       ebp
00493f3c  mov        ebp, esp
00493f3e  add        esp, 0xfffffd30
00493f44  push       ebx
00493f45  push       dword ptr [ebp + 0xc]
00493f48  push       dword ptr [ebp + 8]
00493f4b  call       0x494104

// block 00493f50
00493f50  add        esp, 8
00493f53  push       dword ptr [ebp + 0x14]
00493f56  push       dword ptr [ebp + 0x10]
00493f59  call       0x494104

// block 00493f5e
00493f5e  add        esp, 8
00493f61  wait       
00493f62  fnstcw     word ptr [ebp - 0xa4]
00493f68  or         byte ptr [ebp - 0x2c8], 2
00493f6f  mov        byte ptr [ebp - 0x8f], 1
00493f76  call       0x4941a7

// block 00493f7b
00493f7b  call       0x493f83

// block 00493f80
00493f80  pop        ebx
00493f81  leave      
00493f82  ret        
