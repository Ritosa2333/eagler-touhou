
// block 0047b769
0047b769  mov        edi, edi
0047b76b  push       ebp
0047b76c  mov        ebp, esp
0047b76e  mov        eax, 0x1ae4
0047b773  call       0x48d060

// block 0047b778
0047b778  mov        eax, dword ptr [0x4ad138]
0047b77d  xor        eax, ebp
0047b77f  mov        dword ptr [ebp - 4], eax
0047b782  mov        eax, dword ptr [ebp + 0xc]
0047b785  push       esi
0047b786  xor        esi, esi
0047b788  mov        dword ptr [ebp - 0x1acc], eax
0047b78e  mov        dword ptr [ebp - 0x1ac8], esi
0047b794  mov        dword ptr [ebp - 0x1ad0], esi
0047b79a  cmp        dword ptr [ebp + 0x10], esi
0047b79d  jne        0x47b7a6

// block 0047b79f
0047b79f  xor        eax, eax
0047b7a1  jmp        0x47be8f

// block 0047b7a6
0047b7a6  cmp        eax, esi
0047b7a8  jne        0x47b7d1

// block 0047b7aa
0047b7aa  call       0x46e982

// block 0047b7af
0047b7af  mov        dword ptr [eax], esi
0047b7b1  call       0x46e96f

// block 0047b7b6
0047b7b6  push       esi
0047b7b7  push       esi
0047b7b8  push       esi
0047b7b9  push       esi
0047b7ba  push       esi
0047b7bb  mov        dword ptr [eax], 0x16
0047b7c1  call       0x470f2d

// block 0047b7c6
0047b7c6  add        esp, 0x14
0047b7c9  or         eax, 0xffffffff
0047b7cc  jmp        0x47be8f

// block 0047b7d1
0047b7d1  push       ebx
0047b7d2  push       edi
0047b7d3  mov        edi, dword ptr [ebp + 8]
0047b7d6  mov        eax, edi
0047b7d8  sar        eax, 5
0047b7db  lea        esi, [eax*4 + 0x4d6320]
0047b7e2  mov        eax, dword ptr [esi]
0047b7e4  and        edi, 0x1f
0047b7e7  shl        edi, 6
0047b7ea  add        eax, edi
0047b7ec  mov        bl, byte ptr [eax + 0x24]
0047b7ef  add        bl, bl
0047b7f1  sar        bl, 1
0047b7f3  mov        dword ptr [ebp - 0x1ad8], esi
0047b7f9  mov        byte ptr [ebp - 0x1ad9], bl
0047b7ff  cmp        bl, 2
0047b802  je         0x47b809

// block 0047b804
0047b804  cmp        bl, 1
0047b807  jne        0x47b839

// block 0047b809
0047b809  mov        ecx, dword ptr [ebp + 0x10]
0047b80c  not        ecx
0047b80e  test       cl, 1
0047b811  jne        0x47b839

// block 0047b813
0047b813  call       0x46e982

// block 0047b818
0047b818  xor        esi, esi
0047b81a  mov        dword ptr [eax], esi
0047b81c  call       0x46e96f

// block 0047b821
0047b821  push       esi
0047b822  push       esi
0047b823  push       esi
0047b824  push       esi
0047b825  push       esi
0047b826  mov        dword ptr [eax], 0x16
0047b82c  call       0x470f2d

// block 0047b831
0047b831  add        esp, 0x14
0047b834  jmp        0x47be7c

// block 0047b839
0047b839  test       byte ptr [eax + 4], 0x20
0047b83d  je         0x47b850

// block 0047b83f
0047b83f  push       2
0047b841  push       0
0047b843  push       0
0047b845  push       dword ptr [ebp + 8]
0047b848  call       0x47b5cb

// block 0047b84d
0047b84d  add        esp, 0x10

// block 0047b850
0047b850  push       dword ptr [ebp + 8]
0047b853  call       0x47bfc1

// block 0047b858
0047b858  pop        ecx
0047b859  test       eax, eax
0047b85b  je         0x47bafe

