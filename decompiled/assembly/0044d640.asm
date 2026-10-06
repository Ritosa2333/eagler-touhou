
// block 0044d640
0044d640  sub        esp, 0x6c
0044d643  push       ebp
0044d644  mov        ebp, dword ptr [esp + 0x74]
0044d648  call       0x44d5b0

// block 0044d64d
0044d64d  push       0x6c
0044d64f  lea        eax, [esp + 8]
0044d653  push       0
0044d655  push       eax
0044d656  call       0x477420

// block 0044d65b
0044d65b  mov        ecx, dword ptr [0x4aeb20]
0044d661  add        esp, 0xc
0044d664  xor        eax, eax
0044d666  cmp        ecx, -1
0044d669  je         0x44d684

// block 0044d66b
0044d66b  jmp        0x44d670

// block 0044d670
0044d670  cmp        ecx, ebp
0044d672  je         0x44d684

// block 0044d674
0044d674  inc        eax
0044d675  lea        ecx, [eax + eax*2]
0044d678  mov        ecx, dword ptr [ecx*8 + 0x4aeb20]
0044d67f  cmp        ecx, -1
0044d682  jne        0x44d670

// block 0044d684
0044d684  cmp        ebp, -1
0044d687  je         0x44d697

// block 0044d689
0044d689  lea        ecx, [eax + eax*2]
0044d68c  lea        ecx, [ecx*8 + 0x4aeb20]
0044d693  test       ecx, ecx
0044d695  jne        0x44d6a0

// block 0044d697
0044d697  xor        al, al
0044d699  pop        ebp
0044d69a  add        esp, 0x6c
0044d69d  ret        4

// block 0044d6a0
0044d6a0  mov        eax, dword ptr [ecx + 4]
0044d6a3  shl        eax, 0xa
0044d6a6  cdq        
0044d6a7  and        edx, 7
0044d6aa  add        eax, edx
0044d6ac  sar        eax, 3
0044d6af  add        eax, 3
0044d6b2  cdq        
0044d6b3  and        edx, 3
0044d6b6  add        eax, edx
0044d6b8  push       esi
0044d6b9  mov        esi, eax
0044d6bb  mov        ax, word ptr [ecx + 4]
0044d6bf  sar        esi, 2
0044d6c2  add        esi, esi
0044d6c4  mov        edx, 1
0044d6c9  add        esi, esi
0044d6cb  mov        word ptr [esp + 0x14], dx
0044d6d0  mov        edx, esi
0044d6d2  shl        edx, 6
0044d6d5  mov        dword ptr [esp + 8], 0x6c
0044d6dd  mov        dword ptr [esp + 0xc], 0x400
0044d6e5  mov        dword ptr [esp + 0x10], 0xffffffbf
0044d6ed  mov        word ptr [esp + 0x16], ax
0044d6f2  mov        dword ptr [esp + 0x1c], edx
0044d6f6  cmp        ebp, 0x18
0044d6f9  je         0x44d724

// block 0044d6fb
0044d6fb  cmp        ebp, 0x16
0044d6fe  je         0x44d724

// block 0044d700
0044d700  mov        eax, dword ptr [ecx + 0xc]
0044d703  mov        edx, dword ptr [ecx + 0x10]
0044d706  mov        dword ptr [esp + 0x30], eax
0044d70a  mov        eax, dword ptr [ecx + 0x14]
0044d70d  mov        ecx, dword ptr [ecx + 8]
0044d710  mov        dword ptr [esp + 0x18], 3
0044d718  mov        dword ptr [esp + 0x34], edx
0044d71c  mov        dword ptr [esp + 0x38], eax
0044d720  mov        dword ptr [esp + 0x3c], ecx

// block 0044d724
0044d724  push       ebx
0044d725  push       0
0044d727  push       0
0044d729  lea        edx, [esp + 0x84]
0044d730  push       edx
0044d731  push       0
0044d733  lea        eax, [esp + 0x1c]
0044d737  push       eax
0044d738  push       0
0044d73a  call       dword ptr [0x498024]

// block 0044d740
0044d740  mov        ebx, eax
0044d742  test       ebx, ebx
0044d744  jne        0x44d751

// block 0044d746
0044d746  pop        ebx
0044d747  pop        esi
0044d748  xor        al, al
0044d74a  pop        ebp
0044d74b  add        esp, 0x6c
0044d74e  ret        4

// block 0044d751
0044d751  mov        ecx, dword ptr [esp + 0x20]
0044d755  mov        edx, dword ptr [esp + 0x7c]
0044d759  push       edi
0044d75a  push       ecx
0044d75b  push       0
0044d75d  push       edx
0044d75e  call       0x477420

// block 0044d763
0044d763  add        esp, 0xc
0044d766  push       0
0044d768  call       dword ptr [0x498020]

// block 0044d76e
0044d76e  mov        edi, eax
0044d770  push       ebx
0044d771  push       edi
0044d772  call       dword ptr [0x49802c]

// block 0044d778
0044d778  mov        ecx, dword ptr [esp + 0x80]
0044d77f  mov        edx, dword ptr [esp + 0x24]
0044d783  mov        dword ptr [0x4b0e5c], edi
0044d789  pop        edi
0044d78a  mov        dword ptr [0x4b0e64], ebx
0044d790  pop        ebx
0044d791  mov        dword ptr [0x4b0e58], esi
0044d797  pop        esi
0044d798  mov        dword ptr [0x4b0e60], eax
0044d79d  mov        dword ptr [0x4b0e48], ebp
0044d7a3  mov        dword ptr [0x4b0e68], ecx
0044d7a9  mov        dword ptr [0x4b0e54], edx
0044d7af  mov        dword ptr [0x4b0e4c], 0x400
0044d7b9  mov        dword ptr [0x4b0e50], 0x40
0044d7c3  mov        al, 1
0044d7c5  pop        ebp
0044d7c6  add        esp, 0x6c
0044d7c9  ret        4
