
// block 004470c0
004470c0  push       ebp
004470c1  mov        ebp, dword ptr [esp + 8]
004470c5  mov        eax, dword ptr [ebp + 0x24]
004470c8  cmp        eax, 3
004470cb  ja         0x447b06

// block 004470d1
004470d1  push       ebx
004470d2  push       esi
004470d3  push       edi
004470d4  mov        ebx, 6
004470d9  jmp        dword ptr [eax*4 + 0x447b10]

// block 004470e0
004470e0  mov        eax, ebx
004470e2  xor        edi, edi
004470e4  mov        dword ptr [ebp + 0x30], ebx
004470e7  cmp        eax, edi
004470e9  je         0x4470fa

// block 004470eb
004470eb  jg         0x4470f3

// block 004470ed
004470ed  dec        eax
004470ee  mov        dword ptr [ebp + 0x28], eax
004470f1  jmp        0x4470fd

// block 004470f3
004470f3  xor        eax, eax
004470f5  mov        dword ptr [ebp + 0x28], eax
004470f8  jmp        0x4470fd

// block 004470fa
004470fa  mov        dword ptr [ebp + 0x28], edi

// block 004470fd
004470fd  mov        dword ptr [ebp + 0x108], 5
00447107  mov        eax, dword ptr [ebp + 0x108]
0044710d  cmp        eax, edi
0044710f  je         0x44712c

// block 00447111
00447111  cmp        eax, 1
00447114  jg         0x44711f

// block 00447116
00447116  dec        eax
00447117  mov        dword ptr [ebp + 0x100], eax
0044711d  jmp        0x447136

// block 0044711f
0044711f  mov        eax, 1
00447124  mov        dword ptr [ebp + 0x100], eax
0044712a  jmp        0x447136

// block 0044712c
0044712c  mov        dword ptr [ebp + 0x100], 1

// block 00447136
00447136  mov        edx, dword ptr [ebp + 0x100]
0044713c  mov        esi, 1
00447141  mov        dword ptr [ebp + 0x1d0], esi
00447147  call       0x40d840

// block 0044714c
0044714c  mov        ecx, eax
0044714e  add        ecx, 9
00447151  mov        eax, 0x66666667
00447156  imul       ecx
00447158  sar        edx, 2
0044715b  mov        eax, edx
0044715d  shr        eax, 0x1f
00447160  lea        ecx, [edx + eax + 1]
00447164  mov        eax, ecx
00447166  mov        dword ptr [ebp + 0x1e0], ecx
0044716c  cmp        eax, edi
0044716e  je         0x447185

// block 00447170
00447170  jg         0x44717b

// block 00447172
00447172  dec        eax
00447173  mov        dword ptr [ebp + 0x1d8], eax
00447179  jmp        0x44718b

// block 0044717b
0044717b  xor        eax, eax
0044717d  mov        dword ptr [ebp + 0x1d8], eax
00447183  jmp        0x44718b

// block 00447185
00447185  mov        dword ptr [ebp + 0x1d8], edi

// block 0044718b
0044718b  mov        dword ptr [ebp + 0x2a8], esi
00447191  cmp        dword ptr [ebp + 0x66c], edi
00447197  jne        0x4471c3

// block 00447199
00447199  mov        eax, dword ptr [0x4b43b8]
0044719e  mov        ecx, dword ptr [eax + 0x18fb4]
004471a4  push       edi
004471a5  push       0x14
004471a7  lea        edx, [esp + 0x1c]
004471ab  push       edx
004471ac  push       ecx
004471ad  mov        eax, 0x17
004471b2  xor        ecx, ecx
004471b4  call       0x4615a0

// block 004471b9
004471b9  mov        edx, dword ptr [esp + 0x14]
004471bd  mov        dword ptr [ebp + 0x66c], edx

// block 004471c3
004471c3  mov        ecx, dword ptr [ebp + 0x14]
004471c6  push       edi
004471c7  push       0x72
004471c9  lea        eax, [esp + 0x1c]
004471cd  push       eax
004471ce  push       ecx
004471cf  mov        eax, 0x17
004471d4  xor        ecx, ecx
004471d6  call       0x4615a0

