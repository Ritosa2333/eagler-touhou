004bce10 55                       push       ebp
004bce11 8bec                     mov        ebp, esp
004bce13 83ec1c                   sub        esp, 0x1c
004bce16 894dfc                   mov        dword ptr [ebp - 4], ecx
004bce19 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bce1f e80cbafcff               call       0x488830
004bce24 85c0                     test       eax, eax
004bce26 0f85bf000000             jne        0x4bceeb
004bce2c b9404d5c00               mov        ecx, 0x5c4d40
004bce31 e88af2feff               call       0x4ac0c0
004bce36 83f8ff                   cmp        eax, -1
004bce39 0f84ac000000             je         0x4bceeb
004bce3f b9404d5c00               mov        ecx, 0x5c4d40
004bce44 e877f2feff               call       0x4ac0c0
004bce49 83f803                   cmp        eax, 3
004bce4c 0f8499000000             je         0x4bceeb
004bce52 b958675b00               mov        ecx, 0x5b6758
004bce57 e8b4fcf5ff               call       0x41cb10
004bce5c dd5de4                   fstp       qword ptr [ebp - 0x1c]
004bce5f f20f1045e4               movsd      xmm0, qword ptr [ebp - 0x1c]
004bce64 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bce67 f20f5c80b0020000         subsd      xmm0, qword ptr [eax + 0x2b0]
004bce6f f20f1145ec               movsd      qword ptr [ebp - 0x14], xmm0
004bce74 f20f1045ec               movsd      xmm0, qword ptr [ebp - 0x14]
004bce79 660f2f0550cd5600         comisd     xmm0, xmmword ptr [0x56cd50]
004bce81 7255                     jb         0x4bced8
004bce83 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bce89 e85272faff               call       0x4640e0
004bce8e 8945f8                   mov        dword ptr [ebp - 8], eax
004bce91 f20f1045ec               movsd      xmm0, qword ptr [ebp - 0x14]
004bce96 f20f590570fe5600         mulsd      xmm0, qword ptr [0x56fe70]
004bce9e e87d5e0800               call       0x542d20
004bcea3 52                       push       edx
004bcea4 50                       push       eax
004bcea5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bcea8 e883000000               call       0x4bcf30
004bcead 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bceb3 e8982d0500               call       0x50fc50
004bceb8 8945f4                   mov        dword ptr [ebp - 0xc], eax
004bcebb f20f1045ec               movsd      xmm0, qword ptr [ebp - 0x14]
004bcec0 f20f590570fe5600         mulsd      xmm0, qword ptr [0x56fe70]
004bcec8 e8535e0800               call       0x542d20
004bcecd 52                       push       edx
004bcece 50                       push       eax
004bcecf 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bced2 e819000000               call       0x4bcef0
004bced7 90                       nop        
004bced8 b958675b00               mov        ecx, 0x5b6758
004bcedd e82efcf5ff               call       0x41cb10
004bcee2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bcee5 dd99b0020000             fstp       qword ptr [ecx + 0x2b0]
004bceeb 8be5                     mov        esp, ebp
004bceed 5d                       pop        ebp
004bceee c3                       ret        
004bceef cc                       int3       
