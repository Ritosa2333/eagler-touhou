004aaac0 55                       push       ebp
004aaac1 8bec                     mov        ebp, esp
004aaac3 83ec10                   sub        esp, 0x10
004aaac6 894df8                   mov        dword ptr [ebp - 8], ecx
004aaac9 c745fc00000000           mov        dword ptr [ebp - 4], 0
004aaad0 6a00                     push       0
004aaad2 e889d5fcff               call       0x478060
004aaad7 83c404                   add        esp, 4
004aaada 85c0                     test       eax, eax
004aaadc 7424                     je         0x4aab02
004aaade 6a00                     push       0
004aaae0 e87bd5fcff               call       0x478060
004aaae5 83c404                   add        esp, 4
004aaae8 8945f0                   mov        dword ptr [ebp - 0x10], eax
004aaaeb 8b45f8                   mov        eax, dword ptr [ebp - 8]
004aaaee 8b08                     mov        ecx, dword ptr [eax]
004aaaf0 894df4                   mov        dword ptr [ebp - 0xc], ecx
004aaaf3 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004aaaf6 52                       push       edx
004aaaf7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004aaafa e881dffeff               call       0x498a80
004aaaff 8945fc                   mov        dword ptr [ebp - 4], eax
004aab02 8b45fc                   mov        eax, dword ptr [ebp - 4]
004aab05 8be5                     mov        esp, ebp
004aab07 5d                       pop        ebp
004aab08 c3                       ret        
004aab09 cc                       int3       
004aab0a cc                       int3       
004aab0b cc                       int3       
004aab0c cc                       int3       
004aab0d cc                       int3       
004aab0e cc                       int3       
004aab0f cc                       int3       
