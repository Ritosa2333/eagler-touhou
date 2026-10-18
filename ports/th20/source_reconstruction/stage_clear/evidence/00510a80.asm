00510a80 55                       push       ebp
00510a81 8bec                     mov        ebp, esp
00510a83 6aff                     push       -1
00510a85 6800765600               push       0x567600
00510a8a 64a100000000             mov        eax, dword ptr fs:[0]
00510a90 50                       push       eax
00510a91 51                       push       ecx
00510a92 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00510a97 33c5                     xor        eax, ebp
00510a99 50                       push       eax
00510a9a 8d45f4                   lea        eax, [ebp - 0xc]
00510a9d 64a300000000             mov        dword ptr fs:[0], eax
00510aa3 894df0                   mov        dword ptr [ebp - 0x10], ecx
00510aa6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00510aa9 c700c43e5700             mov        dword ptr [eax], 0x573ec4
00510aaf 68ec3e5700               push       0x573eec
00510ab4 e8f7bbefff               call       0x40c6b0
00510ab9 83c404                   add        esp, 4
00510abc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510abf 8b5108                   mov        edx, dword ptr [ecx + 8]
00510ac2 52                       push       edx
00510ac3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00510ac9 e8e219f0ff               call       0x4124b0
00510ace 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00510ad1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
00510ad4 51                       push       ecx
00510ad5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00510adb e8d019f0ff               call       0x4124b0
00510ae0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510ae3 83c118                   add        ecx, 0x18
00510ae6 e8e5f1f3ff               call       0x44fcd0
00510aeb c70514615c0000000000     mov        dword ptr [0x5c6114], 0
00510af5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00510af8 e8f3f2f0ff               call       0x41fdf0
00510afd 90                       nop        
00510afe 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00510b01 64890d00000000           mov        dword ptr fs:[0], ecx
00510b08 59                       pop        ecx
00510b09 8be5                     mov        esp, ebp
00510b0b 5d                       pop        ebp
00510b0c c3                       ret        
00510b0d cc                       int3       
00510b0e cc                       int3       
00510b0f cc                       int3       
00510b10 cc                       int3       
00510b11 cc                       int3       
00510b12 cc                       int3       
00510b13 cc                       int3       
00510b14 cc                       int3       
00510b15 cc                       int3       
00510b16 cc                       int3       
00510b17 cc                       int3       
00510b18 cc                       int3       
00510b19 cc                       int3       
00510b1a cc                       int3       
00510b1b cc                       int3       
00510b1c cc                       int3       
00510b1d cc                       int3       
00510b1e cc                       int3       
00510b1f cc                       int3       
