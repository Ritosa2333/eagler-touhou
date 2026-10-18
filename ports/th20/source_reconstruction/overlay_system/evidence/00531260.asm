00531260 55                       push       ebp
00531261 8bec                     mov        ebp, esp
00531263 83ec0c                   sub        esp, 0xc
00531266 894dfc                   mov        dword ptr [ebp - 4], ecx
00531269 6a00                     push       0
0053126b e8c0f5f2ff               call       0x460830
00531270 83c404                   add        esp, 4
00531273 8bc8                     mov        ecx, eax
00531275 e836e4fcff               call       0x4ff6b0
0053127a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0053127d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531280 6b480414                 imul       ecx, dword ptr [eax + 4], 0x14
00531284 034d10                   add        ecx, dword ptr [ebp + 0x10]
00531287 894df8                   mov        dword ptr [ebp - 8], ecx
0053128a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053128d 52                       push       edx
0053128e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00531291 50                       push       eax
00531292 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00531295 51                       push       ecx
00531296 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531299 e8a23afdff               call       0x504d40
0053129e 90                       nop        
0053129f 8be5                     mov        esp, ebp
005312a1 5d                       pop        ebp
005312a2 c20c00                   ret        0xc
005312a5 cc                       int3       
005312a6 cc                       int3       
005312a7 cc                       int3       
005312a8 cc                       int3       
005312a9 cc                       int3       
005312aa cc                       int3       
005312ab cc                       int3       
005312ac cc                       int3       
005312ad cc                       int3       
005312ae cc                       int3       
005312af cc                       int3       
