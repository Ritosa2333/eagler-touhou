004a0df0 55                       push       ebp
004a0df1 8bec                     mov        ebp, esp
004a0df3 6aff                     push       -1
004a0df5 688d805600               push       0x56808d
004a0dfa 64a100000000             mov        eax, dword ptr fs:[0]
004a0e00 50                       push       eax
004a0e01 83ec14                   sub        esp, 0x14
004a0e04 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004a0e09 33c5                     xor        eax, ebp
004a0e0b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004a0e0e 50                       push       eax
004a0e0f 8d45f4                   lea        eax, [ebp - 0xc]
004a0e12 64a300000000             mov        dword ptr fs:[0], eax
004a0e18 894de8                   mov        dword ptr [ebp - 0x18], ecx
004a0e1b 6a14                     push       0x14
004a0e1d b940025c00               mov        ecx, 0x5c0240
004a0e22 e8e9b4f6ff               call       0x40c310
004a0e27 50                       push       eax
004a0e28 8d4dec                   lea        ecx, [ebp - 0x14]
004a0e2b e800a9f6ff               call       0x40b730
004a0e30 c745fc00000000           mov        dword ptr [ebp - 4], 0
004a0e37 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004a0e3a e87131fcff               call       0x463fb0
004a0e3f 90                       nop        
004a0e40 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004a0e43 e89832fcff               call       0x4640e0
004a0e48 83c016                   add        eax, 0x16
004a0e4b 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004a0e4e 8b4508                   mov        eax, dword ptr [ebp + 8]
004a0e51 50                       push       eax
004a0e52 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a0e55 e866ffffff               call       0x4a0dc0
004a0e5a 0fbe08                   movsx      ecx, byte ptr [eax]
004a0e5d 894de0                   mov        dword ptr [ebp - 0x20], ecx
004a0e60 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004a0e67 8d4dec                   lea        ecx, [ebp - 0x14]
004a0e6a e891aaf6ff               call       0x40b900
004a0e6f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004a0e72 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a0e75 64890d00000000           mov        dword ptr fs:[0], ecx
004a0e7c 59                       pop        ecx
004a0e7d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a0e80 33cd                     xor        ecx, ebp
004a0e82 e8671b0a00               call       0x5429ee
004a0e87 8be5                     mov        esp, ebp
004a0e89 5d                       pop        ebp
004a0e8a c20400                   ret        4
004a0e8d cc                       int3       
004a0e8e cc                       int3       
004a0e8f cc                       int3       
