
// block 00496a49
00496a49  mov        edi, edi
00496a4b  push       ebp
00496a4c  mov        ebp, esp
00496a4e  mov        eax, dword ptr [ebp + 0xe]
00496a51  fld        qword ptr [ebp + 8]
00496a54  mov        ecx, dword ptr [ebp + 0x10]
00496a57  shr        eax, 4
00496a5a  and        eax, 0x7ff
00496a5f  lea        eax, [eax + ecx - 0x3fe]
00496a66  push       eax
00496a67  push       ecx
00496a68  push       ecx
00496a69  fstp       qword ptr [esp]
00496a6c  call       0x496a05

// block 00496a71
00496a71  add        esp, 0xc
00496a74  pop        ebp
00496a75  ret        
