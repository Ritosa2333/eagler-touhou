
// block 0046fa07
0046fa07  mov        edi, edi
0046fa09  push       ebp
0046fa0a  mov        ebp, esp
0046fa0c  sub        esp, 0x14
0046fa0f  mov        eax, dword ptr [0x4d6440]
0046fa14  mov        ecx, dword ptr [ebp + 8]
0046fa17  imul       eax, eax, 0x14
0046fa1a  add        eax, dword ptr [0x4d6444]
0046fa20  add        ecx, 0x17
0046fa23  and        ecx, 0xfffffff0
0046fa26  mov        dword ptr [ebp - 0x10], ecx
0046fa29  sar        ecx, 4
0046fa2c  push       ebx
0046fa2d  dec        ecx
0046fa2e  cmp        ecx, 0x20
0046fa31  push       esi
0046fa32  push       edi
0046fa33  jge        0x46fa40

// block 0046fa35
0046fa35  or         esi, 0xffffffff
0046fa38  shr        esi, cl
0046fa3a  or         dword ptr [ebp - 8], 0xffffffff
0046fa3e  jmp        0x46fa4d

// block 0046fa40
0046fa40  add        ecx, -0x20
0046fa43  or         edx, 0xffffffff
0046fa46  xor        esi, esi
0046fa48  shr        edx, cl
0046fa4a  mov        dword ptr [ebp - 8], edx

// block 0046fa4d
0046fa4d  mov        ecx, dword ptr [0x4d644c]
0046fa53  mov        ebx, ecx
0046fa55  jmp        0x46fa68

// block 0046fa57
0046fa57  mov        edx, dword ptr [ebx + 4]
0046fa5a  mov        edi, dword ptr [ebx]
0046fa5c  and        edx, dword ptr [ebp - 8]
0046fa5f  and        edi, esi
0046fa61  or         edx, edi
0046fa63  jne        0x46fa6f

// block 0046fa65
0046fa65  add        ebx, 0x14

// block 0046fa68
0046fa68  mov        dword ptr [ebp + 8], ebx
0046fa6b  cmp        ebx, eax
0046fa6d  jb         0x46fa57

// block 0046fa6f
0046fa6f  cmp        ebx, eax
0046fa71  jne        0x46faf2

// block 0046fa73
0046fa73  mov        ebx, dword ptr [0x4d6444]
0046fa79  jmp        0x46fa8c

// block 0046fa7b
0046fa7b  mov        edx, dword ptr [ebx + 4]
0046fa7e  mov        edi, dword ptr [ebx]
0046fa80  and        edx, dword ptr [ebp - 8]
0046fa83  and        edi, esi
0046fa85  or         edx, edi
0046fa87  jne        0x46fa93

// block 0046fa89
0046fa89  add        ebx, 0x14

// block 0046fa8c
0046fa8c  mov        dword ptr [ebp + 8], ebx
0046fa8f  cmp        ebx, ecx
0046fa91  jb         0x46fa7b

// block 0046fa93
0046fa93  cmp        ebx, ecx
0046fa95  jne        0x46faf2

// block 0046fa97
0046fa97  jmp        0x46faa5

// block 0046fa99
0046fa99  cmp        dword ptr [ebx + 8], 0
0046fa9d  jne        0x46faa9

// block 0046fa9f
0046fa9f  add        ebx, 0x14
0046faa2  mov        dword ptr [ebp + 8], ebx

// block 0046faa5
0046faa5  cmp        ebx, eax
0046faa7  jb         0x46fa99

// block 0046faa9
0046faa9  cmp        ebx, eax
0046faab  jne        0x46fade

// block 0046faad
0046faad  mov        ebx, dword ptr [0x4d6444]
0046fab3  jmp        0x46fabe

// block 0046fab5
0046fab5  cmp        dword ptr [ebx + 8], 0
0046fab9  jne        0x46fac5

// block 0046fabb
0046fabb  add        ebx, 0x14

// block 0046fabe
0046fabe  mov        dword ptr [ebp + 8], ebx
0046fac1  cmp        ebx, ecx
0046fac3  jb         0x46fab5

// block 0046fac5
0046fac5  cmp        ebx, ecx
0046fac7  jne        0x46fade

// block 0046fac9
0046fac9  call       0x46f10e

// block 0046face
0046face  mov        ebx, eax
0046fad0  mov        dword ptr [ebp + 8], ebx
0046fad3  test       ebx, ebx
0046fad5  jne        0x46fade

// block 0046fad7
0046fad7  xor        eax, eax
0046fad9  jmp        0x46fce7

// block 0046fade
0046fade  push       ebx
0046fadf  call       0x46f1be

// block 0046fae4
0046fae4  pop        ecx
0046fae5  mov        ecx, dword ptr [ebx + 0x10]
0046fae8  mov        dword ptr [ecx], eax
0046faea  mov        eax, dword ptr [ebx + 0x10]
0046faed  cmp        dword ptr [eax], -1
0046faf0  je         0x46fad7

