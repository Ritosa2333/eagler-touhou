004790c0 55                       push       ebp
004790c1 8bec                     mov        ebp, esp
004790c3 51                       push       ecx
004790c4 894dfc                   mov        dword ptr [ebp - 4], ecx
004790c7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004790ca 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004790cd 83e1fb                   and        ecx, 0xfffffffb
004790d0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004790d3 894a14                   mov        dword ptr [edx + 0x14], ecx
004790d6 8be5                     mov        esp, ebp
004790d8 5d                       pop        ebp
004790d9 c3                       ret        
004790da cc                       int3       
004790db cc                       int3       
004790dc cc                       int3       
004790dd cc                       int3       
004790de cc                       int3       
004790df cc                       int3       
