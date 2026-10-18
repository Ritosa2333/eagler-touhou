0041cb10 55                       push       ebp
0041cb11 8bec                     mov        ebp, esp
0041cb13 83ec40                   sub        esp, 0x40
0041cb16 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0041cb1b 33c5                     xor        eax, ebp
0041cb1d 8945fc                   mov        dword ptr [ebp - 4], eax
0041cb20 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0041cb23 6a05                     push       5
0041cb25 b940025c00               mov        ecx, 0x5c0240
0041cb2a e8e1f7feff               call       0x40c310
0041cb2f 50                       push       eax
0041cb30 8d4df8                   lea        ecx, [ebp - 8]
0041cb33 e8f8ebfeff               call       0x40b730
0041cb38 90                       nop        
0041cb39 a1d0675b00               mov        eax, dword ptr [0x5b67d0]
0041cb3e 0b05d4675b00             or         eax, dword ptr [0x5b67d4]
0041cb44 0f8488000000             je         0x41cbd2
0041cb4a 8d4df0                   lea        ecx, [ebp - 0x10]
0041cb4d 51                       push       ecx
0041cb4e ff151cc15600             call       dword ptr [0x56c11c]
0041cb54 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041cb57 2b0dd8675b00             sub        ecx, dword ptr [0x5b67d8]
0041cb5d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0041cb60 1b15dc675b00             sbb        edx, dword ptr [0x5b67dc]
0041cb66 e895621200               call       0x542e00
0041cb6b 8b15d4675b00             mov        edx, dword ptr [0x5b67d4]
0041cb71 8b0dd0675b00             mov        ecx, dword ptr [0x5b67d0]
0041cb77 f20f1145d0               movsd      qword ptr [ebp - 0x30], xmm0
0041cb7c e87f621200               call       0x542e00
0041cb81 f20f104dd0               movsd      xmm1, qword ptr [ebp - 0x30]
0041cb86 f20f5ec8                 divsd      xmm1, xmm0
0041cb8a f20f114de8               movsd      qword ptr [ebp - 0x18], xmm1
0041cb8f f20f100538885b00         movsd      xmm0, qword ptr [0x5b8838]
0041cb97 660f2f45e8               comisd     xmm0, xmmword ptr [ebp - 0x18]
0041cb9c 760d                     jbe        0x41cbab
0041cb9e f20f1045e8               movsd      xmm0, qword ptr [ebp - 0x18]
0041cba3 f20f110538885b00         movsd      qword ptr [0x5b8838], xmm0
0041cbab f20f1045e8               movsd      xmm0, qword ptr [ebp - 0x18]
0041cbb0 f20f5c0538885b00         subsd      xmm0, qword ptr [0x5b8838]
0041cbb8 f20f1145c8               movsd      qword ptr [ebp - 0x38], xmm0
0041cbbd 8d4df8                   lea        ecx, [ebp - 8]
0041cbc0 e83bedfeff               call       0x40b900
0041cbc5 dd45c8                   fld        qword ptr [ebp - 0x38]
0041cbc8 e98f000000               jmp        0x41cc5c
0041cbcd e981000000               jmp        0x41cc53
0041cbd2 ff15e8c25600             call       dword ptr [0x56c2e8]
0041cbd8 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0041cbdb f20f2a45e4               cvtsi2sd   xmm0, dword ptr [ebp - 0x1c]
0041cbe0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0041cbe3 c1ea1f                   shr        edx, 0x1f
0041cbe6 f20f5804d5d0cd5600       addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
0041cbef f20f1145dc               movsd      qword ptr [ebp - 0x24], xmm0
0041cbf4 f20f100538885b00         movsd      xmm0, qword ptr [0x5b8838]
0041cbfc 660f2f45dc               comisd     xmm0, xmmword ptr [ebp - 0x24]
0041cc01 760d                     jbe        0x41cc10
0041cc03 f20f1045dc               movsd      xmm0, qword ptr [ebp - 0x24]
0041cc08 f20f110538885b00         movsd      qword ptr [0x5b8838], xmm0
0041cc10 f20f100538885b00         movsd      xmm0, qword ptr [0x5b8838]
0041cc18 f20f590588cd5600         mulsd      xmm0, qword ptr [0x56cd88]
0041cc20 f20f1145e8               movsd      qword ptr [ebp - 0x18], xmm0
0041cc25 f20f1045dc               movsd      xmm0, qword ptr [ebp - 0x24]
0041cc2a f20f5c45e8               subsd      xmm0, qword ptr [ebp - 0x18]
0041cc2f f20f5e0588cd5600         divsd      xmm0, qword ptr [0x56cd88]
0041cc37 f20f1145e8               movsd      qword ptr [ebp - 0x18], xmm0
0041cc3c f20f1045e8               movsd      xmm0, qword ptr [ebp - 0x18]
0041cc41 f20f1145c0               movsd      qword ptr [ebp - 0x40], xmm0
0041cc46 8d4df8                   lea        ecx, [ebp - 8]
0041cc49 e8b2ecfeff               call       0x40b900
0041cc4e dd45c0                   fld        qword ptr [ebp - 0x40]
0041cc51 eb09                     jmp        0x41cc5c
0041cc53 8d4df8                   lea        ecx, [ebp - 8]
0041cc56 e8a5ecfeff               call       0x40b900
0041cc5b 90                       nop        
0041cc5c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041cc5f 33cd                     xor        ecx, ebp
0041cc61 e8885d1200               call       0x5429ee
0041cc66 8be5                     mov        esp, ebp
0041cc68 5d                       pop        ebp
0041cc69 c3                       ret        
0041cc6a cc                       int3       
0041cc6b cc                       int3       
0041cc6c cc                       int3       
0041cc6d cc                       int3       
0041cc6e cc                       int3       
0041cc6f cc                       int3       
