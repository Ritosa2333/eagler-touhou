
// block 00438a79
00438a79  mov        edx, dword ptr [esp + 0x14]
00438a7d  mov        dword ptr [esi - 0xc], eax
00438a80  fld        dword ptr [edx + 4]
00438a83  fmul       st(1)
00438a85  call       0x4931e0

// block 00438a8a
00438a8a  mov        dword ptr [esi - 8], eax
00438a8d  mov        eax, dword ptr [esp + 0x4c]
00438a91  mov        ecx, dword ptr [eax + ebx*4 - 4]
00438a95  mov        edx, dword ptr [ebp + 0xa2c]
00438a9b  add        ecx, edi
00438a9d  fld        dword ptr [edx + ecx*8 + 0x78]
00438aa1  lea        eax, [edx + ecx*8 + 0x78]
00438aa5  fmul       st(1)
00438aa7  mov        dword ptr [esp + 0x14], eax
00438aab  call       0x4931e0

// block 00438ab0
00438ab0  mov        dword ptr [esi - 4], eax
00438ab3  mov        eax, dword ptr [esp + 0x14]
00438ab7  fmul       dword ptr [eax + 4]
00438aba  call       0x4931e0

// block 00438abf
00438abf  mov        dword ptr [esi], eax
00438ac1  cmp        dword ptr [ebp + 0xc598], 0
00438ac8  lea        edx, [esi - 4]
00438acb  jne        0x438ad0

// block 00438acd
00438acd  lea        edx, [esi - 0xc]

// block 00438ad0
00438ad0  mov        eax, dword ptr [edx]
00438ad2  mov        ecx, dword ptr [ebp + 0x98c]
00438ad8  add        ecx, dword ptr [edx + 4]
00438adb  add        eax, dword ptr [ebp + 0x988]
00438ae1  mov        dword ptr [esi - 0x18], ecx
00438ae4  mov        dword ptr [esi - 0x1c], eax
00438ae7  push       0
00438ae9  mov        dword ptr [esi - 0x10], ecx
00438aec  push       0xc
00438aee  mov        dword ptr [esi - 0x14], eax
00438af1  mov        edx, dword ptr [ebp + 0x10]
00438af4  lea        ecx, [esp + 0x2c]
00438af8  push       ecx
00438af9  push       edx
00438afa  mov        eax, 0xc
00438aff  xor        ecx, ecx
00438b01  call       0x4615a0

// block 00438b06
00438b06  mov        eax, dword ptr [esp + 0x24]
00438b0a  mov        dword ptr [esi + 0x40], eax
00438b0d  jmp        0x438c85
