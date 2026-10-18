005357a0 55                       push       ebp
005357a1 8bec                     mov        ebp, esp
005357a3 51                       push       ecx
005357a4 894dfc                   mov        dword ptr [ebp - 4], ecx
005357a7 8b4508                   mov        eax, dword ptr [ebp + 8]
005357aa 50                       push       eax
005357ab 6a04                     push       4
005357ad 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005357b0 51                       push       ecx
005357b1 e87a7cf2ff               call       0x45d430
005357b6 83c40c                   add        esp, 0xc
005357b9 8be5                     mov        esp, ebp
005357bb 5d                       pop        ebp
005357bc c20400                   ret        4
005357bf cc                       int3       
