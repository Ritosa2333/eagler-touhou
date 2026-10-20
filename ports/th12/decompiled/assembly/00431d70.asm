
// block 00431d70
00431d70  push       ebp
00431d71  push       esi
00431d72  push       0x24
00431d74  call       0x46c9ea

// block 00431d79
00431d79  xor        ebp, ebp
00431d7b  add        esp, 4
00431d7e  cmp        eax, ebp
00431d80  je         0x431d9e

// block 00431d82
00431d82  and        dword ptr [eax + 4], 0xfffffffe
00431d86  mov        dword ptr [eax + 8], ebp
00431d89  mov        dword ptr [eax + 0xc], ebp
00431d8c  mov        dword ptr [eax + 0x10], ebp
00431d8f  mov        dword ptr [eax], ebp
00431d91  mov        dword ptr [eax + 0x14], eax
00431d94  mov        dword ptr [eax + 0x18], ebp
00431d97  mov        dword ptr [eax + 0x1c], ebp
00431d9a  mov        esi, eax
00431d9c  jmp        0x431da0

// block 00431d9e
00431d9e  xor        esi, esi

// block 00431da0
00431da0  mov        eax, dword ptr [esi + 4]
00431da3  and        eax, 0xfffffffd
00431da6  push       ebx
00431da7  or         eax, 1
00431daa  mov        ebx, 9
00431daf  mov        dword ptr [esi + 8], 0x432700
00431db6  mov        dword ptr [esi + 0xc], ebp
00431db9  mov        dword ptr [esi + 0x10], ebp
00431dbc  mov        dword ptr [esi + 0x20], edi
00431dbf  mov        dword ptr [esi + 4], eax
00431dc2  call       0x462380

// block 00431dc7
00431dc7  push       0x24
00431dc9  mov        dword ptr [edi + 8], esi
00431dcc  call       0x46c9ea

// block 00431dd1
00431dd1  add        esp, 4
00431dd4  cmp        eax, ebp
00431dd6  je         0x431df4

// block 00431dd8
00431dd8  and        dword ptr [eax + 4], 0xfffffffe
00431ddc  mov        dword ptr [eax + 8], ebp
00431ddf  mov        dword ptr [eax + 0xc], ebp
00431de2  mov        dword ptr [eax + 0x10], ebp
00431de5  mov        dword ptr [eax], ebp
00431de7  mov        dword ptr [eax + 0x14], eax
00431dea  mov        dword ptr [eax + 0x18], ebp
00431ded  mov        dword ptr [eax + 0x1c], ebp
00431df0  mov        esi, eax
00431df2  jmp        0x431df6

// block 00431df4
00431df4  xor        esi, esi

// block 00431df6
00431df6  mov        ecx, dword ptr [esi + 4]
00431df9  and        ecx, 0xfffffffd
00431dfc  or         ecx, 1
00431dff  mov        ebx, 0x3d
00431e04  mov        dword ptr [esi + 8], 0x432710
00431e0b  mov        dword ptr [esi + 0xc], ebp
00431e0e  mov        dword ptr [esi + 0x10], ebp
00431e11  mov        dword ptr [esi + 0x20], edi
00431e14  mov        dword ptr [esi + 4], ecx
00431e17  call       0x462420

// block 00431e1c
00431e1c  fldz       
00431e1e  mov        dword ptr [edi + 0xc], esi
00431e21  mov        eax, dword ptr [edi + 0x20]
00431e24  mov        edx, 0xfff0bdc1
00431e29  mov        ecx, 0x4b2ed0
00431e2e  pop        ebx
00431e2f  test       al, 1
00431e31  jne        0x431e45

// block 00431e33
00431e33  or         eax, 1
00431e36  fst        dword ptr [edi + 0x18]
00431e39  mov        dword ptr [edi + 0x14], ebp
00431e3c  mov        dword ptr [edi + 0x10], edx
00431e3f  mov        dword ptr [edi + 0x1c], ecx
00431e42  mov        dword ptr [edi + 0x20], eax

// block 00431e45
00431e45  or         esi, 0xffffffff
00431e48  fst        dword ptr [edi + 0x18]
00431e4b  mov        dword ptr [edi + 0x14], ebp
00431e4e  mov        dword ptr [edi + 0x10], esi
00431e51  mov        eax, dword ptr [edi + 0x34]
00431e54  test       al, 1
00431e56  jne        0x431e6a

// block 00431e58
00431e58  or         eax, 1
00431e5b  fst        dword ptr [edi + 0x2c]
00431e5e  mov        dword ptr [edi + 0x28], ebp
00431e61  mov        dword ptr [edi + 0x24], edx
00431e64  mov        dword ptr [edi + 0x30], ecx
00431e67  mov        dword ptr [edi + 0x34], eax

// block 00431e6a
00431e6a  mov        dword ptr [edi + 0x24], esi
00431e6d  fstp       dword ptr [edi + 0x2c]
00431e70  pop        esi
00431e71  mov        dword ptr [edi + 0x28], ebp
00431e74  xor        eax, eax
00431e76  pop        ebp
00431e77  ret        
