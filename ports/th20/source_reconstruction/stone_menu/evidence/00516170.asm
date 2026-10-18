00516170 55                       push       ebp
00516171 8bec                     mov        ebp, esp
00516173 6aff                     push       -1
00516175 6800765600               push       0x567600
0051617a 64a100000000             mov        eax, dword ptr fs:[0]
00516180 50                       push       eax
00516181 51                       push       ecx
00516182 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00516187 33c5                     xor        eax, ebp
00516189 50                       push       eax
0051618a 8d45f4                   lea        eax, [ebp - 0xc]
0051618d 64a300000000             mov        dword ptr fs:[0], eax
00516193 894df0                   mov        dword ptr [ebp - 0x10], ecx
00516196 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00516199 c700784d5700             mov        dword ptr [eax], 0x574d78
0051619f 68644e5700               push       0x574e64
005161a4 e80765efff               call       0x40c6b0
005161a9 83c404                   add        esp, 4
005161ac 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005161af 8b5108                   mov        edx, dword ptr [ecx + 8]
005161b2 52                       push       edx
005161b3 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
005161b9 e8f2c2efff               call       0x4124b0
005161be 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
005161c1 8b480c                   mov        ecx, dword ptr [eax + 0xc]
005161c4 51                       push       ecx
005161c5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
005161cb e8e0c2efff               call       0x4124b0
005161d0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005161d3 81c1c4000000             add        ecx, 0xc4
005161d9 e8f29af3ff               call       0x44fcd0
005161de 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005161e1 81c1cc000000             add        ecx, 0xcc
005161e7 e8e49af3ff               call       0x44fcd0
005161ec 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005161ef 81c1d0000000             add        ecx, 0xd0
005161f5 e8d69af3ff               call       0x44fcd0
005161fa 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005161fd 81c1c8000000             add        ecx, 0xc8
00516203 e8c89af3ff               call       0x44fcd0
00516208 6a18                     push       0x18
0051620a 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00516210 e81b62f3ff               call       0x44c430
00516215 90                       nop        
00516216 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00516219 83c174                   add        ecx, 0x74
0051621c e84f98f9ff               call       0x4afa70
00516221 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00516224 83c128                   add        ecx, 0x28
00516227 e84498f9ff               call       0x4afa70
0051622c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051622f e8bc9bf0ff               call       0x41fdf0
00516234 90                       nop        
00516235 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00516238 64890d00000000           mov        dword ptr fs:[0], ecx
0051623f 59                       pop        ecx
00516240 8be5                     mov        esp, ebp
00516242 5d                       pop        ebp
00516243 c3                       ret        
00516244 cc                       int3       
00516245 cc                       int3       
00516246 cc                       int3       
00516247 cc                       int3       
00516248 cc                       int3       
00516249 cc                       int3       
0051624a cc                       int3       
0051624b cc                       int3       
0051624c cc                       int3       
0051624d cc                       int3       
0051624e cc                       int3       
0051624f cc                       int3       
