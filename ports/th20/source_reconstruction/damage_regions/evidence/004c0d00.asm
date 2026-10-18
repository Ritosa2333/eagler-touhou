004c0d00 55                       push       ebp
004c0d01 8bec                     mov        ebp, esp
004c0d03 6aff                     push       -1
004c0d05 683d865600               push       0x56863d
004c0d0a 64a100000000             mov        eax, dword ptr fs:[0]
004c0d10 50                       push       eax
004c0d11 83ec24                   sub        esp, 0x24
004c0d14 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c0d19 33c5                     xor        eax, ebp
004c0d1b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c0d1e 50                       push       eax
004c0d1f 8d45f4                   lea        eax, [ebp - 0xc]
004c0d22 64a300000000             mov        dword ptr fs:[0], eax
004c0d28 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004c0d2b 8d4d08                   lea        ecx, [ebp + 8]
004c0d2e e8cdb5f4ff               call       0x40c300
004c0d33 85c0                     test       eax, eax
004c0d35 7507                     jne        0x4c0d3e
004c0d37 33c0                     xor        eax, eax
004c0d39 e9af000000               jmp        0x4c0ded
004c0d3e 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004c0d41 0514c40000               add        eax, 0xc414
004c0d46 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c0d49 6a08                     push       8
004c0d4b 8d4de8                   lea        ecx, [ebp - 0x18]
004c0d4e e82db3f4ff               call       0x40c080
004c0d53 8d4de8                   lea        ecx, [ebp - 0x18]
004c0d56 51                       push       ecx
004c0d57 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0d5a e82115f5ff               call       0x412280
004c0d5f c745fc00000000           mov        dword ptr [ebp - 4], 0
004c0d66 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0d69 e8d217f5ff               call       0x412540
004c0d6e 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c0d71 eb09                     jmp        0x4c0d7c
004c0d73 8d4de8                   lea        ecx, [ebp - 0x18]
004c0d76 e8b50ef5ff               call       0x411c30
004c0d7b 90                       nop        
004c0d7c 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004c0d7f 52                       push       edx
004c0d80 8d4de8                   lea        ecx, [ebp - 0x18]
004c0d83 e8280ef5ff               call       0x411bb0
004c0d88 0fb6c0                   movzx      eax, al
004c0d8b 85c0                     test       eax, eax
004c0d8d 744c                     je         0x4c0ddb
004c0d8f 8d4de8                   lea        ecx, [ebp - 0x18]
004c0d92 e869b5f4ff               call       0x40c300
004c0d97 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c0d9a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c0d9d e85eb5f4ff               call       0x40c300
004c0da2 058c000000               add        eax, 0x8c
004c0da7 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c0daa 8d4d08                   lea        ecx, [ebp + 8]
004c0dad 51                       push       ecx
004c0dae 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c0db1 e85af4ffff               call       0x4c0210
004c0db6 85c0                     test       eax, eax
004c0db8 741f                     je         0x4c0dd9
004c0dba 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c0dbd e83eb5f4ff               call       0x40c300
004c0dc2 8945d0                   mov        dword ptr [ebp - 0x30], eax
004c0dc5 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c0dcc 8d4de8                   lea        ecx, [ebp - 0x18]
004c0dcf e82c0df5ff               call       0x411b00
004c0dd4 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c0dd7 eb14                     jmp        0x4c0ded
004c0dd9 eb98                     jmp        0x4c0d73
004c0ddb c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c0de2 8d4de8                   lea        ecx, [ebp - 0x18]
004c0de5 e8160df5ff               call       0x411b00
004c0dea 90                       nop        
004c0deb 33c0                     xor        eax, eax
004c0ded 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0df0 64890d00000000           mov        dword ptr fs:[0], ecx
004c0df7 59                       pop        ecx
004c0df8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0dfb 33cd                     xor        ecx, ebp
004c0dfd e8ec1b0800               call       0x5429ee
004c0e02 8be5                     mov        esp, ebp
004c0e04 5d                       pop        ebp
004c0e05 c20400                   ret        4
004c0e08 cc                       int3       
004c0e09 cc                       int3       
004c0e0a cc                       int3       
004c0e0b cc                       int3       
004c0e0c cc                       int3       
004c0e0d cc                       int3       
004c0e0e cc                       int3       
004c0e0f cc                       int3       