// block 0047b861
0047b861  mov        eax, dword ptr [esi]
0047b863  test       byte ptr [edi + eax + 4], 0x80
0047b868  je         0x47bafe

// block 0047b86e
0047b86e  call       0x475467

// block 0047b873
0047b873  mov        eax, dword ptr [eax + 0x6c]
0047b876  xor        ecx, ecx
0047b878  cmp        dword ptr [eax + 0x14], ecx
0047b87b  lea        eax, [ebp - 0x1ae4]
0047b881  sete       cl
0047b884  push       eax
0047b885  mov        eax, dword ptr [esi]
0047b887  push       dword ptr [edi + eax]
0047b88a  mov        dword ptr [ebp - 0x1ae0], ecx
0047b890  call       dword ptr [0x498068]

// block 0047b896
0047b896  test       eax, eax
0047b898  je         0x47bafe

// block 0047b89e
0047b89e  xor        ecx, ecx
0047b8a0  cmp        dword ptr [ebp - 0x1ae0], ecx
0047b8a6  je         0x47b8b0

// block 0047b8a8
0047b8a8  test       bl, bl
0047b8aa  je         0x47bb00

// block 0047b8b0
0047b8b0  call       dword ptr [0x49806c]

// block 0047b8b6
0047b8b6  mov        ebx, dword ptr [ebp - 0x1acc]
0047b8bc  mov        dword ptr [ebp - 0x1ae4], eax
0047b8c2  xor        eax, eax
0047b8c4  mov        dword ptr [ebp - 0x1ac4], eax
0047b8ca  cmp        dword ptr [ebp + 0x10], eax
0047b8cd  jbe        0x47be15

// block 0047b8d3
0047b8d3  mov        dword ptr [ebp - 0x1abc], eax

// block 0047b8d9
0047b8d9  mov        al, byte ptr [ebp - 0x1ad9]
0047b8df  test       al, al
0047b8e1  jne        0x47ba4e

// block 0047b8e7
0047b8e7  mov        cl, byte ptr [ebx]
0047b8e9  mov        esi, dword ptr [ebp - 0x1ad8]
0047b8ef  xor        eax, eax
0047b8f1  cmp        cl, 0xa
0047b8f4  sete       al
0047b8f7  mov        dword ptr [ebp - 0x1ae0], eax
0047b8fd  mov        eax, dword ptr [esi]
0047b8ff  add        eax, edi
0047b901  cmp        dword ptr [eax + 0x38], 0
0047b905  je         0x47b91c

// block 0047b907
0047b907  mov        dl, byte ptr [eax + 0x34]
0047b90a  mov        byte ptr [ebp - 0xc], dl
0047b90d  mov        byte ptr [ebp - 0xb], cl
0047b910  and        dword ptr [eax + 0x38], 0
0047b914  push       2
0047b916  lea        eax, [ebp - 0xc]
0047b919  push       eax
0047b91a  jmp        0x47b967

// block 0047b91c
0047b91c  movsx      eax, cl
0047b91f  push       eax
0047b920  call       0x47c5db

// block 0047b925
0047b925  pop        ecx
0047b926  test       eax, eax
0047b928  je         0x47b964

// block 0047b92a
0047b92a  mov        ecx, dword ptr [ebp - 0x1acc]
0047b930  sub        ecx, ebx
0047b932  add        ecx, dword ptr [ebp + 0x10]
0047b935  xor        eax, eax
0047b937  inc        eax
0047b938  cmp        ecx, eax
0047b93a  jbe        0x47bae5

// block 0047b940
0047b940  push       2
0047b942  lea        eax, [ebp - 0x1ac0]
0047b948  push       ebx
0047b949  push       eax
0047b94a  call       0x48c27e

// block 0047b94f
0047b94f  add        esp, 0xc
0047b952  cmp        eax, -1
0047b955  je         0x47be0c

// block 0047b95b
0047b95b  inc        ebx
0047b95c  inc        dword ptr [ebp - 0x1abc]
0047b962  jmp        0x47b97f

// block 0047b964
0047b964  push       1
0047b966  push       ebx

