00464790 55                       push       ebp
00464791 8bec                     mov        ebp, esp
00464793 6aff                     push       -1
00464795 6800765600               push       0x567600
0046479a 64a100000000             mov        eax, dword ptr fs:[0]
004647a0 50                       push       eax
004647a1 51                       push       ecx
004647a2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004647a7 33c5                     xor        eax, ebp
004647a9 50                       push       eax
004647aa 8d45f4                   lea        eax, [ebp - 0xc]
004647ad 64a300000000             mov        dword ptr fs:[0], eax
004647b3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004647b6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004647b9 81c198170000             add        ecx, 0x1798
004647bf e8ac4bfeff               call       0x449370
004647c4 6870934400               push       0x449370
004647c9 6a04                     push       4
004647cb 68e4050000               push       0x5e4
004647d0 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004647d3 83c008                   add        eax, 8
004647d6 50                       push       eax
004647d7 e847e70d00               call       0x542f23
004647dc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004647df e8bc23feff               call       0x446ba0
004647e4 90                       nop        
004647e5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004647e8 64890d00000000           mov        dword ptr fs:[0], ecx
004647ef 59                       pop        ecx
004647f0 8be5                     mov        esp, ebp
004647f2 5d                       pop        ebp
004647f3 c3                       ret        
004647f4 cc                       int3       
004647f5 cc                       int3       
004647f6 cc                       int3       
004647f7 cc                       int3       
004647f8 cc                       int3       
004647f9 cc                       int3       
004647fa cc                       int3       
004647fb cc                       int3       
004647fc cc                       int3       
004647fd cc                       int3       
004647fe cc                       int3       
004647ff cc                       int3       
