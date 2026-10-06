
// block 004841e9
004841e9  mov        edi, edi
004841eb  push       ebp
004841ec  mov        ebp, esp
004841ee  sub        esp, 0x10
004841f1  push       ebx
004841f2  push       esi
004841f3  mov        esi, dword ptr [ebp + 8]
004841f6  push       edi
004841f7  xor        edi, edi
004841f9  mov        dword ptr [ebp - 4], edi
004841fc  mov        dword ptr [ebp - 0x10], esi
004841ff  mov        dword ptr [ebp - 0xc], edi
00484202  cmp        dword ptr [esi + 0x18], edi
00484205  jne        0x48421c

// block 00484207
00484207  cmp        dword ptr [esi + 0x1c], edi
0048420a  jne        0x48421c

// block 0048420c
0048420c  mov        dword ptr [ebp - 4], edi
0048420f  mov        dword ptr [ebp - 8], edi
00484212  mov        ebx, 0x4adf68
00484217  jmp        0x484452

// block 0048421c
0048421c  push       0x30
0048421e  push       1
00484220  call       0x477813

// block 00484225
00484225  mov        ebx, eax
00484227  pop        ecx
00484228  pop        ecx
00484229  cmp        ebx, edi
0048422b  jne        0x484235

// block 0048422d
0048422d  xor        eax, eax
0048422f  inc        eax
00484230  jmp        0x4844aa

// block 00484235
00484235  push       4
00484237  call       0x4777ce

// block 0048423c
0048423c  pop        ecx
0048423d  mov        dword ptr [ebp - 8], eax
00484240  cmp        eax, edi
00484242  jne        0x48424d

// block 00484244
00484244  push       ebx
00484245  call       0x46c941

// block 0048424a
0048424a  pop        ecx
0048424b  jmp        0x48422d

// block 0048424d
0048424d  mov        dword ptr [eax], edi
0048424f  cmp        dword ptr [esi + 0x18], edi
00484252  je         0x484411

// block 00484258
00484258  push       4
0048425a  call       0x4777ce

// block 0048425f
0048425f  pop        ecx
00484260  mov        dword ptr [ebp - 4], eax
00484263  cmp        eax, edi
00484265  jne        0x484278

// block 00484267
00484267  push       ebx
00484268  call       0x46c941

// block 0048426d
0048426d  push       dword ptr [ebp - 8]
00484270  call       0x46c941

// block 00484275
00484275  pop        ecx
00484276  jmp        0x48424a

// block 00484278
00484278  mov        dword ptr [eax], edi
0048427a  movzx      esi, word ptr [esi + 0x38]
0048427e  lea        eax, [ebx + 0xc]
00484281  push       eax
00484282  push       0x15
00484284  push       esi
00484285  lea        eax, [ebp - 0x10]
00484288  push       1
0048428a  push       eax
0048428b  call       0x47a12b

// block 00484290
00484290  mov        edi, eax
00484292  lea        eax, [ebx + 0x10]
00484295  push       eax
00484296  push       0x14
00484298  push       esi
00484299  lea        eax, [ebp - 0x10]
0048429c  push       1
0048429e  push       eax
0048429f  call       0x47a12b

// block 004842a4
004842a4  or         edi, eax
004842a6  lea        eax, [ebx + 0x14]
004842a9  push       eax
004842aa  push       0x16
004842ac  push       esi
004842ad  lea        eax, [ebp - 0x10]
004842b0  push       1
004842b2  push       eax
004842b3  call       0x47a12b

// block 004842b8
004842b8  or         edi, eax
004842ba  lea        eax, [ebx + 0x18]
004842bd  push       eax
004842be  push       0x17
004842c0  push       esi
004842c1  lea        eax, [ebp - 0x10]
004842c4  push       1
004842c6  push       eax
004842c7  call       0x47a12b

