
// block 00496a05
00496a05  mov        edi, edi
00496a07  push       ebp
00496a08  mov        ebp, esp
00496a0a  push       ecx
00496a0b  push       ecx
00496a0c  mov        eax, dword ptr [ebp + 0x10]
00496a0f  fld        qword ptr [ebp + 8]
00496a12  mov        ecx, dword ptr [ebp + 0xe]
00496a15  fstp       qword ptr [ebp - 8]
00496a18  add        eax, 0x3fe
00496a1d  shl        eax, 4
00496a20  and        ecx, 0x800f
00496a26  or         eax, ecx
00496a28  mov        word ptr [ebp - 2], ax
00496a2c  fld        qword ptr [ebp - 8]
00496a2f  leave      
00496a30  ret        
