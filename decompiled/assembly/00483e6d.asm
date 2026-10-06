
// block 00483e6d
00483e6d  mov        edi, edi
00483e6f  push       ebp
00483e70  mov        ebp, esp
00483e72  push       ebx
00483e73  push       esi
00483e74  push       edi
00483e75  mov        edi, dword ptr [ebp + 8]
00483e78  cmp        dword ptr [edi + 0x20], 0
00483e7c  mov        ebx, 0x4adea8
00483e81  je         0x483ec3

// block 00483e83
00483e83  push       0xb8
00483e88  push       1
00483e8a  call       0x477813

// block 00483e8f
00483e8f  mov        esi, eax
00483e91  pop        ecx
00483e92  pop        ecx
00483e93  test       esi, esi
00483e95  jne        0x483e9c

// block 00483e97
00483e97  xor        eax, eax
00483e99  inc        eax
00483e9a  jmp        0x483ee1

// block 00483e9c
00483e9c  mov        eax, edi
00483e9e  call       0x4838bd

// block 00483ea3
00483ea3  test       eax, eax
00483ea5  je         0x483eb7

// block 00483ea7
00483ea7  push       esi
00483ea8  call       0x483cd8

// block 00483ead
00483ead  push       esi
00483eae  call       0x46c941

// block 00483eb3
00483eb3  pop        ecx
00483eb4  pop        ecx
00483eb5  jmp        0x483e97

// block 00483eb7
00483eb7  mov        dword ptr [esi + 0xb4], 1
00483ec1  jmp        0x483ec5

// block 00483ec3
00483ec3  mov        esi, ebx

// block 00483ec5
00483ec5  add        edi, 0xd4
00483ecb  mov        eax, dword ptr [edi]
00483ecd  cmp        eax, ebx
00483ecf  je         0x483edd

// block 00483ed1
00483ed1  add        eax, 0xb4
00483ed6  push       eax
00483ed7  call       dword ptr [0x49815c]

// block 00483edd
00483edd  mov        dword ptr [edi], esi
00483edf  xor        eax, eax

// block 00483ee1
00483ee1  pop        edi
00483ee2  pop        esi
00483ee3  pop        ebx
00483ee4  pop        ebp
00483ee5  ret        