// block 0047b967
0047b967  lea        eax, [ebp - 0x1ac0]
0047b96d  push       eax
0047b96e  call       0x48c27e

// block 0047b973
0047b973  add        esp, 0xc
0047b976  cmp        eax, -1
0047b979  je         0x47be0c

// block 0047b97f
0047b97f  xor        eax, eax
0047b981  push       eax
0047b982  push       eax
0047b983  push       5
0047b985  lea        ecx, [ebp - 0xc]
0047b988  push       ecx
0047b989  push       1
0047b98b  lea        ecx, [ebp - 0x1ac0]
0047b991  push       ecx
0047b992  push       eax
0047b993  push       dword ptr [ebp - 0x1ae4]
0047b999  inc        ebx
0047b99a  inc        dword ptr [ebp - 0x1abc]
0047b9a0  call       dword ptr [0x498134]

// block 0047b9a6
0047b9a6  mov        esi, eax
0047b9a8  test       esi, esi
0047b9aa  je         0x47be0c

// block 0047b9b0
0047b9b0  push       0
0047b9b2  lea        eax, [ebp - 0x1ac4]
0047b9b8  push       eax
0047b9b9  push       esi
0047b9ba  lea        eax, [ebp - 0xc]
0047b9bd  push       eax
0047b9be  mov        eax, dword ptr [ebp - 0x1ad8]
0047b9c4  mov        eax, dword ptr [eax]
0047b9c6  push       dword ptr [edi + eax]
0047b9c9  call       dword ptr [0x4980ac]

// block 0047b9cf
0047b9cf  test       eax, eax
0047b9d1  je         0x47be00

// block 0047b9d7
0047b9d7  mov        eax, dword ptr [ebp - 0x1abc]
0047b9dd  mov        ecx, dword ptr [ebp - 0x1ad0]
0047b9e3  add        eax, ecx
0047b9e5  cmp        dword ptr [ebp - 0x1ac4], esi
0047b9eb  mov        dword ptr [ebp - 0x1ac8], eax
0047b9f1  jl         0x47be0c

// block 0047b9f7
0047b9f7  cmp        dword ptr [ebp - 0x1ae0], 0
0047b9fe  je         0x47bad1

// block 0047ba04
0047ba04  push       0
0047ba06  lea        eax, [ebp - 0x1ac4]
0047ba0c  push       eax
0047ba0d  push       1
0047ba0f  lea        eax, [ebp - 0xc]
0047ba12  push       eax
0047ba13  mov        eax, dword ptr [ebp - 0x1ad8]
0047ba19  mov        eax, dword ptr [eax]
0047ba1b  mov        byte ptr [ebp - 0xc], 0xd
0047ba1f  push       dword ptr [edi + eax]
0047ba22  call       dword ptr [0x4980ac]

// block 0047ba28
0047ba28  test       eax, eax
0047ba2a  je         0x47be00

// block 0047ba30
0047ba30  cmp        dword ptr [ebp - 0x1ac4], 1
0047ba37  jl         0x47be0c

// block 0047ba3d
0047ba3d  inc        dword ptr [ebp - 0x1ad0]
0047ba43  inc        dword ptr [ebp - 0x1ac8]
0047ba49  jmp        0x47bad1

// block 0047ba4e
0047ba4e  cmp        al, 1
0047ba50  je         0x47ba56

// block 0047ba52
0047ba52  cmp        al, 2
0047ba54  jne        0x47ba77

// block 0047ba56
0047ba56  movzx      esi, word ptr [ebx]
0047ba59  xor        ecx, ecx
0047ba5b  cmp        si, 0xa
0047ba5f  sete       cl
0047ba62  inc        ebx
0047ba63  inc        ebx
0047ba64  add        dword ptr [ebp - 0x1abc], 2
0047ba6b  mov        dword ptr [ebp - 0x1ac0], esi
0047ba71  mov        dword ptr [ebp - 0x1ae0], ecx

// block 0047ba77
0047ba77  cmp        al, 1
0047ba79  je         0x47ba7f

// block 0047ba7b
0047ba7b  cmp        al, 2
0047ba7d  jne        0x47bad1

