
// block 0046f2c6
0046f2c6  mov        edi, edi
0046f2c8  push       ebp
0046f2c9  mov        ebp, esp
0046f2cb  sub        esp, 0xc
0046f2ce  mov        ecx, dword ptr [ebp + 8]
0046f2d1  mov        eax, dword ptr [ecx + 0x10]
0046f2d4  push       ebx
0046f2d5  push       esi
0046f2d6  mov        esi, dword ptr [ebp + 0x10]
0046f2d9  push       edi
0046f2da  mov        edi, dword ptr [ebp + 0xc]
0046f2dd  mov        edx, edi
0046f2df  sub        edx, dword ptr [ecx + 0xc]
0046f2e2  add        esi, 0x17
0046f2e5  shr        edx, 0xf
0046f2e8  mov        ecx, edx
0046f2ea  imul       ecx, ecx, 0x204
0046f2f0  lea        ecx, [ecx + eax + 0x144]
0046f2f7  mov        dword ptr [ebp - 0xc], ecx
0046f2fa  mov        ecx, dword ptr [edi - 4]
0046f2fd  and        esi, 0xfffffff0
0046f300  dec        ecx
0046f301  cmp        esi, ecx
0046f303  lea        edi, [ecx + edi - 4]
0046f307  mov        ebx, dword ptr [edi]
0046f309  mov        dword ptr [ebp + 0x10], ecx
0046f30c  mov        dword ptr [ebp - 4], ebx
0046f30f  jle        0x46f46a

// block 0046f315
0046f315  test       bl, 1
0046f318  jne        0x46f463

// block 0046f31e
0046f31e  add        ebx, ecx
0046f320  cmp        esi, ebx
0046f322  jg         0x46f463

// block 0046f328
0046f328  mov        ecx, dword ptr [ebp - 4]
0046f32b  sar        ecx, 4
0046f32e  dec        ecx
0046f32f  mov        dword ptr [ebp - 8], ecx
0046f332  cmp        ecx, 0x3f
0046f335  jbe        0x46f33d

// block 0046f337
0046f337  push       0x3f
0046f339  pop        ecx
0046f33a  mov        dword ptr [ebp - 8], ecx

// block 0046f33d
0046f33d  mov        ebx, dword ptr [edi + 4]
0046f340  cmp        ebx, dword ptr [edi + 8]
0046f343  jne        0x46f388

// block 0046f345
0046f345  mov        ebx, 0x80000000
0046f34a  cmp        ecx, 0x20
0046f34d  jae        0x46f369

// block 0046f34f
0046f34f  shr        ebx, cl
0046f351  mov        ecx, dword ptr [ebp - 8]
0046f354  lea        ecx, [ecx + eax + 4]
0046f358  not        ebx
0046f35a  and        dword ptr [eax + edx*4 + 0x44], ebx
0046f35e  dec        byte ptr [ecx]
0046f360  jne        0x46f388

// block 0046f362
0046f362  mov        ecx, dword ptr [ebp + 8]
0046f365  and        dword ptr [ecx], ebx
0046f367  jmp        0x46f388

// block 0046f369
0046f369  add        ecx, -0x20
0046f36c  shr        ebx, cl
0046f36e  mov        ecx, dword ptr [ebp - 8]
0046f371  lea        ecx, [ecx + eax + 4]
0046f375  not        ebx
0046f377  and        dword ptr [eax + edx*4 + 0xc4], ebx
0046f37e  dec        byte ptr [ecx]
0046f380  jne        0x46f388

// block 0046f382
0046f382  mov        ecx, dword ptr [ebp + 8]
0046f385  and        dword ptr [ecx + 4], ebx

// block 0046f388
0046f388  mov        ecx, dword ptr [edi + 8]
0046f38b  mov        ebx, dword ptr [edi + 4]
0046f38e  mov        dword ptr [ecx + 4], ebx
0046f391  mov        ecx, dword ptr [edi + 4]
0046f394  mov        edi, dword ptr [edi + 8]
0046f397  mov        dword ptr [ecx + 8], edi
0046f39a  mov        ecx, dword ptr [ebp + 0x10]
0046f39d  sub        ecx, esi
0046f39f  add        dword ptr [ebp - 4], ecx
0046f3a2  cmp        dword ptr [ebp - 4], 0
0046f3a6  jle        0x46f451

// block 0046f3ac
0046f3ac  mov        edi, dword ptr [ebp - 4]
0046f3af  mov        ecx, dword ptr [ebp + 0xc]
0046f3b2  sar        edi, 4
0046f3b5  dec        edi
0046f3b6  lea        ecx, [ecx + esi - 4]
0046f3ba  cmp        edi, 0x3f
0046f3bd  jbe        0x46f3c2

// block 0046f3bf
0046f3bf  push       0x3f
0046f3c1  pop        edi

