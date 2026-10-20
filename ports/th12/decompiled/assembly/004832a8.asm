
// block 004832a8
004832a8  mov        edi, edi
004832aa  push       ebp
004832ab  mov        ebp, esp
004832ad  sub        esp, 0x14
004832b0  mov        eax, dword ptr [0x4ad138]
004832b5  xor        eax, ebp
004832b7  mov        dword ptr [ebp - 4], eax
004832ba  push       ebx
004832bb  push       esi
004832bc  xor        ebx, ebx
004832be  push       edi
004832bf  mov        esi, ecx
004832c1  cmp        dword ptr [0x4b4394], ebx
004832c7  jne        0x483301

// block 004832c9
004832c9  push       ebx
004832ca  push       ebx
004832cb  xor        edi, edi
004832cd  inc        edi
004832ce  push       edi
004832cf  push       0x49e1c8
004832d4  push       0x100
004832d9  push       ebx
004832da  call       dword ptr [0x498050]

// block 004832e0
004832e0  test       eax, eax
004832e2  je         0x4832ec

// block 004832e4
004832e4  mov        dword ptr [0x4b4394], edi
004832ea  jmp        0x483301

// block 004832ec
004832ec  call       dword ptr [0x4980e4]

// block 004832f2
004832f2  cmp        eax, 0x78
004832f5  jne        0x483301

// block 004832f7
004832f7  mov        dword ptr [0x4b4394], 2

// block 00483301
00483301  cmp        dword ptr [ebp + 0x14], ebx
00483304  jle        0x483328

// block 00483306
00483306  mov        ecx, dword ptr [ebp + 0x14]
00483309  mov        eax, dword ptr [ebp + 0x10]

// block 0048330c
0048330c  dec        ecx
0048330d  cmp        byte ptr [eax], bl
0048330f  je         0x483319

// block 00483311
00483311  inc        eax
00483312  cmp        ecx, ebx
00483314  jne        0x48330c

// block 00483316
00483316  or         ecx, 0xffffffff

// block 00483319
00483319  mov        eax, dword ptr [ebp + 0x14]
0048331c  sub        eax, ecx
0048331e  dec        eax
0048331f  cmp        eax, dword ptr [ebp + 0x14]
00483322  jge        0x483325

// block 00483324
00483324  inc        eax

// block 00483325
00483325  mov        dword ptr [ebp + 0x14], eax

// block 00483328
00483328  mov        eax, dword ptr [0x4b4394]
0048332d  cmp        eax, 2
00483330  je         0x4834e2

// block 00483336
00483336  cmp        eax, ebx
00483338  je         0x4834e2

// block 0048333e
0048333e  cmp        eax, 1
00483341  jne        0x483513

// block 00483347
00483347  mov        dword ptr [ebp - 8], ebx
0048334a  cmp        dword ptr [ebp + 0x20], ebx
0048334d  jne        0x483357

// block 0048334f
0048334f  mov        eax, dword ptr [esi]
00483351  mov        eax, dword ptr [eax + 4]
00483354  mov        dword ptr [ebp + 0x20], eax

// block 00483357
00483357  mov        esi, dword ptr [0x4980dc]
0048335d  xor        eax, eax
0048335f  cmp        dword ptr [ebp + 0x24], ebx
00483362  push       ebx
00483363  push       ebx
00483364  push       dword ptr [ebp + 0x14]
00483367  setne      al
0048336a  push       dword ptr [ebp + 0x10]
0048336d  lea        eax, [eax*8 + 1]
00483374  push       eax
00483375  push       dword ptr [ebp + 0x20]
00483378  call       esi

// block 0048337a
0048337a  mov        edi, eax
0048337c  cmp        edi, ebx
0048337e  je         0x483513

// block 00483384
00483384  jle        0x4833c9

// block 00483386
00483386  push       -0x20
00483388  xor        edx, edx
0048338a  pop        eax
0048338b  div        edi
0048338d  cmp        eax, 2
00483390  jb         0x4833c9

// block 00483392
00483392  lea        eax, [edi + edi + 8]
00483396  cmp        eax, 0x400
0048339b  ja         0x4833b0

