
// block 004466a0
004466a0  push       ebp
004466a1  mov        ebp, dword ptr [esp + 8]
004466a5  mov        eax, dword ptr [ebp + 0x24]
004466a8  cmp        eax, 5
004466ab  ja         0x446bfe

// block 004466b1
004466b1  push       ebx
004466b2  push       esi
004466b3  push       edi
004466b4  jmp        dword ptr [eax*4 + 0x446c08]

// block 004466bb
004466bb  mov        ecx, dword ptr [0x4ce8b4]
004466c1  mov        eax, 0x51eb851f
004466c6  imul       ecx
004466c8  sar        edx, 3
004466cb  mov        esi, edx
004466cd  shr        esi, 0x1f
004466d0  add        esi, edx
004466d2  mov        eax, esi
004466d4  imul       eax, eax, 0x19
004466d7  sub        ecx, eax
004466d9  lea        edx, [ebp + 0x28]
004466dc  mov        dword ptr [ebp + 0x30], 0x19
004466e3  call       0x40f790

// block 004466e8
004466e8  lea        edx, [ebp + 0x1d8]
004466ee  mov        ecx, esi
004466f0  mov        dword ptr [ebp + 0x1e0], 3
004466fa  call       0x40f790

// block 004466ff
004466ff  xor        ebx, ebx
00446701  mov        dword ptr [ebp + 0x2a8], 1
0044670b  mov        dword ptr [0x4ce8b4], ebx
00446711  cmp        dword ptr [ebp + 0x66c], ebx
00446717  jne        0x446742

// block 00446719
00446719  mov        edx, dword ptr [0x4b43b8]
0044671f  mov        eax, dword ptr [edx + 0x18fb4]
00446725  push       ebx
00446726  push       0x14
00446728  lea        ecx, [esp + 0x1c]
0044672c  push       ecx
0044672d  push       eax
0044672e  lea        eax, [ebx + 0x17]
00446731  xor        ecx, ecx
00446733  call       0x4615a0

// block 00446738
00446738  mov        ecx, dword ptr [esp + 0x14]
0044673c  mov        dword ptr [ebp + 0x66c], ecx

// block 00446742
00446742  mov        esi, 0x71
00446747  mov        edi, ebp
00446749  call       0x43efa0

// block 0044674e
0044674e  lea        ecx, [esi - 0x70]
00446751  mov        eax, ebp
00446753  call       0x43ef40

// block 00446758
00446758  push       0x190
0044675d  lea        edx, [ebp + 0x5a80]
00446763  push       ebx
00446764  push       edx
00446765  call       0x477420

// block 0044676a
0044676a  and        dword ptr [ebp + 0x5c18], 0xfffffff3
00446771  add        esp, 0xc
00446774  push       ebp
00446775  lea        eax, [ebp + 0x5c1c]
0044677b  mov        edi, 0x446690
00446780  mov        dword ptr [ebp + 0x5a74], ebx
00446786  call       0x464cb0

// block 0044678b
0044678b  mov        eax, dword ptr [ebp + 0x454]
00446791  mov        edx, dword ptr [0x4ce8cc]
00446797  push       eax
00446798  call       0x461920

// block 0044679d
0044679d  test       eax, eax
0044679f  jne        0x4467ba

// block 004467a1
004467a1  lea        esi, [eax + 0x63]
004467a4  mov        edi, ebp
004467a6  call       0x43efa0

// block 004467ab
004467ab  mov        ecx, dword ptr [ebp + 0x454]
004467b1  push       ecx
004467b2  lea        ebx, [esi - 0x60]
004467b5  call       0x4619e0

// block 004467ba
004467ba  cmp        dword ptr [ebp + 0x2b8], 6
004467c1  jle        0x446bfb

// block 004467c7
004467c7  mov        ecx, 2
004467cc  mov        eax, ebp
004467ce  call       0x43ef40

// block 004467d3
004467d3  pop        edi
004467d4  pop        esi
004467d5  pop        ebx
004467d6  mov        eax, 1
004467db  pop        ebp
004467dc  ret        4

// block 004467df
004467df  mov        edx, dword ptr [ebp + 0x28]
004467e2  lea        esi, [ebp + 0x28]
004467e5  mov        dword ptr [esi + 4], edx
004467e8  mov        eax, dword ptr [ebp + 0x1d8]
004467ee  lea        edi, [ebp + 0x1d8]
004467f4  mov        dword ptr [edi + 4], eax
004467f7  mov        al, 0x10
004467f9  test       byte ptr [0x4d48c4], al
004467ff  jne        0x446809

