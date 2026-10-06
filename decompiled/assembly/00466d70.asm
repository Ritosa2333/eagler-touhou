
// block 00466d70
00466d70  cmp        dword ptr [esi + 0x7c], 0
00466d74  push       ebx
00466d75  mov        ebx, dword ptr [esp + 0xc]
00466d79  push       ebp
00466d7a  mov        ebp, dword ptr [esp + 0xc]
00466d7e  push       edi
00466d7f  mov        edi, eax
00466d81  je         0x466de5

// block 00466d83
00466d83  mov        ecx, dword ptr [esi + 0x84]
00466d89  test       ecx, ecx
00466d8b  jne        0x466d98

// block 00466d8d
00466d8d  pop        edi
00466d8e  pop        ebp
00466d8f  mov        eax, 0x800401f0
00466d94  pop        ebx
00466d95  ret        8

// block 00466d98
00466d98  test       ebx, ebx
00466d9a  je         0x466da2

// block 00466d9c
00466d9c  mov        dword ptr [ebx], 0

// block 00466da2
00466da2  mov        edx, dword ptr [esi + 0x88]
00466da8  mov        eax, dword ptr [esi + 0x80]
00466dae  lea        ebx, [eax + edx]
00466db1  lea        ebp, [ecx + edi]
00466db4  cmp        ebp, ebx
00466db6  jbe        0x466dbe

// block 00466db8
00466db8  sub        eax, ecx
00466dba  add        eax, edx
00466dbc  mov        edi, eax

// block 00466dbe
00466dbe  mov        eax, dword ptr [esp + 0x10]
00466dc2  push       edi
00466dc3  push       ecx
00466dc4  push       eax
00466dc5  call       0x47a330

// block 00466dca
00466dca  mov        eax, dword ptr [esp + 0x20]
00466dce  add        dword ptr [esi + 0x84], edi
00466dd4  add        esp, 0xc
00466dd7  test       eax, eax
00466dd9  je         0x466ddd

// block 00466ddb
00466ddb  mov        dword ptr [eax], edi

// block 00466ddd
00466ddd  pop        edi
00466dde  pop        ebp
00466ddf  xor        eax, eax
00466de1  pop        ebx
00466de2  ret        8

// block 00466de5
00466de5  mov        edx, dword ptr [esi + 0x8c]
00466deb  test       edx, edx
00466ded  je         0x466d8d

// block 00466def
00466def  test       ebp, ebp
00466df1  je         0x466e25

// block 00466df3
00466df3  test       ebx, ebx
00466df5  je         0x466e25

// block 00466df7
00466df7  mov        eax, dword ptr [esi + 8]
00466dfa  mov        ecx, edi
00466dfc  cmp        edi, eax
00466dfe  jbe        0x466e02

// block 00466e00
00466e00  mov        ecx, eax

// block 00466e02
00466e02  sub        eax, ecx
00466e04  push       0
00466e06  mov        dword ptr [esi + 8], eax
00466e09  lea        eax, [esp + 0x14]
00466e0d  push       eax
00466e0e  push       ecx
00466e0f  push       ebp
00466e10  push       edx
00466e11  call       dword ptr [0x4980a8]

// block 00466e17
00466e17  mov        ecx, dword ptr [esp + 0x10]
00466e1b  pop        edi
00466e1c  pop        ebp
00466e1d  mov        dword ptr [ebx], ecx
00466e1f  xor        eax, eax
00466e21  pop        ebx
00466e22  ret        8

// block 00466e25
00466e25  pop        edi
00466e26  pop        ebp
00466e27  mov        eax, 0x80070057
00466e2c  pop        ebx
00466e2d  ret        8