// block 0047ba7f
0047ba7f  push       dword ptr [ebp - 0x1ac0]
0047ba85  call       0x48ceb4

// block 0047ba8a
0047ba8a  pop        ecx
0047ba8b  cmp        ax, word ptr [ebp - 0x1ac0]
0047ba92  jne        0x47be00

// block 0047ba98
0047ba98  add        dword ptr [ebp - 0x1ac8], 2
0047ba9f  cmp        dword ptr [ebp - 0x1ae0], 0
0047baa6  je         0x47bad1

// block 0047baa8
0047baa8  push       0xd
0047baaa  pop        eax
0047baab  push       eax
0047baac  mov        dword ptr [ebp - 0x1ac0], eax
0047bab2  call       0x48ceb4

// block 0047bab7
0047bab7  pop        ecx
0047bab8  cmp        ax, word ptr [ebp - 0x1ac0]
0047babf  jne        0x47be00

// block 0047bac5
0047bac5  inc        dword ptr [ebp - 0x1ac8]
0047bacb  inc        dword ptr [ebp - 0x1ad0]

// block 0047bad1
0047bad1  mov        eax, dword ptr [ebp + 0x10]
0047bad4  cmp        dword ptr [ebp - 0x1abc], eax
0047bada  jb         0x47b8d9

// block 0047bae0
0047bae0  jmp        0x47be0c

// block 0047bae5
0047bae5  mov        ecx, dword ptr [esi]
0047bae7  mov        dl, byte ptr [ebx]
0047bae9  inc        dword ptr [ebp - 0x1ac8]
0047baef  mov        byte ptr [edi + ecx + 0x34], dl
0047baf3  mov        ecx, dword ptr [esi]
0047baf5  mov        dword ptr [edi + ecx + 0x38], eax
0047baf9  jmp        0x47be0c

// block 0047bafe
0047bafe  xor        ecx, ecx

// block 0047bb00
0047bb00  mov        eax, dword ptr [esi]
0047bb02  add        eax, edi
0047bb04  test       byte ptr [eax + 4], 0x80
0047bb08  je         0x47bdcd

// block 0047bb0e
0047bb0e  mov        eax, dword ptr [ebp - 0x1acc]
0047bb14  mov        dword ptr [ebp - 0x1ac0], ecx
0047bb1a  test       bl, bl
0047bb1c  jne        0x47bbec

// block 0047bb22
0047bb22  mov        dword ptr [ebp - 0x1ac4], eax
0047bb28  cmp        dword ptr [ebp + 0x10], ecx
0047bb2b  jbe        0x47be51

// block 0047bb31
0047bb31  jmp        0x47bb39

// block 0047bb33
0047bb33  mov        esi, dword ptr [ebp - 0x1ad8]

// block 0047bb39
0047bb39  mov        ecx, dword ptr [ebp - 0x1ac4]
0047bb3f  and        dword ptr [ebp - 0x1abc], 0
0047bb46  sub        ecx, dword ptr [ebp - 0x1acc]
0047bb4c  lea        eax, [ebp - 0x1ab8]

// block 0047bb52
0047bb52  cmp        ecx, dword ptr [ebp + 0x10]
0047bb55  jae        0x47bb90

// block 0047bb57
0047bb57  mov        edx, dword ptr [ebp - 0x1ac4]
0047bb5d  inc        dword ptr [ebp - 0x1ac4]
0047bb63  mov        dl, byte ptr [edx]
0047bb65  inc        ecx
0047bb66  cmp        dl, 0xa
0047bb69  jne        0x47bb7b

// block 0047bb6b
0047bb6b  inc        dword ptr [ebp - 0x1ad0]
0047bb71  mov        byte ptr [eax], 0xd
0047bb74  inc        eax
0047bb75  inc        dword ptr [ebp - 0x1abc]

// block 0047bb7b
0047bb7b  mov        byte ptr [eax], dl
0047bb7d  inc        eax
0047bb7e  inc        dword ptr [ebp - 0x1abc]
0047bb84  cmp        dword ptr [ebp - 0x1abc], 0x13ff
0047bb8e  jb         0x47bb52

