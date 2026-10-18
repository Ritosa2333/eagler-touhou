0049ec30 55                       push       ebp
0049ec31 8bec                     mov        ebp, esp
0049ec33 6aff                     push       -1
0049ec35 6800765600               push       0x567600
0049ec3a 64a100000000             mov        eax, dword ptr fs:[0]
0049ec40 50                       push       eax
0049ec41 51                       push       ecx
0049ec42 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049ec47 33c5                     xor        eax, ebp
0049ec49 50                       push       eax
0049ec4a 8d45f4                   lea        eax, [ebp - 0xc]
0049ec4d 64a300000000             mov        dword ptr fs:[0], eax
0049ec53 894df0                   mov        dword ptr [ebp - 0x10], ecx
0049ec56 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ec59 e8c210f8ff               call       0x41fd20
0049ec5e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ec61 c70044025700             mov        dword ptr [eax], 0x570244
0049ec67 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ec6a c7411000000000           mov        dword ptr [ecx + 0x10], 0
0049ec71 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ec74 c7421400000000           mov        dword ptr [edx + 0x14], 0
0049ec7b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ec7e c7401800000000           mov        dword ptr [eax + 0x18], 0
0049ec85 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ec88 c7411c00000000           mov        dword ptr [ecx + 0x1c], 0
0049ec8f 33d2                     xor        edx, edx
0049ec91 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ec94 83c020                   add        eax, 0x20
0049ec97 8910                     mov        dword ptr [eax], edx
0049ec99 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ec9c c7412400000000           mov        dword ptr [ecx + 0x24], 0
0049eca3 6850025700               push       0x570250
0049eca8 e803daf6ff               call       0x40c6b0
0049ecad 83c404                   add        esp, 4
0049ecb0 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ecb3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049ecb6 64890d00000000           mov        dword ptr fs:[0], ecx
0049ecbd 59                       pop        ecx
0049ecbe 8be5                     mov        esp, ebp
0049ecc0 5d                       pop        ebp
0049ecc1 c3                       ret        
0049ecc2 cc                       int3       
0049ecc3 cc                       int3       
0049ecc4 cc                       int3       
0049ecc5 cc                       int3       
0049ecc6 cc                       int3       
0049ecc7 cc                       int3       
0049ecc8 cc                       int3       
0049ecc9 cc                       int3       
0049ecca cc                       int3       
0049eccb cc                       int3       
0049eccc cc                       int3       
0049eccd cc                       int3       
0049ecce cc                       int3       
0049eccf cc                       int3       