// block 004471db
004471db  mov        edx, dword ptr [esp + 0x14]
004471df  mov        ecx, esi
004471e1  mov        eax, ebp
004471e3  mov        dword ptr [ebp + 0x490], edx
004471e9  call       0x43ef40

// block 004471ee
004471ee  mov        eax, dword ptr [ebp + 0x28]
004471f1  mov        ecx, dword ptr [ebp + 0x14]
004471f4  cdq        
004471f5  sub        eax, edx
004471f7  mov        esi, eax
004471f9  push       edi
004471fa  sar        esi, 1
004471fc  add        esi, 0xc1
00447202  push       esi
00447203  lea        eax, [esp + 0x1c]
00447207  push       eax
00447208  push       ecx
00447209  mov        eax, 0x17
0044720e  xor        ecx, ecx
00447210  call       0x4615a0

// block 00447215
00447215  mov        edx, dword ptr [esp + 0x14]
00447219  mov        dword ptr [ebp + esi*4 + 0x2c8], edx
00447220  mov        esi, dword ptr [ebp + 0x28]
00447223  and        esi, 0x80000001
00447229  jns        0x447230

// block 0044722b
0044722b  dec        esi
0044722c  or         esi, 0xfffffffe
0044722f  inc        esi

// block 00447230
00447230  mov        ecx, dword ptr [ebp + 0x14]
00447233  push       edi
00447234  add        esi, 0xc4
0044723a  push       esi
0044723b  lea        eax, [esp + 0x1c]
0044723f  push       eax
00447240  push       ecx
00447241  mov        eax, 0x17
00447246  xor        ecx, ecx
00447248  call       0x4615a0

// block 0044724d
0044724d  mov        edx, dword ptr [esp + 0x14]
00447251  mov        dword ptr [ebp + esi*4 + 0x2c8], edx
00447258  mov        esi, dword ptr [ebp + 0x100]
0044725e  mov        ecx, dword ptr [ebp + 0x14]
00447261  push       edi
00447262  add        esi, 0xc6
00447268  push       esi
00447269  lea        eax, [esp + 0x1c]
0044726d  push       eax
0044726e  push       ecx
0044726f  mov        eax, 0x17
00447274  xor        ecx, ecx
00447276  call       0x4615a0

// block 0044727b
0044727b  mov        edx, dword ptr [esp + 0x14]
0044727f  push       edi
00447280  push       0xce
00447285  lea        eax, [esp + 0x1c]
00447289  mov        dword ptr [ebp + esi*4 + 0x2c8], edx
00447290  mov        ecx, dword ptr [ebp + 0x14]
00447293  push       eax
00447294  push       ecx
00447295  mov        eax, 0x17
0044729a  xor        ecx, ecx
0044729c  call       0x4615a0

// block 004472a1
004472a1  mov        edx, dword ptr [esp + 0x14]
004472a5  push       edi
004472a6  push       0xcf
004472ab  lea        eax, [esp + 0x1c]
004472af  mov        dword ptr [ebp + 0x600], edx
004472b5  mov        ecx, dword ptr [ebp + 0x14]
004472b8  push       eax
004472b9  push       ecx
004472ba  mov        eax, 0x17
004472bf  xor        ecx, ecx
004472c1  call       0x4615a0

// block 004472c6
004472c6  mov        edx, dword ptr [esp + 0x14]
004472ca  push       edi
004472cb  push       0xd0
004472d0  lea        eax, [esp + 0x1c]
004472d4  mov        dword ptr [ebp + 0x604], edx
004472da  mov        ecx, dword ptr [ebp + 0x14]
004472dd  push       eax
004472de  push       ecx
004472df  mov        eax, 0x17
004472e4  xor        ecx, ecx
004472e6  call       0x4615a0

// block 004472eb
004472eb  mov        edx, dword ptr [esp + 0x14]
004472ef  push       edi
004472f0  push       0xd1
004472f5  lea        eax, [esp + 0x1c]
004472f9  mov        dword ptr [ebp + 0x608], edx
004472ff  mov        ecx, dword ptr [ebp + 0x14]
00447302  push       eax
00447303  push       ecx
00447304  mov        eax, 0x17
00447309  xor        ecx, ecx
0044730b  call       0x4615a0