// block 0048339d
0048339d  call       0x48d830

// block 004833a2
004833a2  mov        eax, esp
004833a4  cmp        eax, ebx
004833a6  je         0x4833c4

// block 004833a8
004833a8  mov        dword ptr [eax], 0xcccc
004833ae  jmp        0x4833c1

// block 004833b0
004833b0  push       eax
004833b1  call       0x46d04a

// block 004833b6
004833b6  pop        ecx
004833b7  cmp        eax, ebx
004833b9  je         0x4833c4

// block 004833bb
004833bb  mov        dword ptr [eax], 0xdddd

// block 004833c1
004833c1  add        eax, 8

// block 004833c4
004833c4  mov        dword ptr [ebp - 0xc], eax
004833c7  jmp        0x4833cc

// block 004833c9
004833c9  mov        dword ptr [ebp - 0xc], ebx

// block 004833cc
004833cc  cmp        dword ptr [ebp - 0xc], ebx
004833cf  je         0x483513

// block 004833d5
004833d5  push       edi
004833d6  push       dword ptr [ebp - 0xc]
004833d9  push       dword ptr [ebp + 0x14]
004833dc  push       dword ptr [ebp + 0x10]
004833df  push       1
004833e1  push       dword ptr [ebp + 0x20]
004833e4  call       esi

// block 004833e6
004833e6  test       eax, eax
004833e8  je         0x4834d1

// block 004833ee
004833ee  mov        esi, dword ptr [0x498050]
004833f4  push       ebx
004833f5  push       ebx
004833f6  push       edi
004833f7  push       dword ptr [ebp - 0xc]
004833fa  push       dword ptr [ebp + 0xc]
004833fd  push       dword ptr [ebp + 8]
00483400  call       esi

// block 00483402
00483402  mov        ecx, eax
00483404  mov        dword ptr [ebp - 8], ecx
00483407  cmp        ecx, ebx
00483409  je         0x4834d1

// block 0048340f
0048340f  test       dword ptr [ebp + 0xc], 0x400
00483416  je         0x483441

// block 00483418
00483418  cmp        dword ptr [ebp + 0x1c], ebx
0048341b  je         0x4834d1

// block 00483421
00483421  cmp        ecx, dword ptr [ebp + 0x1c]
00483424  jg         0x4834d1

// block 0048342a
0048342a  push       dword ptr [ebp + 0x1c]
0048342d  push       dword ptr [ebp + 0x18]
00483430  push       edi
00483431  push       dword ptr [ebp - 0xc]
00483434  push       dword ptr [ebp + 0xc]
00483437  push       dword ptr [ebp + 8]
0048343a  call       esi

// block 0048343c
0048343c  jmp        0x4834d1

// block 00483441
00483441  cmp        ecx, ebx
00483443  jle        0x48348a

// block 00483445
00483445  push       -0x20
00483447  xor        edx, edx
00483449  pop        eax
0048344a  div        ecx
0048344c  cmp        eax, 2
0048344f  jb         0x48348a

// block 00483451
00483451  lea        eax, [ecx + ecx + 8]
00483455  cmp        eax, 0x400
0048345a  ja         0x483472

// block 0048345c
0048345c  call       0x48d830

// block 00483461
00483461  mov        esi, esp
00483463  cmp        esi, ebx
00483465  je         0x4834d1

// block 00483467
00483467  mov        dword ptr [esi], 0xcccc
0048346d  add        esi, 8
00483470  jmp        0x48348c

// block 00483472
00483472  push       eax
00483473  call       0x46d04a

// block 00483478
00483478  pop        ecx
00483479  cmp        eax, ebx
0048347b  je         0x483486

// block 0048347d
0048347d  mov        dword ptr [eax], 0xdddd
00483483  add        eax, 8

// block 00483486
00483486  mov        esi, eax
00483488  jmp        0x48348c

// block 0048348a
0048348a  xor        esi, esi

// block 0048348c
0048348c  cmp        esi, ebx
0048348e  je         0x4834d1

