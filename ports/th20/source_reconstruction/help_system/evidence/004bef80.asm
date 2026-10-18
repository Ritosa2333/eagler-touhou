004bef80 55                       push       ebp
004bef81 8bec                     mov        ebp, esp
004bef83 6aff                     push       -1
004bef85 6800765600               push       0x567600
004bef8a 64a100000000             mov        eax, dword ptr fs:[0]
004bef90 50                       push       eax
004bef91 51                       push       ecx
004bef92 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bef97 33c5                     xor        eax, ebp
004bef99 50                       push       eax
004bef9a 8d45f4                   lea        eax, [ebp - 0xc]
004bef9d 64a300000000             mov        dword ptr fs:[0], eax
004befa3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004befa6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004befa9 c700e40b5700             mov        dword ptr [eax], 0x570be4
004befaf 68180c5700               push       0x570c18
004befb4 e8f7d6f4ff               call       0x40c6b0
004befb9 83c404                   add        esp, 4
004befbc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004befbf 8b5108                   mov        edx, dword ptr [ecx + 8]
004befc2 52                       push       edx
004befc3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004befc9 e8e234f5ff               call       0x4124b0
004befce 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004befd1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004befd4 51                       push       ecx
004befd5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004befdb e8d034f5ff               call       0x4124b0
004befe0 6a0e                     push       0xe
004befe2 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004befe8 e843d4f8ff               call       0x44c430
004befed c705244d5c0000000000     mov        dword ptr [0x5c4d24], 0
004beff7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004beffa 83c124                   add        ecx, 0x24
004beffd e86e0affff               call       0x4afa70
004bf002 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bf005 e8e60df6ff               call       0x41fdf0
004bf00a 90                       nop        
004bf00b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bf00e 64890d00000000           mov        dword ptr fs:[0], ecx
004bf015 59                       pop        ecx
004bf016 8be5                     mov        esp, ebp
004bf018 5d                       pop        ebp
004bf019 c3                       ret        
004bf01a cc                       int3       
004bf01b cc                       int3       
004bf01c cc                       int3       
004bf01d cc                       int3       
004bf01e cc                       int3       
004bf01f cc                       int3       
