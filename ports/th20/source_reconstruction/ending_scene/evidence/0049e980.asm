0049e980 55                       push       ebp
0049e981 8bec                     mov        ebp, esp
0049e983 6aff                     push       -1
0049e985 6837955600               push       0x569537
0049e98a 64a100000000             mov        eax, dword ptr fs:[0]
0049e990 50                       push       eax
0049e991 83ec18                   sub        esp, 0x18
0049e994 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049e999 33c5                     xor        eax, ebp
0049e99b 50                       push       eax
0049e99c 8d45f4                   lea        eax, [ebp - 0xc]
0049e99f 64a300000000             mov        dword ptr fs:[0], eax
0049e9a5 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0049e9a8 68f0000000               push       0xf0
0049e9ad e8ab3d0a00               call       0x54275d
0049e9b2 83c404                   add        esp, 4
0049e9b5 8945f0                   mov        dword ptr [ebp - 0x10], eax
0049e9b8 c745fc00000000           mov        dword ptr [ebp - 4], 0
0049e9bf 837df000                 cmp        dword ptr [ebp - 0x10], 0
0049e9c3 7444                     je         0x49ea09
0049e9c5 68f0000000               push       0xf0
0049e9ca 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049e9cd e8aed6f6ff               call       0x40c080
0049e9d2 8d4510                   lea        eax, [ebp + 0x10]
0049e9d5 50                       push       eax
0049e9d6 e8a5c8f6ff               call       0x40b280
0049e9db 83c404                   add        esp, 4
0049e9de 8b08                     mov        ecx, dword ptr [eax]
0049e9e0 894de8                   mov        dword ptr [ebp - 0x18], ecx
0049e9e3 8d550c                   lea        edx, [ebp + 0xc]
0049e9e6 52                       push       edx
0049e9e7 e894c8f6ff               call       0x40b280
0049e9ec 83c404                   add        esp, 4
0049e9ef 8b00                     mov        eax, dword ptr [eax]
0049e9f1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0049e9f4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0049e9f7 51                       push       ecx
0049e9f8 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0049e9fb 52                       push       edx
0049e9fc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049e9ff e8cc020000               call       0x49ecd0
0049ea04 8945ec                   mov        dword ptr [ebp - 0x14], eax
0049ea07 eb07                     jmp        0x49ea10
0049ea09 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0049ea10 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0049ea13 8945e0                   mov        dword ptr [ebp - 0x20], eax
0049ea16 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0049ea1d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0049ea20 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049ea23 64890d00000000           mov        dword ptr fs:[0], ecx
0049ea2a 59                       pop        ecx
0049ea2b 8be5                     mov        esp, ebp
0049ea2d 5d                       pop        ebp
0049ea2e c20c00                   ret        0xc
0049ea31 cc                       int3       
0049ea32 cc                       int3       
0049ea33 cc                       int3       
0049ea34 cc                       int3       
0049ea35 cc                       int3       
0049ea36 cc                       int3       
0049ea37 cc                       int3       
0049ea38 cc                       int3       
0049ea39 cc                       int3       
0049ea3a cc                       int3       
0049ea3b cc                       int3       
0049ea3c cc                       int3       
0049ea3d cc                       int3       
0049ea3e cc                       int3       
0049ea3f cc                       int3       
