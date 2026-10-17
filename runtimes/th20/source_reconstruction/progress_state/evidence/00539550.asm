00539550  55                                 push       ebp
00539551  8b ec                              mov        ebp, esp
00539553  83 ec 3c                           sub        esp, 0x3c
00539556  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0053955b  33 c5                              xor        eax, ebp
0053955d  89 45 fc                           mov        dword ptr [ebp - 4], eax
00539560  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
00539564  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
0053956b  c7 45 e4 00 00 00 00               mov        dword ptr [ebp - 0x1c], 0
00539572  68 ec 67 57 00                     push       0x5767ec ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\pack\\LzssUtil.cpp:57 BYTE'
00539577  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0053957a  d1 e0                              shl        eax, 1
0053957c  50                                 push       eax
0053957d  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
00539583  e8 88 60 ee ff                     call       0x41f610
00539588  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
0053958b  83 7d c8 00                        cmp        dword ptr [ebp - 0x38], 0
0053958f  75 07                              jne        0x539598
00539591  33 c0                              xor        eax, eax
00539593  e9 b6 03 00 00                     jmp        0x53994e
00539598  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0053959b  89 4d dc                           mov        dword ptr [ebp - 0x24], ecx
0053959e  8b 55 c8                           mov        edx, dword ptr [ebp - 0x38]
005395a1  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
005395a4  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
005395a7  c7 00 00 00 00 00                  mov        dword ptr [eax], 0
005395ad  e8 ee 03 00 00                     call       0x5399a0
005395b2  c7 45 d8 01 00 00 00               mov        dword ptr [ebp - 0x28], 1
005395b9  c7 45 e0 00 00 00 00               mov        dword ptr [ebp - 0x20], 0
005395c0  eb 09                              jmp        0x5395cb
005395c2  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
005395c5  83 c1 01                           add        ecx, 1
005395c8  89 4d e0                           mov        dword ptr [ebp - 0x20], ecx
005395cb  83 7d e0 12                        cmp        dword ptr [ebp - 0x20], 0x12
005395cf  7d 3f                              jge        0x539610
005395d1  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
005395d4  2b 55 08                           sub        edx, dword ptr [ebp + 8]
005395d7  3b 55 0c                           cmp        edx, dword ptr [ebp + 0xc]
005395da  7c 09                              jl         0x5395e5
005395dc  c7 45 d4 ff ff ff ff               mov        dword ptr [ebp - 0x2c], 0xffffffff
005395e3  eb 12                              jmp        0x5395f7
005395e5  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
005395e8  0f b6 08                           movzx      ecx, byte ptr [eax]
005395eb  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
005395ee  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
005395f1  83 c2 01                           add        edx, 1
005395f4  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
005395f7  83 7d d4 ff                        cmp        dword ptr [ebp - 0x2c], -1
005395fb  75 02                              jne        0x5395ff
005395fd  eb 11                              jmp        0x539610
005395ff  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
00539602  03 45 e0                           add        eax, dword ptr [ebp - 0x20]
00539605  8a 4d d4                           mov        cl, byte ptr [ebp - 0x2c]
00539608  88 88 38 6b 5c 00                  mov        byte ptr [eax + 0x5c6b38], cl
0053960e  eb b2                              jmp        0x5395c2
00539610  8b 55 e0                           mov        edx, dword ptr [ebp - 0x20]
00539613  89 55 d0                           mov        dword ptr [ebp - 0x30], edx
00539616  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
00539619  50                                 push       eax
0053961a  e8 b1 03 00 00                     call       0x5399d0
0053961f  83 c4 04                           add        esp, 4
00539622  c7 45 cc 00 00 00 00               mov        dword ptr [ebp - 0x34], 0
00539629  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
00539630  83 7d d0 00                        cmp        dword ptr [ebp - 0x30], 0
00539634  0f 8e 73 02 00 00                  jle        0x5398ad
0053963a  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
0053963d  3b 4d d0                           cmp        ecx, dword ptr [ebp - 0x30]
00539640  7e 06                              jle        0x539648
00539642  8b 55 d0                           mov        edx, dword ptr [ebp - 0x30]
00539645  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
00539648  83 7d cc 02                        cmp        dword ptr [ebp - 0x34], 2
0053964c  0f 8f b0 00 00 00                  jg         0x539702
00539652  c7 45 c4 01 00 00 00               mov        dword ptr [ebp - 0x3c], 1
00539659  0f b6 45 f7                        movzx      eax, byte ptr [ebp - 9]
0053965d  0b 45 f0                           or         eax, dword ptr [ebp - 0x10]
00539660  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00539663  8a 4d f7                           mov        cl, byte ptr [ebp - 9]
00539666  d0 e9                              shr        cl, 1
00539668  88 4d f7                           mov        byte ptr [ebp - 9], cl
0053966b  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
0053966f  85 d2                              test       edx, edx
00539671  75 25                              jne        0x539698
00539673  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
00539676  8a 4d f0                           mov        cl, byte ptr [ebp - 0x10]
00539679  88 08                              mov        byte ptr [eax], cl
0053967b  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
0053967e  83 c2 01                           add        edx, 1
00539681  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
00539684  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
00539687  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
0053968a  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
0053968d  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
00539694  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
00539698  c7 45 e8 80 00 00 00               mov        dword ptr [ebp - 0x18], 0x80
0053969f  83 7d e8 00                        cmp        dword ptr [ebp - 0x18], 0
005396a3  74 58                              je         0x5396fd
005396a5  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
005396a8  0f b6 91 38 6b 5c 00               movzx      edx, byte ptr [ecx + 0x5c6b38]
005396af  23 55 e8                           and        edx, dword ptr [ebp - 0x18]
005396b2  74 0a                              je         0x5396be
005396b4  0f b6 45 f7                        movzx      eax, byte ptr [ebp - 9]
005396b8  0b 45 f0                           or         eax, dword ptr [ebp - 0x10]
005396bb  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
005396be  8a 4d f7                           mov        cl, byte ptr [ebp - 9]
005396c1  d0 e9                              shr        cl, 1
005396c3  88 4d f7                           mov        byte ptr [ebp - 9], cl
005396c6  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
005396ca  85 d2                              test       edx, edx
005396cc  75 25                              jne        0x5396f3
005396ce  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
005396d1  8a 4d f0                           mov        cl, byte ptr [ebp - 0x10]
005396d4  88 08                              mov        byte ptr [eax], cl
005396d6  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
005396d9  83 c2 01                           add        edx, 1
005396dc  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
005396df  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
005396e2  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
005396e5  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
005396e8  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
005396ef  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
005396f3  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
005396f6  d1 e9                              shr        ecx, 1
005396f8  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
005396fb  eb a2                              jmp        0x53969f
005396fd  e9 fa 00 00 00                     jmp        0x5397fc
00539702  8a 55 f7                           mov        dl, byte ptr [ebp - 9]
00539705  d0 ea                              shr        dl, 1
00539707  88 55 f7                           mov        byte ptr [ebp - 9], dl
0053970a  0f b6 45 f7                        movzx      eax, byte ptr [ebp - 9]
0053970e  85 c0                              test       eax, eax
00539710  75 25                              jne        0x539737
00539712  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00539715  8a 55 f0                           mov        dl, byte ptr [ebp - 0x10]
00539718  88 11                              mov        byte ptr [ecx], dl
0053971a  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
0053971d  83 c0 01                           add        eax, 1
00539720  89 45 ec                           mov        dword ptr [ebp - 0x14], eax
00539723  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
00539726  03 4d f0                           add        ecx, dword ptr [ebp - 0x10]
00539729  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0053972c  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
00539733  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
00539737  c7 45 e8 00 10 00 00               mov        dword ptr [ebp - 0x18], 0x1000
0053973e  83 7d e8 00                        cmp        dword ptr [ebp - 0x18], 0
00539742  74 51                              je         0x539795
00539744  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
00539747  23 55 f8                           and        edx, dword ptr [ebp - 8]
0053974a  74 0a                              je         0x539756
0053974c  0f b6 45 f7                        movzx      eax, byte ptr [ebp - 9]
00539750  0b 45 f0                           or         eax, dword ptr [ebp - 0x10]
00539753  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00539756  8a 4d f7                           mov        cl, byte ptr [ebp - 9]
00539759  d0 e9                              shr        cl, 1
0053975b  88 4d f7                           mov        byte ptr [ebp - 9], cl
0053975e  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
00539762  85 d2                              test       edx, edx
00539764  75 25                              jne        0x53978b
00539766  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
00539769  8a 4d f0                           mov        cl, byte ptr [ebp - 0x10]
0053976c  88 08                              mov        byte ptr [eax], cl
0053976e  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00539771  83 c2 01                           add        edx, 1
00539774  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
00539777  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
0053977a  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
0053977d  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
00539780  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
00539787  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
0053978b  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
0053978e  d1 e9                              shr        ecx, 1
00539790  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
00539793  eb a9                              jmp        0x53973e
00539795  c7 45 e8 08 00 00 00               mov        dword ptr [ebp - 0x18], 8
0053979c  83 7d e8 00                        cmp        dword ptr [ebp - 0x18], 0
005397a0  74 54                              je         0x5397f6
005397a2  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
005397a5  83 ea 03                           sub        edx, 3
005397a8  23 55 e8                           and        edx, dword ptr [ebp - 0x18]
005397ab  74 0a                              je         0x5397b7
005397ad  0f b6 45 f7                        movzx      eax, byte ptr [ebp - 9]
005397b1  0b 45 f0                           or         eax, dword ptr [ebp - 0x10]
005397b4  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
005397b7  8a 4d f7                           mov        cl, byte ptr [ebp - 9]
005397ba  d0 e9                              shr        cl, 1
005397bc  88 4d f7                           mov        byte ptr [ebp - 9], cl
005397bf  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
005397c3  85 d2                              test       edx, edx
005397c5  75 25                              jne        0x5397ec
005397c7  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
005397ca  8a 4d f0                           mov        cl, byte ptr [ebp - 0x10]
005397cd  88 08                              mov        byte ptr [eax], cl
005397cf  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
005397d2  83 c2 01                           add        edx, 1
005397d5  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
005397d8  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
005397db  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
005397de  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
005397e1  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
005397e8  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
005397ec  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
005397ef  d1 e9                              shr        ecx, 1
005397f1  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
005397f4  eb a6                              jmp        0x53979c
005397f6  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
005397f9  89 55 c4                           mov        dword ptr [ebp - 0x3c], edx
005397fc  c7 45 e0 00 00 00 00               mov        dword ptr [ebp - 0x20], 0
00539803  eb 09                              jmp        0x53980e
00539805  8b 45 e0                           mov        eax, dword ptr [ebp - 0x20]
00539808  83 c0 01                           add        eax, 1
0053980b  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
0053980e  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
00539811  3b 4d c4                           cmp        ecx, dword ptr [ebp - 0x3c]
00539814  0f 8d 8e 00 00 00                  jge        0x5398a8
0053981a  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
0053981d  83 c2 12                           add        edx, 0x12
00539820  81 e2 ff 1f 00 00                  and        edx, 0x1fff
00539826  52                                 push       edx
00539827  e8 94 fc ff ff                     call       0x5394c0
0053982c  83 c4 04                           add        esp, 4
0053982f  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
00539832  2b 45 08                           sub        eax, dword ptr [ebp + 8]
00539835  3b 45 0c                           cmp        eax, dword ptr [ebp + 0xc]
00539838  7c 09                              jl         0x539843
0053983a  c7 45 d4 ff ff ff ff               mov        dword ptr [ebp - 0x2c], 0xffffffff
00539841  eb 12                              jmp        0x539855
00539843  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
00539846  0f b6 11                           movzx      edx, byte ptr [ecx]
00539849  89 55 d4                           mov        dword ptr [ebp - 0x2c], edx
0053984c  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0053984f  83 c0 01                           add        eax, 1
00539852  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
00539855  83 7d d4 ff                        cmp        dword ptr [ebp - 0x2c], -1
00539859  75 0b                              jne        0x539866
0053985b  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
0053985e  83 e9 01                           sub        ecx, 1
00539861  89 4d d0                           mov        dword ptr [ebp - 0x30], ecx
00539864  eb 15                              jmp        0x53987b
00539866  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
00539869  83 c2 12                           add        edx, 0x12
0053986c  81 e2 ff 1f 00 00                  and        edx, 0x1fff
00539872  8a 45 d4                           mov        al, byte ptr [ebp - 0x2c]
00539875  88 82 38 6b 5c 00                  mov        byte ptr [edx + 0x5c6b38], al
0053987b  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
0053987e  83 c1 01                           add        ecx, 1
00539881  81 e1 ff 1f 00 00                  and        ecx, 0x1fff
00539887  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
0053988a  83 7d d0 00                        cmp        dword ptr [ebp - 0x30], 0
0053988e  74 13                              je         0x5398a3
00539890  8d 55 f8                           lea        edx, [ebp - 8]
00539893  52                                 push       edx
00539894  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
00539897  50                                 push       eax
00539898  e8 c3 f7 ff ff                     call       0x539060
0053989d  83 c4 08                           add        esp, 8
005398a0  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
005398a3  e9 5d ff ff ff                     jmp        0x539805
005398a8  e9 83 fd ff ff                     jmp        0x539630
005398ad  8a 4d f7                           mov        cl, byte ptr [ebp - 9]
005398b0  d0 e9                              shr        cl, 1
005398b2  88 4d f7                           mov        byte ptr [ebp - 9], cl
005398b5  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
005398b9  85 d2                              test       edx, edx
005398bb  75 25                              jne        0x5398e2
005398bd  8b 45 ec                           mov        eax, dword ptr [ebp - 0x14]
005398c0  8a 4d f0                           mov        cl, byte ptr [ebp - 0x10]
005398c3  88 08                              mov        byte ptr [eax], cl
005398c5  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
005398c8  83 c2 01                           add        edx, 1
005398cb  89 55 ec                           mov        dword ptr [ebp - 0x14], edx
005398ce  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
005398d1  03 45 f0                           add        eax, dword ptr [ebp - 0x10]
005398d4  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
005398d7  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
005398de  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
005398e2  c7 45 e8 00 10 00 00               mov        dword ptr [ebp - 0x18], 0x1000
005398e9  83 7d e8 00                        cmp        dword ptr [ebp - 0x18], 0
005398ed  74 51                              je         0x539940
005398ef  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
005398f2  83 e1 00                           and        ecx, 0
005398f5  74 0a                              je         0x539901
005398f7  0f b6 55 f7                        movzx      edx, byte ptr [ebp - 9]
005398fb  0b 55 f0                           or         edx, dword ptr [ebp - 0x10]
005398fe  89 55 f0                           mov        dword ptr [ebp - 0x10], edx
00539901  8a 45 f7                           mov        al, byte ptr [ebp - 9]
00539904  d0 e8                              shr        al, 1
00539906  88 45 f7                           mov        byte ptr [ebp - 9], al
00539909  0f b6 4d f7                        movzx      ecx, byte ptr [ebp - 9]
0053990d  85 c9                              test       ecx, ecx
0053990f  75 25                              jne        0x539936
00539911  8b 55 ec                           mov        edx, dword ptr [ebp - 0x14]
00539914  8a 45 f0                           mov        al, byte ptr [ebp - 0x10]
00539917  88 02                              mov        byte ptr [edx], al
00539919  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
0053991c  83 c1 01                           add        ecx, 1
0053991f  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
00539922  8b 55 e4                           mov        edx, dword ptr [ebp - 0x1c]
00539925  03 55 f0                           add        edx, dword ptr [ebp - 0x10]
00539928  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
0053992b  c7 45 f0 00 00 00 00               mov        dword ptr [ebp - 0x10], 0
00539932  c6 45 f7 80                        mov        byte ptr [ebp - 9], 0x80
00539936  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
00539939  d1 e8                              shr        eax, 1
0053993b  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0053993e  eb a9                              jmp        0x5398e9
00539940  8b 4d ec                           mov        ecx, dword ptr [ebp - 0x14]
00539943  2b 4d c8                           sub        ecx, dword ptr [ebp - 0x38]
00539946  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
00539949  89 0a                              mov        dword ptr [edx], ecx
0053994b  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
0053994e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
00539951  33 cd                              xor        ecx, ebp
00539953  e8 96 90 00 00                     call       0x5429ee
00539958  8b e5                              mov        esp, ebp
0053995a  5d                                 pop        ebp
0053995b  c3                                 ret        
