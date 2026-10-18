004d00c0 55                       push       ebp
004d00c1 8bec                     mov        ebp, esp
004d00c3 6aff                     push       -1
004d00c5 688d835600               push       0x56838d
004d00ca 64a100000000             mov        eax, dword ptr fs:[0]
004d00d0 50                       push       eax
004d00d1 83ec20                   sub        esp, 0x20
004d00d4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d00d9 33c5                     xor        eax, ebp
004d00db 8945f0                   mov        dword ptr [ebp - 0x10], eax
004d00de 50                       push       eax
004d00df 8d45f4                   lea        eax, [ebp - 0xc]
004d00e2 64a300000000             mov        dword ptr fs:[0], eax
004d00e8 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d00eb 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004d00ee 8b4850                   mov        ecx, dword ptr [eax + 0x50]
004d00f1 51                       push       ecx
004d00f2 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004d00f8 e8a375faff               call       0x4776a0
004d00fd 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004d0100 8b4250                   mov        eax, dword ptr [edx + 0x50]
004d0103 50                       push       eax
004d0104 6a0c                     push       0xc
004d0106 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004d010c e8bff2f7ff               call       0x44f3d0
004d0111 90                       nop        
004d0112 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004d0115 83c110                   add        ecx, 0x10
004d0118 894de0                   mov        dword ptr [ebp - 0x20], ecx
004d011b 6a08                     push       8
004d011d 8d4de8                   lea        ecx, [ebp - 0x18]
004d0120 e85bbff3ff               call       0x40c080
004d0125 8d55e8                   lea        edx, [ebp - 0x18]
004d0128 52                       push       edx
004d0129 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d012c e84f21f4ff               call       0x412280
004d0131 c745fc00000000           mov        dword ptr [ebp - 4], 0
004d0138 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004d013b e80024f4ff               call       0x412540
004d0140 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004d0143 eb09                     jmp        0x4d014e
004d0145 8d4de8                   lea        ecx, [ebp - 0x18]
004d0148 e8e31af4ff               call       0x411c30
004d014d 90                       nop        
004d014e 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004d0151 50                       push       eax
004d0152 8d4de8                   lea        ecx, [ebp - 0x18]
004d0155 e8561af4ff               call       0x411bb0
004d015a 0fb6c8                   movzx      ecx, al
004d015d 85c9                     test       ecx, ecx
004d015f 7438                     je         0x4d0199
004d0161 8d4de8                   lea        ecx, [ebp - 0x18]
004d0164 e897c1f3ff               call       0x40c300
004d0169 8945dc                   mov        dword ptr [ebp - 0x24], eax
004d016c 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d016f e8eceff3ff               call       0x40f160
004d0174 8b10                     mov        edx, dword ptr [eax]
004d0176 837a2001                 cmp        dword ptr [edx + 0x20], 1
004d017a 741b                     je         0x4d0197
004d017c 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004d017f e8dceff3ff               call       0x40f160
004d0184 8b00                     mov        eax, dword ptr [eax]
004d0186 8945d8                   mov        dword ptr [ebp - 0x28], eax
004d0189 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d018c 8b11                     mov        edx, dword ptr [ecx]
004d018e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004d0191 8b4220                   mov        eax, dword ptr [edx + 0x20]
004d0194 ffd0                     call       eax
004d0196 90                       nop        
004d0197 ebac                     jmp        0x4d0145
004d0199 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004d01a0 8d4de8                   lea        ecx, [ebp - 0x18]
004d01a3 e85819f4ff               call       0x411b00
004d01a8 90                       nop        
004d01a9 b801000000               mov        eax, 1
004d01ae 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d01b1 64890d00000000           mov        dword ptr fs:[0], ecx
004d01b8 59                       pop        ecx
004d01b9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d01bc 33cd                     xor        ecx, ebp
004d01be e82b280700               call       0x5429ee
004d01c3 8be5                     mov        esp, ebp
004d01c5 5d                       pop        ebp
004d01c6 c3                       ret        
004d01c7 cc                       int3       
004d01c8 cc                       int3       
004d01c9 cc                       int3       
004d01ca cc                       int3       
004d01cb cc                       int3       
004d01cc cc                       int3       
004d01cd cc                       int3       
004d01ce cc                       int3       
004d01cf cc                       int3       
