004782d0 55                       push       ebp
004782d1 8bec                     mov        ebp, esp
004782d3 51                       push       ecx
004782d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004782d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004782da 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004782dd 894834                   mov        dword ptr [eax + 0x34], ecx
004782e0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004782e3 8b4234                   mov        eax, dword ptr [edx + 0x34]
004782e6 50                       push       eax
004782e7 b968a55b00               mov        ecx, 0x5ba568
004782ec e8cf38f9ff               call       0x40bbc0
004782f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004782f4 894138                   mov        dword ptr [ecx + 0x38], eax
004782f7 8be5                     mov        esp, ebp
004782f9 5d                       pop        ebp
004782fa c20400                   ret        4
004782fd cc                       int3       
004782fe cc                       int3       
004782ff cc                       int3       
