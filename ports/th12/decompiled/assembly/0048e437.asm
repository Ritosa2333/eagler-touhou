
// block 0048e437
0048e437  mov        edi, edi
0048e439  push       ebp
0048e43a  mov        ebp, esp
0048e43c  sub        esp, 0x1c
0048e43f  mov        edx, dword ptr [ebp + 0x10]
0048e442  push       esi
0048e443  mov        esi, dword ptr [ebp + 8]
0048e446  push       -2
0048e448  pop        eax
0048e449  mov        dword ptr [ebp - 0x14], eax
0048e44c  mov        dword ptr [ebp - 0x1c], edx
0048e44f  cmp        esi, eax
0048e451  jne        0x48e46e

// block 0048e453
0048e453  call       0x46e982

// block 0048e458
0048e458  and        dword ptr [eax], 0
0048e45b  call       0x46e96f

// block 0048e460
0048e460  mov        dword ptr [eax], 9
0048e466  or         eax, 0xffffffff
0048e469  jmp        0x48e9f6

// block 0048e46e
0048e46e  push       ebx
0048e46f  xor        ebx, ebx
0048e471  cmp        esi, ebx
0048e473  jl         0x48e47d

// block 0048e475
0048e475  cmp        esi, dword ptr [0x4d6308]
0048e47b  jb         0x48e4a4

// block 0048e47d
0048e47d  call       0x46e982

// block 0048e482
0048e482  mov        dword ptr [eax], ebx
0048e484  call       0x46e96f

// block 0048e489
0048e489  push       ebx
0048e48a  push       ebx
0048e48b  push       ebx
0048e48c  push       ebx
0048e48d  push       ebx
0048e48e  mov        dword ptr [eax], 9
0048e494  call       0x470f2d

// block 0048e499
0048e499  add        esp, 0x14
0048e49c  or         eax, 0xffffffff
0048e49f  jmp        0x48e9f5

// block 0048e4a4
0048e4a4  mov        eax, esi
0048e4a6  sar        eax, 5
0048e4a9  push       edi
0048e4aa  lea        edi, [eax*4 + 0x4d6320]
0048e4b1  mov        eax, dword ptr [edi]
0048e4b3  and        esi, 0x1f
0048e4b6  shl        esi, 6
0048e4b9  add        eax, esi
0048e4bb  mov        cl, byte ptr [eax + 4]
0048e4be  test       cl, 1
0048e4c1  jne        0x48e4d7

// block 0048e4c3
0048e4c3  call       0x46e982

// block 0048e4c8
0048e4c8  mov        dword ptr [eax], ebx
0048e4ca  call       0x46e96f

// block 0048e4cf
0048e4cf  mov        dword ptr [eax], 9
0048e4d5  jmp        0x48e541

// block 0048e4d7
0048e4d7  cmp        edx, 0x7fffffff
0048e4dd  ja         0x48e52f

// block 0048e4df
0048e4df  mov        dword ptr [ebp - 0x10], ebx
0048e4e2  cmp        edx, ebx
0048e4e4  je         0x48e9f2

// block 0048e4ea
0048e4ea  test       cl, 2
0048e4ed  jne        0x48e9f2

// block 0048e4f3
0048e4f3  cmp        dword ptr [ebp + 0xc], ebx
0048e4f6  je         0x48e52f

// block 0048e4f8
0048e4f8  mov        al, byte ptr [eax + 0x24]
0048e4fb  add        al, al
0048e4fd  sar        al, 1
0048e4ff  mov        byte ptr [ebp - 2], al
0048e502  movsx      eax, al
0048e505  dec        eax
0048e506  push       4
0048e508  pop        ecx
0048e509  je         0x48e527

// block 0048e50b
0048e50b  dec        eax
0048e50c  jne        0x48e51c

// block 0048e50e
0048e50e  mov        eax, edx
0048e510  not        eax
0048e512  test       al, 1
0048e514  je         0x48e52f

// block 0048e516
0048e516  and        edx, 0xfffffffe
0048e519  mov        dword ptr [ebp + 0x10], edx