// block 00447310
00447310  mov        edx, dword ptr [esp + 0x14]
00447314  push       edi
00447315  push       0xcb
0044731a  lea        eax, [esp + 0x1c]
0044731e  mov        dword ptr [ebp + 0x60c], edx
00447324  mov        ecx, dword ptr [ebp + 0x14]
00447327  push       eax
00447328  push       ecx
00447329  mov        eax, 0x17
0044732e  xor        ecx, ecx
00447330  call       0x4615a0

// block 00447335
00447335  mov        edx, dword ptr [esp + 0x14]
00447339  mov        dword ptr [ebp + 0x5f4], edx
0044733f  push       edi
00447340  mov        ecx, dword ptr [ebp + 0x14]
00447343  push       0xcc
00447348  lea        eax, [esp + 0x1c]
0044734c  push       eax
0044734d  push       ecx
0044734e  mov        eax, 0x17
00447353  xor        ecx, ecx
00447355  call       0x4615a0

// block 0044735a
0044735a  mov        edx, dword ptr [esp + 0x14]
0044735e  push       edi
0044735f  push       0xcd
00447364  lea        eax, [esp + 0x1c]
00447368  mov        dword ptr [ebp + 0x5f8], edx
0044736e  mov        ecx, dword ptr [ebp + 0x14]
00447371  push       eax
00447372  push       ecx
00447373  mov        eax, 0x17
00447378  xor        ecx, ecx
0044737a  call       0x4615a0

// block 0044737f
0044737f  mov        edx, dword ptr [esp + 0x14]
00447383  push       edi
00447384  push       0xd2
00447389  lea        eax, [esp + 0x1c]
0044738d  mov        dword ptr [ebp + 0x5fc], edx
00447393  mov        ecx, dword ptr [ebp + 0x14]
00447396  push       eax
00447397  push       ecx
00447398  mov        eax, 0x17
0044739d  xor        ecx, ecx
0044739f  call       0x4615a0

// block 004473a4
004473a4  mov        edx, dword ptr [esp + 0x14]
004473a8  mov        dword ptr [ebp + 0x610], edx

// block 004473ae
004473ae  cmp        dword ptr [ebp + 0x2b8], ebx
004473b4  jle        0x447b03

// block 004473ba
004473ba  mov        ecx, 2
004473bf  mov        eax, ebp
004473c1  call       0x43ef40

// block 004473c6
004473c6  pop        edi
004473c7  pop        esi
004473c8  pop        ebx
004473c9  mov        eax, 1
004473ce  pop        ebp
004473cf  ret        4

// block 004473d2
004473d2  mov        eax, dword ptr [ebp + 0x28]
004473d5  mov        dword ptr [ebp + 0x2c], eax
004473d8  mov        ecx, dword ptr [ebp + 0x100]
004473de  lea        esi, [ebp + 0x100]
004473e4  mov        dword ptr [esi + 4], ecx
004473e7  mov        edx, dword ptr [ebp + 0x1d8]
004473ed  mov        al, 0x10
004473ef  mov        dword ptr [ebp + 0x1dc], edx
004473f5  test       byte ptr [0x4d48c4], al
004473fb  jne        0x447405

// block 004473fd
004473fd  test       byte ptr [0x4d48c0], al
00447403  je         0x44741f

// block 00447405
00447405  push       -1
00447407  mov        eax, esi
00447409  call       0x464970

// block 0044740e
0044740e  mov        eax, dword ptr [ebp + 0x608]
00447414  push       eax
00447415  mov        ebx, 2
0044741a  call       0x4619e0

// block 0044741f
0044741f  mov        al, 0x20
00447421  test       byte ptr [0x4d48c4], al
00447427  jne        0x447431

// block 00447429
00447429  test       byte ptr [0x4d48c0], al
0044742f  je         0x44744b

// block 00447431
00447431  push       1
00447433  mov        eax, esi
00447435  call       0x464970

// block 0044743a
0044743a  mov        ecx, dword ptr [ebp + 0x60c]
00447440  push       ecx
00447441  mov        ebx, 2
00447446  call       0x4619e0