// block 00446801
00446801  test       byte ptr [0x4d48c0], al
00446807  je         0x446812

// block 00446809
00446809  push       -1
0044680b  mov        eax, esi
0044680d  call       0x464970

// block 00446812
00446812  mov        ebx, 0x20
00446817  test       byte ptr [0x4d48c4], bl
0044681d  jne        0x446827

// block 0044681f
0044681f  test       byte ptr [0x4d48c0], bl
00446825  je         0x446830

// block 00446827
00446827  push       1
00446829  mov        eax, esi
0044682b  call       0x464970

// block 00446830
00446830  mov        al, 0x40
00446832  test       byte ptr [0x4d48c4], al
00446838  jne        0x446842

// block 0044683a
0044683a  test       byte ptr [0x4d48c0], al
00446840  je         0x44684b

// block 00446842
00446842  push       -1
00446844  mov        eax, edi
00446846  call       0x464970

// block 0044684b
0044684b  mov        al, 0x80
0044684d  test       byte ptr [0x4d48c4], al
00446853  jne        0x44685d

// block 00446855
00446855  test       byte ptr [0x4d48c0], al
0044685b  je         0x446866

// block 0044685d
0044685d  push       1
0044685f  mov        eax, edi
00446861  call       0x464970

// block 00446866
00446866  mov        ecx, dword ptr [edi + 4]
00446869  cmp        ecx, dword ptr [edi]
0044686b  je         0x446877

// block 0044686d
0044686d  mov        edx, 0xa
00446872  call       0x453d90

// block 00446877
00446877  mov        edx, dword ptr [esi + 4]
0044687a  cmp        edx, dword ptr [esi]
0044687c  je         0x446888

// block 0044687e
0044687e  mov        edx, 0xa
00446883  call       0x453d90

// block 00446888
00446888  mov        eax, dword ptr [0x4d48c4]
0044688d  test       eax, 0x102
00446892  je         0x4468bd

// block 00446894
00446894  mov        ecx, 5
00446899  mov        eax, ebp
0044689b  call       0x43ef40

// block 004468a0
004468a0  mov        edx, 9
004468a5  call       0x453d90

// block 004468aa
004468aa  or         dword ptr [ebp + 0x5c18], 4
004468b1  pop        edi
004468b2  pop        esi
004468b3  pop        ebx
004468b4  mov        eax, 1
004468b9  pop        ebp
004468ba  ret        4

// block 004468bd
004468bd  test       eax, 0x80001
004468c2  je         0x446bfb

// block 004468c8
004468c8  mov        eax, dword ptr [edi]
004468ca  imul       eax, eax, 0x19
004468cd  add        eax, dword ptr [esi]
004468cf  cmp        dword ptr [ebp + eax*4 + 0x5a80], 0
004468d7  je         0x446bfb

// block 004468dd
004468dd  mov        ecx, 4
004468e2  mov        eax, ebp
004468e4  call       0x43ef40

// block 004468e9
004468e9  mov        ecx, dword ptr [edi]
004468eb  imul       ecx, ecx, 0x19
004468ee  add        ecx, dword ptr [esi]
004468f0  mov        eax, esi
004468f2  mov        dword ptr [ebp + 0x5a78], ecx
004468f8  call       0x464900

// block 004468fd
004468fd  mov        edx, 7
00446902  call       0x453d90

// block 00446907
00446907  mov        dword ptr [ebp + 0x30], 7
0044690e  mov        eax, dword ptr [esi + 8]
00446911  test       eax, eax
00446913  je         0x446922

// block 00446915
00446915  jg         0x44691c

// block 00446917
00446917  dec        eax
00446918  mov        dword ptr [esi], eax
0044691a  jmp        0x446928

// block 0044691c
0044691c  xor        eax, eax
0044691e  mov        dword ptr [esi], eax
00446920  jmp        0x446928

// block 00446922
00446922  mov        dword ptr [esi], 0

// block 00446928
00446928  xor        ecx, ecx
0044692a  xor        eax, eax
0044692c  lea        esp, [esp]

// block 00446930
00446930  mov        edx, dword ptr [ebp + 0x5a78]
00446936  mov        edx, dword ptr [ebp + edx*4 + 0x5a80]
0044693d  cmp        dword ptr [edx + eax + 0xdc], 0
00446945  jne        0x44695a

