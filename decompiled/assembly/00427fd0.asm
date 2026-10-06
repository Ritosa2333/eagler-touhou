
// block 00427fd0
00427fd0  push       ebx
00427fd1  push       ebp
00427fd2  mov        ebp, dword ptr [esp + 0xc]
00427fd6  push       edi
00427fd7  mov        ebx, 0x49f69c
00427fdc  mov        ecx, 6
00427fe1  call       0x45fe60

// block 00427fe6
00427fe6  xor        edi, edi
00427fe8  mov        dword ptr [ebp + 0x488], eax
00427fee  cmp        eax, edi
00427ff0  jne        0x42800d

// block 00427ff2
00427ff2  push       0x49f6a8
00427ff7  mov        ecx, 0x4b0ec8
00427ffc  call       0x464220

// block 00428001
00428001  add        esp, 4
00428004  pop        edi
00428005  pop        ebp
00428006  or         eax, 0xffffffff
00428009  pop        ebx
0042800a  ret        4

// block 0042800d
0042800d  push       esi
0042800e  push       0x24
00428010  call       0x46c9ea

// block 00428015
00428015  add        esp, 4
00428018  cmp        eax, edi
0042801a  je         0x428038

// block 0042801c
0042801c  and        dword ptr [eax + 4], 0xfffffffe
00428020  mov        dword ptr [eax + 8], edi
00428023  mov        dword ptr [eax + 0xc], edi
00428026  mov        dword ptr [eax + 0x10], edi
00428029  mov        dword ptr [eax], edi
0042802b  mov        dword ptr [eax + 0x14], eax
0042802e  mov        dword ptr [eax + 0x18], edi
00428031  mov        dword ptr [eax + 0x1c], edi
00428034  mov        esi, eax
00428036  jmp        0x42803a

// block 00428038
00428038  xor        esi, esi

// block 0042803a
0042803a  mov        eax, dword ptr [esi + 4]
0042803d  and        eax, 0xfffffffd
00428040  or         eax, 1
00428043  mov        ebx, 0x14
00428048  mov        dword ptr [esi + 8], 0x4283d0
0042804f  mov        dword ptr [esi + 0xc], edi
00428052  mov        dword ptr [esi + 0x10], edi
00428055  mov        dword ptr [esi + 0x20], ebp
00428058  mov        dword ptr [esi + 4], eax
0042805b  call       0x462380

// block 00428060
00428060  push       0x24
00428062  mov        dword ptr [ebp + 8], esi
00428065  call       0x46c9ea

// block 0042806a
0042806a  add        esp, 4
0042806d  cmp        eax, edi
0042806f  je         0x42808d

// block 00428071
00428071  and        dword ptr [eax + 4], 0xfffffffe
00428075  mov        dword ptr [eax + 8], edi
00428078  mov        dword ptr [eax + 0xc], edi
0042807b  mov        dword ptr [eax + 0x10], edi
0042807e  mov        dword ptr [eax], edi
00428080  mov        dword ptr [eax + 0x14], eax
00428083  mov        dword ptr [eax + 0x18], edi
00428086  mov        dword ptr [eax + 0x1c], edi
00428089  mov        esi, eax
0042808b  jmp        0x42808f

// block 0042808d
0042808d  xor        esi, esi

// block 0042808f
0042808f  mov        ecx, dword ptr [esi + 4]
00428092  and        ecx, 0xfffffffd
00428095  or         ecx, 1
00428098  mov        ebx, 0x1d
0042809d  mov        dword ptr [esi + 8], 0x428430
004280a4  mov        dword ptr [esi + 0xc], edi
004280a7  mov        dword ptr [esi + 0x10], edi
004280aa  mov        dword ptr [esi + 0x20], ebp
004280ad  mov        dword ptr [esi + 4], ecx
004280b0  call       0x462420

// block 004280b5
004280b5  mov        dword ptr [ebp + 0xc], esi
004280b8  pop        esi
004280b9  lea        edx, [ebp + 0x10]
004280bc  pop        edi
004280bd  mov        dword ptr [ebp + 0x464], edx
004280c3  pop        ebp
004280c4  xor        eax, eax
004280c6  pop        ebx
004280c7  ret        4
