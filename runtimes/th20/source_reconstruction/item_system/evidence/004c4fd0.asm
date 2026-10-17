004c4fd0 55                       push       ebp
004c4fd1 8bec                     mov        ebp, esp
004c4fd3 51                       push       ecx
004c4fd4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4fd7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4fda 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c4fdd 898894c84900             mov        dword ptr [eax + 0x49c894], ecx
004c4fe3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4fe6 8b8294c84900             mov        eax, dword ptr [edx + 0x49c894]
004c4fec 50                       push       eax
004c4fed b968a55b00               mov        ecx, 0x5ba568
004c4ff2 e8c96bf4ff               call       0x40bbc0
004c4ff7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4ffa 898198c84900             mov        dword ptr [ecx + 0x49c898], eax
004c5000 8be5                     mov        esp, ebp
004c5002 5d                       pop        ebp
004c5003 c20400                   ret        4
004c5006 cc                       int3       
004c5007 cc                       int3       
004c5008 cc                       int3       
004c5009 cc                       int3       
004c500a cc                       int3       
004c500b cc                       int3       
004c500c cc                       int3       
004c500d cc                       int3       
004c500e cc                       int3       
004c500f cc                       int3       