// block 00446947
00446947  mov        edx, dword ptr [esi + 0xd4]
0044694d  mov        dword ptr [esi + edx*4 + 0x90], ecx
00446954  inc        dword ptr [esi + 0xd4]

// block 0044695a
0044695a  add        eax, 0x24
0044695d  inc        ecx
0044695e  cmp        eax, 0xfc
00446963  jl         0x446930

// block 00446965
00446965  push       -1
00446967  mov        eax, esi
00446969  call       0x464970

// block 0044696e
0044696e  push       1
00446970  mov        eax, esi
00446972  call       0x464970

// block 00446977
00446977  pop        edi
00446978  pop        esi
00446979  pop        ebx
0044697a  mov        eax, 1
0044697f  pop        ebp
00446980  ret        4

// block 00446983
00446983  cmp        dword ptr [ebp + 0x2b8], 0xf
0044698a  jl         0x446bfb

// block 00446990
00446990  mov        eax, dword ptr [ebp + 0x28]
00446993  lea        esi, [ebp + 0x28]
00446996  mov        dword ptr [esi + 4], eax
00446999  mov        al, 0x10
0044699b  test       byte ptr [0x4d48c4], al
004469a1  jne        0x4469ab

// block 004469a3
004469a3  test       byte ptr [0x4d48c0], al
004469a9  je         0x4469b4

// block 004469ab
004469ab  push       -1
004469ad  mov        eax, esi
004469af  call       0x464970

// block 004469b4
004469b4  mov        ebx, 0x20
004469b9  test       byte ptr [0x4d48c4], bl
004469bf  jne        0x4469c9

// block 004469c1
004469c1  test       byte ptr [0x4d48c0], bl
004469c7  je         0x4469d2

// block 004469c9
004469c9  push       1
004469cb  mov        eax, esi
004469cd  call       0x464970

// block 004469d2
004469d2  mov        ecx, dword ptr [esi + 4]
004469d5  cmp        ecx, dword ptr [esi]
004469d7  je         0x4469e3

// block 004469d9
004469d9  mov        edx, 0xa
004469de  call       0x453d90

// block 004469e3
004469e3  mov        eax, dword ptr [0x4d48c4]
004469e8  test       eax, 0x102
004469ed  je         0x446a29

// block 004469ef
004469ef  mov        eax, esi
004469f1  call       0x464940

// block 004469f6
004469f6  mov        ecx, 2
004469fb  mov        eax, ebp
004469fd  mov        dword ptr [ebp + 0x30], 0x19
00446a04  mov        dword ptr [ebp + 0xfc], 0
00446a0e  call       0x43ef40

// block 00446a13
00446a13  mov        edx, 9
00446a18  call       0x453d90

// block 00446a1d
00446a1d  pop        edi
00446a1e  pop        esi
00446a1f  pop        ebx
00446a20  mov        eax, 1
00446a25  pop        ebp
00446a26  ret        4

// block 00446a29
00446a29  test       eax, 0x80001
00446a2e  je         0x446bfb

// block 00446a34
00446a34  mov        edx, dword ptr [esi]
00446a36  mov        ecx, 3
00446a3b  mov        eax, ebp
00446a3d  mov        dword ptr [ebp + 0x5a7c], edx
00446a43  call       0x43ef40

// block 00446a48
00446a48  or         dword ptr [ebp + 0x5c18], 4
00446a4f  pop        edi
00446a50  pop        esi
00446a51  pop        ebx
00446a52  mov        eax, 1
00446a57  pop        ebp
00446a58  ret        4

// block 00446a5b
00446a5b  mov        esi, 2
00446a60  mov        ebx, 0x20
00446a65  cmp        dword ptr [ebp + 0x2b8], esi
00446a6b  jne        0x446a98

// block 00446a6d
00446a6d  push       0x3b
00446a6f  push       0
00446a71  push       0
00446a73  push       0
00446a75  push       ebx
00446a76  push       5
00446a78  call       0x4529a0

// block 00446a7d
00446a7d  fld        dword ptr [0x4a4260]
00446a83  sub        esp, 8
00446a86  fstp       dword ptr [esp + 4]
00446a8a  fld        dword ptr [0x4a3ec0]
00446a90  fstp       dword ptr [esp]
00446a93  call       0x411b80

// block 00446a98
00446a98  cmp        dword ptr [ebp + 0x2b8], ebx
00446a9e  jl         0x446bfb

// block 00446aa4
00446aa4  test       byte ptr [ebp + 0x5c18], 8
00446aab  je         0x446bfb

