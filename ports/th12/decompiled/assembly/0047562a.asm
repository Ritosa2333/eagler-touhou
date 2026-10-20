
// block 0047562a
0047562a  mov        edi, edi
0047562c  push       esi
0047562d  push       edi
0047562e  mov        esi, 0x49d51c
00475633  push       esi
00475634  call       dword ptr [0x498174]

// block 0047563a
0047563a  test       eax, eax
0047563c  jne        0x475645

// block 0047563e
0047563e  push       esi
0047563f  call       0x472bdd

// block 00475644
00475644  pop        ecx

// block 00475645
00475645  mov        edi, eax
00475647  test       edi, edi
00475649  je         0x4757ad

// block 0047564f
0047564f  mov        esi, dword ptr [0x498170]
00475655  push       0x49d568
0047565a  push       edi
0047565b  call       esi

// block 0047565d
0047565d  push       0x49d55c
00475662  push       edi
00475663  mov        dword ptr [0x4b4100], eax
00475668  call       esi

// block 0047566a
0047566a  push       0x49d550
0047566f  push       edi
00475670  mov        dword ptr [0x4b4104], eax
00475675  call       esi

// block 00475677
00475677  push       0x49d548
0047567c  push       edi
0047567d  mov        dword ptr [0x4b4108], eax
00475682  call       esi

// block 00475684
00475684  cmp        dword ptr [0x4b4100], 0
0047568b  mov        esi, dword ptr [0x498144]
00475691  mov        dword ptr [0x4b410c], eax
00475696  je         0x4756ae

// block 00475698
00475698  cmp        dword ptr [0x4b4104], 0
0047569f  je         0x4756ae

// block 004756a1
004756a1  cmp        dword ptr [0x4b4108], 0
004756a8  je         0x4756ae

// block 004756aa
004756aa  test       eax, eax
004756ac  jne        0x4756d2

// block 004756ae
004756ae  mov        eax, dword ptr [0x49814c]
004756b3  mov        dword ptr [0x4b4104], eax
004756b8  mov        eax, dword ptr [0x498140]
004756bd  mov        dword ptr [0x4b4100], 0x475250
004756c7  mov        dword ptr [0x4b4108], esi
004756cd  mov        dword ptr [0x4b410c], eax

// block 004756d2
004756d2  call       dword ptr [0x498148]

// block 004756d8
004756d8  mov        dword ptr [0x4adacc], eax
004756dd  cmp        eax, -1
004756e0  je         0x4757b2

// block 004756e6
004756e6  push       dword ptr [0x4b4104]
004756ec  push       eax
004756ed  call       esi

// block 004756ef
004756ef  test       eax, eax
004756f1  je         0x4757b2

// block 004756f7
004756f7  call       0x472f3f

// block 004756fc
004756fc  push       dword ptr [0x4b4100]
00475702  call       0x475163

// block 00475707
00475707  push       dword ptr [0x4b4104]
0047570d  mov        dword ptr [0x4b4100], eax
00475712  call       0x475163

// block 00475717
00475717  push       dword ptr [0x4b4108]
0047571d  mov        dword ptr [0x4b4104], eax
00475722  call       0x475163

// block 00475727
00475727  push       dword ptr [0x4b410c]
0047572d  mov        dword ptr [0x4b4108], eax
00475732  call       0x475163

// block 00475737
00475737  add        esp, 0x10
0047573a  mov        dword ptr [0x4b410c], eax
0047573f  call       0x46eb06

// block 00475744
00475744  test       eax, eax
00475746  je         0x4757ad

// block 00475748
00475748  push       0x475481
0047574d  push       dword ptr [0x4b4100]
00475753  call       0x4751de

// block 00475758
00475758  pop        ecx
00475759  call       eax

// block 0047575b
0047575b  mov        dword ptr [0x4adac8], eax
00475760  cmp        eax, -1
00475763  je         0x4757ad

// block 00475765
00475765  push       0x214
0047576a  push       1
0047576c  call       0x477813

// block 00475771
00475771  mov        esi, eax
00475773  pop        ecx
00475774  pop        ecx
00475775  test       esi, esi
00475777  je         0x4757ad

// block 00475779
00475779  push       esi
0047577a  push       dword ptr [0x4adac8]
00475780  push       dword ptr [0x4b4108]
00475786  call       0x4751de

// block 0047578b
0047578b  pop        ecx
0047578c  call       eax

// block 0047578e
0047578e  test       eax, eax
00475790  je         0x4757ad

// block 00475792
00475792  push       0
00475794  push       esi
00475795  call       0x475307

// block 0047579a
0047579a  pop        ecx
0047579b  pop        ecx
0047579c  call       dword ptr [0x4981f0]

// block 004757a2
004757a2  or         dword ptr [esi + 4], 0xffffffff
004757a6  mov        dword ptr [esi], eax
004757a8  xor        eax, eax
004757aa  inc        eax
004757ab  jmp        0x4757b4

// block 004757ad
004757ad  call       0x4752ca

// block 004757b2
004757b2  xor        eax, eax

// block 004757b4
004757b4  pop        edi
004757b5  pop        esi
004757b6  ret        
