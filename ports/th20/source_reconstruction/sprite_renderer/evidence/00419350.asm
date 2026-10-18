00419350 55                       push       ebp
00419351 8bec                     mov        ebp, esp
00419353 51                       push       ecx
00419354 894dfc                   mov        dword ptr [ebp - 4], ecx
00419357 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041935a e801ff0200               call       0x449260
0041935f 8b4508                   mov        eax, dword ptr [ebp + 8]
00419362 83e001                   and        eax, 1
00419365 7411                     je         0x419378
00419367 68940ed407               push       0x7d40e94
0041936c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0041936f 51                       push       ecx
00419370 e818941200               call       0x54278d
00419375 83c408                   add        esp, 8
00419378 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041937b 8be5                     mov        esp, ebp
0041937d 5d                       pop        ebp
0041937e c20400                   ret        4
00419381 cc                       int3       
00419382 cc                       int3       
00419383 cc                       int3       
00419384 cc                       int3       
00419385 cc                       int3       
00419386 cc                       int3       
00419387 cc                       int3       
00419388 cc                       int3       
00419389 cc                       int3       
0041938a cc                       int3       
0041938b cc                       int3       
0041938c cc                       int3       
0041938d cc                       int3       
0041938e cc                       int3       
0041938f cc                       int3       
