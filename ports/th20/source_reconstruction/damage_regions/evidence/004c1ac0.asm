004c1ac0 55                       push       ebp
004c1ac1 8bec                     mov        ebp, esp
004c1ac3 51                       push       ecx
004c1ac4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1ac7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1aca 8b8054030000             mov        eax, dword ptr [eax + 0x354]
004c1ad0 c1e81e                   shr        eax, 0x1e
004c1ad3 83e001                   and        eax, 1
004c1ad6 8be5                     mov        esp, ebp
004c1ad8 5d                       pop        ebp
004c1ad9 c3                       ret        
004c1ada cc                       int3       
004c1adb cc                       int3       
004c1adc cc                       int3       
004c1add cc                       int3       
004c1ade cc                       int3       
004c1adf cc                       int3       
