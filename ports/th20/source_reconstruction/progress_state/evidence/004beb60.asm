004beb60  55                                 push       ebp
004beb61  8b ec                              mov        ebp, esp
004beb63  83 ec 0c                           sub        esp, 0xc
004beb66  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004beb69  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004beb6c  e8 af 53 fa ff                     call       0x463f20
004beb71  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004beb74  88 81 d1 01 00 00                  mov        byte ptr [ecx + 0x1d1], al
004beb7a  b9 c4 a4 5b 00                     mov        ecx, 0x5ba4c4
004beb7f  e8 5c 53 f6 ff                     call       0x423ee0 ; candidate_rng_locked
004beb84  33 d2                              xor        edx, edx
004beb86  b9 20 00 00 00                     mov        ecx, 0x20
004beb8b  f7 f1                              div        ecx
004beb8d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004beb90  8d 8c 10 b0 01 00 00               lea        ecx, [eax + edx + 0x1b0]
004beb97  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004beb9a  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004beb9d  8a 02                              mov        al, byte ptr [edx]
004beb9f  04 01                              add        al, 1
004beba1  8b 4d f8                           mov        ecx, dword ptr [ebp - 8]
004beba4  88 01                              mov        byte ptr [ecx], al
004beba6  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004beba9  8a 82 d1 01 00 00                  mov        al, byte ptr [edx + 0x1d1]
004bebaf  04 01                              add        al, 1
004bebb1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bebb4  88 81 d1 01 00 00                  mov        byte ptr [ecx + 0x1d1], al
004bebba  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004bebbd  0f b6 82 d0 01 00 00               movzx      eax, byte ptr [edx + 0x1d0]
004bebc4  83 c0 02                           add        eax, 2
004bebc7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bebca  88 81 d0 01 00 00                  mov        byte ptr [ecx + 0x1d0], al
004bebd0  b9 c4 a4 5b 00                     mov        ecx, 0x5ba4c4
004bebd5  e8 06 53 f6 ff                     call       0x423ee0 ; candidate_rng_locked
004bebda  33 d2                              xor        edx, edx
004bebdc  b9 20 00 00 00                     mov        ecx, 0x20
004bebe1  f7 f1                              div        ecx
004bebe3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004bebe6  8d 8c 10 d2 01 00 00               lea        ecx, [eax + edx + 0x1d2]
004bebed  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
004bebf0  8b 55 f4                           mov        edx, dword ptr [ebp - 0xc]
004bebf3  0f b6 02                           movzx      eax, byte ptr [edx]
004bebf6  04 01                              add        al, 1
004bebf8  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004bebfb  88 01                              mov        byte ptr [ecx], al
004bebfd  8b e5                              mov        esp, ebp
004bebff  5d                                 pop        ebp
004bec00  c3                                 ret        
