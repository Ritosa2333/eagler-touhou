004dda10 55                       push       ebp
004dda11 8bec                     mov        ebp, esp
004dda13 51                       push       ecx
004dda14 894dfc                   mov        dword ptr [ebp - 4], ecx
004dda17 8b4508                   mov        eax, dword ptr [ebp + 8]
004dda1a 50                       push       eax
004dda1b e8e051f4ff               call       0x422c00
004dda20 83c404                   add        esp, 4
004dda23 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dda26 8901                     mov        dword ptr [ecx], eax
004dda28 8be5                     mov        esp, ebp
004dda2a 5d                       pop        ebp
004dda2b c20400                   ret        4
004dda2e cc                       int3       
004dda2f cc                       int3       
