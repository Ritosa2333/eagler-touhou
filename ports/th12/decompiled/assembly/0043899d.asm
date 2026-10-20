
// block 0043899d
0043899d  mov        eax, dword ptr [esp + 0x4c]
004389a1  mov        ecx, dword ptr [eax + ebx*4 - 4]
004389a5  mov        edx, dword ptr [ebp + 0xa2c]
004389ab  add        ecx, edi
004389ad  fld        dword ptr [edx + ecx*8 + 0x28]
004389b1  lea        eax, [edx + ecx*8 + 0x28]
004389b5  fmul       st(1)
004389b7  mov        dword ptr [esp + 0x14], eax
004389bb  call       0x4931e0

// block 004389c0
004389c0  mov        dword ptr [esi - 0xc], eax
004389c3  mov        eax, dword ptr [esp + 0x14]
004389c7  fld        dword ptr [eax + 4]
004389ca  fmul       st(1)
004389cc  call       0x4931e0

// block 004389d1
004389d1  mov        ecx, dword ptr [esp + 0x4c]
004389d5  mov        dword ptr [esi - 8], eax
004389d8  mov        edx, dword ptr [ecx + ebx*4 - 4]
004389dc  mov        eax, dword ptr [ebp + 0xa2c]
004389e2  add        edx, edi
004389e4  fld        dword ptr [eax + edx*8 + 0x78]
004389e8  lea        eax, [eax + edx*8 + 0x78]
004389ec  fmul       st(1)
004389ee  mov        dword ptr [esp + 0x14], eax
004389f2  call       0x4931e0

// block 004389f7
004389f7  mov        ecx, dword ptr [esp + 0x14]
004389fb  mov        dword ptr [esi - 4], eax
004389fe  fmul       dword ptr [ecx + 4]
00438a01  call       0x4931e0

// block 00438a06
00438a06  mov        dword ptr [esi], eax
00438a08  cmp        dword ptr [ebp + 0xc598], 0
00438a0f  lea        edx, [esi - 4]
00438a12  jne        0x438a17

// block 00438a14
00438a14  lea        edx, [esi - 0xc]

// block 00438a17
00438a17  mov        eax, dword ptr [ebp + 0x988]
00438a1d  add        eax, dword ptr [edx]
00438a1f  mov        ecx, dword ptr [ebp + 0x98c]
00438a25  add        ecx, dword ptr [edx + 4]
00438a28  push       0
00438a2a  mov        dword ptr [esi - 0x1c], eax
00438a2d  mov        dword ptr [esi - 0x18], ecx
00438a30  push       0xb
00438a32  mov        dword ptr [esi - 0x14], eax
00438a35  mov        dword ptr [esi - 0x10], ecx
00438a38  mov        eax, dword ptr [ebp + 0x10]
00438a3b  lea        edx, [esp + 0x28]
00438a3f  push       edx
00438a40  push       eax
00438a41  mov        eax, 0xc
00438a46  xor        ecx, ecx
00438a48  call       0x4615a0

// block 00438a4d
00438a4d  mov        ecx, dword ptr [esp + 0x20]
00438a51  jmp        0x438c82

// block 00438c82
00438c82  mov        dword ptr [esi + 0x40], ecx
