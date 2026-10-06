
// block 00466b10
00466b10  test       edi, edi
00466b12  jne        0x466b1a

// block 00466b14
00466b14  mov        eax, 0x80070057
00466b19  ret        

// block 00466b1a
00466b1a  push       edi
00466b1b  push       0x4a3ad8
00466b20  call       0x466e90

// block 00466b25
00466b25  add        esp, 8
00466b28  push       0
00466b2a  push       0x8000080
00466b2f  push       3
00466b31  push       0
00466b33  push       1
00466b35  push       0x80000000
00466b3a  push       edi
00466b3b  call       dword ptr [0x49809c]

// block 00466b41
00466b41  mov        dword ptr [esi + 0x8c], eax
00466b47  cmp        eax, -1
00466b4a  jne        0x466b52

// block 00466b4c
00466b4c  mov        eax, 0x80004005
00466b51  ret        

// block 00466b52
00466b52  cmp        dword ptr [esi + 0x7c], 0
00466b56  mov        dword ptr [esi + 0x90], ebx
00466b5c  mov        dword ptr [esi + 0x94], edi
00466b62  je         0x466b86

// block 00466b64
00466b64  mov        eax, dword ptr [esi + 0x80]
00466b6a  mov        dword ptr [esi + 0x84], eax
00466b70  mov        eax, dword ptr [ebx + 0x1c]
00466b73  test       eax, eax
00466b75  jle        0x466bab

// block 00466b77
00466b77  mov        ecx, dword ptr [esi + 8]
00466b7a  mov        dword ptr [esi + 0x88], eax
00466b80  mov        dword ptr [esi + 0x2c], ecx
00466b83  xor        eax, eax
00466b85  ret        

// block 00466b86
00466b86  test       eax, eax
00466b88  je         0x466bab

// block 00466b8a
00466b8a  mov        ecx, dword ptr [ebx + 0x10]
00466b8d  add        ecx, dword ptr [0x4d4760]
00466b93  push       0
00466b95  push       0
00466b97  push       ecx
00466b98  push       eax
00466b99  call       dword ptr [0x4980a0]

// block 00466b9f
00466b9f  mov        edx, dword ptr [esi + 0x90]
00466ba5  mov        eax, dword ptr [edx + 0x1c]
00466ba8  mov        dword ptr [esi + 8], eax

// block 00466bab
00466bab  mov        ecx, dword ptr [esi + 8]
00466bae  mov        dword ptr [esi + 0x2c], ecx
00466bb1  xor        eax, eax
00466bb3  ret        