// block 0048e51c
0048e51c  mov        eax, dword ptr [ebp + 0xc]
0048e51f  mov        dword ptr [ebp - 0xc], eax
0048e522  jmp        0x48e5a8

// block 0048e527
0048e527  mov        eax, edx
0048e529  not        eax
0048e52b  test       al, 1
0048e52d  jne        0x48e550

// block 0048e52f
0048e52f  call       0x46e982

// block 0048e534
0048e534  mov        dword ptr [eax], ebx
0048e536  call       0x46e96f

// block 0048e53b
0048e53b  mov        dword ptr [eax], 0x16

// block 0048e541
0048e541  push       ebx
0048e542  push       ebx
0048e543  push       ebx
0048e544  push       ebx
0048e545  push       ebx
0048e546  call       0x470f2d

// block 0048e54b
0048e54b  add        esp, 0x14
0048e54e  jmp        0x48e584

// block 0048e550
0048e550  mov        eax, edx
0048e552  shr        eax, 1
0048e554  mov        dword ptr [ebp + 0x10], ecx
0048e557  cmp        eax, ecx
0048e559  jb         0x48e55e

// block 0048e55b
0048e55b  mov        dword ptr [ebp + 0x10], eax

// block 0048e55e
0048e55e  push       dword ptr [ebp + 0x10]
0048e561  call       0x4777ce

// block 0048e566
0048e566  pop        ecx
0048e567  mov        dword ptr [ebp - 0xc], eax
0048e56a  cmp        eax, ebx
0048e56c  jne        0x48e58c

// block 0048e56e
0048e56e  call       0x46e96f

// block 0048e573
0048e573  mov        dword ptr [eax], 0xc
0048e579  call       0x46e982

// block 0048e57e
0048e57e  mov        dword ptr [eax], 8

// block 0048e584
0048e584  or         eax, 0xffffffff
0048e587  jmp        0x48e9f4

// block 0048e58c
0048e58c  push       1
0048e58e  push       ebx
0048e58f  push       ebx
0048e590  push       dword ptr [ebp + 8]
0048e593  call       0x47b5cb

// block 0048e598
0048e598  mov        ecx, dword ptr [edi]
0048e59a  mov        dword ptr [esi + ecx + 0x28], eax
0048e59e  mov        eax, dword ptr [ebp - 0xc]
0048e5a1  add        esp, 0x10
0048e5a4  mov        dword ptr [esi + ecx + 0x2c], edx

// block 0048e5a8
0048e5a8  mov        ecx, dword ptr [edi]
0048e5aa  add        ecx, esi
0048e5ac  test       byte ptr [ecx + 4], 0x48
0048e5b0  je         0x48e626

// block 0048e5b2
0048e5b2  mov        cl, byte ptr [ecx + 5]
0048e5b5  cmp        cl, 0xa
0048e5b8  je         0x48e626

// block 0048e5ba
0048e5ba  cmp        dword ptr [ebp + 0x10], ebx
0048e5bd  je         0x48e626

// block 0048e5bf
0048e5bf  mov        byte ptr [eax], cl
0048e5c1  mov        ecx, dword ptr [edi]
0048e5c3  inc        eax
0048e5c4  dec        dword ptr [ebp + 0x10]
0048e5c7  mov        dword ptr [ebp - 0x10], 1
0048e5ce  mov        byte ptr [esi + ecx + 5], 0xa
0048e5d3  cmp        byte ptr [ebp - 2], bl
0048e5d6  je         0x48e626

// block 0048e5d8
0048e5d8  mov        ecx, dword ptr [edi]
0048e5da  mov        cl, byte ptr [esi + ecx + 0x25]
0048e5de  cmp        cl, 0xa
0048e5e1  je         0x48e626

// block 0048e5e3
0048e5e3  cmp        dword ptr [ebp + 0x10], ebx
0048e5e6  je         0x48e626

