004b8830 55                       push       ebp
004b8831 8bec                     mov        ebp, esp
004b8833 83ec0c                   sub        esp, 0xc
004b8836 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8839 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b883c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004b883f 8988f0010000             mov        dword ptr [eax + 0x1f0], ecx
004b8845 c745f8ffc99a3b           mov        dword ptr [ebp - 8], 0x3b9ac9ff
004b884c c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b8853 8d55f8                   lea        edx, [ebp - 8]
004b8856 52                       push       edx
004b8857 8d45f4                   lea        eax, [ebp - 0xc]
004b885a 50                       push       eax
004b885b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b885e 81c1f0010000             add        ecx, 0x1f0
004b8864 51                       push       ecx
004b8865 e80689fbff               call       0x471170
004b886a 83c40c                   add        esp, 0xc
004b886d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8870 8b00                     mov        eax, dword ptr [eax]
004b8872 8982f0010000             mov        dword ptr [edx + 0x1f0], eax
004b8878 8be5                     mov        esp, ebp
004b887a 5d                       pop        ebp
004b887b c20400                   ret        4
004b887e cc                       int3       
004b887f cc                       int3       
