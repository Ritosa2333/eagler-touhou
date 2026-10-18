004790a0 55                       push       ebp
004790a1 8bec                     mov        ebp, esp
004790a3 51                       push       ecx
004790a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004790a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004790aa 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004790ad 83c904                   or         ecx, 4
004790b0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004790b3 894a14                   mov        dword ptr [edx + 0x14], ecx
004790b6 8be5                     mov        esp, ebp
004790b8 5d                       pop        ebp
004790b9 c3                       ret        
004790ba cc                       int3       
004790bb cc                       int3       
004790bc cc                       int3       
004790bd cc                       int3       
004790be cc                       int3       
004790bf cc                       int3       
