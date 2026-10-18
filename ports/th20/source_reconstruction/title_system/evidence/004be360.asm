004be360 55                       push       ebp
004be361 8bec                     mov        ebp, esp
004be363 83ec08                   sub        esp, 8
004be366 894dfc                   mov        dword ptr [ebp - 4], ecx
004be369 8b45fc                   mov        eax, dword ptr [ebp - 4]
004be36c 0588000000               add        eax, 0x88
004be371 8945f8                   mov        dword ptr [ebp - 8], eax
004be374 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004be377 51                       push       ecx
004be378 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004be37b e810000000               call       0x4be390
004be380 90                       nop        
004be381 8be5                     mov        esp, ebp
004be383 5d                       pop        ebp
004be384 c20400                   ret        4
004be387 cc                       int3       
004be388 cc                       int3       
004be389 cc                       int3       
004be38a cc                       int3       
004be38b cc                       int3       
004be38c cc                       int3       
004be38d cc                       int3       
004be38e cc                       int3       
004be38f cc                       int3       
