004c0120 55                       push       ebp
004c0121 8bec                     mov        ebp, esp
004c0123 6aff                     push       -1
004c0125 68d0995600               push       0x5699d0
004c012a 64a100000000             mov        eax, dword ptr fs:[0]
004c0130 50                       push       eax
004c0131 83ec1c                   sub        esp, 0x1c
004c0134 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c0139 33c5                     xor        eax, ebp
004c013b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c013e 50                       push       eax
004c013f 8d45f4                   lea        eax, [ebp - 0xc]
004c0142 64a300000000             mov        dword ptr fs:[0], eax
004c0148 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004c014b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c014e c700880c5700             mov        dword ptr [eax], 0x570c88
004c0154 68ac0c5700               push       0x570cac
004c0159 e852c5f4ff               call       0x40c6b0
004c015e 83c404                   add        esp, 4
004c0161 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0164 8b5108                   mov        edx, dword ptr [ecx + 8]
004c0167 52                       push       edx
004c0168 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004c016e e83d23f5ff               call       0x4124b0
004c0173 90                       nop        
004c0174 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c0177 0514c40000               add        eax, 0xc414
004c017c 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c017f 6a08                     push       8
004c0181 8d4de8                   lea        ecx, [ebp - 0x18]
004c0184 e8f7bef4ff               call       0x40c080
004c0189 8d4de8                   lea        ecx, [ebp - 0x18]
004c018c 51                       push       ecx
004c018d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c0190 e8eb20f5ff               call       0x412280
004c0195 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c0198 e8a323f5ff               call       0x412540
004c019d 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c01a0 eb09                     jmp        0x4c01ab
004c01a2 8d4de8                   lea        ecx, [ebp - 0x18]
004c01a5 e8861af5ff               call       0x411c30
004c01aa 90                       nop        
004c01ab 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004c01ae 52                       push       edx
004c01af 8d4de8                   lea        ecx, [ebp - 0x18]
004c01b2 e8f919f5ff               call       0x411bb0
004c01b7 0fb6c0                   movzx      eax, al
004c01ba 85c0                     test       eax, eax
004c01bc 741d                     je         0x4c01db
004c01be 8d4de8                   lea        ecx, [ebp - 0x18]
004c01c1 e83ac1f4ff               call       0x40c300
004c01c6 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c01c9 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c01cc e88feff4ff               call       0x40f160
004c01d1 8b08                     mov        ecx, dword ptr [eax]
004c01d3 e8581d0000               call       0x4c1f30
004c01d8 90                       nop        
004c01d9 ebc7                     jmp        0x4c01a2
004c01db 8d4de8                   lea        ecx, [ebp - 0x18]
004c01de e81d19f5ff               call       0x411b00
004c01e3 90                       nop        
004c01e4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c01e7 e804fcf5ff               call       0x41fdf0
004c01ec 90                       nop        
004c01ed 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c01f0 64890d00000000           mov        dword ptr fs:[0], ecx
004c01f7 59                       pop        ecx
004c01f8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c01fb 33cd                     xor        ecx, ebp
004c01fd e8ec270800               call       0x5429ee
004c0202 8be5                     mov        esp, ebp
004c0204 5d                       pop        ebp
004c0205 c3                       ret        
004c0206 cc                       int3       
004c0207 cc                       int3       
004c0208 cc                       int3       
004c0209 cc                       int3       
004c020a cc                       int3       
004c020b cc                       int3       
004c020c cc                       int3       
004c020d cc                       int3       
004c020e cc                       int3       
004c020f cc                       int3       
