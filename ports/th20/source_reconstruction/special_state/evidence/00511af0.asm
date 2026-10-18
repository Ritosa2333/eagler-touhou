00511af0 55                       push       ebp
00511af1 8bec                     mov        ebp, esp
00511af3 83ec20                   sub        esp, 0x20
00511af6 894dfc                   mov        dword ptr [ebp - 4], ecx
00511af9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511afc 83781c07                 cmp        dword ptr [eax + 0x1c], 7
00511b00 7524                     jne        0x511b26
00511b02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b05 0fb611                   movzx      edx, byte ptr [ecx]
00511b08 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511b0b 0fb64801                 movzx      ecx, byte ptr [eax + 1]
00511b0f 03d1                     add        edx, ecx
00511b11 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511b14 8810                     mov        byte ptr [eax], dl
00511b16 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b19 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511b1c 8a02                     mov        al, byte ptr [edx]
00511b1e 884104                   mov        byte ptr [ecx + 4], al
00511b21 e9b2010000               jmp        0x511cd8
00511b26 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b29 83791c11                 cmp        dword ptr [ecx + 0x1c], 0x11
00511b2d 753a                     jne        0x511b69
00511b2f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511b32 0fb602                   movzx      eax, byte ptr [edx]
00511b35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b38 0fb65103                 movzx      edx, byte ptr [ecx + 3]
00511b3c 03c2                     add        eax, edx
00511b3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b41 8801                     mov        byte ptr [ecx], al
00511b43 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511b46 0fb64203                 movzx      eax, byte ptr [edx + 3]
00511b4a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b4d 0fb65101                 movzx      edx, byte ptr [ecx + 1]
00511b51 03c2                     add        eax, edx
00511b53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b56 884103                   mov        byte ptr [ecx + 3], al
00511b59 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511b5c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511b5f 8a08                     mov        cl, byte ptr [eax]
00511b61 884a04                   mov        byte ptr [edx + 4], cl
00511b64 e96f010000               jmp        0x511cd8
00511b69 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511b6c 837a1c08                 cmp        dword ptr [edx + 0x1c], 8
00511b70 0f8527010000             jne        0x511c9d
00511b76 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511b79 83c108                   add        ecx, 8
00511b7c e84f20f1ff               call       0x423bd0
00511b81 d95df4                   fstp       dword ptr [ebp - 0xc]
00511b84 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00511b89 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511b8c f30f2a4818               cvtsi2ss   xmm1, dword ptr [eax + 0x18]
00511b91 f30f5ec1                 divss      xmm0, xmm1
00511b95 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00511b9a f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511b9f f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00511ba7 f30f104df8               movss      xmm1, dword ptr [ebp - 8]
00511bac f30f5c0dccc85600         subss      xmm1, dword ptr [0x56c8cc]
00511bb4 f30f59c1                 mulss      xmm0, xmm1
00511bb8 f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
00511bc0 f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
00511bc5 f30f580dccc85600         addss      xmm1, dword ptr [0x56c8cc]
00511bcd f30f59c1                 mulss      xmm0, xmm1
00511bd1 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00511bd6 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511bdb f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511be0 f30f100dd0c85600         movss      xmm1, dword ptr [0x56c8d0]
00511be8 f30f594df8               mulss      xmm1, dword ptr [ebp - 8]
00511bed f30f1015d8c85600         movss      xmm2, dword ptr [0x56c8d8]
00511bf5 f30f5cd1                 subss      xmm2, xmm1
00511bf9 f30f59c2                 mulss      xmm0, xmm2
00511bfd f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00511c02 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00511c0a f30f5c45f8               subss      xmm0, dword ptr [ebp - 8]
00511c0f f30f100dccc85600         movss      xmm1, dword ptr [0x56c8cc]
00511c17 f30f5c4df8               subss      xmm1, dword ptr [ebp - 8]
00511c1c f30f59c1                 mulss      xmm0, xmm1
00511c20 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511c25 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00511c2a f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00511c2f f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00511c37 f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511c3c f30f5945f8               mulss      xmm0, dword ptr [ebp - 8]
00511c41 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00511c46 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511c49 0fb611                   movzx      edx, byte ptr [ecx]
00511c4c f30f2ac2                 cvtsi2ss   xmm0, edx
00511c50 f30f5945f0               mulss      xmm0, dword ptr [ebp - 0x10]
00511c55 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511c58 0fb64801                 movzx      ecx, byte ptr [eax + 1]
00511c5c f30f2ac9                 cvtsi2ss   xmm1, ecx
00511c60 f30f594dec               mulss      xmm1, dword ptr [ebp - 0x14]
00511c65 f30f58c1                 addss      xmm0, xmm1
00511c69 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511c6c 0fb64202                 movzx      eax, byte ptr [edx + 2]
00511c70 f30f2ac8                 cvtsi2ss   xmm1, eax
00511c74 f30f594de8               mulss      xmm1, dword ptr [ebp - 0x18]
00511c79 f30f58c1                 addss      xmm0, xmm1
00511c7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511c80 0fb65103                 movzx      edx, byte ptr [ecx + 3]
00511c84 f30f2aca                 cvtsi2ss   xmm1, edx
00511c88 f30f594de4               mulss      xmm1, dword ptr [ebp - 0x1c]
00511c8d f30f58c1                 addss      xmm0, xmm1
00511c91 f30f2cc0                 cvttss2si  eax, xmm0
00511c95 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511c98 884104                   mov        byte ptr [ecx + 4], al
00511c9b eb3b                     jmp        0x511cd8
00511c9d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511ca0 e83b200000               call       0x513ce0
00511ca5 d95de0                   fstp       dword ptr [ebp - 0x20]
00511ca8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511cab 0fb64201                 movzx      eax, byte ptr [edx + 1]
00511caf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511cb2 0fb611                   movzx      edx, byte ptr [ecx]
00511cb5 2bc2                     sub        eax, edx
00511cb7 f30f2ac0                 cvtsi2ss   xmm0, eax
00511cbb f30f5945e0               mulss      xmm0, dword ptr [ebp - 0x20]
00511cc0 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511cc3 0fb608                   movzx      ecx, byte ptr [eax]
00511cc6 f30f2ac9                 cvtsi2ss   xmm1, ecx
00511cca f30f58c1                 addss      xmm0, xmm1
00511cce f30f2cd0                 cvttss2si  edx, xmm0
00511cd2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511cd5 885004                   mov        byte ptr [eax + 4], dl
00511cd8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00511cdb 0fb64104                 movzx      eax, byte ptr [ecx + 4]
00511cdf 8be5                     mov        esp, ebp
00511ce1 5d                       pop        ebp
00511ce2 c3                       ret        
00511ce3 cc                       int3       
00511ce4 cc                       int3       
00511ce5 cc                       int3       
00511ce6 cc                       int3       
00511ce7 cc                       int3       
00511ce8 cc                       int3       
00511ce9 cc                       int3       
00511cea cc                       int3       
00511ceb cc                       int3       
00511cec cc                       int3       
00511ced cc                       int3       
00511cee cc                       int3       
00511cef cc                       int3       