// block 0046f3c2
0046f3c2  mov        ebx, dword ptr [ebp - 0xc]
0046f3c5  lea        ebx, [ebx + edi*8]
0046f3c8  mov        dword ptr [ebp + 0x10], ebx
0046f3cb  mov        ebx, dword ptr [ebx + 4]
0046f3ce  mov        dword ptr [ecx + 4], ebx
0046f3d1  mov        ebx, dword ptr [ebp + 0x10]
0046f3d4  mov        dword ptr [ecx + 8], ebx
0046f3d7  mov        dword ptr [ebx + 4], ecx
0046f3da  mov        ebx, dword ptr [ecx + 4]
0046f3dd  mov        dword ptr [ebx + 8], ecx
0046f3e0  mov        ebx, dword ptr [ecx + 4]
0046f3e3  cmp        ebx, dword ptr [ecx + 8]
0046f3e6  jne        0x46f43f

// block 0046f3e8
0046f3e8  mov        cl, byte ptr [edi + eax + 4]
0046f3ec  mov        byte ptr [ebp + 0x13], cl
0046f3ef  inc        cl
0046f3f1  mov        byte ptr [edi + eax + 4], cl
0046f3f5  cmp        edi, 0x20
0046f3f8  jae        0x46f416

// block 0046f3fa
0046f3fa  cmp        byte ptr [ebp + 0x13], 0
0046f3fe  jne        0x46f40e

// block 0046f400
0046f400  mov        ecx, edi
0046f402  mov        ebx, 0x80000000
0046f407  shr        ebx, cl
0046f409  mov        ecx, dword ptr [ebp + 8]
0046f40c  or         dword ptr [ecx], ebx

// block 0046f40e
0046f40e  lea        eax, [eax + edx*4 + 0x44]
0046f412  mov        ecx, edi
0046f414  jmp        0x46f436

// block 0046f416
0046f416  cmp        byte ptr [ebp + 0x13], 0
0046f41a  jne        0x46f42c

// block 0046f41c
0046f41c  lea        ecx, [edi - 0x20]
0046f41f  mov        ebx, 0x80000000
0046f424  shr        ebx, cl
0046f426  mov        ecx, dword ptr [ebp + 8]
0046f429  or         dword ptr [ecx + 4], ebx

// block 0046f42c
0046f42c  lea        eax, [eax + edx*4 + 0xc4]
0046f433  lea        ecx, [edi - 0x20]

// block 0046f436
0046f436  mov        edx, 0x80000000
0046f43b  shr        edx, cl
0046f43d  or         dword ptr [eax], edx

// block 0046f43f
0046f43f  mov        edx, dword ptr [ebp + 0xc]
0046f442  mov        ecx, dword ptr [ebp - 4]
0046f445  lea        eax, [edx + esi - 4]
0046f449  mov        dword ptr [eax], ecx
0046f44b  mov        dword ptr [ecx + eax - 4], ecx
0046f44f  jmp        0x46f454

// block 0046f451
0046f451  mov        edx, dword ptr [ebp + 0xc]

// block 0046f454
0046f454  lea        eax, [esi + 1]
0046f457  mov        dword ptr [edx - 4], eax
0046f45a  mov        dword ptr [edx + esi - 8], eax
0046f45e  jmp        0x46f59f

// block 0046f463
0046f463  xor        eax, eax
0046f465  jmp        0x46f5a2

// block 0046f46a
0046f46a  jge        0x46f59f

// block 0046f470
0046f470  mov        ebx, dword ptr [ebp + 0xc]
0046f473  sub        dword ptr [ebp + 0x10], esi
0046f476  lea        ecx, [esi + 1]
0046f479  mov        dword ptr [ebx - 4], ecx
0046f47c  lea        ebx, [ebx + esi - 4]
0046f480  mov        esi, dword ptr [ebp + 0x10]
0046f483  sar        esi, 4
0046f486  dec        esi
0046f487  mov        dword ptr [ebp + 0xc], ebx
0046f48a  mov        dword ptr [ebx - 4], ecx
0046f48d  cmp        esi, 0x3f
0046f490  jbe        0x46f495

// block 0046f492
0046f492  push       0x3f
0046f494  pop        esi

// block 0046f495
0046f495  test       byte ptr [ebp - 4], 1
0046f499  jne        0x46f51f

// block 0046f49f
0046f49f  mov        esi, dword ptr [ebp - 4]
0046f4a2  sar        esi, 4
0046f4a5  dec        esi
0046f4a6  cmp        esi, 0x3f
0046f4a9  jbe        0x46f4ae

// block 0046f4ab
0046f4ab  push       0x3f
0046f4ad  pop        esi

// block 0046f4ae
0046f4ae  mov        ecx, dword ptr [edi + 4]
0046f4b1  cmp        ecx, dword ptr [edi + 8]
0046f4b4  jne        0x46f4f8

// block 0046f4b6
0046f4b6  mov        ebx, 0x80000000
0046f4bb  cmp        esi, 0x20
0046f4be  jae        0x46f4d9

