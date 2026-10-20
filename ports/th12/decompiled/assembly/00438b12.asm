
// block 00438b12
00438b12  mov        ecx, dword ptr [esp + 0x4c]
00438b16  mov        edx, dword ptr [ecx + ebx*4 - 4]
00438b1a  mov        eax, dword ptr [ebp + 0xa2c]
00438b20  add        edx, edi
00438b22  fld        dword ptr [eax + edx*8 + 0x28]
00438b26  lea        eax, [eax + edx*8 + 0x28]
00438b2a  fmul       st(1)
00438b2c  mov        dword ptr [esp + 0x14], eax
00438b30  call       0x4931e0

// block 00438b35
00438b35  mov        ecx, dword ptr [esp + 0x14]
00438b39  mov        dword ptr [esi - 0xc], eax
00438b3c  fld        dword ptr [ecx + 4]
00438b3f  fmul       st(1)
00438b41  call       0x4931e0

// block 00438b46
00438b46  mov        edx, dword ptr [esp + 0x4c]
00438b4a  mov        dword ptr [esi - 8], eax
00438b4d  mov        eax, dword ptr [edx + ebx*4 - 4]
00438b51  mov        ecx, dword ptr [ebp + 0xa2c]
00438b57  add        eax, edi
00438b59  fld        dword ptr [ecx + eax*8 + 0x78]
00438b5d  lea        eax, [ecx + eax*8 + 0x78]
00438b61  fmul       st(1)
00438b63  mov        dword ptr [esp + 0x14], eax
00438b67  call       0x4931e0

// block 00438b6c
00438b6c  mov        edx, dword ptr [esp + 0x14]
00438b70  mov        dword ptr [esi - 4], eax
00438b73  fmul       dword ptr [edx + 4]
00438b76  call       0x4931e0

// block 00438b7b
00438b7b  mov        dword ptr [esi], eax
00438b7d  cmp        dword ptr [ebp + 0xc598], 0
00438b84  lea        edx, [esi - 4]
00438b87  jne        0x438b8c

// block 00438b89
00438b89  lea        edx, [esi - 0xc]

// block 00438b8c
00438b8c  mov        eax, dword ptr [ebp + 0x988]
00438b92  mov        ecx, dword ptr [ebp + 0x98c]
00438b98  add        eax, dword ptr [edx]
00438b9a  add        ecx, dword ptr [edx + 4]
00438b9d  mov        dword ptr [esi - 0x1c], eax
00438ba0  mov        dword ptr [esi - 0x18], ecx
00438ba3  push       0
00438ba5  mov        dword ptr [esi - 0x14], eax
00438ba8  push       0xf
00438baa  mov        dword ptr [esi - 0x10], ecx
00438bad  mov        ecx, dword ptr [ebp + 0x10]
00438bb0  lea        eax, [esp + 0x30]
00438bb4  push       eax
00438bb5  push       ecx
00438bb6  mov        eax, 0xc
00438bbb  xor        ecx, ecx
00438bbd  call       0x4615a0

// block 00438bc2
00438bc2  mov        edx, dword ptr [esp + 0x28]
00438bc6  mov        dword ptr [esi + 0x40], edx
00438bc9  jmp        0x438c85
