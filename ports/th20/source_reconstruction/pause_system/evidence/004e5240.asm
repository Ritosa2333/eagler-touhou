004e5240 55                       push       ebp
004e5241 8bec                     mov        ebp, esp
004e5243 83ec24                   sub        esp, 0x24
004e5246 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e524b 33c5                     xor        eax, ebp
004e524d 8945fc                   mov        dword ptr [ebp - 4], eax
004e5250 894de8                   mov        dword ptr [ebp - 0x18], ecx
004e5253 51                       push       ecx
004e5254 0f57c0                   xorps      xmm0, xmm0
004e5257 f30f110424               movss      dword ptr [esp], xmm0
004e525c 51                       push       ecx
004e525d f30f100528fa5600         movss      xmm0, dword ptr [0x56fa28]
004e5265 f30f110424               movss      dword ptr [esp], xmm0
004e526a 51                       push       ecx
004e526b f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
004e5273 f30f110424               movss      dword ptr [esp], xmm0
004e5278 8d4df0                   lea        ecx, [ebp - 0x10]
004e527b e850dbf3ff               call       0x422dd0
004e5280 90                       nop        
004e5281 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004e5288 eb09                     jmp        0x4e5293
004e528a 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e528d 83c001                   add        eax, 1
004e5290 8945ec                   mov        dword ptr [ebp - 0x14], eax
004e5293 837dec19                 cmp        dword ptr [ebp - 0x14], 0x19
004e5297 0f8dc2000000             jge        0x4e535f
004e529d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004e52a0 83c130                   add        ecx, 0x30
004e52a3 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004e52a6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004e52a9 52                       push       edx
004e52aa 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004e52ad e8fe5ef8ff               call       0x46b1b0
004e52b2 85c0                     test       eax, eax
004e52b4 7413                     je         0x4e52c9
004e52b6 6800ffffff               push       0xffffff00
004e52bb 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e52c1 e80ab8f8ff               call       0x470ad0
004e52c6 90                       nop        
004e52c7 eb11                     jmp        0x4e52da
004e52c9 68808080ff               push       0xff808080
004e52ce 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e52d4 e8f7b7f8ff               call       0x470ad0
004e52d9 90                       nop        
004e52da 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004e52dd 05f0000000               add        eax, 0xf0
004e52e2 8945e0                   mov        dword ptr [ebp - 0x20], eax
004e52e5 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004e52e8 51                       push       ecx
004e52e9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004e52ec e88ff2f2ff               call       0x414580
004e52f1 833800                   cmp        dword ptr [eax], 0
004e52f4 7433                     je         0x4e5329
004e52f6 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004e52f9 81c2f0000000             add        edx, 0xf0
004e52ff 8955dc                   mov        dword ptr [ebp - 0x24], edx
004e5302 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e5305 50                       push       eax
004e5306 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004e5309 e872f2f2ff               call       0x414580
004e530e 8b08                     mov        ecx, dword ptr [eax]
004e5310 e8fbc4f3ff               call       0x421810
004e5315 50                       push       eax
004e5316 8d4df0                   lea        ecx, [ebp - 0x10]
004e5319 51                       push       ecx
004e531a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004e531d 52                       push       edx
004e531e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004e5321 e86a000000               call       0x4e5390
004e5326 90                       nop        
004e5327 eb1f                     jmp        0x4e5348
004e5329 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004e532c 83c001                   add        eax, 1
004e532f 50                       push       eax
004e5330 6870255700               push       0x572570
004e5335 8d4df0                   lea        ecx, [ebp - 0x10]
004e5338 51                       push       ecx
004e5339 8b1598065c00             mov        edx, dword ptr [0x5c0698]
004e533f 52                       push       edx
004e5340 e84b76f8ff               call       0x46c990
004e5345 83c410                   add        esp, 0x10
004e5348 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
004e534d f30f5805fcf25600         addss      xmm0, dword ptr [0x56f2fc]
004e5355 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004e535a e92bffffff               jmp        0x4e528a
004e535f 6a00                     push       0
004e5361 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5367 e89437faff               call       0x488b00
004e536c 6aff                     push       -1
004e536e 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5374 e857b7f8ff               call       0x470ad0
004e5379 90                       nop        
004e537a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e537d 33cd                     xor        ecx, ebp
004e537f e86ad60500               call       0x5429ee
004e5384 8be5                     mov        esp, ebp
004e5386 5d                       pop        ebp
004e5387 c3                       ret        
004e5388 cc                       int3       
004e5389 cc                       int3       
004e538a cc                       int3       
004e538b cc                       int3       
004e538c cc                       int3       
004e538d cc                       int3       
004e538e cc                       int3       
004e538f cc                       int3       
