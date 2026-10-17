0046cc32  ec                                 in         al, dx
0046cc33  83 ec 08                           sub        esp, 8
0046cc36  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0046cc39  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cc3c  e8 df 39 00 00                     call       0x470620
0046cc41  89 45 fc                           mov        dword ptr [ebp - 4], eax
0046cc44  b8 01 00 00 00                     mov        eax, 1
0046cc49  6b c8 00                           imul       ecx, eax, 0
0046cc4c  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046cc4f  0f b6 45 0c                        movzx      eax, byte ptr [ebp + 0xc]
0046cc53  88 04 0a                           mov        byte ptr [edx + ecx], al
0046cc56  b9 01 00 00 00                     mov        ecx, 1
0046cc5b  c1 e1 00                           shl        ecx, 0
0046cc5e  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0046cc61  c6 04 0a 00                        mov        byte ptr [edx + ecx], 0
0046cc65  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046cc68  50                                 push       eax
0046cc69  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
0046cc6c  e8 cf fd ff ff                     call       0x46ca40
0046cc71  90                                 nop        
0046cc72  8b e5                              mov        esp, ebp
0046cc74  5d                                 pop        ebp
0046cc75  c2 08 00                           ret        8
