
// block 0042feb0
0042feb0  push       ebx
0042feb1  push       ebp
0042feb2  mov        ebp, dword ptr [esp + 0xc]
0042feb6  push       esi
0042feb7  push       edi
0042feb8  mov        esi, 0x4ceab0
0042febd  call       0x423250

// block 0042fec2
0042fec2  push       1
0042fec4  lea        eax, [esp + 0x18]
0042fec8  push       eax
0042fec9  mov        eax, 0x4a250c
0042fece  call       0x463c10

// block 0042fed3
0042fed3  mov        bl, 4
0042fed5  test       eax, eax
0042fed7  jne        0x42fee3

// block 0042fed9
0042fed9  push       0x4a0b2c
0042fede  jmp        0x42ff84

// block 0042fee3
0042fee3  mov        esi, eax
0042fee5  mov        ecx, 0xf
0042feea  mov        edi, 0x4ceab0
0042feef  push       eax

// block 0042fef0
0042fef0  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042fef2
0042fef2  call       0x46c941

// block 0042fef7
0042fef7  add        esp, 4
0042fefa  cmp        byte ptr [0x4ceaca], 2
0042ff01  jae        0x42ff7f

// block 0042ff03
0042ff03  mov        al, 3
0042ff05  cmp        byte ptr [0x4ceacb], al
0042ff0b  jae        0x42ff7f

// block 0042ff0d
0042ff0d  cmp        byte ptr [0x4ceacc], 2
0042ff14  jae        0x42ff7f

// block 0042ff16
0042ff16  cmp        byte ptr [0x4ceacd], bl
0042ff1c  jae        0x42ff7f

// block 0042ff1e
0042ff1e  cmp        byte ptr [0x4ceace], al
0042ff24  jae        0x42ff7f

// block 0042ff26
0042ff26  cmp        byte ptr [0x4ceacf], al
0042ff2c  jae        0x42ff7f

// block 0042ff2e
0042ff2e  cmp        dword ptr [0x4ceab0], 0x120001
0042ff38  jne        0x42ff7f

// block 0042ff3a
0042ff3a  cmp        dword ptr [esp + 0x14], 0x3c
0042ff3f  jne        0x42ff7f

// block 0042ff41
0042ff41  mov        ecx, dword ptr [0x4ceab4]
0042ff47  mov        edx, dword ptr [0x4ceab8]
0042ff4d  mov        eax, dword ptr [0x4ceabc]
0042ff52  mov        dword ptr [0x4d49ec], ecx
0042ff58  mov        ecx, dword ptr [0x4ceac0]
0042ff5e  mov        dword ptr [0x4d49f0], edx
0042ff64  mov        dx, word ptr [0x4ceac4]
0042ff6b  mov        dword ptr [0x4d49f4], eax
0042ff70  mov        dword ptr [0x4d49f8], ecx
0042ff76  mov        word ptr [0x4d49fc], dx
0042ff7d  jmp        0x42ff9b

// block 0042ff7f
0042ff7f  push       0x4a0b60

// block 0042ff84
0042ff84  mov        ecx, 0x4b0ec8
0042ff89  call       0x464220

// block 0042ff8e
0042ff8e  add        esp, 4
0042ff91  mov        esi, 0x4ceab0
0042ff96  call       0x423250

// block 0042ff9b
0042ff9b  mov        dword ptr [ebp + 0x57c], 0
0042ffa5  test       byte ptr [ebp + 0x200], bl
0042ffab  je         0x42ffbf

// block 0042ffad
0042ffad  push       0x4a0b94
0042ffb2  mov        ecx, 0x4b0ec8
0042ffb7  call       0x464220

// block 0042ffbc
0042ffbc  add        esp, 4

// block 0042ffbf
0042ffbf  test       byte ptr [ebp + 0x200], 1
0042ffc6  je         0x42ffda

// block 0042ffc8
0042ffc8  push       0x4a0bb0
0042ffcd  mov        ecx, 0x4b0ec8
0042ffd2  call       0x464220

// block 0042ffd7
0042ffd7  add        esp, 4

// block 0042ffda
0042ffda  cmp        dword ptr [ebp + 0x114], 0
0042ffe1  je         0x42fff5

// block 0042ffe3
0042ffe3  push       0x4a0bd8
0042ffe8  mov        ecx, 0x4b0ec8
0042ffed  call       0x464220

// block 0042fff2
0042fff2  add        esp, 4

// block 0042fff5
0042fff5  test       byte ptr [ebp + 0x200], 2
0042fffc  je         0x430010

// block 0042fffe
0042fffe  push       0x4a0bf8
00430003  mov        ecx, 0x4b0ec8
00430008  call       0x464220

// block 0043000d
0043000d  add        esp, 4

// block 00430010
00430010  test       byte ptr [ebp + 0x200], 8
00430017  je         0x43002b

// block 00430019
00430019  push       0x4a0c20
0043001e  mov        ecx, 0x4b0ec8
00430023  call       0x464220

// block 00430028
00430028  add        esp, 4

// block 0043002b
0043002b  test       byte ptr [ebp + 0x200], 0x10
00430032  je         0x430046

// block 00430034
00430034  push       0x4a0c58
00430039  mov        ecx, 0x4b0ec8
0043003e  call       0x464220

// block 00430043
00430043  add        esp, 4

// block 00430046
00430046  test       byte ptr [ebp + 0x200], 0x20
0043004d  je         0x43006b

// block 0043004f
0043004f  push       0x4a0c78
00430054  mov        ecx, 0x4b0ec8
00430059  call       0x464220

// block 0043005e
0043005e  add        esp, 4
00430061  mov        dword ptr [0x4cee64], 1

// block 0043006b
0043006b  test       byte ptr [ebp + 0x200], 0x40
00430072  je         0x430086

// block 00430074
00430074  push       0x4a0c90
00430079  mov        ecx, 0x4b0ec8
0043007e  call       0x464220

// block 00430083
00430083  add        esp, 4

// block 00430086
00430086  push       0x4ceab0
0043008b  mov        ebx, 0x3c
00430090  mov        edi, 0x4a250c
00430095  call       0x463e80

// block 0043009a
0043009a  test       eax, eax
0043009c  je         0x4300c5

// block 0043009e
0043009e  push       edi
0043009f  push       0x4a0cb4
004300a4  mov        edi, 0x4b0ec8
004300a9  call       0x464300

// block 004300ae
004300ae  push       0x4a0cd8
004300b3  call       0x464300

// block 004300b8
004300b8  add        esp, 0xc
004300bb  pop        edi
004300bc  pop        esi
004300bd  pop        ebp
004300be  or         eax, 0xffffffff
004300c1  pop        ebx
004300c2  ret        4

// block 004300c5
004300c5  pop        edi
004300c6  pop        esi
004300c7  pop        ebp
004300c8  xor        eax, eax
004300ca  pop        ebx
004300cb  ret        4
