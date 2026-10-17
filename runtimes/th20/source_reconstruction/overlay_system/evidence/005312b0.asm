005312b0 55                       push       ebp
005312b1 8bec                     mov        ebp, esp
005312b3 51                       push       ecx
005312b4 894dfc                   mov        dword ptr [ebp - 4], ecx
005312b7 8b4508                   mov        eax, dword ptr [ebp + 8]
005312ba c7801c010000c0225300     mov        dword ptr [eax + 0x11c], 0x5322c0
005312c4 8be5                     mov        esp, ebp
005312c6 5d                       pop        ebp
005312c7 c20800                   ret        8
005312ca cc                       int3       
005312cb cc                       int3       
005312cc cc                       int3       
005312cd cc                       int3       
005312ce cc                       int3       
005312cf cc                       int3       
