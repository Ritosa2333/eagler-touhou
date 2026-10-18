004b8880 55                       push       ebp
004b8881 8bec                     mov        ebp, esp
004b8883 83ec10                   sub        esp, 0x10
004b8886 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8889 c745f800000000           mov        dword ptr [ebp - 8], 0
004b8890 eb09                     jmp        0x4b889b
004b8892 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b8895 83c001                   add        eax, 1
004b8898 8945f8                   mov        dword ptr [ebp - 8], eax
004b889b 837df804                 cmp        dword ptr [ebp - 8], 4
004b889f 7d3b                     jge        0x4b88dc
004b88a1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b88a4 83c134                   add        ecx, 0x34
004b88a7 894df4                   mov        dword ptr [ebp - 0xc], ecx
004b88aa 8b55f8                   mov        edx, dword ptr [ebp - 8]
004b88ad 52                       push       edx
004b88ae 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004b88b1 e8cabcf5ff               call       0x414580
004b88b6 8bc8                     mov        ecx, eax
004b88b8 e87371f9ff               call       0x44fa30
004b88bd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b88c0 83c044                   add        eax, 0x44
004b88c3 8945f0                   mov        dword ptr [ebp - 0x10], eax
004b88c6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b88c9 51                       push       ecx
004b88ca 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b88cd e8aebcf5ff               call       0x414580
004b88d2 8bc8                     mov        ecx, eax
004b88d4 e85771f9ff               call       0x44fa30
004b88d9 90                       nop        
004b88da ebb6                     jmp        0x4b8892
004b88dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b88df 83c154                   add        ecx, 0x54
004b88e2 e84971f9ff               call       0x44fa30
004b88e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b88ea 83c158                   add        ecx, 0x58
004b88ed e83e71f9ff               call       0x44fa30
004b88f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b88f5 83c15c                   add        ecx, 0x5c
004b88f8 e83371f9ff               call       0x44fa30
004b88fd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8900 83c160                   add        ecx, 0x60
004b8903 e82871f9ff               call       0x44fa30
004b8908 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b890b 83c164                   add        ecx, 0x64
004b890e e81d71f9ff               call       0x44fa30
004b8913 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8916 83c168                   add        ecx, 0x68
004b8919 e81271f9ff               call       0x44fa30
004b891e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8921 83c170                   add        ecx, 0x70
004b8924 e80771f9ff               call       0x44fa30
004b8929 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b892c 83c16c                   add        ecx, 0x6c
004b892f e8fc70f9ff               call       0x44fa30
004b8934 90                       nop        
004b8935 8be5                     mov        esp, ebp
004b8937 5d                       pop        ebp
004b8938 c3                       ret        
004b8939 cc                       int3       
004b893a cc                       int3       
004b893b cc                       int3       
004b893c cc                       int3       
004b893d cc                       int3       
004b893e cc                       int3       
004b893f cc                       int3       
