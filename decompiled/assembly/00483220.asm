
// block 00483220
00483220  mov        eax, dword ptr [esp + 8]
00483224  mov        ecx, dword ptr [esp + 0x10]
00483228  or         ecx, eax
0048322a  mov        ecx, dword ptr [esp + 0xc]
0048322e  jne        0x483239

// block 00483230
00483230  mov        eax, dword ptr [esp + 4]
00483234  mul        ecx
00483236  ret        0x10

// block 00483239
00483239  push       ebx
0048323a  mul        ecx
0048323c  mov        ebx, eax
0048323e  mov        eax, dword ptr [esp + 8]
00483242  mul        dword ptr [esp + 0x14]
00483246  add        ebx, eax
00483248  mov        eax, dword ptr [esp + 8]
0048324c  mul        ecx
0048324e  add        edx, ebx
00483250  pop        ebx
00483251  ret        0x10
