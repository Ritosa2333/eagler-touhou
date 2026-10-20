
// block 004725d8
004725d8  mov        edi, edi
004725da  push       ebp
004725db  mov        ebp, esp
004725dd  push       ecx
004725de  push       ebx
004725df  xor        ebx, ebx
004725e1  cmp        dword ptr [ebp + 0x14], ebx
004725e4  jne        0x472606

// block 004725e6
004725e6  call       0x46e96f

// block 004725eb
004725eb  push       ebx
004725ec  push       ebx
004725ed  push       ebx
004725ee  push       ebx
004725ef  push       ebx
004725f0  mov        dword ptr [eax], 0x16
004725f6  call       0x470f2d

// block 004725fb
004725fb  add        esp, 0x14
004725fe  or         eax, 0xffffffff
00472601  jmp        0x4726dc

// block 00472606
00472606  push       esi
00472607  mov        esi, dword ptr [ebp + 8]
0047260a  push       edi
0047260b  cmp        dword ptr [ebp + 0x10], ebx
0047260e  jne        0x472620

// block 00472610
00472610  cmp        esi, ebx
00472612  jne        0x472624

// block 00472614
00472614  cmp        dword ptr [ebp + 0xc], ebx
00472617  jne        0x47262b

// block 00472619
00472619  xor        eax, eax
0047261b  jmp        0x4726da

// block 00472620
00472620  cmp        esi, ebx
00472622  je         0x47262b

// block 00472624
00472624  mov        edi, dword ptr [ebp + 0xc]
00472627  cmp        edi, ebx
00472629  ja         0x47263b

// block 0047262b
0047262b  call       0x46e96f

// block 00472630
00472630  mov        dword ptr [eax], 0x16
00472636  jmp        0x4726ca

// block 0047263b
0047263b  call       0x46e96f

// block 00472640
00472640  push       dword ptr [ebp + 0x1c]
00472643  push       dword ptr [ebp + 0x18]
00472646  push       dword ptr [ebp + 0x14]
00472649  cmp        edi, dword ptr [ebp + 0x10]
0047264c  jbe        0x47267b

// block 0047264e
0047264e  mov        edi, dword ptr [eax]
00472650  mov        eax, dword ptr [ebp + 0x10]
00472653  inc        eax
00472654  push       eax
00472655  push       esi
00472656  push       0x47c9cb
0047265b  call       0x472414

// block 00472660
00472660  add        esp, 0x18
00472663  cmp        eax, -2
00472666  jne        0x4726b4

// block 00472668
00472668  call       0x46e96f

// block 0047266d
0047266d  cmp        dword ptr [eax], 0x22
00472670  jne        0x4726d7

// block 00472672
00472672  call       0x46e96f

// block 00472677
00472677  mov        dword ptr [eax], edi
00472679  jmp        0x4726d7

// block 0047267b
0047267b  mov        eax, dword ptr [eax]
0047267d  push       edi
0047267e  push       esi
0047267f  push       0x47c9cb
00472684  mov        dword ptr [ebp - 4], eax
00472687  call       0x472414

// block 0047268c
0047268c  add        esp, 0x18
0047268f  mov        byte ptr [esi + edi - 1], bl
00472693  cmp        eax, -2
00472696  jne        0x4726b4

// block 00472698
00472698  cmp        dword ptr [ebp + 0x10], -1
0047269c  jne        0x4726b8

// block 0047269e
0047269e  call       0x46e96f

// block 004726a3
004726a3  cmp        dword ptr [eax], 0x22
004726a6  jne        0x4726d7

// block 004726a8
004726a8  call       0x46e96f

// block 004726ad
004726ad  mov        ecx, dword ptr [ebp - 4]
004726b0  mov        dword ptr [eax], ecx
004726b2  jmp        0x4726d7

// block 004726b4
004726b4  cmp        eax, ebx
004726b6  jge        0x4726da

// block 004726b8
004726b8  mov        byte ptr [esi], bl
004726ba  cmp        eax, -2
004726bd  jne        0x4726d7

// block 004726bf
004726bf  call       0x46e96f

// block 004726c4
004726c4  mov        dword ptr [eax], 0x22

// block 004726ca
004726ca  push       ebx
004726cb  push       ebx
004726cc  push       ebx
004726cd  push       ebx
004726ce  push       ebx
004726cf  call       0x470f2d

// block 004726d4
004726d4  add        esp, 0x14

// block 004726d7
004726d7  or         eax, 0xffffffff

// block 004726da
004726da  pop        edi
004726db  pop        esi

// block 004726dc
004726dc  pop        ebx
004726dd  leave      
004726de  ret        
