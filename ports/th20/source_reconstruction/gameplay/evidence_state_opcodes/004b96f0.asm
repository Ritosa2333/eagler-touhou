004b96f0 55                       push       ebp
004b96f1 8bec                     mov        ebp, esp
004b96f3 83ec0c                   sub        esp, 0xc
004b96f6 894dfc                   mov        dword ptr [ebp - 4], ecx
004b96f9 b9404d5c00               mov        ecx, 0x5c4d40
004b96fe e8bd29ffff               call       0x4ac0c0
004b9703 83f808                   cmp        eax, 8
004b9706 742f                     je         0x4b9737
004b9708 b968a55b00               mov        ecx, 0x5ba568
004b970d e87eecffff               call       0x4b8390
004b9712 85c0                     test       eax, eax
004b9714 7521                     jne        0x4b9737
004b9716 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b9719 8b8870010000             mov        ecx, dword ptr [eax + 0x170]
004b971f 894df8                   mov        dword ptr [ebp - 8], ecx
004b9722 6a00                     push       0
004b9724 6aff                     push       -1
004b9726 6a00                     push       0
004b9728 6a00                     push       0
004b972a 8d55f4                   lea        edx, [ebp - 0xc]
004b972d 52                       push       edx
004b972e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b9731 e83a75f9ff               call       0x450c70
004b9736 90                       nop        
004b9737 8be5                     mov        esp, ebp
004b9739 5d                       pop        ebp
004b973a c3                       ret        
004b973b cc                       int3       
004b973c cc                       int3       
004b973d cc                       int3       
004b973e cc                       int3       
004b973f cc                       int3       
