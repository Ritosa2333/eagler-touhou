00457300 55                       push       ebp
00457301 8bec                     mov        ebp, esp
00457303 83ec0c                   sub        esp, 0xc
00457306 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0045730b f30f5c4510               subss      xmm0, dword ptr [ebp + 0x10]
00457310 51                       push       ecx
00457311 f30f110424               movss      dword ptr [esp], xmm0
00457316 e865e3feff               call       0x445680
0045731b 83c404                   add        esp, 4
0045731e d95df8                   fstp       dword ptr [ebp - 8]
00457321 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00457326 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045732e 0f2f45f8                 comiss     xmm0, dword ptr [ebp - 8]
00457332 7637                     jbe        0x45736b
00457334 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
00457339 f30f5c4514               subss      xmm0, dword ptr [ebp + 0x14]
0045733e 51                       push       ecx
0045733f f30f110424               movss      dword ptr [esp], xmm0
00457344 e837e3feff               call       0x445680
00457349 83c404                   add        esp, 4
0045734c d95df4                   fstp       dword ptr [ebp - 0xc]
0045734f f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
00457354 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0045735c 0f2f45f4                 comiss     xmm0, dword ptr [ebp - 0xc]
00457360 7609                     jbe        0x45736b
00457362 c745fc01000000           mov        dword ptr [ebp - 4], 1
00457369 eb07                     jmp        0x457372
0045736b c745fc00000000           mov        dword ptr [ebp - 4], 0
00457372 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
00457376 8be5                     mov        esp, ebp
00457378 5d                       pop        ebp
00457379 c3                       ret        
0045737a cc                       int3       
0045737b cc                       int3       
0045737c cc                       int3       
0045737d cc                       int3       
0045737e cc                       int3       
0045737f cc                       int3       
