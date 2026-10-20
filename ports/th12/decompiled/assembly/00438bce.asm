
// block 00438bce
00438bce  mov        eax, dword ptr [esp + 0x4c]
00438bd2  mov        ecx, dword ptr [eax + ebx*4 - 4]
00438bd6  mov        edx, dword ptr [ebp + 0xa2c]
00438bdc  add        ecx, edi
00438bde  fld        dword ptr [edx + ecx*8 + 0x28]
00438be2  lea        eax, [edx + ecx*8 + 0x28]
00438be6  fmul       st(1)
00438be8  mov        dword ptr [esp + 0x14], eax
00438bec  call       0x4931e0

// block 00438bf1
00438bf1  mov        dword ptr [esi - 0xc], eax
00438bf4  mov        eax, dword ptr [esp + 0x14]
00438bf8  fld        dword ptr [eax + 4]
00438bfb  fmul       st(1)
00438bfd  call       0x4931e0

// block 00438c02
00438c02  mov        ecx, dword ptr [esp + 0x4c]
00438c06  mov        dword ptr [esi - 8], eax
00438c09  mov        edx, dword ptr [ecx + ebx*4 - 4]
00438c0d  mov        eax, dword ptr [ebp + 0xa2c]
00438c13  add        edx, edi
00438c15  fld        dword ptr [eax + edx*8 + 0x78]
00438c19  lea        eax, [eax + edx*8 + 0x78]
00438c1d  fmul       st(1)
00438c1f  mov        dword ptr [esp + 0x14], eax
00438c23  call       0x4931e0

// block 00438c28
00438c28  mov        ecx, dword ptr [esp + 0x14]
00438c2c  mov        dword ptr [esi - 4], eax
00438c2f  fmul       dword ptr [ecx + 4]
00438c32  call       0x4931e0

// block 00438c37
00438c37  mov        dword ptr [esi], eax
00438c39  cmp        dword ptr [ebp + 0xc598], 0
00438c40  lea        edx, [esi - 4]
00438c43  jne        0x438c48

// block 00438c45
00438c45  lea        edx, [esi - 0xc]

// block 00438c48
00438c48  mov        eax, dword ptr [edx]
00438c4a  mov        ecx, dword ptr [ebp + 0x98c]
00438c50  add        eax, dword ptr [ebp + 0x988]
00438c56  add        ecx, dword ptr [edx + 4]
00438c59  push       0
00438c5b  mov        dword ptr [esi - 0x1c], eax
00438c5e  mov        dword ptr [esi - 0x18], ecx
00438c61  push       0x10
00438c63  mov        dword ptr [esi - 0x14], eax
00438c66  mov        dword ptr [esi - 0x10], ecx
00438c69  mov        eax, dword ptr [ebp + 0x10]
00438c6c  lea        edx, [esp + 0x34]
00438c70  push       edx
00438c71  push       eax
00438c72  mov        eax, 0xc
00438c77  xor        ecx, ecx
00438c79  call       0x4615a0

// block 00438c7e
00438c7e  mov        ecx, dword ptr [esp + 0x2c]
