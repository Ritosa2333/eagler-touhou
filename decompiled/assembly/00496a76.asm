
// block 00496a76
00496a76  mov        edi, edi
00496a78  push       ebp
00496a79  mov        ebp, esp
00496a7b  push       ecx
00496a7c  push       ecx
00496a7d  mov        eax, dword ptr [ebp + 0x10]
00496a80  fld        qword ptr [ebp + 8]
00496a83  mov        ecx, dword ptr [ebp + 0xe]
00496a86  fstp       qword ptr [ebp - 8]
00496a89  shl        eax, 4
00496a8c  and        ecx, 0x800f
00496a92  or         eax, ecx
00496a94  mov        word ptr [ebp - 2], ax
00496a98  fld        qword ptr [ebp - 8]
00496a9b  leave      
00496a9c  ret        
