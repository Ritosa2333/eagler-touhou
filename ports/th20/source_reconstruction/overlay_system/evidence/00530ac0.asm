00530ac0 55                       push       ebp
00530ac1 8bec                     mov        ebp, esp
00530ac3 51                       push       ecx
00530ac4 894dfc                   mov        dword ptr [ebp - 4], ecx
00530ac7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530aca 83c138                   add        ecx, 0x38
00530acd e84ef6f1ff               call       0x450120
00530ad2 33c0                     xor        eax, eax
00530ad4 8be5                     mov        esp, ebp
00530ad6 5d                       pop        ebp
00530ad7 c3                       ret        
00530ad8 cc                       int3       
00530ad9 cc                       int3       
00530ada cc                       int3       
00530adb cc                       int3       
00530adc cc                       int3       
00530add cc                       int3       
00530ade cc                       int3       
00530adf cc                       int3       
