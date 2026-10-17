0051f220 55                       push       ebp
0051f221 8bec                     mov        ebp, esp
0051f223 83ec08                   sub        esp, 8
0051f226 894dfc                   mov        dword ptr [ebp - 4], ecx
0051f229 833de8495c0000           cmp        dword ptr [0x5c49e8], 0
0051f230 740a                     je         0x51f23c
0051f232 b801000000               mov        eax, 1
0051f237 e98f000000               jmp        0x51f2cb
0051f23c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051f23f 8b4818                   mov        ecx, dword ptr [eax + 0x18]
0051f242 894df8                   mov        dword ptr [ebp - 8], ecx
0051f245 8b55f8                   mov        edx, dword ptr [ebp - 8]
0051f248 83ea01                   sub        edx, 1
0051f24b 8955f8                   mov        dword ptr [ebp - 8], edx
0051f24e 837df816                 cmp        dword ptr [ebp - 8], 0x16
0051f252 7772                     ja         0x51f2c6
0051f254 8b45f8                   mov        eax, dword ptr [ebp - 8]
0051f257 0fb688f8f25100           movzx      ecx, byte ptr [eax + 0x51f2f8]
0051f25e ff248dd0f25100           jmp        dword ptr [ecx*4 + 0x51f2d0]
0051f265 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f268 e893b40000               call       0x52a700
0051f26d 90                       nop        
0051f26e eb56                     jmp        0x51f2c6
0051f270 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f273 e8584e0000               call       0x5240d0
0051f278 90                       nop        
0051f279 eb4b                     jmp        0x51f2c6
0051f27b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f27e e86d650000               call       0x5257f0
0051f283 90                       nop        
0051f284 eb40                     jmp        0x51f2c6
0051f286 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f289 e8923b0000               call       0x522e20
0051f28e 90                       nop        
0051f28f eb35                     jmp        0x51f2c6
0051f291 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f294 e857850000               call       0x5277f0
0051f299 90                       nop        
0051f29a eb2a                     jmp        0x51f2c6
0051f29c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f29f e89ca80000               call       0x529b40
0051f2a4 90                       nop        
0051f2a5 eb1f                     jmp        0x51f2c6
0051f2a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f2aa e8d1190000               call       0x520c80
0051f2af 90                       nop        
0051f2b0 eb14                     jmp        0x51f2c6
0051f2b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f2b5 e896970000               call       0x528a50
0051f2ba 90                       nop        
0051f2bb eb09                     jmp        0x51f2c6
0051f2bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051f2c0 e8bbc10000               call       0x52b480
0051f2c5 90                       nop        
0051f2c6 b801000000               mov        eax, 1
0051f2cb 8be5                     mov        esp, ebp
0051f2cd 5d                       pop        ebp
0051f2ce c3                       ret        
0051f2cf 90                       nop        
0051f2d0 65f251                   push       ecx
0051f2d3 009cf251007bf2           add        byte ptr [edx + esi*8 - 0xd84ffaf], bl
0051f2da 51                       push       ecx
0051f2db 0070f2                   add        byte ptr [eax - 0xe], dh
0051f2de 51                       push       ecx
0051f2df 00a7f2510086             add        byte ptr [edi - 0x79ffae0e], ah
0051f2e5 f251                     push       ecx
0051f2e7 0091f25100b2             add        byte ptr [ecx - 0x4dffae0e], dl
0051f2ed f251                     push       ecx
0051f2ef 00bdf25100c6             add        byte ptr [ebp - 0x39ffae0e], bh
0051f2f5 f251                     push       ecx
0051f2f7 0000                     add        byte ptr [eax], al
0051f2f9 0909                     or         dword ptr [ecx], ecx
0051f2fb 0909                     or         dword ptr [ecx], ecx
0051f2fd 0909                     or         dword ptr [ecx], ecx
0051f2ff 0109                     add        dword ptr [ecx], ecx
0051f301 0902                     or         dword ptr [edx], eax
0051f303 0309                     add        ecx, dword ptr [ecx]
0051f305 0405                     add        al, 5
0051f307 06                       push       es
0051f308 0909                     or         dword ptr [ecx], ecx
0051f30a 07                       pop        es
0051f30b 07                       pop        es
0051f30c 0909                     or         dword ptr [ecx], ecx
0051f30e 08cc                     or         ah, cl
