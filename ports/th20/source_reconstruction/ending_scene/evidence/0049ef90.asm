0049ef90 55                       push       ebp
0049ef91 8bec                     mov        ebp, esp
0049ef93 6aff                     push       -1
0049ef95 6800765600               push       0x567600
0049ef9a 64a100000000             mov        eax, dword ptr fs:[0]
0049efa0 50                       push       eax
0049efa1 51                       push       ecx
0049efa2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049efa7 33c5                     xor        eax, ebp
0049efa9 50                       push       eax
0049efaa 8d45f4                   lea        eax, [ebp - 0xc]
0049efad 64a300000000             mov        dword ptr fs:[0], eax
0049efb3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0049efb6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049efb9 c70044025700             mov        dword ptr [eax], 0x570244
0049efbf 68ac025700               push       0x5702ac
0049efc4 e8e7d6f6ff               call       0x40c6b0
0049efc9 83c404                   add        esp, 4
0049efcc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049efcf 8b5108                   mov        edx, dword ptr [ecx + 8]
0049efd2 52                       push       edx
0049efd3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
0049efd9 e8d234f7ff               call       0x4124b0
0049efde 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049efe1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
0049efe4 51                       push       ecx
0049efe5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
0049efeb e8c034f7ff               call       0x4124b0
0049eff0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049eff3 8b4218                   mov        eax, dword ptr [edx + 0x18]
0049eff6 50                       push       eax
0049eff7 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0049effd e8bef8ffff               call       0x49e8c0
0049f002 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049f005 c7411800000000           mov        dword ptr [ecx + 0x18], 0
0049f00c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049f00f c7421800000000           mov        dword ptr [edx + 0x18], 0
0049f016 6a0f                     push       0xf
0049f018 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0049f01e e80dd4faff               call       0x44c430
0049f023 6a10                     push       0x10
0049f025 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0049f02b e800d4faff               call       0x44c430
0049f030 6a11                     push       0x11
0049f032 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0049f038 e8f3d3faff               call       0x44c430
0049f03d 6a12                     push       0x12
0049f03f 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0049f045 e8e6d3faff               call       0x44c430
0049f04a 90                       nop        
0049f04b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049f04e 83781400                 cmp        dword ptr [eax + 0x14], 0
0049f052 741c                     je         0x49f070
0049f054 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049f057 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0049f05a 52                       push       edx
0049f05b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0049f061 e80a06f8ff               call       0x41f670
0049f066 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049f069 c7401400000000           mov        dword ptr [eax + 0x14], 0
0049f070 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049f073 c7411400000000           mov        dword ptr [ecx + 0x14], 0
0049f07a c705e8495c0000000000     mov        dword ptr [0x5c49e8], 0
0049f084 c70518a55b0000000000     mov        dword ptr [0x5ba518], 0
0049f08e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049f091 e85a0df8ff               call       0x41fdf0
0049f096 90                       nop        
0049f097 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049f09a 64890d00000000           mov        dword ptr fs:[0], ecx
0049f0a1 59                       pop        ecx
0049f0a2 8be5                     mov        esp, ebp
0049f0a4 5d                       pop        ebp
0049f0a5 c3                       ret        
0049f0a6 cc                       int3       
0049f0a7 cc                       int3       
0049f0a8 cc                       int3       
0049f0a9 cc                       int3       
0049f0aa cc                       int3       
0049f0ab cc                       int3       
0049f0ac cc                       int3       
0049f0ad cc                       int3       
0049f0ae cc                       int3       
0049f0af cc                       int3       
