00477a60 55                       push       ebp
00477a61 8bec                     mov        ebp, esp
00477a63 6aff                     push       -1
00477a65 6800765600               push       0x567600
00477a6a 64a100000000             mov        eax, dword ptr fs:[0]
00477a70 50                       push       eax
00477a71 51                       push       ecx
00477a72 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00477a77 33c5                     xor        eax, ebp
00477a79 50                       push       eax
00477a7a 8d45f4                   lea        eax, [ebp - 0xc]
00477a7d 64a300000000             mov        dword ptr fs:[0], eax
00477a83 894df0                   mov        dword ptr [ebp - 0x10], ecx
00477a86 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00477a89 c70078fa5600             mov        dword ptr [eax], 0x56fa78
00477a8f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477a92 83c170                   add        ecx, 0x70
00477a95 e83682fdff               call       0x44fcd0
00477a9a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477a9d 83c174                   add        ecx, 0x74
00477aa0 e82b82fdff               call       0x44fcd0
00477aa5 6a00                     push       0
00477aa7 e894050000               call       0x478040
00477aac 83c404                   add        esp, 4
00477aaf c7401400000000           mov        dword ptr [eax + 0x14], 0
00477ab6 6a00                     push       0
00477ab8 e883050000               call       0x478040
00477abd 83c404                   add        esp, 4
00477ac0 c7401800000000           mov        dword ptr [eax + 0x18], 0
00477ac7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00477aca 64890d00000000           mov        dword ptr fs:[0], ecx
00477ad1 59                       pop        ecx
00477ad2 8be5                     mov        esp, ebp
00477ad4 5d                       pop        ebp
00477ad5 c3                       ret        
00477ad6 cc                       int3       
00477ad7 cc                       int3       
00477ad8 cc                       int3       
00477ad9 cc                       int3       
00477ada cc                       int3       
00477adb cc                       int3       
00477adc cc                       int3       
00477add cc                       int3       
00477ade cc                       int3       
00477adf cc                       int3       
