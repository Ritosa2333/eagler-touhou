004292e0 55                       push       ebp
004292e1 8bec                     mov        ebp, esp
004292e3 51                       push       ecx
004292e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004292e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004292ea d900                     fld        dword ptr [eax]
004292ec 8be5                     mov        esp, ebp
004292ee 5d                       pop        ebp
004292ef c3                       ret        