// block 0044744b
0044744b  mov        edx, dword ptr [esi + 4]
0044744e  cmp        edx, dword ptr [esi]
00447450  je         0x4474ef

// block 00447456
00447456  mov        edx, 0xa
0044745b  call       0x453d90

// block 00447460
00447460  mov        esi, dword ptr [ebp + 0x104]
00447466  add        esi, 0xc6
0044746c  mov        edi, ebp
0044746e  call       0x43efd0

// block 00447473
00447473  mov        esi, dword ptr [ebp + 0x100]
00447479  add        esi, 0xc6
0044747f  call       0x43efa0

// block 00447484
00447484  xor        ebx, ebx
00447486  cmp        dword ptr [ebp + 0x1d8], ebx
0044748c  jle        0x4474c4

// block 0044748e
0044748e  mov        eax, dword ptr [ebp + 0x1e0]
00447494  cmp        eax, ebx
00447496  je         0x4474b3

// block 00447498
00447498  cmp        eax, 1
0044749b  jg         0x4474a6

// block 0044749d
0044749d  dec        eax
0044749e  mov        dword ptr [ebp + 0x1d8], eax
004474a4  jmp        0x4474bd

// block 004474a6
004474a6  mov        eax, 1
004474ab  mov        dword ptr [ebp + 0x1d8], eax
004474b1  jmp        0x4474bd

// block 004474b3
004474b3  mov        dword ptr [ebp + 0x1d8], 1

// block 004474bd
004474bd  mov        ecx, ebp
004474bf  call       0x447b20

// block 004474c4
004474c4  mov        edx, dword ptr [ebp + 0x100]
004474ca  call       0x40d840

// block 004474cf
004474cf  mov        ecx, eax
004474d1  add        ecx, 9
004474d4  mov        eax, 0x66666667
004474d9  imul       ecx
004474db  sar        edx, 2
004474de  mov        eax, edx
004474e0  shr        eax, 0x1f
004474e3  lea        ecx, [edx + eax + 1]
004474e7  mov        dword ptr [ebp + 0x1e0], ecx
004474ed  jmp        0x4474f1

// block 004474ef
004474ef  xor        ebx, ebx

// block 004474f1
004474f1  test       byte ptr [0x4d48c4], 0x40
004474f8  jne        0x447503

// block 004474fa
004474fa  test       byte ptr [0x4d48c0], 0x40
00447501  je         0x447520

// block 00447503
00447503  push       -1
00447505  lea        eax, [ebp + 0x28]
00447508  call       0x464970

// block 0044750d
0044750d  mov        edx, dword ptr [ebp + 0x600]
00447513  push       edx
00447514  mov        ebx, 2
00447519  call       0x4619e0

// block 0044751e
0044751e  xor        ebx, ebx

// block 00447520
00447520  test       byte ptr [0x4d48c4], 0x80
00447527  jne        0x447532

// block 00447529
00447529  test       byte ptr [0x4d48c0], 0x80
00447530  je         0x44754f

// block 00447532
00447532  push       1
00447534  lea        eax, [ebp + 0x28]
00447537  call       0x464970

// block 0044753c
0044753c  mov        eax, dword ptr [ebp + 0x604]
00447542  push       eax
00447543  mov        ebx, 2
00447548  call       0x4619e0

// block 0044754d
0044754d  xor        ebx, ebx

// block 0044754f
0044754f  mov        ecx, dword ptr [ebp + 0x2c]
00447552  cmp        ecx, dword ptr [ebp + 0x28]
00447555  je         0x4475e4

// block 0044755b
0044755b  mov        edx, 0xa
00447560  call       0x453d90

// block 00447565
00447565  mov        eax, dword ptr [ebp + 0x2c]
00447568  cdq        
00447569  sub        eax, edx
0044756b  mov        ecx, eax
0044756d  mov        eax, dword ptr [ebp + 0x28]
00447570  cdq        
00447571  sub        eax, edx
00447573  sar        ecx, 1
00447575  sar        eax, 1
00447577  cmp        eax, ecx
00447579  je         0x44759d

