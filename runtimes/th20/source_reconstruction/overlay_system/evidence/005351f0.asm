005351f0 55                       push       ebp
005351f1 8bec                     mov        ebp, esp
005351f3 83ec0c                   sub        esp, 0xc
005351f6 894dfc                   mov        dword ptr [ebp - 4], ecx
005351f9 e822b3ffff               call       0x530520
005351fe 0fb6c0                   movzx      eax, al
00535201 85c0                     test       eax, eax
00535203 753a                     jne        0x53523f
00535205 6a00                     push       0
00535207 e824b6f2ff               call       0x460830
0053520c 83c404                   add        esp, 4
0053520f 8bc8                     mov        ecx, eax
00535211 e89aa4fcff               call       0x4ff6b0
00535216 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535219 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053521c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
00535220 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00535223 8d4c020a                 lea        ecx, [edx + eax + 0xa]
00535227 894df8                   mov        dword ptr [ebp - 8], ecx
0053522a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053522d 52                       push       edx
0053522e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00535231 50                       push       eax
00535232 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00535235 51                       push       ecx
00535236 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535239 e802fbfcff               call       0x504d40
0053523e 90                       nop        
0053523f 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00535242 52                       push       edx
00535243 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535246 e895010000               call       0x5353e0
0053524b 90                       nop        
0053524c 8be5                     mov        esp, ebp
0053524e 5d                       pop        ebp
0053524f c20c00                   ret        0xc
00535252 cc                       int3       
00535253 cc                       int3       
00535254 cc                       int3       
00535255 cc                       int3       
00535256 cc                       int3       
00535257 cc                       int3       
00535258 cc                       int3       
00535259 cc                       int3       
0053525a cc                       int3       
0053525b cc                       int3       
0053525c cc                       int3       
0053525d cc                       int3       
0053525e cc                       int3       
0053525f cc                       int3       