// block 0047bb90
0047bb90  mov        ebx, eax
0047bb92  lea        eax, [ebp - 0x1ab8]
0047bb98  sub        ebx, eax
0047bb9a  push       0
0047bb9c  lea        eax, [ebp - 0x1ad4]
0047bba2  push       eax
0047bba3  push       ebx
0047bba4  lea        eax, [ebp - 0x1ab8]
0047bbaa  push       eax
0047bbab  mov        eax, dword ptr [esi]
0047bbad  push       dword ptr [edi + eax]
0047bbb0  call       dword ptr [0x4980ac]

// block 0047bbb6
0047bbb6  test       eax, eax
0047bbb8  je         0x47be00

// block 0047bbbe
0047bbbe  mov        eax, dword ptr [ebp - 0x1ad4]
0047bbc4  add        dword ptr [ebp - 0x1ac8], eax
0047bbca  cmp        eax, ebx
0047bbcc  jl         0x47be0c

// block 0047bbd2
0047bbd2  mov        eax, dword ptr [ebp - 0x1ac4]
0047bbd8  sub        eax, dword ptr [ebp - 0x1acc]
0047bbde  cmp        eax, dword ptr [ebp + 0x10]
0047bbe1  jb         0x47bb33

// block 0047bbe7
0047bbe7  jmp        0x47be0c

// block 0047bbec
0047bbec  mov        dword ptr [ebp - 0x1abc], eax
0047bbf2  cmp        bl, 2
0047bbf5  jne        0x47bccc

// block 0047bbfb
0047bbfb  cmp        dword ptr [ebp + 0x10], ecx
0047bbfe  jbe        0x47be51

// block 0047bc04
0047bc04  jmp        0x47bc0c

// block 0047bc06
0047bc06  mov        esi, dword ptr [ebp - 0x1ad8]

// block 0047bc0c
0047bc0c  mov        ecx, dword ptr [ebp - 0x1abc]
0047bc12  and        dword ptr [ebp - 0x1ac4], 0
0047bc19  sub        ecx, dword ptr [ebp - 0x1acc]
0047bc1f  lea        eax, [ebp - 0x1ab8]

// block 0047bc25
0047bc25  cmp        ecx, dword ptr [ebp + 0x10]
0047bc28  jae        0x47bc70

// block 0047bc2a
0047bc2a  mov        edx, dword ptr [ebp - 0x1abc]
0047bc30  add        dword ptr [ebp - 0x1abc], 2
0047bc37  movzx      edx, word ptr [edx]
0047bc3a  inc        ecx
0047bc3b  inc        ecx
0047bc3c  cmp        dx, 0xa
0047bc40  jne        0x47bc58

// block 0047bc42
0047bc42  add        dword ptr [ebp - 0x1ad0], 2
0047bc49  push       0xd
0047bc4b  pop        ebx
0047bc4c  mov        word ptr [eax], bx
0047bc4f  inc        eax
0047bc50  inc        eax
0047bc51  add        dword ptr [ebp - 0x1ac4], 2

// block 0047bc58
0047bc58  add        dword ptr [ebp - 0x1ac4], 2
0047bc5f  mov        word ptr [eax], dx
0047bc62  inc        eax
0047bc63  inc        eax
0047bc64  cmp        dword ptr [ebp - 0x1ac4], 0x13fe
0047bc6e  jb         0x47bc25

// block 0047bc70
0047bc70  mov        ebx, eax
0047bc72  lea        eax, [ebp - 0x1ab8]
0047bc78  sub        ebx, eax
0047bc7a  push       0
0047bc7c  lea        eax, [ebp - 0x1ad4]
0047bc82  push       eax
0047bc83  push       ebx
0047bc84  lea        eax, [ebp - 0x1ab8]
0047bc8a  push       eax
0047bc8b  mov        eax, dword ptr [esi]
0047bc8d  push       dword ptr [edi + eax]
0047bc90  call       dword ptr [0x4980ac]