// block 00483490
00483490  push       dword ptr [ebp - 8]
00483493  push       esi
00483494  push       edi
00483495  push       dword ptr [ebp - 0xc]
00483498  push       dword ptr [ebp + 0xc]
0048349b  push       dword ptr [ebp + 8]
0048349e  call       dword ptr [0x498050]

// block 004834a4
004834a4  test       eax, eax
004834a6  je         0x4834ca

// block 004834a8
004834a8  push       ebx
004834a9  push       ebx
004834aa  cmp        dword ptr [ebp + 0x1c], ebx
004834ad  jne        0x4834b3

// block 004834af
004834af  push       ebx
004834b0  push       ebx
004834b1  jmp        0x4834b9

// block 004834b3
004834b3  push       dword ptr [ebp + 0x1c]
004834b6  push       dword ptr [ebp + 0x18]

// block 004834b9
004834b9  push       dword ptr [ebp - 8]
004834bc  push       esi
004834bd  push       ebx
004834be  push       dword ptr [ebp + 0x20]
004834c1  call       dword ptr [0x498134]

// block 004834c7
004834c7  mov        dword ptr [ebp - 8], eax

// block 004834ca
004834ca  push       esi
004834cb  call       0x48326a

// block 004834d0
004834d0  pop        ecx

// block 004834d1
004834d1  push       dword ptr [ebp - 0xc]
004834d4  call       0x48326a

// block 004834d9
004834d9  mov        eax, dword ptr [ebp - 8]
004834dc  pop        ecx
004834dd  jmp        0x48363b

// block 004834e2
004834e2  mov        dword ptr [ebp - 0xc], ebx
004834e5  mov        dword ptr [ebp - 0x10], ebx
004834e8  cmp        dword ptr [ebp + 8], ebx
004834eb  jne        0x4834f5

// block 004834ed
004834ed  mov        eax, dword ptr [esi]
004834ef  mov        eax, dword ptr [eax + 0x14]
004834f2  mov        dword ptr [ebp + 8], eax

// block 004834f5
004834f5  cmp        dword ptr [ebp + 0x20], ebx
004834f8  jne        0x483502

// block 004834fa
004834fa  mov        eax, dword ptr [esi]
004834fc  mov        eax, dword ptr [eax + 4]
004834ff  mov        dword ptr [ebp + 0x20], eax

// block 00483502
00483502  push       dword ptr [ebp + 8]
00483505  call       0x48d62f

// block 0048350a
0048350a  pop        ecx
0048350b  mov        dword ptr [ebp - 0x14], eax
0048350e  cmp        eax, -1
00483511  jne        0x48351a

// block 00483513
00483513  xor        eax, eax
00483515  jmp        0x48363b

// block 0048351a
0048351a  cmp        eax, dword ptr [ebp + 0x20]
0048351d  je         0x4835fe

// block 00483523
00483523  push       ebx
00483524  push       ebx
00483525  lea        ecx, [ebp + 0x14]
00483528  push       ecx
00483529  push       dword ptr [ebp + 0x10]
0048352c  push       eax
0048352d  push       dword ptr [ebp + 0x20]
00483530  call       0x48d678

// block 00483535
00483535  add        esp, 0x18
00483538  mov        dword ptr [ebp - 0xc], eax
0048353b  cmp        eax, ebx
0048353d  je         0x483513

// block 0048353f
0048353f  mov        esi, dword ptr [0x498054]
00483545  push       ebx
00483546  push       ebx
00483547  push       dword ptr [ebp + 0x14]
0048354a  push       eax
0048354b  push       dword ptr [ebp + 0xc]
0048354e  push       dword ptr [ebp + 8]
00483551  call       esi

// block 00483553
00483553  mov        dword ptr [ebp - 8], eax
00483556  cmp        eax, ebx
00483558  jne        0x483561

// block 0048355a
0048355a  xor        esi, esi
0048355c  jmp        0x483618

// block 00483561
00483561  jle        0x4835a0

// block 00483563
00483563  cmp        eax, -0x20
00483566  ja         0x4835a0

