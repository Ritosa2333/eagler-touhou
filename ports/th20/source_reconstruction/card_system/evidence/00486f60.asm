00486f60 55                       push       ebp
00486f61 8bec                     mov        ebp, esp
00486f63 83ec50                   sub        esp, 0x50
00486f66 56                       push       esi
00486f67 894dfc                   mov        dword ptr [ebp - 4], ecx
00486f6a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486f6d 8b4878                   mov        ecx, dword ptr [eax + 0x78]
00486f70 83e101                   and        ecx, 1
00486f73 0f8448030000             je         0x4872c1
00486f79 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486f7c 8b828c000000             mov        eax, dword ptr [edx + 0x8c]
00486f82 83c001                   add        eax, 1
00486f85 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486f88 89818c000000             mov        dword ptr [ecx + 0x8c], eax
00486f8e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486f91 83c224                   add        edx, 0x24
00486f94 8955f0                   mov        dword ptr [ebp - 0x10], edx
00486f97 6a3c                     push       0x3c
00486f99 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486f9c e81fc6f9ff               call       0x4235c0
00486fa1 0fb6c0                   movzx      eax, al
00486fa4 85c0                     test       eax, eax
00486fa6 741a                     je         0x486fc2
00486fa8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486fab 8b5178                   mov        edx, dword ptr [ecx + 0x78]
00486fae c1ea09                   shr        edx, 9
00486fb1 83e201                   and        edx, 1
00486fb4 750c                     jne        0x486fc2
00486fb6 8b0d9c065c00             mov        ecx, dword ptr [0x5c069c]
00486fbc e8cf0b0000               call       0x487b90
00486fc1 90                       nop        
00486fc2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486fc5 83c024                   add        eax, 0x24
00486fc8 8945ec                   mov        dword ptr [ebp - 0x14], eax
00486fcb 682c010000               push       0x12c
00486fd0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00486fd3 e8e8c5f9ff               call       0x4235c0
00486fd8 0fb6c8                   movzx      ecx, al
00486fdb 85c9                     test       ecx, ecx
00486fdd 7468                     je         0x487047
00486fdf 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486fe2 8b4278                   mov        eax, dword ptr [edx + 0x78]
00486fe5 c1e803                   shr        eax, 3
00486fe8 83e001                   and        eax, 1
00486feb 755a                     jne        0x487047
00486fed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486ff0 8b8180000000             mov        eax, dword ptr [ecx + 0x80]
00486ff6 99                       cdq        
00486ff7 b903000000               mov        ecx, 3
00486ffc f7f9                     idiv       ecx
00486ffe 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487001 8b8a80000000             mov        ecx, dword ptr [edx + 0x80]
00487007 2bc8                     sub        ecx, eax
00487009 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048700c 8bb284000000             mov        esi, dword ptr [edx + 0x84]
00487012 81ee2c010000             sub        esi, 0x12c
00487018 8bc1                     mov        eax, ecx
0048701a 99                       cdq        
0048701b f7fe                     idiv       esi
0048701d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487020 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00487023 2bd0                     sub        edx, eax
00487025 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487028 89507c                   mov        dword ptr [eax + 0x7c], edx
0048702b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048702e 8b417c                   mov        eax, dword ptr [ecx + 0x7c]
00487031 99                       cdq        
00487032 b90a000000               mov        ecx, 0xa
00487037 f7f9                     idiv       ecx
00487039 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048703c 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
0048703f 2bca                     sub        ecx, edx
00487041 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487044 894a7c                   mov        dword ptr [edx + 0x7c], ecx
00487047 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048704a 83c024                   add        eax, 0x24
0048704d 8945e8                   mov        dword ptr [ebp - 0x18], eax
00487050 6a00                     push       0
00487052 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00487055 e8e6c4f9ff               call       0x423540
0048705a 90                       nop        
0048705b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048705e 83c124                   add        ecx, 0x24
00487061 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00487064 6a78                     push       0x78
00487066 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00487069 e852c5f9ff               call       0x4235c0
0048706e 0fb6d0                   movzx      edx, al
00487071 85d2                     test       edx, edx
00487073 0f846d010000             je         0x4871e6
00487079 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048707c 8b4878                   mov        ecx, dword ptr [eax + 0x78]
0048707f c1e902                   shr        ecx, 2
00487082 83e101                   and        ecx, 1
00487085 0f85b0000000             jne        0x48713b
0048708b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048708e e80d180000               call       0x4888a0
00487093 85c0                     test       eax, eax
00487095 7523                     jne        0x4870ba
00487097 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048709a 8b8ac0000000             mov        ecx, dword ptr [edx + 0xc0]
004870a0 e8eb8ef8ff               call       0x40ff90
004870a5 8bc8                     mov        ecx, eax
004870a7 e8a497fdff               call       0x460850
004870ac f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
004870b4 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
004870b8 773b                     ja         0x4870f5
004870ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004870bd e8de170000               call       0x4888a0
004870c2 85c0                     test       eax, eax
004870c4 7470                     je         0x487136
004870c6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004870c9 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
004870cf e8bc8ef8ff               call       0x40ff90
004870d4 8bc8                     mov        ecx, eax
004870d6 e87597fdff               call       0x460850
004870db f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004870e3 f30f5c057cfe5600         subss      xmm0, dword ptr [0x56fe7c]
004870eb f30f104804               movss      xmm1, dword ptr [eax + 4]
004870f0 0f2fc8                   comiss     xmm1, xmm0
004870f3 7641                     jbe        0x487136
004870f5 c745f800000000           mov        dword ptr [ebp - 8], 0
004870fc eb09                     jmp        0x487107
004870fe 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00487101 83c101                   add        ecx, 1
00487104 894df8                   mov        dword ptr [ebp - 8], ecx
00487107 837df803                 cmp        dword ptr [ebp - 8], 3
0048710b 7d1a                     jge        0x487127
0048710d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00487110 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487113 8d4c9014                 lea        ecx, [eax + edx*4 + 0x14]
00487117 894de0                   mov        dword ptr [ebp - 0x20], ecx
0048711a 6a03                     push       3
0048711c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048711f e86c7efcff               call       0x44ef90
00487124 90                       nop        
00487125 ebd7                     jmp        0x4870fe
00487127 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048712a 8b4278                   mov        eax, dword ptr [edx + 0x78]
0048712d 83c804                   or         eax, 4
00487130 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487133 894178                   mov        dword ptr [ecx + 0x78], eax
00487136 e9ab000000               jmp        0x4871e6
0048713b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048713e 8b4278                   mov        eax, dword ptr [edx + 0x78]
00487141 c1e808                   shr        eax, 8
00487144 83e001                   and        eax, 1
00487147 7523                     jne        0x48716c
00487149 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048714c 8b89c0000000             mov        ecx, dword ptr [ecx + 0xc0]
00487152 e8398ef8ff               call       0x40ff90
00487157 8bc8                     mov        ecx, eax
00487159 e8f296fdff               call       0x460850
0048715e f30f104004               movss      xmm0, dword ptr [eax + 4]
00487163 0f2f0598cd5600           comiss     xmm0, dword ptr [0x56cd98]
0048716a 7739                     ja         0x4871a5
0048716c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048716f 8b4278                   mov        eax, dword ptr [edx + 0x78]
00487172 c1e808                   shr        eax, 8
00487175 83e001                   and        eax, 1
00487178 746c                     je         0x4871e6
0048717a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048717d 8b89c0000000             mov        ecx, dword ptr [ecx + 0xc0]
00487183 e8088ef8ff               call       0x40ff90
00487188 8bc8                     mov        ecx, eax
0048718a e8c196fdff               call       0x460850
0048718f f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00487197 f30f5c0598cd5600         subss      xmm0, dword ptr [0x56cd98]
0048719f 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
004871a3 7641                     jbe        0x4871e6
004871a5 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004871ac eb09                     jmp        0x4871b7
004871ae 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004871b1 83c201                   add        edx, 1
004871b4 8955f4                   mov        dword ptr [ebp - 0xc], edx
004871b7 837df403                 cmp        dword ptr [ebp - 0xc], 3
004871bb 7d1a                     jge        0x4871d7
004871bd 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004871c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004871c3 8d548114                 lea        edx, [ecx + eax*4 + 0x14]
004871c7 8955dc                   mov        dword ptr [ebp - 0x24], edx
004871ca 6a02                     push       2
004871cc 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004871cf e8bc7dfcff               call       0x44ef90
004871d4 90                       nop        
004871d5 ebd7                     jmp        0x4871ae
004871d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004871da 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004871dd 83e1fb                   and        ecx, 0xfffffffb
004871e0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004871e3 894a78                   mov        dword ptr [edx + 0x78], ecx
004871e6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004871e9 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
004871ef e83cb5f8ff               call       0x412730
004871f4 8945d8                   mov        dword ptr [ebp - 0x28], eax
004871f7 6a00                     push       0
004871f9 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004871fc e85fe4ffff               call       0x485660
00487201 85c0                     test       eax, eax
00487203 7468                     je         0x48726d
00487205 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487208 8b89c0000000             mov        ecx, dword ptr [ecx + 0xc0]
0048720e e81db5f8ff               call       0x412730
00487213 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00487216 6a00                     push       0
00487218 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0048721b e840e4ffff               call       0x485660
00487220 8bc8                     mov        ecx, eax
00487222 e89930ffff               call       0x47a2c0
00487227 8945d0                   mov        dword ptr [ebp - 0x30], eax
0048722a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048722d 81c2ac000000             add        edx, 0xac
00487233 52                       push       edx
00487234 8d45bc                   lea        eax, [ebp - 0x44]
00487237 50                       push       eax
00487238 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0048723b e80022faff               call       0x429440
00487240 8945cc                   mov        dword ptr [ebp - 0x34], eax
00487243 51                       push       ecx
00487244 f30f10055cfe5600         movss      xmm0, dword ptr [0x56fe5c]
0048724c f30f110424               movss      dword ptr [esp], xmm0
00487251 8d4db0                   lea        ecx, [ebp - 0x50]
00487254 51                       push       ecx
00487255 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00487258 e89320faff               call       0x4292f0
0048725d 50                       push       eax
0048725e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487261 81c1ac000000             add        ecx, 0xac
00487267 e87424faff               call       0x4296e0
0048726c 90                       nop        
0048726d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487270 83c220                   add        edx, 0x20
00487273 8955c8                   mov        dword ptr [ebp - 0x38], edx
00487276 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487279 05ac000000               add        eax, 0xac
0048727e 50                       push       eax
0048727f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00487282 e83990fcff               call       0x4502c0
00487287 90                       nop        
00487288 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048728b 8b5178                   mov        edx, dword ptr [ecx + 0x78]
0048728e c1ea05                   shr        edx, 5
00487291 83e201                   and        edx, 1
00487294 742b                     je         0x4872c1
00487296 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487299 8b88c0000000             mov        ecx, dword ptr [eax + 0xc0]
0048729f e86ccdf9ff               call       0x424010
004872a4 8bc8                     mov        ecx, eax
004872a6 e8850effff               call       0x478130
004872ab 0fb6c8                   movzx      ecx, al
004872ae 85c9                     test       ecx, ecx
004872b0 750f                     jne        0x4872c1
004872b2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004872b5 8b4278                   mov        eax, dword ptr [edx + 0x78]
004872b8 83e0df                   and        eax, 0xffffffdf
004872bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004872be 894178                   mov        dword ptr [ecx + 0x78], eax
004872c1 b801000000               mov        eax, 1
004872c6 5e                       pop        esi
004872c7 8be5                     mov        esp, ebp
004872c9 5d                       pop        ebp
004872ca c3                       ret        
004872cb cc                       int3       
004872cc cc                       int3       
004872cd cc                       int3       
004872ce cc                       int3       
004872cf cc                       int3       
