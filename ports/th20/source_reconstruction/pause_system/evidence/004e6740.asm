004e6740 55                       push       ebp
004e6741 8bec                     mov        ebp, esp
004e6743 83ec0c                   sub        esp, 0xc
004e6746 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6749 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e674c e8af0df5ff               call       0x437500
004e6751 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e6754 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6757 8b4820                   mov        ecx, dword ptr [eax + 0x20]
004e675a 894df8                   mov        dword ptr [ebp - 8], ecx
004e675d 8b55f8                   mov        edx, dword ptr [ebp - 8]
004e6760 52                       push       edx
004e6761 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e6764 e81715f5ff               call       0x437c80
004e6769 8b4004                   mov        eax, dword ptr [eax + 4]
004e676c 8be5                     mov        esp, ebp
004e676e 5d                       pop        ebp
004e676f c3                       ret        
