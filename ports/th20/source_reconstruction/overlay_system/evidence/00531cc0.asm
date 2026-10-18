00531cc0 55                       push       ebp
00531cc1 8bec                     mov        ebp, esp
00531cc3 51                       push       ecx
00531cc4 894dfc                   mov        dword ptr [ebp - 4], ecx
00531cc7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531cca e801deffff               call       0x52fad0
00531ccf 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531cd2 c700605d5700             mov        dword ptr [eax], 0x575d60
00531cd8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531cdb 83c138                   add        ecx, 0x38
00531cde e8dd3fefff               call       0x425cc0
00531ce3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531ce6 83c13c                   add        ecx, 0x3c
00531ce9 e8a210efff               call       0x422d90
00531cee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531cf1 83c14c                   add        ecx, 0x4c
00531cf4 e89710efff               call       0x422d90
00531cf9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531cfc 0f57c0                   xorps      xmm0, xmm0
00531cff f30f11415c               movss      dword ptr [ecx + 0x5c], xmm0
00531d04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531d07 83c160                   add        ecx, 0x60
00531d0a e8d15cf1ff               call       0x4479e0
00531d0f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531d12 81c18c000000             add        ecx, 0x8c
00531d18 e8f310efff               call       0x422e10
00531d1d 90                       nop        
00531d1e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531d21 8be5                     mov        esp, ebp
00531d23 5d                       pop        ebp
00531d24 c3                       ret        
00531d25 cc                       int3       
00531d26 cc                       int3       
00531d27 cc                       int3       
00531d28 cc                       int3       
00531d29 cc                       int3       
00531d2a cc                       int3       
00531d2b cc                       int3       
00531d2c cc                       int3       
00531d2d cc                       int3       
00531d2e cc                       int3       
00531d2f cc                       int3       