// block 0048e5e8
0048e5e8  mov        byte ptr [eax], cl
0048e5ea  mov        ecx, dword ptr [edi]
0048e5ec  inc        eax
0048e5ed  dec        dword ptr [ebp + 0x10]
0048e5f0  cmp        byte ptr [ebp - 2], 1
0048e5f4  mov        dword ptr [ebp - 0x10], 2
0048e5fb  mov        byte ptr [esi + ecx + 0x25], 0xa
0048e600  jne        0x48e626

// block 0048e602
0048e602  mov        ecx, dword ptr [edi]
0048e604  mov        cl, byte ptr [esi + ecx + 0x26]
0048e608  cmp        cl, 0xa
0048e60b  je         0x48e626

// block 0048e60d
0048e60d  cmp        dword ptr [ebp + 0x10], ebx
0048e610  je         0x48e626

// block 0048e612
0048e612  mov        byte ptr [eax], cl
0048e614  mov        ecx, dword ptr [edi]
0048e616  inc        eax
0048e617  dec        dword ptr [ebp + 0x10]
0048e61a  mov        dword ptr [ebp - 0x10], 3
0048e621  mov        byte ptr [esi + ecx + 0x26], 0xa

// block 0048e626
0048e626  push       ebx
0048e627  lea        ecx, [ebp - 0x18]
0048e62a  push       ecx
0048e62b  push       dword ptr [ebp + 0x10]
0048e62e  push       eax
0048e62f  mov        eax, dword ptr [edi]
0048e631  push       dword ptr [esi + eax]
0048e634  call       dword ptr [0x4980a8]

// block 0048e63a
0048e63a  test       eax, eax
0048e63c  je         0x48e9bd

// block 0048e642
0048e642  mov        ecx, dword ptr [ebp - 0x18]
0048e645  cmp        ecx, ebx
0048e647  jl         0x48e9bd

// block 0048e64d
0048e64d  cmp        ecx, dword ptr [ebp + 0x10]
0048e650  ja         0x48e9bd

// block 0048e656
0048e656  mov        eax, dword ptr [edi]
0048e658  add        dword ptr [ebp - 0x10], ecx
0048e65b  lea        eax, [esi + eax + 4]
0048e65f  test       byte ptr [eax], 0x80
0048e662  je         0x48e84e

// block 0048e668
0048e668  cmp        byte ptr [ebp - 2], 2
0048e66c  je         0x48e888

// block 0048e672
0048e672  cmp        ecx, ebx
0048e674  je         0x48e683

// block 0048e676
0048e676  mov        ecx, dword ptr [ebp - 0xc]
0048e679  cmp        byte ptr [ecx], 0xa
0048e67c  jne        0x48e683

// block 0048e67e
0048e67e  or         byte ptr [eax], 4
0048e681  jmp        0x48e686

// block 0048e683
0048e683  and        byte ptr [eax], 0xfb

// block 0048e686
0048e686  mov        ebx, dword ptr [ebp - 0xc]
0048e689  mov        eax, dword ptr [ebp - 0x10]
0048e68c  add        eax, ebx
0048e68e  mov        dword ptr [ebp + 0x10], ebx
0048e691  mov        dword ptr [ebp - 0x10], eax
0048e694  cmp        ebx, eax
0048e696  jae        0x48e76c

// block 0048e69c
0048e69c  mov        ecx, dword ptr [ebp + 0x10]
0048e69f  mov        al, byte ptr [ecx]
0048e6a1  cmp        al, 0x1a
0048e6a3  je         0x48e757

// block 0048e6a9
0048e6a9  cmp        al, 0xd
0048e6ab  je         0x48e6b9

// block 0048e6ad
0048e6ad  mov        byte ptr [ebx], al
0048e6af  inc        ebx
0048e6b0  inc        ecx
0048e6b1  mov        dword ptr [ebp + 0x10], ecx
0048e6b4  jmp        0x48e749

// block 0048e6b9
0048e6b9  mov        eax, dword ptr [ebp - 0x10]
0048e6bc  dec        eax
0048e6bd  cmp        ecx, eax
0048e6bf  jae        0x48e6d8

// block 0048e6c1
0048e6c1  lea        eax, [ecx + 1]
0048e6c4  cmp        byte ptr [eax], 0xa
0048e6c7  jne        0x48e6d3

