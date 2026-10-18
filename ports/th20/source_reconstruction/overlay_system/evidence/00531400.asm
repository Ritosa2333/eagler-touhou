00531400 55                       push       ebp
00531401 8bec                     mov        ebp, esp
00531403 83ec0c                   sub        esp, 0xc
00531406 894df4                   mov        dword ptr [ebp - 0xc], ecx
00531409 6a00                     push       0
0053140b e8702cf3ff               call       0x464080
00531410 83c404                   add        esp, 4
00531413 8bc8                     mov        ecx, eax
00531415 e8e62ff3ff               call       0x464400
0053141a 0fb6c0                   movzx      eax, al
0053141d 85c0                     test       eax, eax
0053141f 741a                     je         0x53143b
00531421 6a00                     push       0
00531423 e8582cf3ff               call       0x464080
00531428 83c404                   add        esp, 4
0053142b 8945fc                   mov        dword ptr [ebp - 4], eax
0053142e 6a14                     push       0x14
00531430 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531433 e808d0f8ff               call       0x4be440
00531438 90                       nop        
00531439 eb18                     jmp        0x531453
0053143b 6a00                     push       0
0053143d e83e2cf3ff               call       0x464080
00531442 83c404                   add        esp, 4
00531445 8945f8                   mov        dword ptr [ebp - 8], eax
00531448 6a1e                     push       0x1e
0053144a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053144d e8eecff8ff               call       0x4be440
00531452 90                       nop        
00531453 8be5                     mov        esp, ebp
00531455 5d                       pop        ebp
00531456 c3                       ret        
00531457 cc                       int3       
00531458 cc                       int3       
00531459 cc                       int3       
0053145a cc                       int3       
0053145b cc                       int3       
0053145c cc                       int3       
0053145d cc                       int3       
0053145e cc                       int3       
0053145f cc                       int3       
