
// block 00462db0
00462db0  push       ebp
00462db1  mov        ebp, esp
00462db3  and        esp, 0xfffffff8
00462db6  sub        esp, 0x150
00462dbc  mov        eax, dword ptr [0x4ad138]
00462dc1  xor        eax, esp
00462dc3  mov        dword ptr [esp + 0x14c], eax
00462dca  push       esi
00462dcb  push       edi
00462dcc  push       0x80
00462dd1  push       0
00462dd3  push       0x4d4eb0
00462dd8  call       0x477420

// block 00462ddd
00462ddd  add        esp, 0xc
00462de0  test       dword ptr [0x4cee78], 0x800
00462dea  jne        0x462e3c

// block 00462dec
00462dec  push       0x34
00462dee  lea        eax, [esp + 0x10]
00462df2  push       0
00462df4  push       eax
00462df5  call       0x477420

// block 00462dfa
00462dfa  add        esp, 0xc
00462dfd  lea        ecx, [esp + 0xc]
00462e01  push       ecx
00462e02  push       0
00462e04  mov        dword ptr [esp + 0x14], 0x34
00462e0c  mov        dword ptr [esp + 0x18], 0xff
00462e14  call       dword ptr [0x49829c]

// block 00462e1a
00462e1a  test       eax, eax
00462e1c  jne        0x462ea2

// block 00462e22
00462e22  mov        ecx, dword ptr [esp + 0x2c]

// block 00462e26
00462e26  test       cl, 1
00462e29  je         0x462e32

// block 00462e2b
00462e2b  mov        byte ptr [eax + 0x4d4eb0], 0x80

// block 00462e32
00462e32  inc        eax
00462e33  shr        ecx, 1
00462e35  cmp        eax, 0x20
00462e38  jb         0x462e26

// block 00462e3a
00462e3a  jmp        0x462ea2

// block 00462e3c
00462e3c  mov        eax, dword ptr [0x4ce90c]
00462e41  mov        edx, dword ptr [eax]
00462e43  push       eax
00462e44  mov        eax, dword ptr [edx + 0x64]
00462e47  call       eax

// block 00462e49
00462e49  test       eax, eax
00462e4b  mov        eax, dword ptr [0x4ce90c]
00462e50  mov        ecx, dword ptr [eax]
00462e52  jge        0x462e82

// block 00462e54
00462e54  mov        edx, dword ptr [ecx + 0x1c]
00462e57  push       eax
00462e58  xor        esi, esi
00462e5a  call       edx

// block 00462e5c
00462e5c  cmp        eax, 0x8007001e
00462e61  jne        0x462ea2

// block 00462e63
00462e63  mov        eax, dword ptr [0x4ce90c]
00462e68  mov        ecx, dword ptr [eax]
00462e6a  mov        edx, dword ptr [ecx + 0x1c]
00462e6d  push       eax
00462e6e  call       edx

// block 00462e70
00462e70  inc        esi
00462e71  cmp        esi, 0x190
00462e77  jge        0x462ea2

// block 00462e79
00462e79  cmp        eax, 0x8007001e
00462e7e  je         0x462e63

// block 00462e80
00462e80  jmp        0x462ea2

// block 00462e82
00462e82  lea        edx, [esp + 0x40]
00462e86  push       edx
00462e87  push       0x110
00462e8c  push       eax
00462e8d  mov        eax, dword ptr [ecx + 0x24]
00462e90  call       eax

// block 00462e92
00462e92  mov        ecx, 0x20
00462e97  lea        esi, [esp + 0x70]
00462e9b  mov        edi, 0x4d4eb0

// block 00462ea0
00462ea0  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00462ea2
00462ea2  mov        ecx, dword ptr [esp + 0x154]
00462ea9  pop        edi
00462eaa  pop        esi
00462eab  xor        ecx, esp
00462ead  mov        eax, 0x4d4eb0
00462eb2  call       0x46c932

// block 00462eb7
00462eb7  mov        esp, ebp
00462eb9  pop        ebp
00462eba  ret        
