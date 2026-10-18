00438600 55                       push       ebp
00438601 8bec                     mov        ebp, esp
00438603 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00438608 f30f58450c               addss      xmm0, dword ptr [ebp + 0xc]
0043860d 51                       push       ecx
0043860e f30f110424               movss      dword ptr [esp], xmm0
00438613 e828ffffff               call       0x438540
00438618 83c404                   add        esp, 4
0043861b 5d                       pop        ebp
0043861c c3                       ret        
0043861d cc                       int3       
0043861e cc                       int3       
0043861f cc                       int3       
