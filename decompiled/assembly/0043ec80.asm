
// block 0043ec80
0043ec80  sub        esp, 0xc
0043ec83  fldz       
0043ec85  push       ebx
0043ec86  fst        dword ptr [esp + 4]
0043ec8a  push       esi
0043ec8b  mov        esi, dword ptr [0x4b43b8]
0043ec91  fst        dword ptr [esp + 0xc]
0043ec95  push       0x4a176c
0043ec9a  fstp       dword ptr [esp + 0x14]
0043ec9e  lea        ebx, [esp + 0xc]
0043eca2  call       0x401720

// block 0043eca7
0043eca7  mov        eax, dword ptr [edi + 0x30]
0043ecaa  add        esp, 4
0043ecad  sub        eax, 1
0043ecb0  je         0x43ed94

// block 0043ecb6
0043ecb6  sub        eax, 1
0043ecb9  je         0x43ed31

// block 0043ecbb
0043ecbb  sub        eax, 1
0043ecbe  jne        0x43ee8f

// block 0043ecc4
0043ecc4  fld        dword ptr [0x4a3d78]
0043ecca  mov        esi, dword ptr [0x4b43b8]
0043ecd0  fstp       dword ptr [esp + 8]
0043ecd4  mov        dword ptr [esi + 0x18f80], 0xffa0a0a0
0043ecde  fld        dword ptr [0x4a3e68]
0043ece4  fstp       dword ptr [esp + 0xc]
0043ece8  fldz       
0043ecea  fstp       dword ptr [esp + 0x10]
0043ecee  fld        dword ptr [edi + 0x2c8]
0043ecf4  call       0x4931e0

// block 0043ecf9
0043ecf9  fld        dword ptr [edi + 0x2c4]
0043ecff  push       eax
0043ed00  call       0x4931e0

// block 0043ed05
0043ed05  push       eax
0043ed06  push       0x49f890
0043ed0b  lea        ebx, [esp + 0x14]
0043ed0f  call       0x401720

// block 0043ed14
0043ed14  mov        eax, dword ptr [0x4b43b8]
0043ed19  add        esp, 0xc
0043ed1c  pop        esi
0043ed1d  mov        dword ptr [eax + 0x18f80], 0xffffffff
0043ed27  mov        eax, 1
0043ed2c  pop        ebx
0043ed2d  add        esp, 0xc
0043ed30  ret        

// block 0043ed31
0043ed31  mov        esi, dword ptr [0x4b43b8]
0043ed37  fld        dword ptr [0x4a3d78]
0043ed3d  fstp       dword ptr [esp + 8]
0043ed41  mov        dword ptr [esi + 0x18f80], 0xffff4040
0043ed4b  mov        ecx, dword ptr [edi + 0x114]
0043ed51  fld        dword ptr [0x4a3e68]
0043ed57  mov        edx, dword ptr [edi + 0x34]
0043ed5a  fstp       dword ptr [esp + 0xc]
0043ed5e  mov        eax, dword ptr [edx + ecx*4]
0043ed61  fldz       
0043ed63  push       eax
0043ed64  fstp       dword ptr [esp + 0x14]
0043ed68  push       0x49f844
0043ed6d  lea        ebx, [esp + 0x10]
0043ed71  call       0x401720

// block 0043ed76
0043ed76  mov        ecx, dword ptr [0x4b43b8]
0043ed7c  add        esp, 8
0043ed7f  pop        esi
0043ed80  mov        dword ptr [ecx + 0x18f80], 0xffffffff
0043ed8a  mov        eax, 1
0043ed8f  pop        ebx
0043ed90  add        esp, 0xc
0043ed93  ret        

// block 0043ed94
0043ed94  cmp        dword ptr [edi + 0x38], 0
0043ed98  fld        dword ptr [0x4a3f80]
0043ed9e  mov        esi, dword ptr [0x4b43b8]
0043eda4  fstp       dword ptr [esp + 8]
0043eda8  fld        dword ptr [0x4a3e68]
0043edae  lea        ebx, [esp + 8]
0043edb2  fstp       dword ptr [esp + 0xc]
0043edb6  fldz       
0043edb8  fstp       dword ptr [esp + 0x10]
0043edbc  je         0x43edda

// block 0043edbe
0043edbe  mov        edx, dword ptr [edi + 0x114]
0043edc4  mov        eax, dword ptr [edi + 0x34]
0043edc7  mov        ecx, dword ptr [eax + edx*4]
0043edca  push       ecx
0043edcb  push       0x49f850
0043edd0  call       0x401720

// block 0043edd5
0043edd5  add        esp, 8
0043edd8  jmp        0x43ede7

// block 0043edda
0043edda  push       0x49f858
0043eddf  call       0x401720

// block 0043ede4
0043ede4  add        esp, 4

// block 0043ede7
0043ede7  mov        eax, dword ptr [edi + 0x2d8]
0043eded  fld        dword ptr [0x4a3f7c]
0043edf3  mov        esi, dword ptr [0x4b43b8]
0043edf9  fstp       dword ptr [esp + 0xc]
0043edfd  lea        ebx, [esp + 8]
0043ee01  test       eax, eax
0043ee03  je         0x43ee24

// block 0043ee05
0043ee05  mov        edx, dword ptr [edi + 0x1ec]
0043ee0b  mov        eax, dword ptr [eax + 0x8c]
0043ee11  mov        ecx, dword ptr [eax + edx*8]
0043ee14  push       ecx
0043ee15  push       0x49f868
0043ee1a  call       0x401720

// block 0043ee1f
0043ee1f  add        esp, 8
0043ee22  jmp        0x43ee31

// block 0043ee24
0043ee24  push       0x4a1778
0043ee29  call       0x401720

// block 0043ee2e
0043ee2e  add        esp, 4

// block 0043ee31
0043ee31  fld        dword ptr [0x4a3f78]
0043ee37  mov        esi, dword ptr [0x4b43b8]
0043ee3d  push       0x49f878
0043ee42  fstp       dword ptr [esp + 0x10]
0043ee46  lea        ebx, [esp + 0xc]
0043ee4a  call       0x401720

// block 0043ee4f
0043ee4f  fld        dword ptr [0x4a3d78]
0043ee55  mov        esi, dword ptr [0x4b43b8]
0043ee5b  fstp       dword ptr [esp + 0xc]
0043ee5f  fild       dword ptr [edi + 0x3c]
0043ee62  push       0x49f880
0043ee67  fmul       qword ptr [0x4a3e40]
0043ee6d  fadd       qword ptr [0x4a3d68]
0043ee73  fstp       dword ptr [esp + 0x14]
0043ee77  call       0x401720

// block 0043ee7c
0043ee7c  mov        edx, dword ptr [0x4b43b8]
0043ee82  mov        dword ptr [edx + 0x18f80], 0xffffffff
0043ee8c  add        esp, 8

// block 0043ee8f
0043ee8f  pop        esi
0043ee90  mov        eax, 1
0043ee95  pop        ebx
0043ee96  add        esp, 0xc
0043ee99  ret        
