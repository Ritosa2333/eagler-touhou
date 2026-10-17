00449260 55                       push       ebp
00449261 8bec                     mov        ebp, esp
00449263 6aff                     push       -1
00449265 68a0795600               push       0x5679a0
0044926a 64a100000000             mov        eax, dword ptr fs:[0]
00449270 50                       push       eax
00449271 83ec08                   sub        esp, 8
00449274 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00449279 33c5                     xor        eax, ebp
0044927b 50                       push       eax
0044927c 8d45f4                   lea        eax, [ebp - 0xc]
0044927f 64a300000000             mov        dword ptr fs:[0], eax
00449285 894df0                   mov        dword ptr [ebp - 0x10], ecx
00449288 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0044928b 05c8060000               add        eax, 0x6c8
00449290 8945ec                   mov        dword ptr [ebp - 0x14], eax
00449293 6a00                     push       0
00449295 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00449298 e8f3010000               call       0x449490
0044929d 8bc8                     mov        ecx, eax
0044929f e8fc100000               call       0x44a3a0
004492a4 90                       nop        
004492a5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004492a8 81c118080006             add        ecx, 0x6000818
004492ae e8bd000000               call       0x449370
004492b3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004492b6 81c110070000             add        ecx, 0x710
004492bc e81fffffff               call       0x4491e0
004492c1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004492c4 81c1dc000000             add        ecx, 0xdc
004492ca e8a1000000               call       0x449370
004492cf 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004492d2 83c104                   add        ecx, 4
004492d5 e8a626fcff               call       0x40b980
004492da 90                       nop        
004492db 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004492de 64890d00000000           mov        dword ptr fs:[0], ecx
004492e5 59                       pop        ecx
004492e6 8be5                     mov        esp, ebp
004492e8 5d                       pop        ebp
004492e9 c3                       ret        
004492ea cc                       int3       
004492eb cc                       int3       
004492ec cc                       int3       
004492ed cc                       int3       
004492ee cc                       int3       
004492ef cc                       int3       