// block 0047bc96
0047bc96  test       eax, eax
0047bc98  je         0x47be00

// block 0047bc9e
0047bc9e  mov        eax, dword ptr [ebp - 0x1ad4]
0047bca4  add        dword ptr [ebp - 0x1ac8], eax
0047bcaa  cmp        eax, ebx
0047bcac  jl         0x47be0c

// block 0047bcb2
0047bcb2  mov        eax, dword ptr [ebp - 0x1abc]
0047bcb8  sub        eax, dword ptr [ebp - 0x1acc]
0047bcbe  cmp        eax, dword ptr [ebp + 0x10]
0047bcc1  jb         0x47bc06

// block 0047bcc7
0047bcc7  jmp        0x47be0c

// block 0047bccc
0047bccc  cmp        dword ptr [ebp + 0x10], ecx
0047bccf  jbe        0x47be51

// block 0047bcd5
0047bcd5  mov        ecx, dword ptr [ebp - 0x1abc]
0047bcdb  and        dword ptr [ebp - 0x1ac4], 0
0047bce2  sub        ecx, dword ptr [ebp - 0x1acc]
0047bce8  push       2
0047bcea  lea        eax, [ebp - 0x6b8]
0047bcf0  pop        esi

// block 0047bcf1
0047bcf1  cmp        ecx, dword ptr [ebp + 0x10]
0047bcf4  jae        0x47bd32

// block 0047bcf6
0047bcf6  mov        edx, dword ptr [ebp - 0x1abc]
0047bcfc  movzx      edx, word ptr [edx]
0047bcff  add        dword ptr [ebp - 0x1abc], esi
0047bd05  add        ecx, esi
0047bd07  cmp        dx, 0xa
0047bd0b  jne        0x47bd1b

// block 0047bd0d
0047bd0d  push       0xd
0047bd0f  pop        ebx
0047bd10  mov        word ptr [eax], bx
0047bd13  add        eax, esi
0047bd15  add        dword ptr [ebp - 0x1ac4], esi

// block 0047bd1b
0047bd1b  add        dword ptr [ebp - 0x1ac4], esi
0047bd21  mov        word ptr [eax], dx
0047bd24  add        eax, esi
0047bd26  cmp        dword ptr [ebp - 0x1ac4], 0x6a8
0047bd30  jb         0x47bcf1

// block 0047bd32
0047bd32  xor        esi, esi
0047bd34  push       esi
0047bd35  push       esi
0047bd36  push       0xd55
0047bd3b  lea        ecx, [ebp - 0x1410]
0047bd41  push       ecx
0047bd42  lea        ecx, [ebp - 0x6b8]
0047bd48  sub        eax, ecx
0047bd4a  cdq        
0047bd4b  sub        eax, edx
0047bd4d  sar        eax, 1
0047bd4f  push       eax
0047bd50  mov        eax, ecx
0047bd52  push       eax
0047bd53  push       esi
0047bd54  push       0xfde9
0047bd59  call       dword ptr [0x498134]

// block 0047bd5f
0047bd5f  mov        ebx, eax
0047bd61  cmp        ebx, esi
0047bd63  je         0x47be00

// block 0047bd69
0047bd69  push       0
0047bd6b  lea        eax, [ebp - 0x1ad4]
0047bd71  push       eax
0047bd72  mov        eax, ebx
0047bd74  sub        eax, esi
0047bd76  push       eax
0047bd77  lea        eax, [ebp + esi - 0x1410]
0047bd7e  push       eax
0047bd7f  mov        eax, dword ptr [ebp - 0x1ad8]
0047bd85  mov        eax, dword ptr [eax]
0047bd87  push       dword ptr [edi + eax]
0047bd8a  call       dword ptr [0x4980ac]

// block 0047bd90
0047bd90  test       eax, eax
0047bd92  je         0x47bda0

// block 0047bd94
0047bd94  add        esi, dword ptr [ebp - 0x1ad4]
0047bd9a  cmp        ebx, esi
0047bd9c  jg         0x47bd69

// block 0047bd9e
0047bd9e  jmp        0x47bdac