// block 0044757b
0044757b  lea        esi, [ecx + 0xc1]
00447581  mov        edi, ebp
00447583  call       0x43efd0

// block 00447588
00447588  mov        eax, dword ptr [ebp + 0x28]
0044758b  cdq        
0044758c  sub        eax, edx
0044758e  mov        esi, eax
00447590  sar        esi, 1
00447592  add        esi, 0xc1
00447598  call       0x43efa0

// block 0044759d
0044759d  mov        esi, dword ptr [ebp + 0x2c]
004475a0  and        esi, 0x80000001
004475a6  jns        0x4475ad

// block 004475a8
004475a8  dec        esi
004475a9  or         esi, 0xfffffffe
004475ac  inc        esi

// block 004475ad
004475ad  add        esi, 0xc4
004475b3  mov        edi, ebp
004475b5  call       0x43efd0

// block 004475ba
004475ba  mov        esi, dword ptr [ebp + 0x28]
004475bd  and        esi, 0x80000001
004475c3  jns        0x4475ca

// block 004475c5
004475c5  dec        esi
004475c6  or         esi, 0xfffffffe
004475c9  inc        esi

// block 004475ca
004475ca  add        esi, 0xc4
004475d0  call       0x43efa0

// block 004475d5
004475d5  cmp        dword ptr [ebp + 0x1d8], ebx
004475db  jle        0x4475e4

// block 004475dd
004475dd  mov        ecx, ebp
004475df  call       0x447b20

// block 004475e4
004475e4  test       dword ptr [0x4d48c4], 0x80001
004475ee  je         0x447679

// block 004475f4
004475f4  cmp        dword ptr [ebp + 0x1d8], ebx
004475fa  jne        0x447630

// block 004475fc
004475fc  xor        esi, esi
004475fe  lea        edi, [ebp + 0x670]

// block 00447604
00447604  mov        ecx, dword ptr [0x4cee70]
0044760a  push       ebx
0044760b  lea        edx, [esi + 0x19]
0044760e  push       edx
0044760f  lea        eax, [esp + 0x1c]
00447613  push       eax
00447614  push       ecx
00447615  mov        eax, 0x17
0044761a  xor        ecx, ecx
0044761c  call       0x4615a0

// block 00447621
00447621  mov        edx, dword ptr [esp + 0x14]
00447625  mov        dword ptr [edi], edx
00447627  inc        esi
00447628  add        edi, 4
0044762b  cmp        esi, 0xa
0044762e  jl         0x447604

// block 00447630
00447630  lea        esi, [ebp + 0x1d8]
00447636  push       1
00447638  mov        eax, esi
0044763a  call       0x464970

// block 0044763f
0044763f  cmp        dword ptr [esi], ebx
00447641  jne        0x447668

// block 00447643
00447643  lea        esi, [ebp + 0x670]
00447649  mov        edi, 0xa
0044764e  mov        edi, edi

// block 00447650
00447650  mov        eax, dword ptr [esi]
00447652  push       eax
00447653  mov        ebx, 1
00447658  call       0x461970

// block 0044765d
0044765d  add        esi, 4
00447660  sub        edi, ebx
00447662  jne        0x447650

// block 00447664
00447664  xor        ebx, ebx
00447666  jmp        0x44766f

// block 00447668
00447668  mov        ecx, ebp
0044766a  call       0x447b20

// block 0044766f
0044766f  mov        edx, 7
00447674  call       0x453d90

// block 00447679
00447679  cmp        dword ptr [ebp + 0x100], 4
00447680  jne        0x447944

// block 00447686
00447686  cmp        dword ptr [ebp + 0x28], 2
0044768a  jne        0x447944

// block 00447690
00447690  test       dword ptr [0x4d48c4], 0x80103
0044769a  je         0x4476a8

// block 0044769c
0044769c  mov        dword ptr [0x4d4ea0], ebx
004476a2  mov        dword ptr [0x4d4ea4], ebx

// block 004476a8
004476a8  mov        ecx, 0x40
004476ad  mov        esi, 0x4d51d0
004476b2  mov        edi, 0x4d50d0

