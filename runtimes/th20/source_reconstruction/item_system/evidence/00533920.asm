00533920 55                       push       ebp
00533921 8bec                     mov        ebp, esp
00533923 83ec0c                   sub        esp, 0xc
00533926 894dfc                   mov        dword ptr [ebp - 4], ecx
00533929 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053392c 8b484c                   mov        ecx, dword ptr [eax + 0x4c]
0053392f 034d08                   add        ecx, dword ptr [ebp + 8]
00533932 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533935 894a4c                   mov        dword ptr [edx + 0x4c], ecx
00533938 c745f8f4010000           mov        dword ptr [ebp - 8], 0x1f4
0053393f c745f400000000           mov        dword ptr [ebp - 0xc], 0
00533946 8d45f8                   lea        eax, [ebp - 8]
00533949 50                       push       eax
0053394a 8d4df4                   lea        ecx, [ebp - 0xc]
0053394d 51                       push       ecx
0053394e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00533951 83c24c                   add        edx, 0x4c
00533954 52                       push       edx
00533955 e816d8f3ff               call       0x471170
0053395a 83c40c                   add        esp, 0xc
0053395d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00533960 8b10                     mov        edx, dword ptr [eax]
00533962 89514c                   mov        dword ptr [ecx + 0x4c], edx
00533965 8be5                     mov        esp, ebp
00533967 5d                       pop        ebp
00533968 c20400                   ret        4
0053396b cc                       int3       
0053396c cc                       int3       
0053396d cc                       int3       
0053396e cc                       int3       
0053396f cc                       int3       