// block 0046faf2
0046faf2  mov        dword ptr [0x4d644c], ebx
0046faf8  mov        eax, dword ptr [ebx + 0x10]
0046fafb  mov        edx, dword ptr [eax]
0046fafd  mov        dword ptr [ebp - 4], edx
0046fb00  cmp        edx, -1
0046fb03  je         0x46fb19

// block 0046fb05
0046fb05  mov        ecx, dword ptr [eax + edx*4 + 0xc4]
0046fb0c  mov        edi, dword ptr [eax + edx*4 + 0x44]
0046fb10  and        ecx, dword ptr [ebp - 8]
0046fb13  and        edi, esi
0046fb15  or         ecx, edi
0046fb17  jne        0x46fb42

// block 0046fb19
0046fb19  and        dword ptr [ebp - 4], 0
0046fb1d  mov        edx, dword ptr [eax + 0xc4]
0046fb23  lea        ecx, [eax + 0x44]

// block 0046fb26
0046fb26  mov        edi, dword ptr [ecx]
0046fb28  and        edx, dword ptr [ebp - 8]
0046fb2b  and        edi, esi
0046fb2d  or         edx, edi
0046fb2f  jne        0x46fb3f

// block 0046fb31
0046fb31  inc        dword ptr [ebp - 4]
0046fb34  mov        edx, dword ptr [ecx + 0x84]
0046fb3a  add        ecx, 4
0046fb3d  jmp        0x46fb26

// block 0046fb3f
0046fb3f  mov        edx, dword ptr [ebp - 4]

// block 0046fb42
0046fb42  mov        ecx, edx
0046fb44  imul       ecx, ecx, 0x204
0046fb4a  lea        ecx, [ecx + eax + 0x144]
0046fb51  mov        dword ptr [ebp - 0xc], ecx
0046fb54  mov        ecx, dword ptr [eax + edx*4 + 0x44]
0046fb58  xor        edi, edi
0046fb5a  and        ecx, esi
0046fb5c  jne        0x46fb70

// block 0046fb5e
0046fb5e  mov        ecx, dword ptr [eax + edx*4 + 0xc4]
0046fb65  and        ecx, dword ptr [ebp - 8]
0046fb68  push       0x20
0046fb6a  pop        edi
0046fb6b  jmp        0x46fb70

// block 0046fb6d
0046fb6d  add        ecx, ecx
0046fb6f  inc        edi

// block 0046fb70
0046fb70  test       ecx, ecx
0046fb72  jge        0x46fb6d

// block 0046fb74
0046fb74  mov        ecx, dword ptr [ebp - 0xc]
0046fb77  mov        edx, dword ptr [ecx + edi*8 + 4]
0046fb7b  mov        ecx, dword ptr [edx]
0046fb7d  sub        ecx, dword ptr [ebp - 0x10]
0046fb80  mov        esi, ecx
0046fb82  sar        esi, 4
0046fb85  dec        esi
0046fb86  cmp        esi, 0x3f
0046fb89  mov        dword ptr [ebp - 8], ecx
0046fb8c  jle        0x46fb91

// block 0046fb8e
0046fb8e  push       0x3f
0046fb90  pop        esi

// block 0046fb91
0046fb91  cmp        esi, edi
0046fb93  je         0x46fc9a

// block 0046fb99
0046fb99  mov        ecx, dword ptr [edx + 4]
0046fb9c  cmp        ecx, dword ptr [edx + 8]
0046fb9f  jne        0x46fbfd

// block 0046fba1
0046fba1  cmp        edi, 0x20
0046fba4  mov        ebx, 0x80000000
0046fba9  jge        0x46fbd1

// block 0046fbab
0046fbab  mov        ecx, edi
0046fbad  shr        ebx, cl
0046fbaf  mov        ecx, dword ptr [ebp - 4]
0046fbb2  lea        edi, [eax + edi + 4]
0046fbb6  not        ebx
0046fbb8  mov        dword ptr [ebp - 0x14], ebx
0046fbbb  and        ebx, dword ptr [eax + ecx*4 + 0x44]
0046fbbf  mov        dword ptr [eax + ecx*4 + 0x44], ebx
0046fbc3  dec        byte ptr [edi]
0046fbc5  jne        0x46fbfa

// block 0046fbc7
0046fbc7  mov        ecx, dword ptr [ebp - 0x14]
0046fbca  mov        ebx, dword ptr [ebp + 8]
0046fbcd  and        dword ptr [ebx], ecx
0046fbcf  jmp        0x46fbfd

// block 0046fbd1
0046fbd1  lea        ecx, [edi - 0x20]
0046fbd4  shr        ebx, cl
0046fbd6  mov        ecx, dword ptr [ebp - 4]
0046fbd9  lea        ecx, [eax + ecx*4 + 0xc4]
0046fbe0  lea        edi, [eax + edi + 4]
0046fbe4  not        ebx
0046fbe6  and        dword ptr [ecx], ebx
0046fbe8  dec        byte ptr [edi]
0046fbea  mov        dword ptr [ebp - 0x14], ebx
0046fbed  jne        0x46fbfa

// block 0046fbef
0046fbef  mov        ebx, dword ptr [ebp + 8]
0046fbf2  mov        ecx, dword ptr [ebp - 0x14]
0046fbf5  and        dword ptr [ebx + 4], ecx
0046fbf8  jmp        0x46fbfd

