004c9190 55                       push       ebp
004c9191 8bec                     mov        ebp, esp
004c9193 51                       push       ecx
004c9194 894dfc                   mov        dword ptr [ebp - 4], ecx
004c9197 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c919a e8a1f8ffff               call       0x4c8a40
004c919f 8b4508                   mov        eax, dword ptr [ebp + 8]
004c91a2 83e001                   and        eax, 1
004c91a5 7411                     je         0x4c91b8
004c91a7 68301d0000               push       0x1d30
004c91ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c91af 51                       push       ecx
004c91b0 e8d8950700               call       0x54278d
004c91b5 83c408                   add        esp, 8
004c91b8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c91bb 8be5                     mov        esp, ebp
004c91bd 5d                       pop        ebp
004c91be c20400                   ret        4
004c91c1 cc                       int3       
004c91c2 cc                       int3       
004c91c3 cc                       int3       
004c91c4 cc                       int3       
004c91c5 cc                       int3       
004c91c6 cc                       int3       
004c91c7 cc                       int3       
004c91c8 cc                       int3       
004c91c9 cc                       int3       
004c91ca cc                       int3       
004c91cb cc                       int3       
004c91cc cc                       int3       
004c91cd cc                       int3       
004c91ce cc                       int3       
004c91cf cc                       int3       
