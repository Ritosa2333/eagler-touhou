004672e0 55                       push       ebp
004672e1 8bec                     mov        ebp, esp
004672e3 51                       push       ecx
004672e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004672e7 8b4508                   mov        eax, dword ptr [ebp + 8]
004672ea 50                       push       eax
004672eb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004672ee e86df8fdff               call       0x446b60
004672f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004672f6 c70188f25600             mov        dword ptr [ecx], 0x56f288
004672fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004672ff 83c108                   add        ecx, 8
00467302 e889f0ffff               call       0x466390
00467307 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046730a 81c1f8000000             add        ecx, 0xf8
00467310 e82bf0ffff               call       0x466340
00467315 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467318 81c170010000             add        ecx, 0x170
0046731e e8bd0afeff               call       0x447de0
00467323 8b55fc                   mov        edx, dword ptr [ebp - 4]
00467326 0f57c0                   xorps      xmm0, xmm0
00467329 f30f118274010000         movss      dword ptr [edx + 0x174], xmm0
00467331 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467334 81c178010000             add        ecx, 0x178
0046733a e851bafbff               call       0x422d90
0046733f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00467342 c7808801000000000000     mov        dword ptr [eax + 0x188], 0
0046734c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046734f 81c18c010000             add        ecx, 0x18c
00467355 e84625fdff               call       0x4398a0
0046735a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046735d 81c194010000             add        ecx, 0x194
00467363 e83825fdff               call       0x4398a0
00467368 90                       nop        
00467369 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046736c 8be5                     mov        esp, ebp
0046736e 5d                       pop        ebp
0046736f c20400                   ret        4
00467372 cc                       int3       
00467373 cc                       int3       
00467374 cc                       int3       
00467375 cc                       int3       
00467376 cc                       int3       
00467377 cc                       int3       
00467378 cc                       int3       
00467379 cc                       int3       
0046737a cc                       int3       
0046737b cc                       int3       
0046737c cc                       int3       
0046737d cc                       int3       
0046737e cc                       int3       
0046737f cc                       int3       