// block 00483568
00483568  add        eax, 8
0048356b  cmp        eax, 0x400
00483570  ja         0x483588

// block 00483572
00483572  call       0x48d830

// block 00483577
00483577  mov        edi, esp
00483579  cmp        edi, ebx
0048357b  je         0x48355a

// block 0048357d
0048357d  mov        dword ptr [edi], 0xcccc
00483583  add        edi, 8
00483586  jmp        0x4835a2

// block 00483588
00483588  push       eax
00483589  call       0x46d04a

// block 0048358e
0048358e  pop        ecx
0048358f  cmp        eax, ebx
00483591  je         0x48359c

// block 00483593
00483593  mov        dword ptr [eax], 0xdddd
00483599  add        eax, 8

// block 0048359c
0048359c  mov        edi, eax
0048359e  jmp        0x4835a2

// block 004835a0
004835a0  xor        edi, edi

// block 004835a2
004835a2  cmp        edi, ebx
004835a4  je         0x48355a

// block 004835a6
004835a6  push       dword ptr [ebp - 8]
004835a9  push       ebx
004835aa  push       edi
004835ab  call       0x477420

// block 004835b0
004835b0  add        esp, 0xc
004835b3  push       dword ptr [ebp - 8]
004835b6  push       edi
004835b7  push       dword ptr [ebp + 0x14]
004835ba  push       dword ptr [ebp - 0xc]
004835bd  push       dword ptr [ebp + 0xc]
004835c0  push       dword ptr [ebp + 8]
004835c3  call       esi

// block 004835c5
004835c5  mov        dword ptr [ebp - 8], eax
004835c8  cmp        eax, ebx
004835ca  jne        0x4835d0

// block 004835cc
004835cc  xor        esi, esi
004835ce  jmp        0x4835f5

// block 004835d0
004835d0  push       dword ptr [ebp + 0x1c]
004835d3  lea        eax, [ebp - 8]
004835d6  push       dword ptr [ebp + 0x18]
004835d9  push       eax
004835da  push       edi
004835db  push       dword ptr [ebp + 0x20]
004835de  push       dword ptr [ebp - 0x14]
004835e1  call       0x48d678

// block 004835e6
004835e6  mov        esi, eax
004835e8  mov        dword ptr [ebp - 0x10], esi
004835eb  add        esp, 0x18
004835ee  neg        esi
004835f0  sbb        esi, esi
004835f2  and        esi, dword ptr [ebp - 8]

// block 004835f5
004835f5  push       edi
004835f6  call       0x48326a

// block 004835fb
004835fb  pop        ecx
004835fc  jmp        0x483618

// block 004835fe
004835fe  push       dword ptr [ebp + 0x1c]
00483601  push       dword ptr [ebp + 0x18]
00483604  push       dword ptr [ebp + 0x14]
00483607  push       dword ptr [ebp + 0x10]
0048360a  push       dword ptr [ebp + 0xc]
0048360d  push       dword ptr [ebp + 8]
00483610  call       dword ptr [0x498054]

// block 00483616
00483616  mov        esi, eax

// block 00483618
00483618  cmp        dword ptr [ebp - 0xc], ebx
0048361b  je         0x483626

// block 0048361d
0048361d  push       dword ptr [ebp - 0xc]
00483620  call       0x46c941

// block 00483625
00483625  pop        ecx

// block 00483626
00483626  mov        eax, dword ptr [ebp - 0x10]
00483629  cmp        eax, ebx
0048362b  je         0x483639

// block 0048362d
0048362d  cmp        dword ptr [ebp + 0x18], eax
00483630  je         0x483639

// block 00483632
00483632  push       eax
00483633  call       0x46c941

// block 00483638
00483638  pop        ecx

// block 00483639
00483639  mov        eax, esi

// block 0048363b
0048363b  lea        esp, [ebp - 0x20]
0048363e  pop        edi
0048363f  pop        esi
00483640  pop        ebx
00483641  mov        ecx, dword ptr [ebp - 4]
00483644  xor        ecx, ebp
00483646  call       0x46c932

// block 0048364b
0048364b  leave      
0048364c  ret        
