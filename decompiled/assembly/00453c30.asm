
// block 00453c30
00453c30  mov        eax, dword ptr [esi + 0x526c]
00453c36  test       eax, eax
00453c38  je         0x453cc9

// block 00453c3e
00453c3e  push       1
00453c40  call       0x466460

// block 00453c45
00453c45  cmp        dword ptr [esi + 0x18], 0
00453c49  je         0x453cad

// block 00453c4b
00453c4b  mov        eax, dword ptr [esi + 0x14]
00453c4e  push       ebx
00453c4f  push       edi
00453c50  mov        edi, dword ptr [0x498220]
00453c56  push       0
00453c58  push       0
00453c5a  push       0x12
00453c5c  push       eax
00453c5d  call       edi

// block 00453c5f
00453c5f  mov        ecx, dword ptr [esi + 0x18]
00453c62  mov        ebx, dword ptr [0x4980f4]
00453c68  push       0x100
00453c6d  push       ecx
00453c6e  call       ebx

// block 00453c70
00453c70  test       eax, eax
00453c72  je         0x453c8f

// block 00453c74
00453c74  mov        edx, dword ptr [esi + 0x14]
00453c77  push       0
00453c79  push       0
00453c7b  push       0x12
00453c7d  push       edx
00453c7e  call       edi

// block 00453c80
00453c80  mov        eax, dword ptr [esi + 0x18]
00453c83  push       0x100
00453c88  push       eax
00453c89  call       ebx

// block 00453c8b
00453c8b  test       eax, eax
00453c8d  jne        0x453c74

// block 00453c8f
00453c8f  mov        ecx, dword ptr [esi + 0x18]
00453c92  mov        edi, dword ptr [0x4980a4]
00453c98  push       ecx
00453c99  call       edi

// block 00453c9b
00453c9b  mov        edx, dword ptr [esi + 0x5270]
00453ca1  push       edx
00453ca2  call       edi

// block 00453ca4
00453ca4  pop        edi
00453ca5  mov        dword ptr [esi + 0x18], 0
00453cac  pop        ebx

// block 00453cad
00453cad  mov        ecx, dword ptr [esi + 0x526c]
00453cb3  test       ecx, ecx
00453cb5  je         0x453cc9

// block 00453cb7
00453cb7  mov        eax, dword ptr [ecx]
00453cb9  mov        edx, dword ptr [eax]
00453cbb  push       1
00453cbd  call       edx

// block 00453cbf
00453cbf  mov        dword ptr [esi + 0x526c], 0

// block 00453cc9
00453cc9  ret        