// block 004476b7
004476b7  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004476b9
004476b9  mov        esi, 0x4d51d0
004476be  call       0x463910

// block 004476c3
004476c3  cmp        eax, 2
004476c6  jne        0x44783c

// block 004476cc
004476cc  push       0x100
004476d1  push       ebx
004476d2  push       0x4d4da0
004476d7  call       0x477420

// block 004476dc
004476dc  movzx      ecx, byte ptr [0x4d5211]
004476e3  movzx      edx, byte ptr [0x4d5212]
004476ea  movzx      eax, byte ptr [0x4d5213]
004476f1  mov        byte ptr [0x4d4dbe], cl
004476f7  movzx      ecx, byte ptr [0x4d5214]
004476fe  mov        byte ptr [0x4d4dd0], dl
00447704  movzx      edx, byte ptr [0x4d5215]
0044770b  mov        byte ptr [0x4d4dc0], cl
00447711  movzx      ecx, byte ptr [0x4d5217]
00447718  mov        byte ptr [0x4d4dce], al
0044771d  movzx      eax, byte ptr [0x4d5216]
00447724  mov        byte ptr [0x4d4db2], dl
0044772a  movzx      edx, byte ptr [0x4d5218]
00447731  mov        byte ptr [0x4d4dc2], cl
00447737  movzx      ecx, byte ptr [0x4d521a]
0044773e  mov        byte ptr [0x4d4dc1], al
00447743  movzx      eax, byte ptr [0x4d5219]
0044774a  mov        byte ptr [0x4d4dc3], dl
00447750  movzx      edx, byte ptr [0x4d521b]
00447757  mov        byte ptr [0x4d4dc4], cl
0044775d  movzx      ecx, byte ptr [0x4d521d]
00447764  mov        byte ptr [0x4d4db7], al
00447769  movzx      eax, byte ptr [0x4d521c]
00447770  mov        byte ptr [0x4d4dc5], dl
00447776  movzx      edx, byte ptr [0x4d521e]
0044777d  mov        byte ptr [0x4d4dd2], cl
00447783  movzx      ecx, byte ptr [0x4d5220]
0044778a  mov        byte ptr [0x4d4dc6], al
0044778f  movzx      eax, byte ptr [0x4d521f]
00447796  mov        byte ptr [0x4d4dd1], dl
0044779c  movzx      edx, byte ptr [0x4d5221]
004477a3  mov        byte ptr [0x4d4db9], cl
004477a9  movzx      ecx, byte ptr [0x4d5223]
004477b0  mov        byte ptr [0x4d4db8], al
004477b5  movzx      eax, byte ptr [0x4d5222]
004477bc  mov        byte ptr [0x4d4db0], dl
004477c2  movzx      edx, byte ptr [0x4d5224]
004477c9  mov        byte ptr [0x4d4dbf], cl
004477cf  movzx      ecx, byte ptr [0x4d5226]
004477d6  mov        byte ptr [0x4d4db3], al
004477db  movzx      eax, byte ptr [0x4d5225]
004477e2  mov        byte ptr [0x4d4db4], dl
004477e8  movzx      edx, byte ptr [0x4d5227]
004477ef  mov        byte ptr [0x4d4dcf], cl
004477f5  movzx      ecx, byte ptr [0x4d5229]
004477fc  mov        byte ptr [0x4d4db6], al
00447801  movzx      eax, byte ptr [0x4d5228]
00447808  mov        byte ptr [0x4d4db1], dl
0044780e  movzx      edx, byte ptr [0x4d522a]
00447815  mov        byte ptr [0x4d4db5], cl
0044781b  mov        ecx, 0x40
00447820  mov        esi, 0x4d4da0
00447825  mov        edi, 0x4d51d0
0044782a  mov        byte ptr [0x4d4dcd], al
0044782f  mov        byte ptr [0x4d4dcc], dl
00447835  add        esp, 0xc

// block 00447838
00447838  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0044783a
0044783a  jmp        0x447845

// block 0044783c
0044783c  cmp        eax, 1
0044783f  jne        0x447926

// block 00447845
00447845  xor        eax, eax
00447847  jmp        0x447850

