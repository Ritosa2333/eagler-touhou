
// block 00472dc9
00472dc9  push       0x18
00472dcb  push       0x4aac20
00472dd0  call       0x46fcec

// block 00472dd5
00472dd5  push       8
00472dd7  call       0x46ec9a

// block 00472ddc
00472ddc  pop        ecx
00472ddd  and        dword ptr [ebp - 4], 0
00472de1  xor        ebx, ebx
00472de3  inc        ebx
00472de4  cmp        dword ptr [0x4b3da0], ebx
00472dea  je         0x472eb5

// block 00472df0
00472df0  mov        dword ptr [0x4b3d9c], ebx
00472df6  mov        al, byte ptr [ebp + 0x10]
00472df9  mov        byte ptr [0x4b3d98], al
00472dfe  cmp        dword ptr [ebp + 0xc], 0
00472e02  jne        0x472ea5

// block 00472e08
00472e08  push       dword ptr [0x4d6430]
00472e0e  call       0x4751de

// block 00472e13
00472e13  pop        ecx
00472e14  mov        edi, eax
00472e16  mov        dword ptr [ebp - 0x28], edi
00472e19  test       edi, edi
00472e1b  je         0x472e95

// block 00472e1d
00472e1d  push       dword ptr [0x4d642c]
00472e23  call       0x4751de

// block 00472e28
00472e28  pop        ecx
00472e29  mov        esi, eax
00472e2b  mov        dword ptr [ebp - 0x24], esi
00472e2e  mov        dword ptr [ebp - 0x1c], edi
00472e31  mov        dword ptr [ebp - 0x20], esi

// block 00472e34
00472e34  sub        esi, 4
00472e37  mov        dword ptr [ebp - 0x24], esi
00472e3a  cmp        esi, edi
00472e3c  jb         0x472e95

// block 00472e3e
00472e3e  call       0x4751d5

// block 00472e43
00472e43  cmp        dword ptr [esi], eax
00472e45  je         0x472e34

// block 00472e47
00472e47  cmp        esi, edi
00472e49  jb         0x472e95

// block 00472e4b
00472e4b  push       dword ptr [esi]
00472e4d  call       0x4751de

// block 00472e52
00472e52  mov        edi, eax
00472e54  call       0x4751d5

// block 00472e59
00472e59  mov        dword ptr [esi], eax
00472e5b  call       edi

// block 00472e5d
00472e5d  push       dword ptr [0x4d6430]
00472e63  call       0x4751de

// block 00472e68
00472e68  mov        edi, eax
00472e6a  push       dword ptr [0x4d642c]
00472e70  call       0x4751de

// block 00472e75
00472e75  add        esp, 0xc
00472e78  cmp        dword ptr [ebp - 0x1c], edi
00472e7b  jne        0x472e82

// block 00472e7d
00472e7d  cmp        dword ptr [ebp - 0x20], eax
00472e80  je         0x472e90

// block 00472e82
00472e82  mov        dword ptr [ebp - 0x1c], edi
00472e85  mov        dword ptr [ebp - 0x28], edi
00472e88  mov        dword ptr [ebp - 0x20], eax
00472e8b  mov        esi, eax
00472e8d  mov        dword ptr [ebp - 0x24], esi

// block 00472e90
00472e90  mov        edi, dword ptr [ebp - 0x28]
00472e93  jmp        0x472e34

// block 00472e95
00472e95  push       0x498364
00472e9a  mov        eax, 0x498358
00472e9f  call       0x472c8b

// block 00472ea4
00472ea4  pop        ecx

// block 00472ea5
00472ea5  push       0x49836c
00472eaa  mov        eax, 0x498368
00472eaf  call       0x472c8b

// block 00472eb4
00472eb4  pop        ecx

// block 00472eb5
00472eb5  mov        dword ptr [ebp - 4], 0xfffffffe
00472ebc  call       0x472ee0

// block 00472ec1
00472ec1  cmp        dword ptr [ebp + 0x10], 0
00472ec5  jne        0x472eef

// block 00472ec7
00472ec7  mov        dword ptr [0x4b3da0], ebx
00472ecd  push       8
00472ecf  call       0x46eba8

// block 00472ed4
00472ed4  pop        ecx
00472ed5  push       dword ptr [ebp + 8]
00472ed8  call       0x472c61

// block 00472eef
00472eef  call       0x46fd31

// block 00472ef4
00472ef4  ret        
