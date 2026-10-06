
// block 0040fbe0
0040fbe0  push       -1
0040fbe2  push       0x49750b
0040fbe7  mov        eax, dword ptr fs:[0]
0040fbed  push       eax
0040fbee  push       ebx
0040fbef  push       ebp
0040fbf0  push       esi
0040fbf1  push       edi
0040fbf2  mov        eax, dword ptr [0x4ad138]
0040fbf7  xor        eax, esp
0040fbf9  push       eax
0040fbfa  lea        eax, [esp + 0x14]
0040fbfe  mov        dword ptr fs:[0], eax
0040fc04  mov        eax, dword ptr [esp + 0x28]
0040fc08  mov        ebp, dword ptr [esp + 0x24]
0040fc0c  mov        esi, dword ptr [0x4b43d4]
0040fc12  lea        edi, [eax + eax*2]
0040fc15  movzx      eax, word ptr [edi*8 + 0x4af160]
0040fc1d  lea        edi, [edi*8 + 0x4af160]
0040fc24  mov        dword ptr [ebp], 0
0040fc2b  test       ax, ax
0040fc2e  jl         0x40fc9a

// block 0040fc30
0040fc30  mov        edx, dword ptr [esi + 0x10]
0040fc33  cwde       
0040fc34  push       0
0040fc36  push       eax
0040fc37  lea        ecx, [esp + 0x30]
0040fc3b  push       ecx
0040fc3c  push       edx
0040fc3d  mov        eax, 0x17
0040fc42  xor        ecx, ecx
0040fc44  call       0x4615a0

// block 0040fc49
0040fc49  mov        eax, dword ptr [esp + 0x28]
0040fc4d  mov        edx, dword ptr [0x4ce8cc]
0040fc53  push       eax
0040fc54  mov        dword ptr [ebp], eax
0040fc57  call       0x461920

// block 0040fc5c
0040fc5c  mov        esi, eax
0040fc5e  test       esi, esi
0040fc60  jne        0x40fc65

// block 0040fc62
0040fc62  mov        dword ptr [ebp], eax

// block 0040fc65
0040fc65  mov        eax, dword ptr [edi + 4]
0040fc68  test       eax, eax
0040fc6a  je         0x40fc74

// block 0040fc6c
0040fc6c  mov        edx, dword ptr [esp + 0x2c]
0040fc70  mov        ecx, esi
0040fc72  call       eax

// block 0040fc74
0040fc74  mov        eax, dword ptr [edi + 8]
0040fc77  mov        dword ptr [esi + 0x488], eax
0040fc7d  mov        ecx, dword ptr [edi + 0xc]
0040fc80  mov        dword ptr [esi + 0x48c], ecx
0040fc86  mov        edx, dword ptr [edi + 0x10]
0040fc89  mov        dword ptr [esi + 0x490], edx
0040fc8f  mov        eax, dword ptr [edi + 0x14]
0040fc92  mov        dword ptr [esi + 0x494], eax
0040fc98  jmp        0x40fd0c

// block 0040fc9a
0040fc9a  cmp        ax, -1
0040fc9e  jne        0x40fd0c

// block 0040fca0
0040fca0  push       0x20
0040fca2  call       0x46c9ea

// block 0040fca7
0040fca7  add        esp, 4
0040fcaa  test       eax, eax
0040fcac  je         0x40fccb

// block 0040fcae
0040fcae  xor        ecx, ecx
0040fcb0  mov        dword ptr [eax], ecx
0040fcb2  mov        dword ptr [eax + 4], ecx
0040fcb5  mov        dword ptr [eax + 8], ecx
0040fcb8  mov        dword ptr [eax + 0xc], ecx
0040fcbb  mov        dword ptr [eax + 0x10], ecx
0040fcbe  mov        dword ptr [eax + 0x14], ecx
0040fcc1  mov        dword ptr [eax + 0x18], ecx
0040fcc4  mov        dword ptr [eax + 0x1c], ecx
0040fcc7  mov        ebx, eax
0040fcc9  jmp        0x40fccd

// block 0040fccb
0040fccb  xor        ebx, ebx

// block 0040fccd
0040fccd  lea        eax, [esi + 0x18]
0040fcd0  mov        edx, ebx
0040fcd2  call       0x4109f0

// block 0040fcd7
0040fcd7  push       0x18
0040fcd9  call       0x46c9ea

// block 0040fcde
0040fcde  mov        esi, eax
0040fce0  add        esp, 4
0040fce3  mov        dword ptr [esp + 0x28], esi
0040fce7  xor        eax, eax
0040fce9  mov        dword ptr [esp + 0x1c], eax
0040fced  cmp        esi, eax
0040fcef  je         0x40fcfd

// block 0040fcf1
0040fcf1  push       0x10
0040fcf3  mov        eax, 0x10
0040fcf8  call       0x40ff10

// block 0040fcfd
0040fcfd  mov        dword ptr [ebx + 0x14], eax
0040fd00  mov        ecx, dword ptr [edi + 8]
0040fd03  mov        dword ptr [ebx + 0x18], ecx
0040fd06  mov        edx, dword ptr [edi + 0xc]
0040fd09  mov        dword ptr [ebx + 0x1c], edx

// block 0040fd0c
0040fd0c  mov        eax, ebp
0040fd0e  mov        ecx, dword ptr [esp + 0x14]
0040fd12  mov        dword ptr fs:[0], ecx
0040fd19  pop        ecx
0040fd1a  pop        edi
0040fd1b  pop        esi
0040fd1c  pop        ebp
0040fd1d  pop        ebx
0040fd1e  add        esp, 0xc
0040fd21  ret        0xc
