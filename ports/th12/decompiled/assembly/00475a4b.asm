
// block 00475a4b
00475a4b  mov        edi, edi
00475a4d  push       ebp
00475a4e  mov        ebp, esp
00475a50  call       0x4759d6

// block 00475a55
00475a55  call       0x48acda

// block 00475a5a
00475a5a  cmp        dword ptr [ebp + 8], 0
00475a5e  mov        dword ptr [0x4b4114], eax
00475a63  je         0x475a6a

// block 00475a65
00475a65  call       0x48ac71

// block 00475a6a
00475a6a  fnclex     
00475a6c  pop        ebp
00475a6d  ret        
