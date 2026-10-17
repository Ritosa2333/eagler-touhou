0047cc90 55                       push       ebp
0047cc91 8bec                     mov        ebp, esp
0047cc93 6aff                     push       -1
0047cc95 688d905600               push       0x56908d
0047cc9a 64a100000000             mov        eax, dword ptr fs:[0]
0047cca0 50                       push       eax
0047cca1 83ec60                   sub        esp, 0x60
0047cca4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047cca9 33c5                     xor        eax, ebp
0047ccab 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047ccae 50                       push       eax
0047ccaf 8d45f4                   lea        eax, [ebp - 0xc]
0047ccb2 64a300000000             mov        dword ptr fs:[0], eax
0047ccb8 894dc8                   mov        dword ptr [ebp - 0x38], ecx
0047ccbb 8d4dd0                   lea        ecx, [ebp - 0x30]
0047ccbe e84d61faff               call       0x422e10
0047ccc3 8d4ddc                   lea        ecx, [ebp - 0x24]
0047ccc6 e84561faff               call       0x422e10
0047cccb c745c400000000           mov        dword ptr [ebp - 0x3c], 0
0047ccd2 51                       push       ecx
0047ccd3 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0047ccdb f30f110424               movss      dword ptr [esp], xmm0
0047cce0 8d45ac                   lea        eax, [ebp - 0x54]
0047cce3 50                       push       eax
0047cce4 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0047cce7 e80463fdff               call       0x452ff0
0047ccec 8b08                     mov        ecx, dword ptr [eax]
0047ccee 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0047ccf1 8b5004                   mov        edx, dword ptr [eax + 4]
0047ccf4 8955e0                   mov        dword ptr [ebp - 0x20], edx
0047ccf7 8b4008                   mov        eax, dword ptr [eax + 8]
0047ccfa 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047ccfd 8d4ddc                   lea        ecx, [ebp - 0x24]
0047cd00 51                       push       ecx
0047cd01 8d55a0                   lea        edx, [ebp - 0x60]
0047cd04 52                       push       edx
0047cd05 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047cd08 e833c7faff               call       0x429440
0047cd0d 8b08                     mov        ecx, dword ptr [eax]
0047cd0f 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0047cd12 8b5004                   mov        edx, dword ptr [eax + 4]
0047cd15 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0047cd18 8b4008                   mov        eax, dword ptr [eax + 8]
0047cd1b 8945d8                   mov        dword ptr [ebp - 0x28], eax
0047cd1e 8d4ddc                   lea        ecx, [ebp - 0x24]
0047cd21 51                       push       ecx
0047cd22 8d5594                   lea        edx, [ebp - 0x6c]
0047cd25 52                       push       edx
0047cd26 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047cd29 e842c8faff               call       0x429570
0047cd2e 8b08                     mov        ecx, dword ptr [eax]
0047cd30 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0047cd33 8b5004                   mov        edx, dword ptr [eax + 4]
0047cd36 8955e0                   mov        dword ptr [ebp - 0x20], edx
0047cd39 8b4008                   mov        eax, dword ptr [eax + 8]
0047cd3c 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0047cd3f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0047cd42 e8298c0000               call       0x485970
0047cd47 8945c0                   mov        dword ptr [ebp - 0x40], eax
0047cd4a 6a08                     push       8
0047cd4c 8d4de8                   lea        ecx, [ebp - 0x18]
0047cd4f e82cf3f8ff               call       0x40c080
0047cd54 8d4de8                   lea        ecx, [ebp - 0x18]
0047cd57 51                       push       ecx
0047cd58 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cd5b e82055f9ff               call       0x412280
0047cd60 c745fc00000000           mov        dword ptr [ebp - 4], 0
0047cd67 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047cd6a e8d157f9ff               call       0x412540
0047cd6f 8945bc                   mov        dword ptr [ebp - 0x44], eax
0047cd72 eb09                     jmp        0x47cd7d
0047cd74 8d4de8                   lea        ecx, [ebp - 0x18]
0047cd77 e8b44ef9ff               call       0x411c30
0047cd7c 90                       nop        
0047cd7d 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0047cd80 52                       push       edx
0047cd81 8d4de8                   lea        ecx, [ebp - 0x18]
0047cd84 e8274ef9ff               call       0x411bb0
0047cd89 0fb6c0                   movzx      eax, al
0047cd8c 85c0                     test       eax, eax
0047cd8e 0f8491010000             je         0x47cf25
0047cd94 8d4de8                   lea        ecx, [ebp - 0x18]
0047cd97 e864f5f8ff               call       0x40c300
0047cd9c 8945b8                   mov        dword ptr [ebp - 0x48], eax
0047cd9f 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0047cda2 e859f5f8ff               call       0x40c300
0047cda7 8945cc                   mov        dword ptr [ebp - 0x34], eax
0047cdaa 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047cdad 83795002                 cmp        dword ptr [ecx + 0x50], 2
0047cdb1 740b                     je         0x47cdbe
0047cdb3 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0047cdb6 837a5001                 cmp        dword ptr [edx + 0x50], 1
0047cdba 7402                     je         0x47cdbe
0047cdbc ebb6                     jmp        0x47cd74
0047cdbe 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0047cdc1 83781800                 cmp        dword ptr [eax + 0x18], 0
0047cdc5 7402                     je         0x47cdc9
0047cdc7 ebab                     jmp        0x47cd74
0047cdc9 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047cdcc 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0047cdcf f30f10417c               movss      xmm0, dword ptr [ecx + 0x7c]
0047cdd4 f30f594248               mulss      xmm0, dword ptr [edx + 0x48]
0047cdd9 51                       push       ecx
0047cdda f30f110424               movss      dword ptr [esp], xmm0
0047cddf 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0047cde2 51                       push       ecx
0047cde3 f30f104068               movss      xmm0, dword ptr [eax + 0x68]
0047cde8 f30f110424               movss      dword ptr [esp], xmm0
0047cded 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047cdf0 51                       push       ecx
0047cdf1 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
0047cdf6 f30f110424               movss      dword ptr [esp], xmm0
0047cdfb 51                       push       ecx
0047cdfc f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0047ce01 f30f110424               movss      dword ptr [esp], xmm0
0047ce06 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0047ce09 51                       push       ecx
0047ce0a f30f104204               movss      xmm0, dword ptr [edx + 4]
0047ce0f f30f110424               movss      dword ptr [esp], xmm0
0047ce14 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0047ce17 51                       push       ecx
0047ce18 f30f1000                 movss      xmm0, dword ptr [eax]
0047ce1c f30f110424               movss      dword ptr [esp], xmm0
0047ce21 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047ce24 51                       push       ecx
0047ce25 f30f104104               movss      xmm0, dword ptr [ecx + 4]
0047ce2a f30f110424               movss      dword ptr [esp], xmm0
0047ce2f 8b5508                   mov        edx, dword ptr [ebp + 8]
0047ce32 51                       push       ecx
0047ce33 f30f1002                 movss      xmm0, dword ptr [edx]
0047ce37 f30f110424               movss      dword ptr [esp], xmm0
0047ce3c e8cfa7fdff               call       0x457610
0047ce41 83c420                   add        esp, 0x20
0047ce44 0fb6c0                   movzx      eax, al
0047ce47 85c0                     test       eax, eax
0047ce49 0f8485000000             je         0x47ced4
0047ce4f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047ce52 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0047ce55 f30f10417c               movss      xmm0, dword ptr [ecx + 0x7c]
0047ce5a f30f594248               mulss      xmm0, dword ptr [edx + 0x48]
0047ce5f 51                       push       ecx
0047ce60 f30f110424               movss      dword ptr [esp], xmm0
0047ce65 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0047ce68 51                       push       ecx
0047ce69 f30f104068               movss      xmm0, dword ptr [eax + 0x68]
0047ce6e f30f110424               movss      dword ptr [esp], xmm0
0047ce73 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047ce76 51                       push       ecx
0047ce77 f30f104164               movss      xmm0, dword ptr [ecx + 0x64]
0047ce7c f30f110424               movss      dword ptr [esp], xmm0
0047ce81 51                       push       ecx
0047ce82 0f57c0                   xorps      xmm0, xmm0
0047ce85 f30f110424               movss      dword ptr [esp], xmm0
0047ce8a 51                       push       ecx
0047ce8b f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
0047ce93 f30f110424               movss      dword ptr [esp], xmm0
0047ce98 51                       push       ecx
0047ce99 f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
0047cea1 f30f110424               movss      dword ptr [esp], xmm0
0047cea6 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
0047ceae f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0047ceb6 51                       push       ecx
0047ceb7 f30f110424               movss      dword ptr [esp], xmm0
0047cebc 51                       push       ecx
0047cebd 0f57c0                   xorps      xmm0, xmm0
0047cec0 f30f110424               movss      dword ptr [esp], xmm0
0047cec5 e846a7fdff               call       0x457610
0047ceca 83c420                   add        esp, 0x20
0047cecd 0fb6d0                   movzx      edx, al
0047ced0 85d2                     test       edx, edx
0047ced2 7505                     jne        0x47ced9
0047ced4 e99bfeffff               jmp        0x47cd74
0047ced9 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
0047cedc 83c001                   add        eax, 1
0047cedf 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0047cee2 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0047cee5 8b91886d2800             mov        edx, dword ptr [ecx + 0x286d88]
0047ceeb 83c201                   add        edx, 1
0047ceee 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0047cef1 8990886d2800             mov        dword ptr [eax + 0x286d88], edx
0047cef7 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
0047cefa 83e103                   and        ecx, 3
0047cefd c1e10b                   shl        ecx, 0xb
0047cf00 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0047cf03 8b4214                   mov        eax, dword ptr [edx + 0x14]
0047cf06 25ffe7ffff               and        eax, 0xffffe7ff
0047cf0b 0bc1                     or         eax, ecx
0047cf0d 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047cf10 894114                   mov        dword ptr [ecx + 0x14], eax
0047cf13 8b5514                   mov        edx, dword ptr [ebp + 0x14]
0047cf16 52                       push       edx
0047cf17 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047cf1a e8d1f9ffff               call       0x47c8f0
0047cf1f 90                       nop        
0047cf20 e94ffeffff               jmp        0x47cd74
0047cf25 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047cf2c 8d4de8                   lea        ecx, [ebp - 0x18]
0047cf2f e8cc4bf9ff               call       0x411b00
0047cf34 90                       nop        
0047cf35 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
0047cf38 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047cf3b 64890d00000000           mov        dword ptr fs:[0], ecx
0047cf42 59                       pop        ecx
0047cf43 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047cf46 33cd                     xor        ecx, ebp
0047cf48 e8a15a0c00               call       0x5429ee
0047cf4d 8be5                     mov        esp, ebp
0047cf4f 5d                       pop        ebp
0047cf50 c21400                   ret        0x14
0047cf53 cc                       int3       
0047cf54 cc                       int3       
0047cf55 cc                       int3       
0047cf56 cc                       int3       
0047cf57 cc                       int3       
0047cf58 cc                       int3       
0047cf59 cc                       int3       
0047cf5a cc                       int3       
0047cf5b cc                       int3       
0047cf5c cc                       int3       
0047cf5d cc                       int3       
0047cf5e cc                       int3       
0047cf5f cc                       int3       