// block 0046fbfa
0046fbfa  mov        ebx, dword ptr [ebp + 8]

// block 0046fbfd
0046fbfd  cmp        dword ptr [ebp - 8], 0
0046fc01  mov        ecx, dword ptr [edx + 8]
0046fc04  mov        edi, dword ptr [edx + 4]
0046fc07  mov        dword ptr [ecx + 4], edi
0046fc0a  mov        ecx, dword ptr [edx + 4]
0046fc0d  mov        edi, dword ptr [edx + 8]
0046fc10  mov        dword ptr [ecx + 8], edi
0046fc13  je         0x46fca6

// block 0046fc19
0046fc19  mov        ecx, dword ptr [ebp - 0xc]
0046fc1c  lea        ecx, [ecx + esi*8]
0046fc1f  mov        edi, dword ptr [ecx + 4]
0046fc22  mov        dword ptr [edx + 8], ecx
0046fc25  mov        dword ptr [edx + 4], edi
0046fc28  mov        dword ptr [ecx + 4], edx
0046fc2b  mov        ecx, dword ptr [edx + 4]
0046fc2e  mov        dword ptr [ecx + 8], edx
0046fc31  mov        ecx, dword ptr [edx + 4]
0046fc34  cmp        ecx, dword ptr [edx + 8]
0046fc37  jne        0x46fc97

// block 0046fc39
0046fc39  mov        cl, byte ptr [esi + eax + 4]
0046fc3d  mov        byte ptr [ebp + 0xb], cl
0046fc40  inc        cl
0046fc42  cmp        esi, 0x20
0046fc45  mov        byte ptr [esi + eax + 4], cl
0046fc49  jge        0x46fc6e

// block 0046fc4b
0046fc4b  cmp        byte ptr [ebp + 0xb], 0
0046fc4f  jne        0x46fc5c

// block 0046fc51
0046fc51  mov        edi, 0x80000000
0046fc56  mov        ecx, esi
0046fc58  shr        edi, cl
0046fc5a  or         dword ptr [ebx], edi

// block 0046fc5c
0046fc5c  mov        ecx, esi
0046fc5e  mov        edi, 0x80000000
0046fc63  shr        edi, cl
0046fc65  mov        ecx, dword ptr [ebp - 4]
0046fc68  or         dword ptr [eax + ecx*4 + 0x44], edi
0046fc6c  jmp        0x46fc97

// block 0046fc6e
0046fc6e  cmp        byte ptr [ebp + 0xb], 0
0046fc72  jne        0x46fc81

// block 0046fc74
0046fc74  lea        ecx, [esi - 0x20]
0046fc77  mov        edi, 0x80000000
0046fc7c  shr        edi, cl
0046fc7e  or         dword ptr [ebx + 4], edi

// block 0046fc81
0046fc81  mov        ecx, dword ptr [ebp - 4]
0046fc84  lea        edi, [eax + ecx*4 + 0xc4]
0046fc8b  lea        ecx, [esi - 0x20]
0046fc8e  mov        esi, 0x80000000
0046fc93  shr        esi, cl
0046fc95  or         dword ptr [edi], esi

// block 0046fc97
0046fc97  mov        ecx, dword ptr [ebp - 8]

// block 0046fc9a
0046fc9a  test       ecx, ecx
0046fc9c  je         0x46fca9

// block 0046fc9e
0046fc9e  mov        dword ptr [edx], ecx
0046fca0  mov        dword ptr [ecx + edx - 4], ecx
0046fca4  jmp        0x46fca9

// block 0046fca6
0046fca6  mov        ecx, dword ptr [ebp - 8]

// block 0046fca9
0046fca9  mov        esi, dword ptr [ebp - 0x10]
0046fcac  add        edx, ecx
0046fcae  lea        ecx, [esi + 1]
0046fcb1  mov        dword ptr [edx], ecx
0046fcb3  mov        dword ptr [edx + esi - 4], ecx
0046fcb7  mov        esi, dword ptr [ebp - 0xc]
0046fcba  mov        ecx, dword ptr [esi]
0046fcbc  lea        edi, [ecx + 1]
0046fcbf  mov        dword ptr [esi], edi
0046fcc1  test       ecx, ecx
0046fcc3  jne        0x46fcdf

// block 0046fcc5
0046fcc5  cmp        ebx, dword ptr [0x4b3d58]
0046fccb  jne        0x46fcdf

// block 0046fccd
0046fccd  mov        ecx, dword ptr [ebp - 4]
0046fcd0  cmp        ecx, dword ptr [0x4d6454]
0046fcd6  jne        0x46fcdf

// block 0046fcd8
0046fcd8  and        dword ptr [0x4b3d58], 0

// block 0046fcdf
0046fcdf  mov        ecx, dword ptr [ebp - 4]
0046fce2  mov        dword ptr [eax], ecx
0046fce4  lea        eax, [edx + 4]

// block 0046fce7
0046fce7  pop        edi
0046fce8  pop        esi
0046fce9  pop        ebx
0046fcea  leave      
0046fceb  ret        
