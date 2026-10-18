004dfad0 55                       push       ebp
004dfad1 8bec                     mov        ebp, esp
004dfad3 51                       push       ecx
004dfad4 894dfc                   mov        dword ptr [ebp - 4], ecx
004dfad7 8b4508                   mov        eax, dword ptr [ebp + 8]
004dfada c1e002                   shl        eax, 2
004dfadd c1e008                   shl        eax, 8
004dfae0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004dfae3 8d8401f4000000           lea        eax, [ecx + eax + 0xf4]
004dfaea 8be5                     mov        esp, ebp
004dfaec 5d                       pop        ebp
004dfaed c20400                   ret        4