// block 004842cc
004842cc  add        esp, 0x50
004842cf  or         edi, eax
004842d1  lea        eax, [ebx + 0x1c]
004842d4  push       eax
004842d5  push       0x18
004842d7  push       esi
004842d8  lea        eax, [ebp - 0x10]
004842db  push       1
004842dd  push       eax
004842de  call       0x47a12b

// block 004842e3
004842e3  or         edi, eax
004842e5  lea        eax, [ebx + 0x20]
004842e8  push       eax
004842e9  push       0x50
004842eb  push       esi
004842ec  lea        eax, [ebp - 0x10]
004842ef  push       1
004842f1  push       eax
004842f2  call       0x47a12b

// block 004842f7
004842f7  or         edi, eax
004842f9  lea        eax, [ebx + 0x24]
004842fc  push       eax
004842fd  push       0x51
004842ff  push       esi
00484300  lea        eax, [ebp - 0x10]
00484303  push       1
00484305  push       eax
00484306  call       0x47a12b

// block 0048430b
0048430b  or         edi, eax
0048430d  lea        eax, [ebx + 0x28]
00484310  push       eax
00484311  push       0x1a
00484313  push       esi
00484314  lea        eax, [ebp - 0x10]
00484317  push       0
00484319  push       eax
0048431a  call       0x47a12b

// block 0048431f
0048431f  add        esp, 0x50
00484322  or         edi, eax
00484324  lea        eax, [ebx + 0x29]
00484327  push       eax
00484328  push       0x19
0048432a  push       esi
0048432b  push       0
0048432d  lea        eax, [ebp - 0x10]
00484330  push       eax
00484331  call       0x47a12b

// block 00484336
00484336  or         edi, eax
00484338  lea        eax, [ebx + 0x2a]
0048433b  push       eax
0048433c  push       0x54
0048433e  push       esi
0048433f  lea        eax, [ebp - 0x10]
00484342  push       0
00484344  push       eax
00484345  call       0x47a12b

// block 0048434a
0048434a  or         edi, eax
0048434c  lea        eax, [ebx + 0x2b]
0048434f  push       eax
00484350  push       0x55
00484352  push       esi
00484353  lea        eax, [ebp - 0x10]
00484356  push       0
00484358  push       eax
00484359  call       0x47a12b

// block 0048435e
0048435e  or         edi, eax
00484360  lea        eax, [ebx + 0x2c]
00484363  push       eax
00484364  push       0x56
00484366  push       esi
00484367  lea        eax, [ebp - 0x10]
0048436a  push       0
0048436c  push       eax
0048436d  call       0x47a12b

// block 00484372
00484372  add        esp, 0x50
00484375  or         edi, eax
00484377  lea        eax, [ebx + 0x2d]
0048437a  push       eax
0048437b  push       0x57
0048437d  push       esi
0048437e  lea        eax, [ebp - 0x10]
00484381  push       0
00484383  push       eax
00484384  call       0x47a12b

// block 00484389
00484389  or         edi, eax
0048438b  lea        eax, [ebx + 0x2e]
0048438e  push       eax
0048438f  push       0x52
00484391  push       esi
00484392  lea        eax, [ebp - 0x10]
00484395  push       0
00484397  push       eax
00484398  call       0x47a12b

// block 0048439d
0048439d  or         edi, eax
0048439f  lea        eax, [ebx + 0x2f]
004843a2  push       eax
004843a3  push       0x53
004843a5  push       esi
004843a6  lea        eax, [ebp - 0x10]
004843a9  push       0
004843ab  push       eax
004843ac  call       0x47a12b

// block 004843b1
004843b1  add        esp, 0x3c
004843b4  or         eax, edi
004843b6  je         0x4843dc

// block 004843b8
004843b8  push       ebx
004843b9  call       0x48415b

// block 004843be
004843be  push       ebx
004843bf  call       0x46c941

// block 004843c4
004843c4  push       dword ptr [ebp - 8]
004843c7  call       0x46c941

// block 004843cc
004843cc  push       dword ptr [ebp - 4]
004843cf  call       0x46c941