// block 0047bda0
0047bda0  call       dword ptr [0x4980e4]

// block 0047bda6
0047bda6  mov        dword ptr [ebp - 0x1ac0], eax

// block 0047bdac
0047bdac  cmp        ebx, esi
0047bdae  jg         0x47be0c

// block 0047bdb0
0047bdb0  mov        eax, dword ptr [ebp - 0x1abc]
0047bdb6  sub        eax, dword ptr [ebp - 0x1acc]
0047bdbc  mov        dword ptr [ebp - 0x1ac8], eax
0047bdc2  cmp        eax, dword ptr [ebp + 0x10]
0047bdc5  jb         0x47bcd5

// block 0047bdcb
0047bdcb  jmp        0x47be0c

// block 0047bdcd
0047bdcd  push       0
0047bdcf  lea        ecx, [ebp - 0x1ad4]
0047bdd5  push       ecx
0047bdd6  push       dword ptr [ebp + 0x10]
0047bdd9  push       dword ptr [ebp - 0x1acc]
0047bddf  push       dword ptr [eax]
0047bde1  call       dword ptr [0x4980ac]

// block 0047bde7
0047bde7  test       eax, eax
0047bde9  je         0x47be00

// block 0047bdeb
0047bdeb  mov        eax, dword ptr [ebp - 0x1ad4]
0047bdf1  and        dword ptr [ebp - 0x1ac0], 0
0047bdf8  mov        dword ptr [ebp - 0x1ac8], eax
0047bdfe  jmp        0x47be0c

// block 0047be00
0047be00  call       dword ptr [0x4980e4]

// block 0047be06
0047be06  mov        dword ptr [ebp - 0x1ac0], eax

// block 0047be0c
0047be0c  cmp        dword ptr [ebp - 0x1ac8], 0
0047be13  jne        0x47be81

// block 0047be15
0047be15  cmp        dword ptr [ebp - 0x1ac0], 0
0047be1c  je         0x47be4b

// block 0047be1e
0047be1e  push       5
0047be20  pop        esi
0047be21  cmp        dword ptr [ebp - 0x1ac0], esi
0047be27  jne        0x47be3d

// block 0047be29
0047be29  call       0x46e96f

// block 0047be2e
0047be2e  mov        dword ptr [eax], 9
0047be34  call       0x46e982

// block 0047be39
0047be39  mov        dword ptr [eax], esi
0047be3b  jmp        0x47be7c

// block 0047be3d
0047be3d  push       dword ptr [ebp - 0x1ac0]
0047be43  call       0x46e995

// block 0047be48
0047be48  pop        ecx
0047be49  jmp        0x47be7c

// block 0047be4b
0047be4b  mov        esi, dword ptr [ebp - 0x1ad8]

// block 0047be51
0047be51  mov        eax, dword ptr [esi]
0047be53  test       byte ptr [edi + eax + 4], 0x40
0047be58  je         0x47be69

// block 0047be5a
0047be5a  mov        eax, dword ptr [ebp - 0x1acc]
0047be60  cmp        byte ptr [eax], 0x1a
0047be63  jne        0x47be69

// block 0047be65
0047be65  xor        eax, eax
0047be67  jmp        0x47be8d

// block 0047be69
0047be69  call       0x46e96f

// block 0047be6e
0047be6e  mov        dword ptr [eax], 0x1c
0047be74  call       0x46e982

// block 0047be79
0047be79  and        dword ptr [eax], 0

// block 0047be7c
0047be7c  or         eax, 0xffffffff
0047be7f  jmp        0x47be8d

// block 0047be81
0047be81  mov        eax, dword ptr [ebp - 0x1ac8]
0047be87  sub        eax, dword ptr [ebp - 0x1ad0]

// block 0047be8d
0047be8d  pop        edi
0047be8e  pop        ebx

// block 0047be8f
0047be8f  mov        ecx, dword ptr [ebp - 4]
0047be92  xor        ecx, ebp
0047be94  pop        esi
0047be95  call       0x46c932

// block 0047be9a
0047be9a  leave      
0047be9b  ret        
