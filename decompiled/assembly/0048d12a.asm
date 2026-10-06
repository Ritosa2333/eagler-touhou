
// block 0048d12a
0048d12a  mov        edi, edi
0048d12c  push       ebp
0048d12d  mov        ebp, esp
0048d12f  push       ebx
0048d130  push       esi
0048d131  mov        esi, dword ptr [ebp + 8]
0048d134  mov        eax, dword ptr [esi + 0xc]
0048d137  mov        ecx, eax
0048d139  and        cl, 3
0048d13c  xor        ebx, ebx
0048d13e  cmp        cl, 2
0048d141  jne        0x48d183

// block 0048d143
0048d143  test       eax, 0x108
0048d148  je         0x48d183

// block 0048d14a
0048d14a  mov        eax, dword ptr [esi + 8]
0048d14d  push       edi
0048d14e  mov        edi, dword ptr [esi]
0048d150  sub        edi, eax
0048d152  test       edi, edi
0048d154  jle        0x48d182

// block 0048d156
0048d156  push       edi
0048d157  push       eax
0048d158  push       esi
0048d159  call       0x47c1da

// block 0048d15e
0048d15e  pop        ecx
0048d15f  push       eax
0048d160  call       0x47be9c

// block 0048d165
0048d165  add        esp, 0xc
0048d168  cmp        eax, edi
0048d16a  jne        0x48d17b

// block 0048d16c
0048d16c  mov        eax, dword ptr [esi + 0xc]
0048d16f  test       al, al
0048d171  jns        0x48d182

// block 0048d173
0048d173  and        eax, 0xfffffffd
0048d176  mov        dword ptr [esi + 0xc], eax
0048d179  jmp        0x48d182

// block 0048d17b
0048d17b  or         dword ptr [esi + 0xc], 0x20
0048d17f  or         ebx, 0xffffffff

// block 0048d182
0048d182  pop        edi

// block 0048d183
0048d183  mov        eax, dword ptr [esi + 8]
0048d186  and        dword ptr [esi + 4], 0
0048d18a  mov        dword ptr [esi], eax
0048d18c  pop        esi
0048d18d  mov        eax, ebx
0048d18f  pop        ebx
0048d190  pop        ebp
0048d191  ret        