// block 00446ab1
00446ab1  mov        edx, esi
00446ab3  mov        eax, ebp
00446ab5  call       0x43eee0

// block 00446aba
00446aba  fld        dword ptr [0x4a4250]
00446ac0  mov        eax, dword ptr [ebp + 0x5a7c]
00446ac6  inc        eax
00446ac7  mov        ecx, eax
00446ac9  shl        ecx, 6
00446acc  add        ecx, 0x4aebf0
00446ad2  push       ecx
00446ad3  fstp       dword ptr [esp]
00446ad6  mov        dword ptr [0x4b452c], ecx
00446adc  mov        dword ptr [0x4b0cb0], eax
00446ae1  mov        dword ptr [0x4b0cb4], eax
00446ae6  call       0x430270

// block 00446aeb
00446aeb  mov        dword ptr [0x4cee40], 0xd
00446af5  mov        edx, dword ptr [ebp + 0x5a78]
00446afb  mov        eax, dword ptr [ebp + edx*4 + 0x5a80]
00446b02  add        eax, 0x1e0
00446b07  mov        edx, 0x4b43e8
00446b0c  lea        esp, [esp]

// block 00446b10
00446b10  mov        cl, byte ptr [eax]
00446b12  mov        byte ptr [edx], cl
00446b14  inc        eax
00446b15  inc        edx
00446b16  test       cl, cl
00446b18  jne        0x446b10

// block 00446b1a
00446b1a  mov        eax, dword ptr [ebp + 0x5a78]
00446b20  mov        ecx, dword ptr [ebp + eax*4 + 0x5a80]
00446b27  mov        eax, dword ptr [ecx + 0x1c]
00446b2a  mov        edx, dword ptr [eax + 0x5c]
00446b2d  mov        dword ptr [0x4b0c90], edx
00446b33  mov        ecx, dword ptr [eax + 0x60]
00446b36  mov        dword ptr [0x4b0c94], ecx
00446b3c  mov        edx, dword ptr [eax + 0x64]
00446b3f  pop        edi
00446b40  mov        dword ptr [0x4ce8b0], esi
00446b46  pop        esi
00446b47  mov        dword ptr [0x4b0ca8], edx
00446b4d  mov        eax, dword ptr [ebp + 0x5a78]
00446b53  pop        ebx
00446b54  mov        dword ptr [0x4ce8b4], eax
00446b59  mov        eax, 1
00446b5e  pop        ebp
00446b5f  ret        4

// block 00446b62
00446b62  cmp        dword ptr [ebp + 0x2b8], 6
00446b69  jl         0x446bfb

// block 00446b6f
00446b6f  test       byte ptr [ebp + 0x5c18], 8
00446b76  je         0x446bfb

// block 00446b7c
00446b7c  lea        esi, [ebp + 0x5a80]
00446b82  mov        ebx, 0x64

// block 00446b87
00446b87  mov        edi, dword ptr [esi]
00446b89  test       edi, edi
00446b8b  je         0x446b9c

// block 00446b8d
00446b8d  push       edi
00446b8e  call       0x43b450

// block 00446b93
00446b93  push       edi
00446b94  call       0x46ca4f

// block 00446b99
00446b99  add        esp, 4

// block 00446b9c
00446b9c  add        esi, 4
00446b9f  sub        ebx, 1
00446ba2  jne        0x446b87

// block 00446ba4
00446ba4  push       0x190
00446ba9  lea        eax, [ebp + 0x5a80]
00446baf  push       ebx
00446bb0  push       eax
00446bb1  call       0x477420

// block 00446bb6
00446bb6  mov        ecx, dword ptr [ebp + 0x48c]
00446bbc  add        esp, 0xc
00446bbf  push       ecx
00446bc0  mov        ebx, 1
00446bc5  call       0x461970

// block 00446bca
00446bca  mov        dword ptr [ebp + 0x48c], 0
00446bd4  mov        edx, dword ptr [ebp + 0x66c]
00446bda  push       edx
00446bdb  call       0x461970

// block 00446be0
00446be0  mov        edx, ebx
00446be2  mov        eax, ebp
00446be4  mov        dword ptr [ebp + 0x66c], 0
00446bee  call       0x43eee0

// block 00446bf3
00446bf3  lea        eax, [ebp + 0x28]
00446bf6  call       0x464940

// block 00446bfb
00446bfb  pop        edi
00446bfc  pop        esi
00446bfd  pop        ebx

// block 00446bfe
00446bfe  mov        eax, 1
00446c03  pop        ebp
00446c04  ret        4
