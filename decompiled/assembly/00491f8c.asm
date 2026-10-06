
// block 00491f8c
00491f8c  mov        edi, edi
00491f8e  push       ebp
00491f8f  mov        ebp, esp
00491f91  push       esi
00491f92  mov        esi, ecx
00491f94  mov        dword ptr [esi], 0x49f3f8
00491f9a  call       0x46e086

// block 00491f9f
00491f9f  test       byte ptr [ebp + 8], 1
00491fa3  je         0x491fac

// block 00491fa5
00491fa5  push       esi
00491fa6  call       0x46ca4f

// block 00491fab
00491fab  pop        ecx

// block 00491fac
00491fac  mov        eax, esi
00491fae  pop        esi
00491faf  pop        ebp
00491fb0  ret        4
