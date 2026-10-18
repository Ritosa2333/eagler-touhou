004e1ba0 55                       push       ebp
004e1ba1 8bec                     mov        ebp, esp
004e1ba3 6aff                     push       -1
004e1ba5 6827985600               push       0x569827
004e1baa 64a100000000             mov        eax, dword ptr fs:[0]
004e1bb0 50                       push       eax
004e1bb1 83ec10                   sub        esp, 0x10
004e1bb4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e1bb9 33c5                     xor        eax, ebp
004e1bbb 50                       push       eax
004e1bbc 8d45f4                   lea        eax, [ebp - 0xc]
004e1bbf 64a300000000             mov        dword ptr fs:[0], eax
004e1bc5 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004e1bc8 68d8020000               push       0x2d8
004e1bcd e88b0b0600               call       0x54275d
004e1bd2 83c404                   add        esp, 4
004e1bd5 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e1bd8 c745fc00000000           mov        dword ptr [ebp - 4], 0
004e1bdf 837df000                 cmp        dword ptr [ebp - 0x10], 0
004e1be3 741a                     je         0x4e1bff
004e1be5 68d8020000               push       0x2d8
004e1bea 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1bed e88ea4f2ff               call       0x40c080
004e1bf2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1bf5 e8e6000000               call       0x4e1ce0
004e1bfa 8945ec                   mov        dword ptr [ebp - 0x14], eax
004e1bfd eb07                     jmp        0x4e1c06
004e1bff c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004e1c06 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e1c09 8945e8                   mov        dword ptr [ebp - 0x18], eax
004e1c0c c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004e1c13 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004e1c16 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e1c19 64890d00000000           mov        dword ptr fs:[0], ecx
004e1c20 59                       pop        ecx
004e1c21 8be5                     mov        esp, ebp
004e1c23 5d                       pop        ebp
004e1c24 c20400                   ret        4
004e1c27 cc                       int3       
004e1c28 cc                       int3       
004e1c29 cc                       int3       
004e1c2a cc                       int3       
004e1c2b cc                       int3       
004e1c2c cc                       int3       
004e1c2d cc                       int3       
004e1c2e cc                       int3       
004e1c2f cc                       int3       