// block 004843d4
004843d4  add        esp, 0x10
004843d7  jmp        0x48422d

// block 004843dc
004843dc  mov        eax, dword ptr [ebx + 0x1c]
004843df  jmp        0x4843f3

// block 004843e1
004843e1  mov        cl, byte ptr [eax]
004843e3  cmp        cl, 0x30
004843e6  jl         0x4843fa

// block 004843e8
004843e8  cmp        cl, 0x39
004843eb  jg         0x4843fa

// block 004843ed
004843ed  sub        cl, 0x30
004843f0  mov        byte ptr [eax], cl

// block 004843f2
004843f2  inc        eax

// block 004843f3
004843f3  cmp        byte ptr [eax], 0
004843f6  jne        0x4843e1

// block 004843f8
004843f8  jmp        0x48441d

// block 004843fa
004843fa  cmp        cl, 0x3b
004843fd  jne        0x4843f2

// block 004843ff
004843ff  mov        esi, eax

// block 00484401
00484401  lea        edi, [esi + 1]
00484404  mov        cl, byte ptr [edi]
00484406  mov        byte ptr [esi], cl
00484408  mov        esi, edi
0048440a  cmp        byte ptr [esi], 0
0048440d  jne        0x484401

// block 0048440f
0048440f  jmp        0x4843f3

// block 00484411
00484411  push       0xc
00484413  pop        ecx
00484414  mov        esi, 0x4adf68
00484419  mov        edi, ebx

// block 0048441b
0048441b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0048441d
0048441d  mov        eax, dword ptr [ebp + 8]
00484420  mov        esi, dword ptr [ebp + 8]
00484423  add        eax, 0xbc
00484428  mov        ecx, dword ptr [eax]
0048442a  mov        ecx, dword ptr [ecx]
0048442c  mov        dword ptr [ebx], ecx
0048442e  mov        ecx, dword ptr [eax]
00484430  mov        ecx, dword ptr [ecx + 4]
00484433  mov        dword ptr [ebx + 4], ecx
00484436  mov        eax, dword ptr [eax]
00484438  mov        eax, dword ptr [eax + 8]
0048443b  mov        ecx, dword ptr [ebp - 8]
0048443e  mov        dword ptr [ebx + 8], eax
00484441  xor        eax, eax
00484443  inc        eax
00484444  xor        edi, edi
00484446  mov        dword ptr [ecx], eax
00484448  cmp        dword ptr [ebp - 4], edi
0048444b  je         0x484452

// block 0048444d
0048444d  mov        ecx, dword ptr [ebp - 4]
00484450  mov        dword ptr [ecx], eax

// block 00484452
00484452  mov        eax, dword ptr [esi + 0xb8]
00484458  cmp        eax, edi
0048445a  je         0x484463

// block 0048445c
0048445c  push       eax
0048445d  call       dword ptr [0x49815c]

// block 00484463
00484463  mov        eax, dword ptr [esi + 0xb0]
00484469  cmp        eax, edi
0048446b  je         0x484490

// block 0048446d
0048446d  push       eax
0048446e  call       dword ptr [0x49815c]

// block 00484474
00484474  test       eax, eax
00484476  jne        0x484490

// block 00484478
00484478  push       dword ptr [esi + 0xbc]
0048447e  call       0x46c941

// block 00484483
00484483  push       dword ptr [esi + 0xb0]
00484489  call       0x46c941

// block 0048448e
0048448e  pop        ecx
0048448f  pop        ecx

// block 00484490
00484490  mov        eax, dword ptr [ebp - 4]
00484493  mov        dword ptr [esi + 0xb8], eax
00484499  mov        eax, dword ptr [ebp - 8]
0048449c  mov        dword ptr [esi + 0xb0], eax
004844a2  mov        dword ptr [esi + 0xbc], ebx
004844a8  xor        eax, eax

// block 004844aa
004844aa  pop        edi
004844ab  pop        esi
004844ac  pop        ebx
004844ad  leave      
004844ae  ret        
