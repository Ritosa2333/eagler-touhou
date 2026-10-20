
// block 00402cc0
00402cc0  push       -1
00402cc2  push       0x4972f6
00402cc7  mov        eax, dword ptr fs:[0]
00402ccd  push       eax
00402cce  push       ebx
00402ccf  push       ebp
00402cd0  push       esi
00402cd1  push       edi
00402cd2  mov        eax, dword ptr [0x4ad138]
00402cd7  xor        eax, esp
00402cd9  push       eax
00402cda  lea        eax, [esp + 0x14]
00402cde  mov        dword ptr fs:[0], eax
00402ce4  mov        ebp, dword ptr [esp + 0x24]
00402ce8  mov        dword ptr [esp + 0x1c], 1
00402cf0  mov        esi, dword ptr [ebp + 8]
00402cf3  mov        edi, dword ptr [0x4ce89c]
00402cf9  mov        ebx, dword ptr [0x49808c]
00402cff  test       esi, esi
00402d01  je         0x402d42

// block 00402d03
00402d03  test       dword ptr [0x4cee78], 0x8000
00402d0d  je         0x402d20

// block 00402d0f
00402d0f  push       0x4cf0f8
00402d14  call       dword ptr [0x498088]

// block 00402d1a
00402d1a  inc        byte ptr [0x4cf218]

// block 00402d20
00402d20  mov        ecx, esi
00402d22  mov        edx, edi
00402d24  call       0x462890

// block 00402d29
00402d29  test       dword ptr [0x4cee78], 0x8000
00402d33  je         0x402d42

// block 00402d35
00402d35  push       0x4cf0f8
00402d3a  call       ebx

// block 00402d3c
00402d3c  dec        byte ptr [0x4cf218]

// block 00402d42
00402d42  mov        esi, dword ptr [ebp + 0xc]
00402d45  mov        edi, dword ptr [0x4ce89c]
00402d4b  test       esi, esi
00402d4d  je         0x402d8e

// block 00402d4f
00402d4f  test       dword ptr [0x4cee78], 0x8000
00402d59  je         0x402d6c

// block 00402d5b
00402d5b  push       0x4cf0f8
00402d60  call       dword ptr [0x498088]

// block 00402d66
00402d66  inc        byte ptr [0x4cf218]

// block 00402d6c
00402d6c  mov        ecx, esi
00402d6e  mov        edx, edi
00402d70  call       0x462890

// block 00402d75
00402d75  test       dword ptr [0x4cee78], 0x8000
00402d7f  je         0x402d8e

// block 00402d81
00402d81  push       0x4cf0f8
00402d86  call       ebx

// block 00402d88
00402d88  dec        byte ptr [0x4cf218]

// block 00402d8e
00402d8e  mov        esi, dword ptr [ebp + 0x3600]
00402d94  mov        edi, dword ptr [0x4ce89c]
00402d9a  test       esi, esi
00402d9c  je         0x402ddd

// block 00402d9e
00402d9e  test       dword ptr [0x4cee78], 0x8000
00402da8  je         0x402dbb

// block 00402daa
00402daa  push       0x4cf0f8
00402daf  call       dword ptr [0x498088]

// block 00402db5
00402db5  inc        byte ptr [0x4cf218]

// block 00402dbb
00402dbb  mov        ecx, esi
00402dbd  mov        edx, edi
00402dbf  call       0x462890

// block 00402dc4
00402dc4  test       dword ptr [0x4cee78], 0x8000
00402dce  je         0x402ddd

// block 00402dd0
00402dd0  push       0x4cf0f8
00402dd5  call       ebx

// block 00402dd7
00402dd7  dec        byte ptr [0x4cf218]

// block 00402ddd
00402ddd  mov        eax, dword ptr [ebp + 0x10]
00402de0  test       eax, eax
00402de2  je         0x402df4

// block 00402de4
00402de4  push       eax
00402de5  call       0x46c941

