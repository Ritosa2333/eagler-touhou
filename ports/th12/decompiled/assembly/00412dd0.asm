
// block 00412dd0
00412dd0  push       ecx
00412dd1  push       edi
00412dd2  mov        edi, dword ptr [esp + 0xc]
00412dd6  cmp        dword ptr [edi], 0x4d494e41
00412ddc  mov        dword ptr [esp + 4], ecx
00412de0  je         0x412de9

// block 00412de2
00412de2  xor        eax, eax
00412de4  pop        edi
00412de5  pop        ecx
00412de6  ret        4

// block 00412de9
00412de9  push       ebx
00412dea  push       ebp
00412deb  push       esi
00412dec  xor        esi, esi
00412dee  lea        ebx, [edi + 8]
00412df1  cmp        dword ptr [edi + 4], esi
00412df4  jbe        0x412e3a

// block 00412df6
00412df6  lea        ebp, [esi + 0x44]
00412df9  lea        esp, [esp]

// block 00412e00
00412e00  lea        ecx, [esi + 8]
00412e03  call       0x45fe60

// block 00412e08
00412e08  mov        ecx, dword ptr [0x4b43dc]
00412e0e  mov        dword ptr [ecx + ebp], eax
00412e11  test       eax, eax
00412e13  je         0x412e6b

// block 00412e15
00412e15  mov        eax, ebx
00412e17  lea        edx, [eax + 1]
00412e1a  lea        ebx, [ebx]

// block 00412e20
00412e20  mov        cl, byte ptr [eax]
00412e22  inc        eax
00412e23  test       cl, cl
00412e25  jne        0x412e20

// block 00412e27
00412e27  sub        eax, edx
00412e29  inc        esi
00412e2a  add        ebp, 4
00412e2d  lea        ebx, [ebx + eax + 1]
00412e31  cmp        esi, dword ptr [edi + 4]
00412e34  jb         0x412e00

// block 00412e36
00412e36  mov        ecx, dword ptr [esp + 0x10]

// block 00412e3a
00412e3a  mov        eax, ebx
00412e3c  sub        eax, edi
00412e3e  and        eax, 0x80000003
00412e43  jns        0x412e4a

// block 00412e45
00412e45  dec        eax
00412e46  or         eax, 0xfffffffc
00412e49  inc        eax

// block 00412e4a
00412e4a  je         0x412e55

// block 00412e4c
00412e4c  mov        edx, 4
00412e51  sub        edx, eax
00412e53  add        ebx, edx

// block 00412e55
00412e55  cmp        dword ptr [ebx], 0x494c4345
00412e5b  mov        edi, ebx
00412e5d  jne        0x412eb4

// block 00412e5f
00412e5f  xor        esi, esi
00412e61  add        ebx, 8
00412e64  cmp        dword ptr [edi + 4], esi
00412e67  jbe        0x412eb4

// block 00412e69
00412e69  jmp        0x412e94

// block 00412e6b
00412e6b  push       0x49f434
00412e70  mov        ecx, 0x4b0ec8
00412e75  call       0x464220

// block 00412e7a
00412e7a  add        esp, 4
00412e7d  pop        esi
00412e7e  pop        ebp
00412e7f  pop        ebx
00412e80  or         eax, 0xffffffff
00412e83  pop        edi
00412e84  pop        ecx
00412e85  ret        4

// block 00412e90
00412e90  mov        ecx, dword ptr [esp + 0x10]

// block 00412e94
00412e94  mov        eax, dword ptr [ecx]
00412e96  mov        edx, dword ptr [eax + 8]
00412e99  push       ebx
00412e9a  call       edx

// block 00412e9c
00412e9c  mov        eax, ebx
00412e9e  lea        edx, [eax + 1]

// block 00412ea1
00412ea1  mov        cl, byte ptr [eax]
00412ea3  inc        eax
00412ea4  test       cl, cl
00412ea6  jne        0x412ea1

// block 00412ea8
00412ea8  sub        eax, edx
00412eaa  inc        esi
00412eab  lea        ebx, [ebx + eax + 1]
00412eaf  cmp        esi, dword ptr [edi + 4]
00412eb2  jb         0x412e90

// block 00412eb4
00412eb4  pop        esi
00412eb5  pop        ebp
00412eb6  pop        ebx
00412eb7  xor        eax, eax
00412eb9  pop        edi
00412eba  pop        ecx
00412ebb  ret        4
