005376a0 55                       push       ebp
005376a1 8bec                     mov        ebp, esp
005376a3 51                       push       ecx
005376a4 894dfc                   mov        dword ptr [ebp - 4], ecx
005376a7 8b4508                   mov        eax, dword ptr [ebp + 8]
005376aa c7801c010000c07e5300     mov        dword ptr [eax + 0x11c], 0x537ec0
005376b4 8be5                     mov        esp, ebp
005376b6 5d                       pop        ebp
005376b7 c20800                   ret        8
005376ba cc                       int3       
005376bb cc                       int3       
005376bc cc                       int3       
005376bd cc                       int3       
005376be cc                       int3       
005376bf cc                       int3       