// block 00402dea
00402dea  add        esp, 4
00402ded  mov        dword ptr [ebp + 0x10], 0

// block 00402df4
00402df4  mov        eax, dword ptr [ebp + 0x3604]
00402dfa  test       eax, eax
00402dfc  je         0x402e11

// block 00402dfe
00402dfe  push       eax
00402dff  call       0x46c941

// block 00402e04
00402e04  add        esp, 4
00402e07  mov        dword ptr [ebp + 0x3604], 0

// block 00402e11
00402e11  mov        eax, dword ptr [ebp + 0x1c8]
00402e17  mov        dword ptr [ebp + 0x3604], 0
00402e21  test       eax, eax
00402e23  je         0x402e38

// block 00402e25
00402e25  push       eax
00402e26  call       0x46c941

// block 00402e2b
00402e2b  add        esp, 4
00402e2e  mov        dword ptr [ebp + 0x1c8], 0

// block 00402e38
00402e38  mov        esi, dword ptr [ebp + 0x35dc]
00402e3e  test       esi, esi
00402e40  je         0x402e5a

// block 00402e42
00402e42  call       0x402870

// block 00402e47
00402e47  push       esi
00402e48  call       0x46ca4f

// block 00402e4d
00402e4d  add        esp, 4
00402e50  mov        dword ptr [ebp + 0x35dc], 0

// block 00402e5a
00402e5a  test       byte ptr [0x4b0ce0], 1
00402e61  jne        0x402ea4

// block 00402e63
00402e63  mov        eax, dword ptr [ebp + 0x35d4]
00402e69  and        eax, 1
00402e6c  add        eax, 3
00402e6f  js         0x402ea4

// block 00402e71
00402e71  cmp        eax, 0x20
00402e74  jae        0x402ea4

// block 00402e76
00402e76  mov        ecx, dword ptr [0x4ce8cc]
00402e7c  mov        edi, dword ptr [ecx + eax*4 + 0x4b50c0]
00402e83  lea        esi, [ecx + eax*4 + 0x4b50c0]
00402e8a  test       edi, edi
00402e8c  je         0x402ea4

// block 00402e8e
00402e8e  call       0x4604e0

// block 00402e93
00402e93  mov        eax, dword ptr [esi]
00402e95  push       eax
00402e96  call       0x46ca4f

// block 00402e9b
00402e9b  add        esp, 4
00402e9e  mov        dword ptr [esi], 0

// block 00402ea4
00402ea4  cmp        dword ptr [0x4b43c0], ebp
00402eaa  jne        0x402eb6

// block 00402eac
00402eac  mov        dword ptr [0x4b43c0], 0

// block 00402eb6
00402eb6  cmp        dword ptr [0x4b43bc], ebp
00402ebc  jne        0x402ec8

// block 00402ebe
00402ebe  mov        dword ptr [0x4b43bc], 0

// block 00402ec8
00402ec8  push       0x4026e0
00402ecd  push       3
00402ecf  push       0x4b4
00402ed4  lea        edx, [ebp + 0x2794]
00402eda  push       edx
00402edb  mov        byte ptr [esp + 0x2c], 0
00402ee0  call       0x46cf1e

// block 00402ee5
00402ee5  push       0x4026e0
00402eea  push       8
00402eec  push       0x4b4
00402ef1  add        ebp, 0x1cc
00402ef7  push       ebp
00402ef8  mov        dword ptr [esp + 0x2c], 0xffffffff
00402f00  call       0x46cf1e

// block 00402f05
00402f05  mov        ecx, dword ptr [esp + 0x14]
00402f09  mov        dword ptr fs:[0], ecx
00402f10  pop        ecx
00402f11  pop        edi
00402f12  pop        esi
00402f13  pop        ebp
00402f14  pop        ebx
00402f15  add        esp, 0xc
00402f18  ret        4
