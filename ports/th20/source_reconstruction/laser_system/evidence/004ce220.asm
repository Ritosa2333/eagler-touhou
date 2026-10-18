004ce220 55                       push       ebp
004ce221 8bec                     mov        ebp, esp
004ce223 6aff                     push       -1
004ce225 684d9b5600               push       0x569b4d
004ce22a 64a100000000             mov        eax, dword ptr fs:[0]
004ce230 50                       push       eax
004ce231 81ec94000000             sub        esp, 0x94
004ce237 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004ce23c 33c5                     xor        eax, ebp
004ce23e 8945f0                   mov        dword ptr [ebp - 0x10], eax
004ce241 50                       push       eax
004ce242 8d45f4                   lea        eax, [ebp - 0xc]
004ce245 64a300000000             mov        dword ptr fs:[0], eax
004ce24b 894d88                   mov        dword ptr [ebp - 0x78], ecx
004ce24e c7458c00000000           mov        dword ptr [ebp - 0x74], 0
004ce255 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce258 e8b34bf5ff               call       0x422e10
004ce25d f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004ce265 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004ce26d 51                       push       ecx
004ce26e f30f110424               movss      dword ptr [esp], xmm0
004ce273 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004ce276 51                       push       ecx
004ce277 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004ce27c f30f110424               movss      dword ptr [esp], xmm0
004ce281 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce284 51                       push       ecx
004ce285 e8a6b0f6ff               call       0x439330
004ce28a 83c40c                   add        esp, 0xc
004ce28d 0f57c0                   xorps      xmm0, xmm0
004ce290 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004ce295 8d55e4                   lea        edx, [ebp - 0x1c]
004ce298 52                       push       edx
004ce299 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce29c e83fb4f5ff               call       0x4296e0
004ce2a1 90                       nop        
004ce2a2 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce2a5 3b450c                   cmp        eax, dword ptr [ebp + 0xc]
004ce2a8 7d18                     jge        0x4ce2c2
004ce2aa 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ce2ad 034d8c                   add        ecx, dword ptr [ebp - 0x74]
004ce2b0 0fb611                   movzx      edx, byte ptr [ecx]
004ce2b3 85d2                     test       edx, edx
004ce2b5 740b                     je         0x4ce2c2
004ce2b7 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce2ba 83c001                   add        eax, 1
004ce2bd 89458c                   mov        dword ptr [ebp - 0x74], eax
004ce2c0 ebe0                     jmp        0x4ce2a2
004ce2c2 837d8c00                 cmp        dword ptr [ebp - 0x74], 0
004ce2c6 7414                     je         0x4ce2dc
004ce2c8 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004ce2cb 0f57c0                   xorps      xmm0, xmm0
004ce2ce f30f114174               movss      dword ptr [ecx + 0x74], xmm0
004ce2d3 c7458400000000           mov        dword ptr [ebp - 0x7c], 0
004ce2da eb51                     jmp        0x4ce32d
004ce2dc c7458400000000           mov        dword ptr [ebp - 0x7c], 0
004ce2e3 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce2e6 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
004ce2e9 7d21                     jge        0x4ce30c
004ce2eb 8b4508                   mov        eax, dword ptr [ebp + 8]
004ce2ee 03458c                   add        eax, dword ptr [ebp - 0x74]
004ce2f1 0fb608                   movzx      ecx, byte ptr [eax]
004ce2f4 85c9                     test       ecx, ecx
004ce2f6 7514                     jne        0x4ce30c
004ce2f8 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce2fb 83c201                   add        edx, 1
004ce2fe 89558c                   mov        dword ptr [ebp - 0x74], edx
004ce301 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
004ce304 83c001                   add        eax, 1
004ce307 894584                   mov        dword ptr [ebp - 0x7c], eax
004ce30a ebd7                     jmp        0x4ce2e3
004ce30c 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce30f 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce312 0f8dd1010000             jge        0x4ce4e9
004ce318 f30f2a4584               cvtsi2ss   xmm0, dword ptr [ebp - 0x7c]
004ce31d f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce325 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004ce328 f30f114274               movss      dword ptr [edx + 0x74], xmm0
004ce32d 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce330 3b450c                   cmp        eax, dword ptr [ebp + 0xc]
004ce333 0f8db0010000             jge        0x4ce4e9
004ce339 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce33c 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce33f 7d18                     jge        0x4ce359
004ce341 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce344 03558c                   add        edx, dword ptr [ebp - 0x74]
004ce347 0fb602                   movzx      eax, byte ptr [edx]
004ce34a 85c0                     test       eax, eax
004ce34c 740b                     je         0x4ce359
004ce34e 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce351 83c101                   add        ecx, 1
004ce354 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004ce357 ebe0                     jmp        0x4ce339
004ce359 8b558c                   mov        edx, dword ptr [ebp - 0x74]
004ce35c 3b550c                   cmp        edx, dword ptr [ebp + 0xc]
004ce35f 7c05                     jl         0x4ce366
004ce361 e983010000               jmp        0x4ce4e9
004ce366 c7458400000000           mov        dword ptr [ebp - 0x7c], 0
004ce36d 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004ce370 894580                   mov        dword ptr [ebp - 0x80], eax
004ce373 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce376 3b4d0c                   cmp        ecx, dword ptr [ebp + 0xc]
004ce379 7d21                     jge        0x4ce39c
004ce37b 8b5508                   mov        edx, dword ptr [ebp + 8]
004ce37e 03558c                   add        edx, dword ptr [ebp - 0x74]
004ce381 0fb602                   movzx      eax, byte ptr [edx]
004ce384 85c0                     test       eax, eax
004ce386 7514                     jne        0x4ce39c
004ce388 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004ce38b 83c101                   add        ecx, 1
004ce38e 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004ce391 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
004ce394 83c201                   add        edx, 1
004ce397 895584                   mov        dword ptr [ebp - 0x7c], edx
004ce39a ebd7                     jmp        0x4ce373
004ce39c 8d4d90                   lea        ecx, [ebp - 0x70]
004ce39f e88cddfaff               call       0x47c130
004ce3a4 c745fc00000000           mov        dword ptr [ebp - 4], 0
004ce3ab f30f2a4584               cvtsi2ss   xmm0, dword ptr [ebp - 0x7c]
004ce3b0 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce3b8 f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
004ce3bd f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004ce3c2 f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
004ce3c7 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004ce3ca 83c058                   add        eax, 0x58
004ce3cd 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004ce3d3 f30f2a4580               cvtsi2ss   xmm0, dword ptr [ebp - 0x80]
004ce3d8 51                       push       ecx
004ce3d9 f30f110424               movss      dword ptr [esp], xmm0
004ce3de 8d8d6cffffff             lea        ecx, [ebp - 0x94]
004ce3e4 51                       push       ecx
004ce3e5 8d4de4                   lea        ecx, [ebp - 0x1c]
004ce3e8 e803aff5ff               call       0x4292f0
004ce3ed 50                       push       eax
004ce3ee 8d9560ffffff             lea        edx, [ebp - 0xa0]
004ce3f4 52                       push       edx
004ce3f5 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004ce3fb e870b1f5ff               call       0x429570
004ce400 8b08                     mov        ecx, dword ptr [eax]
004ce402 894d90                   mov        dword ptr [ebp - 0x70], ecx
004ce405 8b5004                   mov        edx, dword ptr [eax + 4]
004ce408 895594                   mov        dword ptr [ebp - 0x6c], edx
004ce40b 8b4008                   mov        eax, dword ptr [eax + 8]
004ce40e 894598                   mov        dword ptr [ebp - 0x68], eax
004ce411 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
004ce419 f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
004ce41e 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004ce421 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004ce426 f30f11459c               movss      dword ptr [ebp - 0x64], xmm0
004ce42b 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004ce42e f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004ce433 f30f1145ac               movss      dword ptr [ebp - 0x54], xmm0
004ce438 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004ce43b 8b884c070000             mov        ecx, dword ptr [eax + 0x74c]
004ce441 894db4                   mov        dword ptr [ebp - 0x4c], ecx
004ce444 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004ce447 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004ce44d 8945b8                   mov        dword ptr [ebp - 0x48], eax
004ce450 f30f2a4580               cvtsi2ss   xmm0, dword ptr [ebp - 0x80]
004ce455 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004ce45d 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004ce460 f30f108918070000         movss      xmm1, dword ptr [ecx + 0x718]
004ce468 f30f5cc8                 subss      xmm1, xmm0
004ce46c f30f114da8               movss      dword ptr [ebp - 0x58], xmm1
004ce471 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004ce474 8b8254070000             mov        eax, dword ptr [edx + 0x754]
004ce47a d1e8                     shr        eax, 1
004ce47c 83e001                   and        eax, 1
004ce47f 83e001                   and        eax, 1
004ce482 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004ce485 83e1fe                   and        ecx, 0xfffffffe
004ce488 0bc8                     or         ecx, eax
004ce48a 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004ce48d 0f57c0                   xorps      xmm0, xmm0
004ce490 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
004ce495 c745d8ffffffff           mov        dword ptr [ebp - 0x28], 0xffffffff
004ce49c c745dcffffffff           mov        dword ptr [ebp - 0x24], 0xffffffff
004ce4a3 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004ce4a6 8b82ec060000             mov        eax, dword ptr [edx + 0x6ec]
004ce4ac 8945e0                   mov        dword ptr [ebp - 0x20], eax
004ce4af 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004ce4b2 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004ce4b8 e8d3e5f4ff               call       0x41ca90
004ce4bd 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004ce4c3 8d5590                   lea        edx, [ebp - 0x70]
004ce4c6 52                       push       edx
004ce4c7 6a00                     push       0
004ce4c9 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004ce4cf e81c630000               call       0x4d47f0
004ce4d4 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004ce4db 8d4d90                   lea        ecx, [ebp - 0x70]
004ce4de e8cddffaff               call       0x47c4b0
004ce4e3 90                       nop        
004ce4e4 e944feffff               jmp        0x4ce32d
004ce4e9 33c0                     xor        eax, eax
004ce4eb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ce4ee 64890d00000000           mov        dword ptr fs:[0], ecx
004ce4f5 59                       pop        ecx
004ce4f6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ce4f9 33cd                     xor        ecx, ebp
004ce4fb e8ee440700               call       0x5429ee
004ce500 8be5                     mov        esp, ebp
004ce502 5d                       pop        ebp
004ce503 c20800                   ret        8
004ce506 cc                       int3       
004ce507 cc                       int3       
004ce508 cc                       int3       
004ce509 cc                       int3       
004ce50a cc                       int3       
004ce50b cc                       int3       
004ce50c cc                       int3       
004ce50d cc                       int3       
004ce50e cc                       int3       
004ce50f cc                       int3       
