
// block 00465d40
00465d40  xor        ecx, ecx
00465d42  mov        eax, 1
00465d47  mov        edx, 4
00465d4c  mul        edx
00465d4e  seto       cl
00465d51  push       ebx
00465d52  mov        dword ptr [esi], 0x4a3b1c
00465d58  neg        ecx
00465d5a  or         ecx, eax
00465d5c  push       ecx
00465d5d  call       0x46c9ea

// block 00465d62
00465d62  mov        ecx, dword ptr [esp + 0xc]
00465d66  mov        dword ptr [esi + 4], eax
00465d69  mov        edx, dword ptr [ecx]
00465d6b  mov        ecx, dword ptr [esp + 0x14]
00465d6f  mov        dword ptr [eax], edx
00465d71  mov        eax, dword ptr [esp + 0x10]
00465d75  mov        edx, dword ptr [esi + 4]
00465d78  add        esp, 4
00465d7b  mov        dword ptr [esi + 8], eax
00465d7e  mov        dword ptr [esi + 0x10], 1
00465d85  mov        dword ptr [esi + 0xc], ecx
00465d88  mov        eax, dword ptr [edx]
00465d8a  push       0
00465d8c  push       eax
00465d8d  mov        ebx, esi
00465d8f  call       0x465fe0

// block 00465d94
00465d94  mov        ecx, dword ptr [esi + 4]
00465d97  mov        eax, dword ptr [ecx]
00465d99  mov        edx, dword ptr [eax]
00465d9b  push       0
00465d9d  push       eax
00465d9e  mov        eax, dword ptr [edx + 0x34]
00465da1  call       eax

// block 00465da3
00465da3  mov        dword ptr [esi + 0x30], 0
00465daa  mov        dword ptr [esi + 0x34], 0
00465db1  mov        eax, esi
00465db3  pop        ebx
00465db4  ret        0xc