// block 00447850
00447850  movzx      edx, byte ptr [eax + 0x4d50d0]
00447857  mov        cl, byte ptr [eax + 0x4d51d0]
0044785d  xor        dl, cl
0044785f  and        dl, cl
00447861  mov        cl, byte ptr [eax + 0x4d51d1]
00447867  mov        byte ptr [eax + 0x4d4da0], dl
0044786d  movzx      edx, byte ptr [eax + 0x4d50d1]
00447874  xor        dl, cl
00447876  and        dl, cl
00447878  mov        cl, byte ptr [eax + 0x4d51d2]
0044787e  mov        byte ptr [eax + 0x4d4da1], dl
00447884  movzx      edx, byte ptr [eax + 0x4d50d2]
0044788b  xor        dl, cl
0044788d  and        dl, cl
0044788f  mov        cl, byte ptr [eax + 0x4d51d3]
00447895  mov        byte ptr [eax + 0x4d4da2], dl
0044789b  movzx      edx, byte ptr [eax + 0x4d50d3]
004478a2  xor        dl, cl
004478a4  and        dl, cl
004478a6  mov        byte ptr [eax + 0x4d4da3], dl
004478ac  add        eax, 4
004478af  cmp        eax, 0x100
004478b4  jl         0x447850

// block 004478b6
004478b6  mov        eax, dword ptr [0x4d4ea0]
004478bb  cmp        eax, 0xc
004478be  jb         0x4478d1

// block 004478c0
004478c0  call       0x43f1e0

// block 004478c5
004478c5  mov        edx, 0x12
004478ca  call       0x453d90

// block 004478cf
004478cf  jmp        0x447920

// block 004478d1
004478d1  mov        ecx, dword ptr [eax*4 + 0x4a1f94]
004478d8  test       byte ptr [ecx + 0x4d4da0], 0x80
004478df  je         0x4478ef

// block 004478e1
004478e1  inc        eax
004478e2  mov        dword ptr [0x4d4ea0], eax
004478e7  mov        dword ptr [0x4d4ea4], ebx
004478ed  jmp        0x447926

// block 004478ef
004478ef  xor        cl, cl
004478f1  xor        eax, eax
004478f3  jmp        0x447900

// block 00447900
00447900  mov        dl, byte ptr [eax + 0x4d4da0]
00447906  or         dl, byte ptr [eax + 0x4d4da2]
0044790c  add        eax, 3
0044790f  or         dl, byte ptr [eax + 0x4d4d9e]
00447915  or         cl, dl
00447917  cmp        eax, 0x39
0044791a  jl         0x447900

// block 0044791c
0044791c  test       cl, cl
0044791e  jns        0x447926

// block 00447920
00447920  mov        dword ptr [0x4d4ea0], ebx

// block 00447926
00447926  mov        eax, dword ptr [0x4d4ea4]
0044792b  inc        eax
0044792c  cmp        eax, 0x12c
00447931  mov        dword ptr [0x4d4ea4], eax
00447936  jle        0x447944

// block 00447938
00447938  mov        dword ptr [0x4d4ea0], ebx
0044793e  mov        dword ptr [0x4d4ea4], ebx

// block 00447944
00447944  test       dword ptr [0x4d48c4], 0x102
0044794e  je         0x447b03

// block 00447954
00447954  mov        ecx, 3
00447959  mov        eax, ebp
0044795b  call       0x43ef40

// block 00447960
00447960  mov        edx, 9
00447965  call       0x453d90

// block 0044796a
0044796a  mov        esi, dword ptr [ebp + 0x100]
00447970  mov        eax, dword ptr [ebp + esi*4 + 0x5e0]
00447977  add        esi, 0xc6
0044797d  push       eax
0044797e  mov        ebx, 1
00447983  call       0x461970

// block 00447988
00447988  mov        dword ptr [ebp + esi*4 + 0x2c8], 0
00447993  mov        esi, dword ptr [ebp + 0x28]
00447996  and        esi, 0x80000001
0044799c  jns        0x4479a3

// block 0044799e
0044799e  dec        esi
0044799f  or         esi, 0xfffffffe
004479a2  inc        esi

