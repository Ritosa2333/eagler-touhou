00477ff0 55                       push       ebp
00477ff1 8bec                     mov        ebp, esp
00477ff3 83ec0c                   sub        esp, 0xc
00477ff6 894dfc                   mov        dword ptr [ebp - 4], ecx
00477ff9 c745f80a000000           mov        dword ptr [ebp - 8], 0xa
00478000 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00478007 8d45f8                   lea        eax, [ebp - 8]
0047800a 50                       push       eax
0047800b 8d4df4                   lea        ecx, [ebp - 0xc]
0047800e 51                       push       ecx
0047800f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00478012 81c2cc000000             add        edx, 0xcc
00478018 52                       push       edx
00478019 e85291ffff               call       0x471170
0047801e 83c40c                   add        esp, 0xc
00478021 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478024 8b10                     mov        edx, dword ptr [eax]
00478026 8991cc000000             mov        dword ptr [ecx + 0xcc], edx
0047802c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047802f 8b80cc000000             mov        eax, dword ptr [eax + 0xcc]
00478035 8be5                     mov        esp, ebp
00478037 5d                       pop        ebp
00478038 c3                       ret        
00478039 cc                       int3       
0047803a cc                       int3       
0047803b cc                       int3       
0047803c cc                       int3       
0047803d cc                       int3       
0047803e cc                       int3       
0047803f cc                       int3       