// block 0048e6c9
0048e6c9  inc        ecx
0048e6ca  inc        ecx
0048e6cb  mov        dword ptr [ebp + 0x10], ecx

// block 0048e6ce
0048e6ce  mov        byte ptr [ebx], 0xa
0048e6d1  jmp        0x48e748

// block 0048e6d3
0048e6d3  mov        dword ptr [ebp + 0x10], eax
0048e6d6  jmp        0x48e745

// block 0048e6d8
0048e6d8  inc        dword ptr [ebp + 0x10]
0048e6db  push       0
0048e6dd  lea        eax, [ebp - 0x18]
0048e6e0  push       eax
0048e6e1  push       1
0048e6e3  lea        eax, [ebp - 1]
0048e6e6  push       eax
0048e6e7  mov        eax, dword ptr [edi]
0048e6e9  push       dword ptr [esi + eax]
0048e6ec  call       dword ptr [0x4980a8]

// block 0048e6f2
0048e6f2  test       eax, eax
0048e6f4  jne        0x48e700

// block 0048e6f6
0048e6f6  call       dword ptr [0x4980e4]

// block 0048e6fc
0048e6fc  test       eax, eax
0048e6fe  jne        0x48e745

// block 0048e700
0048e700  cmp        dword ptr [ebp - 0x18], 0
0048e704  je         0x48e745

// block 0048e706
0048e706  mov        eax, dword ptr [edi]
0048e708  test       byte ptr [esi + eax + 4], 0x48
0048e70d  je         0x48e723

// block 0048e70f
0048e70f  cmp        byte ptr [ebp - 1], 0xa
0048e713  je         0x48e6ce

// block 0048e715
0048e715  mov        byte ptr [ebx], 0xd
0048e718  mov        eax, dword ptr [edi]
0048e71a  mov        cl, byte ptr [ebp - 1]
0048e71d  mov        byte ptr [esi + eax + 5], cl
0048e721  jmp        0x48e748

// block 0048e723
0048e723  cmp        ebx, dword ptr [ebp - 0xc]
0048e726  jne        0x48e72e

// block 0048e728
0048e728  cmp        byte ptr [ebp - 1], 0xa
0048e72c  je         0x48e6ce

// block 0048e72e
0048e72e  push       1
0048e730  push       -1
0048e732  push       -1
0048e734  push       dword ptr [ebp + 8]
0048e737  call       0x47b5cb

// block 0048e73c
0048e73c  add        esp, 0x10
0048e73f  cmp        byte ptr [ebp - 1], 0xa
0048e743  je         0x48e749

// block 0048e745
0048e745  mov        byte ptr [ebx], 0xd

// block 0048e748
0048e748  inc        ebx

// block 0048e749
0048e749  mov        eax, dword ptr [ebp - 0x10]
0048e74c  cmp        dword ptr [ebp + 0x10], eax
0048e74f  jb         0x48e69c

// block 0048e755
0048e755  jmp        0x48e76c

// block 0048e757
0048e757  mov        eax, dword ptr [edi]
0048e759  lea        eax, [esi + eax + 4]
0048e75d  test       byte ptr [eax], 0x40
0048e760  jne        0x48e767

// block 0048e762
0048e762  or         byte ptr [eax], 2
0048e765  jmp        0x48e76c

// block 0048e767
0048e767  mov        al, byte ptr [ecx]
0048e769  mov        byte ptr [ebx], al
0048e76b  inc        ebx

// block 0048e76c
0048e76c  mov        eax, ebx
0048e76e  sub        eax, dword ptr [ebp - 0xc]
0048e771  cmp        byte ptr [ebp - 2], 1
0048e775  mov        dword ptr [ebp - 0x10], eax
0048e778  jne        0x48e84e

// block 0048e77e
0048e77e  test       eax, eax
0048e780  je         0x48e84e

// block 0048e786
0048e786  dec        ebx
0048e787  mov        cl, byte ptr [ebx]
0048e789  test       cl, cl
0048e78b  js         0x48e793

// block 0048e78d
0048e78d  inc        ebx
0048e78e  jmp        0x48e819