// block 004479a3
004479a3  mov        ecx, dword ptr [ebp + esi*4 + 0x5d8]
004479aa  add        esi, 0xc4
004479b0  push       ecx
004479b1  call       0x461970

// block 004479b6
004479b6  mov        dword ptr [ebp + esi*4 + 0x2c8], 0
004479c1  mov        eax, dword ptr [ebp + 0x28]
004479c4  cdq        
004479c5  sub        eax, edx
004479c7  mov        esi, eax
004479c9  sar        esi, 1
004479cb  mov        edx, dword ptr [ebp + esi*4 + 0x5cc]
004479d2  add        esi, 0xc1
004479d8  push       edx
004479d9  call       0x461970

// block 004479de
004479de  mov        dword ptr [ebp + esi*4 + 0x2c8], 0
004479e9  mov        eax, dword ptr [ebp + 0x600]
004479ef  push       eax
004479f0  call       0x461970

// block 004479f5
004479f5  mov        ecx, dword ptr [ebp + 0x604]
004479fb  push       ecx
004479fc  mov        dword ptr [ebp + 0x600], 0
00447a06  call       0x461970

// block 00447a0b
00447a0b  mov        edx, dword ptr [ebp + 0x608]
00447a11  push       edx
00447a12  mov        dword ptr [ebp + 0x604], 0
00447a1c  call       0x461970

// block 00447a21
00447a21  mov        eax, dword ptr [ebp + 0x60c]
00447a27  push       eax
00447a28  mov        dword ptr [ebp + 0x608], 0
00447a32  call       0x461970

// block 00447a37
00447a37  mov        ecx, dword ptr [ebp + 0x5f4]
00447a3d  push       ecx
00447a3e  mov        dword ptr [ebp + 0x60c], 0
00447a48  call       0x461970

// block 00447a4d
00447a4d  mov        edx, dword ptr [ebp + 0x5f8]
00447a53  push       edx
00447a54  mov        dword ptr [ebp + 0x5f4], 0
00447a5e  call       0x461970

// block 00447a63
00447a63  mov        eax, dword ptr [ebp + 0x5fc]
00447a69  push       eax
00447a6a  mov        dword ptr [ebp + 0x5f8], 0
00447a74  call       0x461970

// block 00447a79
00447a79  mov        ecx, dword ptr [ebp + 0x610]
00447a7f  push       ecx
00447a80  mov        dword ptr [ebp + 0x5fc], 0
00447a8a  call       0x461970

// block 00447a8f
00447a8f  mov        dword ptr [ebp + 0x610], 0
00447a99  add        ebp, 0x670
00447a9f  mov        esi, 0xa

// block 00447aa4
00447aa4  mov        edx, dword ptr [ebp]
00447aa7  push       edx
00447aa8  mov        ebx, 1
00447aad  call       0x461970

// block 00447ab2
00447ab2  add        ebp, 4
00447ab5  sub        esi, ebx
00447ab7  jne        0x447aa4

// block 00447ab9
00447ab9  pop        edi
00447aba  pop        esi
00447abb  pop        ebx
00447abc  mov        eax, 1
00447ac1  pop        ebp
00447ac2  ret        4

// block 00447ac5
00447ac5  cmp        dword ptr [ebp + 0x2b8], ebx
00447acb  jl         0x447b03

// block 00447acd
00447acd  mov        esi, 0x72
00447ad2  mov        edi, ebp
00447ad4  call       0x43efd0

// block 00447ad9
00447ad9  mov        eax, dword ptr [ebp + 0x66c]
00447adf  push       eax
00447ae0  lea        ebx, [esi - 0x71]
00447ae3  call       0x461970

// block 00447ae8
00447ae8  mov        edx, ebx
00447aea  mov        eax, ebp
00447aec  mov        dword ptr [ebp + 0x66c], 0
00447af6  call       0x43eee0

// block 00447afb
00447afb  lea        eax, [ebp + 0x28]
00447afe  call       0x464940

// block 00447b03
00447b03  pop        edi
00447b04  pop        esi
00447b05  pop        ebx

// block 00447b06
00447b06  mov        eax, 1
00447b0b  pop        ebp
00447b0c  ret        4
