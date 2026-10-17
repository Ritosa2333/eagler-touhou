00506360 55                       push       ebp
00506361 8bec                     mov        ebp, esp
00506363 6aff                     push       -1
00506365 688d835600               push       0x56838d
0050636a 64a100000000             mov        eax, dword ptr fs:[0]
00506370 50                       push       eax
00506371 83ec20                   sub        esp, 0x20
00506374 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00506379 33c5                     xor        eax, ebp
0050637b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0050637e 50                       push       eax
0050637f 8d45f4                   lea        eax, [ebp - 0xc]
00506382 64a300000000             mov        dword ptr fs:[0], eax
00506388 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0050638b 837d0800                 cmp        dword ptr [ebp + 8], 0
0050638f 7507                     jne        0x506398
00506391 33c0                     xor        eax, eax
00506393 e9a3000000               jmp        0x50643b
00506398 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0050639b 0530240100               add        eax, 0x12430
005063a0 8945e4                   mov        dword ptr [ebp - 0x1c], eax
005063a3 6a08                     push       8
005063a5 8d4de8                   lea        ecx, [ebp - 0x18]
005063a8 e8d35cf0ff               call       0x40c080
005063ad 8d4de8                   lea        ecx, [ebp - 0x18]
005063b0 51                       push       ecx
005063b1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005063b4 e8c7bef0ff               call       0x412280
005063b9 c745fc00000000           mov        dword ptr [ebp - 4], 0
005063c0 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005063c3 e878c1f0ff               call       0x412540
005063c8 8945d8                   mov        dword ptr [ebp - 0x28], eax
005063cb eb09                     jmp        0x5063d6
005063cd 8d4de8                   lea        ecx, [ebp - 0x18]
005063d0 e85bb8f0ff               call       0x411c30
005063d5 90                       nop        
005063d6 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
005063d9 52                       push       edx
005063da 8d4de8                   lea        ecx, [ebp - 0x18]
005063dd e8ceb7f0ff               call       0x411bb0
005063e2 0fb6c0                   movzx      eax, al
005063e5 85c0                     test       eax, eax
005063e7 7440                     je         0x506429
005063e9 8d4de8                   lea        ecx, [ebp - 0x18]
005063ec e80f5ff0ff               call       0x40c300
005063f1 8945e0                   mov        dword ptr [ebp - 0x20], eax
005063f4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
005063f7 e8648df0ff               call       0x40f160
005063fc 8b08                     mov        ecx, dword ptr [eax]
005063fe 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00506401 3b5508                   cmp        edx, dword ptr [ebp + 8]
00506404 7521                     jne        0x506427
00506406 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00506409 e8528df0ff               call       0x40f160
0050640e 8b00                     mov        eax, dword ptr [eax]
00506410 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00506413 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0050641a 8d4de8                   lea        ecx, [ebp - 0x18]
0050641d e8deb6f0ff               call       0x411b00
00506422 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00506425 eb14                     jmp        0x50643b
00506427 eba4                     jmp        0x5063cd
00506429 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00506430 8d4de8                   lea        ecx, [ebp - 0x18]
00506433 e8c8b6f0ff               call       0x411b00
00506438 90                       nop        
00506439 33c0                     xor        eax, eax
0050643b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050643e 64890d00000000           mov        dword ptr fs:[0], ecx
00506445 59                       pop        ecx
00506446 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00506449 33cd                     xor        ecx, ebp
0050644b e89ec50300               call       0x5429ee
00506450 8be5                     mov        esp, ebp
00506452 5d                       pop        ebp
00506453 c20400                   ret        4
00506456 cc                       int3       
00506457 cc                       int3       
00506458 cc                       int3       
00506459 cc                       int3       
0050645a cc                       int3       
0050645b cc                       int3       
0050645c cc                       int3       
0050645d cc                       int3       
0050645e cc                       int3       
0050645f cc                       int3       