// block 0048e793
0048e793  xor        eax, eax
0048e795  inc        eax
0048e796  movzx      ecx, cl
0048e799  jmp        0x48e7aa

// block 0048e79b
0048e79b  cmp        eax, 4
0048e79e  jg         0x48e7b3

// block 0048e7a0
0048e7a0  cmp        ebx, dword ptr [ebp - 0xc]
0048e7a3  jb         0x48e7b3

// block 0048e7a5
0048e7a5  dec        ebx
0048e7a6  movzx      ecx, byte ptr [ebx]
0048e7a9  inc        eax

// block 0048e7aa
0048e7aa  cmp        byte ptr [ecx + 0x4ae320], 0
0048e7b1  je         0x48e79b

// block 0048e7b3
0048e7b3  mov        dl, byte ptr [ebx]
0048e7b5  movzx      ecx, dl
0048e7b8  movsx      ecx, byte ptr [ecx + 0x4ae320]
0048e7bf  test       ecx, ecx
0048e7c1  jne        0x48e7d0

// block 0048e7c3
0048e7c3  call       0x46e96f

// block 0048e7c8
0048e7c8  mov        dword ptr [eax], 0x2a
0048e7ce  jmp        0x48e84a

// block 0048e7d0
0048e7d0  inc        ecx
0048e7d1  cmp        ecx, eax
0048e7d3  jne        0x48e7d9

// block 0048e7d5
0048e7d5  add        ebx, eax
0048e7d7  jmp        0x48e819

// block 0048e7d9
0048e7d9  mov        ecx, dword ptr [edi]
0048e7db  add        ecx, esi
0048e7dd  test       byte ptr [ecx + 4], 0x48
0048e7e1  je         0x48e807

// block 0048e7e3
0048e7e3  inc        ebx
0048e7e4  cmp        eax, 2
0048e7e7  mov        byte ptr [ecx + 5], dl
0048e7ea  jl         0x48e7f5

// block 0048e7ec
0048e7ec  mov        dl, byte ptr [ebx]
0048e7ee  mov        ecx, dword ptr [edi]
0048e7f0  mov        byte ptr [esi + ecx + 0x25], dl
0048e7f4  inc        ebx

// block 0048e7f5
0048e7f5  cmp        eax, 3
0048e7f8  jne        0x48e803

// block 0048e7fa
0048e7fa  mov        dl, byte ptr [ebx]
0048e7fc  mov        ecx, dword ptr [edi]
0048e7fe  mov        byte ptr [esi + ecx + 0x26], dl
0048e802  inc        ebx

// block 0048e803
0048e803  sub        ebx, eax
0048e805  jmp        0x48e819

// block 0048e807
0048e807  neg        eax
0048e809  cdq        
0048e80a  push       1
0048e80c  push       edx
0048e80d  push       eax
0048e80e  push       dword ptr [ebp + 8]
0048e811  call       0x47b5cb

// block 0048e816
0048e816  add        esp, 0x10

// block 0048e819
0048e819  mov        eax, dword ptr [ebp - 0x1c]
0048e81c  sub        ebx, dword ptr [ebp - 0xc]
0048e81f  shr        eax, 1
0048e821  push       eax
0048e822  push       dword ptr [ebp + 0xc]
0048e825  push       ebx
0048e826  push       dword ptr [ebp - 0xc]
0048e829  push       0
0048e82b  push       0xfde9
0048e830  call       dword ptr [0x4980dc]

// block 0048e836
0048e836  mov        dword ptr [ebp - 0x10], eax
0048e839  test       eax, eax
0048e83b  jne        0x48e871

// block 0048e83d
0048e83d  call       dword ptr [0x4980e4]

// block 0048e843
0048e843  push       eax
0048e844  call       0x46e995

// block 0048e849
0048e849  pop        ecx

// block 0048e84a
0048e84a  or         dword ptr [ebp - 0x14], 0xffffffff

// block 0048e84e
0048e84e  mov        eax, dword ptr [ebp - 0xc]
0048e851  cmp        eax, dword ptr [ebp + 0xc]
0048e854  je         0x48e85d

