
// block 00410a50
00410a50  push       ecx
00410a51  push       ebx
00410a52  push       ebp
00410a53  push       esi
00410a54  push       edi
00410a55  mov        edi, eax
00410a57  lea        esi, [edi + 0xd0]
00410a5d  mov        dword ptr [esp + 0x10], esi
00410a61  call       0x464c40

// block 00410a66
00410a66  mov        ebx, dword ptr [0x4ce8cc]
00410a6c  lea        edx, [edi + 0x40]
00410a6f  mov        ebp, 5
00410a74  mov        edi, 0x10000000
00410a79  lea        esp, [esp]

// block 00410a80
00410a80  mov        ecx, dword ptr [edx]
00410a82  test       ecx, ecx
00410a84  je         0x410adf

// block 00410a86
00410a86  mov        eax, dword ptr [ebx + 0x8856b8]
00410a8c  test       eax, eax
00410a8e  je         0x410a9d

// block 00410a90
00410a90  mov        esi, dword ptr [eax]
00410a92  cmp        dword ptr [esi], ecx
00410a94  je         0x410ab6

// block 00410a96
00410a96  mov        eax, dword ptr [eax + 4]
00410a99  test       eax, eax
00410a9b  jne        0x410a90

// block 00410a9d
00410a9d  mov        eax, dword ptr [ebx + 0x8856c0]
00410aa3  test       eax, eax
00410aa5  je         0x410adf

// block 00410aa7
00410aa7  mov        esi, dword ptr [eax]
00410aa9  cmp        dword ptr [esi], ecx
00410aab  je         0x410ab6

// block 00410aad
00410aad  mov        eax, dword ptr [eax + 4]
00410ab0  test       eax, eax
00410ab2  jne        0x410aa7

// block 00410ab4
00410ab4  jmp        0x410adf

// block 00410ab6
00410ab6  mov        eax, esi
00410ab8  test       eax, eax
00410aba  je         0x410adf

// block 00410abc
00410abc  or         dword ptr [eax + 0x47c], edi
00410ac2  cmp        dword ptr [eax + 0x18], 0
00410ac6  jne        0x410adf

// block 00410ac8
00410ac8  mov        eax, dword ptr [eax + 0x14]
00410acb  test       eax, eax
00410acd  je         0x410adf

// block 00410acf
00410acf  nop        

// block 00410ad0
00410ad0  mov        ecx, dword ptr [eax]
00410ad2  or         dword ptr [ecx + 0x47c], edi
00410ad8  mov        eax, dword ptr [eax + 4]
00410adb  test       eax, eax
00410add  jne        0x410ad0

// block 00410adf
00410adf  mov        dword ptr [edx], 0
00410ae5  add        edx, 4
00410ae8  sub        ebp, 1
00410aeb  jne        0x410a80

// block 00410aed
00410aed  mov        esi, dword ptr [esp + 0x10]
00410af1  mov        dword ptr [esi], 0x4a3738
00410af7  call       0x464c40

// block 00410afc
00410afc  pop        edi
00410afd  pop        esi
00410afe  pop        ebp
00410aff  pop        ebx
00410b00  pop        ecx
00410b01  ret        
