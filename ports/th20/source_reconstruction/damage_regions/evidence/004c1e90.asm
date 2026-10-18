004c1e90 55                       push       ebp
004c1e91 8bec                     mov        ebp, esp
004c1e93 51                       push       ecx
004c1e94 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1e97 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1e9a 8b4818                   mov        ecx, dword ptr [eax + 0x18]
004c1e9d 83c902                   or         ecx, 2
004c1ea0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1ea3 894a18                   mov        dword ptr [edx + 0x18], ecx
004c1ea6 8be5                     mov        esp, ebp
004c1ea8 5d                       pop        ebp
004c1ea9 c3                       ret        
004c1eaa cc                       int3       
004c1eab cc                       int3       
004c1eac cc                       int3       
004c1ead cc                       int3       
004c1eae cc                       int3       
004c1eaf cc                       int3       
