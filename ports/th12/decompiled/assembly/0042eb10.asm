
// block 0042eb10
0042eb10  push       ebx
0042eb11  push       ebp
0042eb12  mov        ebp, dword ptr [esp + 0xc]
0042eb16  push       esi
0042eb17  push       edi
0042eb18  push       0x24
0042eb1a  call       0x46c9ea

// block 0042eb1f
0042eb1f  xor        edi, edi
0042eb21  add        esp, 4
0042eb24  cmp        eax, edi
0042eb26  je         0x42eb44

// block 0042eb28
0042eb28  and        dword ptr [eax + 4], 0xfffffffe
0042eb2c  mov        dword ptr [eax + 8], edi
0042eb2f  mov        dword ptr [eax + 0xc], edi
0042eb32  mov        dword ptr [eax + 0x10], edi
0042eb35  mov        dword ptr [eax], edi
0042eb37  mov        dword ptr [eax + 0x14], eax
0042eb3a  mov        dword ptr [eax + 0x18], edi
0042eb3d  mov        dword ptr [eax + 0x1c], edi
0042eb40  mov        esi, eax
0042eb42  jmp        0x42eb46

// block 0042eb44
0042eb44  xor        esi, esi

// block 0042eb46
0042eb46  mov        eax, dword ptr [esi + 4]
0042eb49  and        eax, 0xfffffffd
0042eb4c  or         eax, 1
0042eb4f  mov        ebx, 3
0042eb54  mov        dword ptr [esi + 8], 0x42ef70
0042eb5b  mov        dword ptr [esi + 0xc], edi
0042eb5e  mov        dword ptr [esi + 0x10], edi
0042eb61  mov        dword ptr [esi + 0x20], ebp
0042eb64  mov        dword ptr [esi + 4], eax
0042eb67  call       0x462380

// block 0042eb6c
0042eb6c  push       0x24
0042eb6e  mov        dword ptr [ebp + 8], esi
0042eb71  call       0x46c9ea

// block 0042eb76
0042eb76  add        esp, 4
0042eb79  cmp        eax, edi
0042eb7b  je         0x42eb99

// block 0042eb7d
0042eb7d  and        dword ptr [eax + 4], 0xfffffffe
0042eb81  mov        dword ptr [eax + 8], edi
0042eb84  mov        dword ptr [eax + 0xc], edi
0042eb87  mov        dword ptr [eax + 0x10], edi
0042eb8a  mov        dword ptr [eax], edi
0042eb8c  mov        dword ptr [eax + 0x14], eax
0042eb8f  mov        dword ptr [eax + 0x18], edi
0042eb92  mov        dword ptr [eax + 0x1c], edi
0042eb95  mov        esi, eax
0042eb97  jmp        0x42eb9b

// block 0042eb99
0042eb99  xor        esi, esi

// block 0042eb9b
0042eb9b  mov        ecx, dword ptr [esi + 4]
0042eb9e  and        ecx, 0xfffffffd
0042eba1  or         ecx, 1
0042eba4  mov        ebx, 0x37
0042eba9  mov        dword ptr [esi + 8], 0x42ef80
0042ebb0  mov        dword ptr [esi + 0xc], edi
0042ebb3  mov        dword ptr [esi + 0x10], edi
0042ebb6  mov        dword ptr [esi + 0x20], ebp
0042ebb9  mov        dword ptr [esi + 4], ecx
0042ebbc  call       0x462420

// block 0042ebc1
0042ebc1  mov        dword ptr [ebp + 0xc], esi
0042ebc4  lea        esi, [ebp + 0x10]
0042ebc7  call       0x464c40

// block 0042ebcc
0042ebcc  lea        edx, [esi + 8]
0042ebcf  push       edx
0042ebd0  push       edi
0042ebd1  push       ebp
0042ebd2  push       0x42e9b0
0042ebd7  push       edi
0042ebd8  push       edi
0042ebd9  mov        dword ptr [esi + 0x18], 0x42e9b0
0042ebe0  mov        dword ptr [esi + 0x10], 1
0042ebe7  mov        dword ptr [esi + 0xc], edi
0042ebea  call       0x46e54b

// block 0042ebef
0042ebef  add        esp, 0x18
0042ebf2  pop        edi
0042ebf3  mov        dword ptr [esi + 4], eax
0042ebf6  pop        esi
0042ebf7  pop        ebp
0042ebf8  xor        eax, eax
0042ebfa  pop        ebx
0042ebfb  ret        4
