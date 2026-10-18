0052c750 55                       push       ebp
0052c751 8bec                     mov        ebp, esp
0052c753 83ec10                   sub        esp, 0x10
0052c756 894df8                   mov        dword ptr [ebp - 8], ecx
0052c759 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052c75c 0564010000               add        eax, 0x164
0052c761 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052c764 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c767 51                       push       ecx
0052c768 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c76b e8107eeeff               call       0x414580
0052c770 8b10                     mov        edx, dword ptr [eax]
0052c772 8955f0                   mov        dword ptr [ebp - 0x10], edx
0052c775 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0052c778 50                       push       eax
0052c779 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052c77f e87c05f2ff               call       0x44cd00
0052c784 85c0                     test       eax, eax
0052c786 7409                     je         0x52c791
0052c788 c745fc01000000           mov        dword ptr [ebp - 4], 1
0052c78f eb07                     jmp        0x52c798
0052c791 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c798 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052c79b 8be5                     mov        esp, ebp
0052c79d 5d                       pop        ebp
0052c79e c20400                   ret        4
0052c7a1 cc                       int3       
0052c7a2 cc                       int3       
0052c7a3 cc                       int3       
0052c7a4 cc                       int3       
0052c7a5 cc                       int3       
0052c7a6 cc                       int3       
0052c7a7 cc                       int3       
0052c7a8 cc                       int3       
0052c7a9 cc                       int3       
0052c7aa cc                       int3       
0052c7ab cc                       int3       
0052c7ac cc                       int3       
0052c7ad cc                       int3       
0052c7ae cc                       int3       
0052c7af cc                       int3       