// block 0048e856
0048e856  push       eax
0048e857  call       0x46c941

// block 0048e85c
0048e85c  pop        ecx

// block 0048e85d
0048e85d  mov        eax, dword ptr [ebp - 0x14]
0048e860  cmp        eax, -2
0048e863  jne        0x48e9f4

// block 0048e869
0048e869  mov        eax, dword ptr [ebp - 0x10]
0048e86c  jmp        0x48e9f4

// block 0048e871
0048e871  mov        eax, dword ptr [ebp - 0x10]
0048e874  mov        edx, dword ptr [edi]
0048e876  xor        ecx, ecx
0048e878  cmp        eax, ebx
0048e87a  setne      cl
0048e87d  add        eax, eax
0048e87f  mov        dword ptr [ebp - 0x10], eax
0048e882  mov        dword ptr [esi + edx + 0x30], ecx
0048e886  jmp        0x48e84e

// block 0048e888
0048e888  cmp        ecx, ebx
0048e88a  je         0x48e89a

// block 0048e88c
0048e88c  mov        ecx, dword ptr [ebp - 0xc]
0048e88f  cmp        word ptr [ecx], 0xa
0048e893  jne        0x48e89a

// block 0048e895
0048e895  or         byte ptr [eax], 4
0048e898  jmp        0x48e89d

// block 0048e89a
0048e89a  and        byte ptr [eax], 0xfb

// block 0048e89d
0048e89d  mov        ebx, dword ptr [ebp - 0xc]
0048e8a0  mov        eax, dword ptr [ebp - 0x10]
0048e8a3  add        eax, ebx
0048e8a5  mov        dword ptr [ebp + 0x10], ebx
0048e8a8  mov        dword ptr [ebp - 0x10], eax
0048e8ab  cmp        ebx, eax
0048e8ad  jae        0x48e9b2

// block 0048e8b3
0048e8b3  mov        eax, dword ptr [ebp + 0x10]
0048e8b6  movzx      ecx, word ptr [eax]
0048e8b9  cmp        cx, 0x1a
0048e8bd  je         0x48e99a

// block 0048e8c3
0048e8c3  cmp        cx, 0xd
0048e8c7  je         0x48e8d8

// block 0048e8c9
0048e8c9  mov        word ptr [ebx], cx
0048e8cc  inc        ebx
0048e8cd  inc        ebx
0048e8ce  inc        eax
0048e8cf  inc        eax
0048e8d0  mov        dword ptr [ebp + 0x10], eax
0048e8d3  jmp        0x48e98c

// block 0048e8d8
0048e8d8  mov        ecx, dword ptr [ebp - 0x10]
0048e8db  add        ecx, -2
0048e8de  cmp        eax, ecx
0048e8e0  jae        0x48e900

// block 0048e8e2
0048e8e2  lea        ecx, [eax + 2]
0048e8e5  cmp        word ptr [ecx], 0xa
0048e8e9  jne        0x48e8f8

// block 0048e8eb
0048e8eb  add        eax, 4
0048e8ee  mov        dword ptr [ebp + 0x10], eax

// block 0048e8f1
0048e8f1  push       0xa
0048e8f3  jmp        0x48e986

// block 0048e8f8
0048e8f8  mov        dword ptr [ebp + 0x10], ecx
0048e8fb  jmp        0x48e984

// block 0048e900
0048e900  add        dword ptr [ebp + 0x10], 2
0048e904  push       0
0048e906  lea        eax, [ebp - 0x18]
0048e909  push       eax
0048e90a  push       2
0048e90c  lea        eax, [ebp - 8]
0048e90f  push       eax
0048e910  mov        eax, dword ptr [edi]
0048e912  push       dword ptr [esi + eax]
0048e915  call       dword ptr [0x4980a8]

// block 0048e91b
0048e91b  test       eax, eax
0048e91d  jne        0x48e929

// block 0048e91f
0048e91f  call       dword ptr [0x4980e4]

// block 0048e925
0048e925  test       eax, eax
0048e927  jne        0x48e984

