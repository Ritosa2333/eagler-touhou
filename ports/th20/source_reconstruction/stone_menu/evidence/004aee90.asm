004aee90 55                       push       ebp
004aee91 8bec                     mov        ebp, esp
004aee93 83ec14                   sub        esp, 0x14
004aee96 894df8                   mov        dword ptr [ebp - 8], ecx
004aee99 8b45f8                   mov        eax, dword ptr [ebp - 8]
004aee9c 8945f4                   mov        dword ptr [ebp - 0xc], eax
004aee9f 0fb64dff                 movzx      ecx, byte ptr [ebp - 1]
004aeea3 51                       push       ecx
004aeea4 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004aeea7 e804d4ffff               call       0x4ac2b0
004aeeac 90                       nop        
004aeead 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004aeeb0 e8ebcef5ff               call       0x40bda0
004aeeb5 8945ec                   mov        dword ptr [ebp - 0x14], eax
004aeeb8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004aeebb e8a002f6ff               call       0x40f160
004aeec0 50                       push       eax
004aeec1 8d4dfe                   lea        ecx, [ebp - 2]
004aeec4 e857c2f5ff               call       0x40b120
004aeec9 8945f0                   mov        dword ptr [ebp - 0x10], eax
004aeecc 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004aeecf 52                       push       edx
004aeed0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004aeed3 e848e8ffff               call       0x4ad720
004aeed8 90                       nop        
004aeed9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004aeedc 8be5                     mov        esp, ebp
004aeede 5d                       pop        ebp
004aeedf c3                       ret        
