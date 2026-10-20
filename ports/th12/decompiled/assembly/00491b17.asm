
// block 00491b17
00491b17  mov        edi, edi
00491b19  push       ebp
00491b1a  mov        ebp, esp
00491b1c  mov        eax, dword ptr [ebp + 8]
00491b1f  push       dword ptr [eax + 0x1c]
00491b22  push       dword ptr [eax + 0x28]
00491b25  push       0
00491b27  push       dword ptr [eax + 0x18]
00491b2a  call       0x49205b

// block 00491b2f
00491b2f  add        esp, 0x10
00491b32  pop        ebp
00491b33  ret        4