// block 0048e929
0048e929  cmp        dword ptr [ebp - 0x18], 0
0048e92d  je         0x48e984

// block 0048e92f
0048e92f  mov        eax, dword ptr [edi]
0048e931  test       byte ptr [esi + eax + 4], 0x48
0048e936  je         0x48e960

// block 0048e938
0048e938  cmp        word ptr [ebp - 8], 0xa
0048e93d  je         0x48e8f1

// block 0048e93f
0048e93f  push       0xd
0048e941  pop        eax
0048e942  mov        word ptr [ebx], ax
0048e945  mov        eax, dword ptr [edi]
0048e947  mov        cl, byte ptr [ebp - 8]
0048e94a  mov        byte ptr [esi + eax + 5], cl
0048e94e  mov        eax, dword ptr [edi]
0048e950  mov        cl, byte ptr [ebp - 7]
0048e953  mov        byte ptr [esi + eax + 0x25], cl
0048e957  mov        eax, dword ptr [edi]
0048e959  mov        byte ptr [esi + eax + 0x26], 0xa
0048e95e  jmp        0x48e98a

// block 0048e960
0048e960  cmp        ebx, dword ptr [ebp - 0xc]
0048e963  jne        0x48e96c

// block 0048e965
0048e965  cmp        word ptr [ebp - 8], 0xa
0048e96a  je         0x48e8f1

// block 0048e96c
0048e96c  push       1
0048e96e  push       -1
0048e970  push       -2
0048e972  push       dword ptr [ebp + 8]
0048e975  call       0x47b5cb

// block 0048e97a
0048e97a  add        esp, 0x10
0048e97d  cmp        word ptr [ebp - 8], 0xa
0048e982  je         0x48e98c

// block 0048e984
0048e984  push       0xd

// block 0048e986
0048e986  pop        eax
0048e987  mov        word ptr [ebx], ax

// block 0048e98a
0048e98a  inc        ebx
0048e98b  inc        ebx

// block 0048e98c
0048e98c  mov        eax, dword ptr [ebp - 0x10]
0048e98f  cmp        dword ptr [ebp + 0x10], eax
0048e992  jb         0x48e8b3

// block 0048e998
0048e998  jmp        0x48e9b2

// block 0048e99a
0048e99a  mov        ecx, dword ptr [edi]
0048e99c  lea        esi, [esi + ecx + 4]
0048e9a0  test       byte ptr [esi], 0x40
0048e9a3  jne        0x48e9aa

// block 0048e9a5
0048e9a5  or         byte ptr [esi], 2
0048e9a8  jmp        0x48e9b2

// block 0048e9aa
0048e9aa  mov        ax, word ptr [eax]
0048e9ad  mov        word ptr [ebx], ax
0048e9b0  inc        ebx
0048e9b1  inc        ebx

// block 0048e9b2
0048e9b2  sub        ebx, dword ptr [ebp - 0xc]
0048e9b5  mov        dword ptr [ebp - 0x10], ebx
0048e9b8  jmp        0x48e84e

// block 0048e9bd
0048e9bd  call       dword ptr [0x4980e4]

// block 0048e9c3
0048e9c3  push       5
0048e9c5  pop        esi
0048e9c6  cmp        eax, esi
0048e9c8  jne        0x48e9e1

// block 0048e9ca
0048e9ca  call       0x46e96f

// block 0048e9cf
0048e9cf  mov        dword ptr [eax], 9
0048e9d5  call       0x46e982

// block 0048e9da
0048e9da  mov        dword ptr [eax], esi
0048e9dc  jmp        0x48e84a

// block 0048e9e1
0048e9e1  cmp        eax, 0x6d
0048e9e4  jne        0x48e843

// block 0048e9ea
0048e9ea  mov        dword ptr [ebp - 0x14], ebx
0048e9ed  jmp        0x48e84e

// block 0048e9f2
0048e9f2  xor        eax, eax

// block 0048e9f4
0048e9f4  pop        edi

// block 0048e9f5
0048e9f5  pop        ebx

// block 0048e9f6
0048e9f6  pop        esi
0048e9f7  leave      
0048e9f8  ret        
