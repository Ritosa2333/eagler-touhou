0051d190 55                       push       ebp
0051d191 8bec                     mov        ebp, esp
0051d193 6aff                     push       -1
0051d195 6828755600               push       0x567528
0051d19a 64a100000000             mov        eax, dword ptr fs:[0]
0051d1a0 50                       push       eax
0051d1a1 83ec10                   sub        esp, 0x10
0051d1a4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051d1a9 33c5                     xor        eax, ebp
0051d1ab 50                       push       eax
0051d1ac 8d45f4                   lea        eax, [ebp - 0xc]
0051d1af 64a300000000             mov        dword ptr fs:[0], eax
0051d1b5 894df0                   mov        dword ptr [ebp - 0x10], ecx
0051d1b8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051d1bb e820e7eeff               call       0x40b8e0
0051d1c0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051d1c7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051d1ca 83c108                   add        ecx, 8
0051d1cd e8cee6eeff               call       0x40b8a0
0051d1d2 c645fc01                 mov        byte ptr [ebp - 4], 1
0051d1d6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0051d1d9 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0051d1dc 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0051d1df 51                       push       ecx
0051d1e0 e89be0eeff               call       0x40b280
0051d1e5 83c404                   add        esp, 4
0051d1e8 8945ec                   mov        dword ptr [ebp - 0x14], eax
0051d1eb 8b5508                   mov        edx, dword ptr [ebp + 8]
0051d1ee 52                       push       edx
0051d1ef e88ce0eeff               call       0x40b280
0051d1f4 83c404                   add        esp, 4
0051d1f7 8945e8                   mov        dword ptr [ebp - 0x18], eax
0051d1fa 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0051d1fd 50                       push       eax
0051d1fe 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051d201 51                       push       ecx
0051d202 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0051d205 e896040000               call       0x51d6a0
0051d20a 90                       nop        
0051d20b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051d212 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0051d215 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051d218 64890d00000000           mov        dword ptr fs:[0], ecx
0051d21f 59                       pop        ecx
0051d220 8be5                     mov        esp, ebp
0051d222 5d                       pop        ebp
0051d223 c20800                   ret        8
0051d226 cc                       int3       
0051d227 cc                       int3       
0051d228 cc                       int3       
0051d229 cc                       int3       
0051d22a cc                       int3       
0051d22b cc                       int3       
0051d22c cc                       int3       
0051d22d cc                       int3       
0051d22e cc                       int3       
0051d22f cc                       int3       
