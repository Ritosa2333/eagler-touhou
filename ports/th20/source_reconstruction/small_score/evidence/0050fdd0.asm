0050fdd0 55                       push       ebp
0050fdd1 8bec                     mov        ebp, esp
0050fdd3 6aff                     push       -1
0050fdd5 6800765600               push       0x567600
0050fdda 64a100000000             mov        eax, dword ptr fs:[0]
0050fde0 50                       push       eax
0050fde1 51                       push       ecx
0050fde2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0050fde7 33c5                     xor        eax, ebp
0050fde9 50                       push       eax
0050fdea 8d45f4                   lea        eax, [ebp - 0xc]
0050fded 64a300000000             mov        dword ptr fs:[0], eax
0050fdf3 894df0                   mov        dword ptr [ebp - 0x10], ecx
0050fdf6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fdf9 e822fff0ff               call       0x41fd20
0050fdfe 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fe01 c70054395700             mov        dword ptr [eax], 0x573954
0050fe07 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fe0a c7411000000000           mov        dword ptr [ecx + 0x10], 0
0050fe11 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050fe14 c7421400000000           mov        dword ptr [edx + 0x14], 0
0050fe1b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fe1e c7401800000000           mov        dword ptr [eax + 0x18], 0
0050fe25 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fe28 83c11c                   add        ecx, 0x1c
0050fe2b e8108df3ff               call       0x448b40
0050fe30 6850fd5000               push       0x50fd50
0050fe35 6a12                     push       0x12
0050fe37 6a44                     push       0x44
0050fe39 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fe3c 81c100060000             add        ecx, 0x600
0050fe42 51                       push       ecx
0050fe43 e8d8bdefff               call       0x40bc20
0050fe48 6850fd5000               push       0x50fd50
0050fe4d 6a12                     push       0x12
0050fe4f 6a44                     push       0x44
0050fe51 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050fe54 81c2c80a0000             add        edx, 0xac8
0050fe5a 52                       push       edx
0050fe5b e8c0bdefff               call       0x40bc20
0050fe60 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fe63 c780900f000000000000     mov        dword ptr [eax + 0xf90], 0
0050fe6d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050fe70 c781940f000000000000     mov        dword ptr [ecx + 0xf94], 0
0050fe7a 6860395700               push       0x573960
0050fe7f e82cc8efff               call       0x40c6b0
0050fe84 83c404                   add        esp, 4
0050fe87 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0050fe8a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050fe8d 64890d00000000           mov        dword ptr fs:[0], ecx
0050fe94 59                       pop        ecx
0050fe95 8be5                     mov        esp, ebp
0050fe97 5d                       pop        ebp
0050fe98 c3                       ret        
0050fe99 cc                       int3       
0050fe9a cc                       int3       
0050fe9b cc                       int3       
0050fe9c cc                       int3       
0050fe9d cc                       int3       
0050fe9e cc                       int3       
0050fe9f cc                       int3       