// block 0046f4c0
0046f4c0  mov        ecx, esi
0046f4c2  shr        ebx, cl
0046f4c4  lea        esi, [esi + eax + 4]
0046f4c8  not        ebx
0046f4ca  and        dword ptr [eax + edx*4 + 0x44], ebx
0046f4ce  dec        byte ptr [esi]
0046f4d0  jne        0x46f4f5

// block 0046f4d2
0046f4d2  mov        ecx, dword ptr [ebp + 8]
0046f4d5  and        dword ptr [ecx], ebx
0046f4d7  jmp        0x46f4f5

// block 0046f4d9
0046f4d9  lea        ecx, [esi - 0x20]
0046f4dc  shr        ebx, cl
0046f4de  lea        ecx, [esi + eax + 4]
0046f4e2  not        ebx
0046f4e4  and        dword ptr [eax + edx*4 + 0xc4], ebx
0046f4eb  dec        byte ptr [ecx]
0046f4ed  jne        0x46f4f5

// block 0046f4ef
0046f4ef  mov        ecx, dword ptr [ebp + 8]
0046f4f2  and        dword ptr [ecx + 4], ebx

// block 0046f4f5
0046f4f5  mov        ebx, dword ptr [ebp + 0xc]

// block 0046f4f8
0046f4f8  mov        ecx, dword ptr [edi + 8]
0046f4fb  mov        esi, dword ptr [edi + 4]
0046f4fe  mov        dword ptr [ecx + 4], esi
0046f501  mov        esi, dword ptr [edi + 8]
0046f504  mov        ecx, dword ptr [edi + 4]
0046f507  mov        dword ptr [ecx + 8], esi
0046f50a  mov        esi, dword ptr [ebp + 0x10]
0046f50d  add        esi, dword ptr [ebp - 4]
0046f510  mov        dword ptr [ebp + 0x10], esi
0046f513  sar        esi, 4
0046f516  dec        esi
0046f517  cmp        esi, 0x3f
0046f51a  jbe        0x46f51f

// block 0046f51c
0046f51c  push       0x3f
0046f51e  pop        esi

// block 0046f51f
0046f51f  mov        ecx, dword ptr [ebp - 0xc]
0046f522  lea        ecx, [ecx + esi*8]
0046f525  mov        edi, dword ptr [ecx + 4]
0046f528  mov        dword ptr [ebx + 8], ecx
0046f52b  mov        dword ptr [ebx + 4], edi
0046f52e  mov        dword ptr [ecx + 4], ebx
0046f531  mov        ecx, dword ptr [ebx + 4]
0046f534  mov        dword ptr [ecx + 8], ebx
0046f537  mov        ecx, dword ptr [ebx + 4]
0046f53a  cmp        ecx, dword ptr [ebx + 8]
0046f53d  jne        0x46f596

// block 0046f53f
0046f53f  mov        cl, byte ptr [esi + eax + 4]
0046f543  mov        byte ptr [ebp + 0xf], cl
0046f546  inc        cl
0046f548  mov        byte ptr [esi + eax + 4], cl
0046f54c  cmp        esi, 0x20
0046f54f  jae        0x46f56d

// block 0046f551
0046f551  cmp        byte ptr [ebp + 0xf], 0
0046f555  jne        0x46f565

// block 0046f557
0046f557  mov        ecx, esi
0046f559  mov        edi, 0x80000000
0046f55e  shr        edi, cl
0046f560  mov        ecx, dword ptr [ebp + 8]
0046f563  or         dword ptr [ecx], edi

// block 0046f565
0046f565  lea        eax, [eax + edx*4 + 0x44]
0046f569  mov        ecx, esi
0046f56b  jmp        0x46f58d

// block 0046f56d
0046f56d  cmp        byte ptr [ebp + 0xf], 0
0046f571  jne        0x46f583

// block 0046f573
0046f573  lea        ecx, [esi - 0x20]
0046f576  mov        edi, 0x80000000
0046f57b  shr        edi, cl
0046f57d  mov        ecx, dword ptr [ebp + 8]
0046f580  or         dword ptr [ecx + 4], edi

// block 0046f583
0046f583  lea        eax, [eax + edx*4 + 0xc4]
0046f58a  lea        ecx, [esi - 0x20]

// block 0046f58d
0046f58d  mov        edx, 0x80000000
0046f592  shr        edx, cl
0046f594  or         dword ptr [eax], edx

// block 0046f596
0046f596  mov        eax, dword ptr [ebp + 0x10]
0046f599  mov        dword ptr [ebx], eax
0046f59b  mov        dword ptr [eax + ebx - 4], eax

// block 0046f59f
0046f59f  xor        eax, eax
0046f5a1  inc        eax

// block 0046f5a2
0046f5a2  pop        edi
0046f5a3  pop        esi
0046f5a4  pop        ebx
0046f5a5  leave      
0046f5a6  ret        
