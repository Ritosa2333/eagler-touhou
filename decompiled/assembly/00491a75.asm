
// block 00491a75
00491a75  push       ebp
00491a76  mov        ebp, esp
00491a78  sub        esp, 8
00491a7b  push       ebx
00491a7c  push       esi
00491a7d  push       edi
00491a7e  cld        
00491a7f  mov        dword ptr [ebp - 4], eax
00491a82  xor        eax, eax
00491a84  push       eax
00491a85  push       eax
00491a86  push       eax
00491a87  push       dword ptr [ebp - 4]
00491a8a  push       dword ptr [ebp + 0x14]
00491a8d  push       dword ptr [ebp + 0x10]
00491a90  push       dword ptr [ebp + 0xc]
00491a93  push       dword ptr [ebp + 8]
00491a96  call       0x493064

// block 00491a9b
00491a9b  add        esp, 0x20
00491a9e  mov        dword ptr [ebp - 8], eax
00491aa1  pop        edi
00491aa2  pop        esi
00491aa3  pop        ebx
00491aa4  mov        eax, dword ptr [ebp - 8]
00491aa7  mov        esp, ebp
00491aa9  pop        ebp
00491aaa  ret        
