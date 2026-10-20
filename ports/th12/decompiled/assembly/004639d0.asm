
// block 004639d0
004639d0  sub        esp, 8
004639d3  push       ebx
004639d4  push       ebp
004639d5  mov        ebp, dword ptr [esp + 0x20]
004639d9  mov        bl, al
004639db  push       esi
004639dc  push       edi
004639dd  mov        edi, dword ptr [esp + 0x20]
004639e1  mov        eax, edi
004639e3  cdq        
004639e4  idiv       ebp
004639e6  mov        eax, ebp
004639e8  mov        dword ptr [esp + 0x28], edi
004639ec  mov        ecx, edx
004639ee  cdq        
004639ef  and        edx, 3
004639f2  add        eax, edx
004639f4  sar        eax, 2
004639f7  xor        edx, edx
004639f9  cmp        ecx, eax
004639fb  mov        eax, dword ptr [esp + 0x2c]
004639ff  setge      dl
00463a02  dec        edx
00463a03  and        edx, ecx
00463a05  cmp        eax, edi
00463a07  mov        dword ptr [esp + 0x20], edx
00463a0b  jg         0x463a11

// block 00463a0d
00463a0d  mov        dword ptr [esp + 0x28], eax

// block 00463a11
00463a11  mov        ecx, dword ptr [esp + 0x28]
00463a15  mov        eax, dword ptr [esp + 0x1c]
00463a19  push       ecx
00463a1a  mov        dword ptr [esp + 0x14], eax
00463a1e  call       0x46d04a

// block 00463a23
00463a23  add        esp, 4
00463a26  mov        dword ptr [esp + 0x14], eax
00463a2a  mov        esi, eax
00463a2c  test       eax, eax
00463a2e  je         0x463ade

// block 00463a34
00463a34  mov        ecx, dword ptr [esp + 0x28]
00463a38  mov        edx, edi
00463a3a  and        edx, 1
00463a3d  add        edx, dword ptr [esp + 0x20]
00463a41  push       ecx
00463a42  sub        edi, edx
00463a44  mov        edx, dword ptr [esp + 0x20]
00463a48  push       edx
00463a49  push       eax
00463a4a  mov        dword ptr [esp + 0x2c], edi
00463a4e  call       0x47a330

// block 00463a53
00463a53  add        esp, 0xc
00463a56  test       edi, edi
00463a58  jle        0x463ad1

// block 00463a5a
00463a5a  jmp        0x463a64

// block 00463a60
00463a60  mov        edi, dword ptr [esp + 0x20]

// block 00463a64
00463a64  cmp        dword ptr [esp + 0x2c], 0
00463a69  jle        0x463ad1

// block 00463a6b
00463a6b  cmp        edi, ebp
00463a6d  jge        0x463a71

// block 00463a6f
00463a6f  mov        ebp, edi

// block 00463a71
00463a71  mov        eax, dword ptr [esp + 0x10]
00463a75  lea        ecx, [eax + ebp]
00463a78  lea        eax, [ebp + 1]
00463a7b  cdq        
00463a7c  sub        eax, edx
00463a7e  sar        eax, 1
00463a80  lea        edi, [ecx - 1]
00463a83  test       eax, eax
00463a85  jle        0x463a9a

// block 00463a87
00463a87  mov        dl, byte ptr [esi]
00463a89  xor        dl, bl
00463a8b  add        bl, byte ptr [esp + 0x24]
00463a8f  mov        byte ptr [edi], dl
00463a91  dec        eax
00463a92  sub        edi, 2
00463a95  inc        esi
00463a96  test       eax, eax
00463a98  jg         0x463a87

// block 00463a9a
00463a9a  mov        eax, ebp
00463a9c  cdq        
00463a9d  sub        eax, edx
00463a9f  sar        eax, 1
00463aa1  lea        edi, [ecx - 2]
00463aa4  test       eax, eax
00463aa6  jle        0x463abb

// block 00463aa8
00463aa8  mov        dl, byte ptr [esi]
00463aaa  xor        dl, bl
00463aac  add        bl, byte ptr [esp + 0x24]
00463ab0  mov        byte ptr [edi], dl
00463ab2  dec        eax
00463ab3  sub        edi, 2
00463ab6  inc        esi
00463ab7  test       eax, eax
00463ab9  jg         0x463aa8

// block 00463abb
00463abb  mov        eax, dword ptr [esp + 0x20]
00463abf  sub        dword ptr [esp + 0x2c], ebp
00463ac3  sub        eax, ebp
00463ac5  mov        dword ptr [esp + 0x20], eax
00463ac9  mov        dword ptr [esp + 0x10], ecx
00463acd  test       eax, eax
00463acf  jg         0x463a60

// block 00463ad1
00463ad1  mov        eax, dword ptr [esp + 0x14]
00463ad5  push       eax
00463ad6  call       0x46c941

// block 00463adb
00463adb  add        esp, 4

// block 00463ade
00463ade  mov        eax, dword ptr [esp + 0x1c]
00463ae2  pop        edi
00463ae3  pop        esi
00463ae4  pop        ebp
00463ae5  pop        ebx
00463ae6  add        esp, 8
00463ae9  ret        0x14
