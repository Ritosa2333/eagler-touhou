0052cc30 55                       push       ebp
0052cc31 8bec                     mov        ebp, esp
0052cc33 83ec10                   sub        esp, 0x10
0052cc36 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cc39 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cc3c 0564010000               add        eax, 0x164
0052cc41 8945f8                   mov        dword ptr [ebp - 8], eax
0052cc44 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cc47 51                       push       ecx
0052cc48 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cc4b e83079eeff               call       0x414580
0052cc50 8b10                     mov        edx, dword ptr [eax]
0052cc52 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052cc55 6a01                     push       1
0052cc57 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052cc5a 50                       push       eax
0052cc5b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052cc61 e82a22f2ff               call       0x44ee90
0052cc66 90                       nop        
0052cc67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052cc6a 81c164010000             add        ecx, 0x164
0052cc70 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052cc73 6a00                     push       0
0052cc75 8b5508                   mov        edx, dword ptr [ebp + 8]
0052cc78 52                       push       edx
0052cc79 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052cc7c e8ff78eeff               call       0x414580
0052cc81 8bc8                     mov        ecx, eax
0052cc83 e8184beeff               call       0x4117a0
0052cc88 90                       nop        
0052cc89 8be5                     mov        esp, ebp
0052cc8b 5d                       pop        ebp
0052cc8c c20400                   ret        4
0052cc8f cc                       int3       
