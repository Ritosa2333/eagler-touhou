0050fea0 55                       push       ebp
0050fea1 8bec                     mov        ebp, esp
0050fea3 6aff                     push       -1
0050fea5 6800765600               push       0x567600
0050feaa 64a100000000             mov        eax, dword ptr fs:[0]
0050feb0 50                       push       eax
0050feb1 51                       push       ecx
0050feb2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0050feb7 33c5                     xor        eax, ebp
0050feb9 50                       push       eax
0050feba 8d45f4                   lea        eax, [ebp - 0xc]
0050febd 64a300000000             mov        dword ptr fs:[0], eax
0050fec3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0050fec6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fec9 c70054395700             mov        dword ptr [eax], 0x573954
0050fecf 687c395700               push       0x57397c
0050fed4 e8d7c7efff               call       0x40c6b0
0050fed9 83c404                   add        esp, 4
0050fedc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fedf 8b5108                   mov        edx, dword ptr [ecx + 8]
0050fee2 52                       push       edx
0050fee3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
0050fee9 e8c225f0ff               call       0x4124b0
0050feee 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fef1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
0050fef4 51                       push       ecx
0050fef5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
0050fefb e8b025f0ff               call       0x4124b0
0050ff00 90                       nop        
0050ff01 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050ff04 83c11c                   add        ecx, 0x1c
0050ff07 e86494f3ff               call       0x449370
0050ff0c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050ff0f e8dcfef0ff               call       0x41fdf0
0050ff14 90                       nop        
0050ff15 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050ff18 64890d00000000           mov        dword ptr fs:[0], ecx
0050ff1f 59                       pop        ecx
0050ff20 8be5                     mov        esp, ebp
0050ff22 5d                       pop        ebp
0050ff23 c3                       ret        
0050ff24 cc                       int3       
0050ff25 cc                       int3       
0050ff26 cc                       int3       
0050ff27 cc                       int3       
0050ff28 cc                       int3       
0050ff29 cc                       int3       
0050ff2a cc                       int3       
0050ff2b cc                       int3       
0050ff2c cc                       int3       
0050ff2d cc                       int3       
0050ff2e cc                       int3       
0050ff2f cc                       int3       
