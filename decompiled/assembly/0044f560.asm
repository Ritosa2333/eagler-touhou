
// block 0044f560
0044f560  push       ebp
0044f561  mov        ebp, esp
0044f563  and        esp, 0xfffffff8
0044f566  sub        esp, 0x12c
0044f56c  mov        eax, dword ptr [0x4ad138]
0044f571  xor        eax, esp
0044f573  mov        dword ptr [esp + 0x128], eax
0044f57a  mov        eax, dword ptr [ebp + 8]
0044f57d  push       ebx
0044f57e  push       esi
0044f57f  push       edi
0044f580  xor        ebx, ebx
0044f582  push       1
0044f584  mov        dword ptr [esp + 0x14], eax
0044f588  mov        dword ptr [esp + 0x10], ebx
0044f58c  mov        dword ptr [0x4cf3f8], eax
0044f591  call       dword ptr [0x498294]

// block 0044f597
0044f597  push       4
0044f599  call       0x46c9ea

// block 0044f59e
0044f59e  add        esp, 4
0044f5a1  cmp        eax, ebx
0044f5a3  je         0x44f5a9

// block 0044f5a5
0044f5a5  mov        dword ptr [eax], ebx
0044f5a7  jmp        0x44f5ab

// block 0044f5a9
0044f5a9  xor        eax, eax

// block 0044f5ab
0044f5ab  or         dword ptr [0x4cee78], 0x8000
0044f5b5  mov        dword ptr [0x4ce898], eax
0044f5ba  mov        edi, 0x4cf0f8
0044f5bf  mov        esi, 0xc

// block 0044f5c4
0044f5c4  push       edi
0044f5c5  call       dword ptr [0x498090]

// block 0044f5cb
0044f5cb  add        edi, 0x18
0044f5ce  sub        esi, 1
0044f5d1  jne        0x44f5c4

// block 0044f5d3
0044f5d3  push       0x4a24cc
0044f5d8  mov        ecx, 0x4b0ec8
0044f5dd  call       0x464220

// block 0044f5e2
0044f5e2  add        esp, 4
0044f5e5  push       0x4a2974
0044f5ea  push       1
0044f5ec  push       ebx
0044f5ed  call       dword ptr [0x4980e8]

// block 0044f5f3
0044f5f3  mov        dword ptr [0x4d4ea8], eax
0044f5f8  call       dword ptr [0x4980e4]

// block 0044f5fe
0044f5fe  cmp        eax, 0xb7
0044f603  jne        0x44f61c

// block 0044f605
0044f605  push       0x4a2984
0044f60a  mov        edi, 0x4b0ec8
0044f60f  call       0x464300

// block 0044f614
0044f614  add        esp, 4
0044f617  jmp        0x44fb99

// block 0044f61c
0044f61c  call       0x451530

// block 0044f621
0044f621  cmp        eax, -1
0044f624  je         0x44fb99

// block 0044f62a
0044f62a  mov        esi, dword ptr [esp + 0x10]
0044f62e  mov        dword ptr [0x4ce8e8], esi
0044f634  call       0x44ffb0

// block 0044f639
0044f639  push       0x4ce8e8
0044f63e  call       0x42feb0

// block 0044f643
0044f643  test       eax, eax
0044f645  jne        0x44fb99

// block 0044f64b
0044f64b  lea        eax, [esp + 0x30]
0044f64f  push       eax
0044f650  call       dword ptr [0x498244]

// block 0044f656
0044f656  test       dword ptr [0x4ceae8], 0x100
0044f660  jne        0x44f669

// block 0044f662
0044f662  test       byte ptr [esp + 0x40], 0x80
0044f667  je         0x44f67c

// block 0044f669
0044f669  push       ebx
0044f66a  push       0x4518c0
0044f66f  push       ebx
0044f670  push       0xcb
0044f675  push       esi
0044f676  call       dword ptr [0x498204]

// block 0044f67c
0044f67c  test       byte ptr [0x4cf428], 0x60
0044f683  jne        0x44fb99

// block 0044f689
0044f689  movzx      ecx, byte ptr [0x4ceacd]
0044f690  mov        eax, dword ptr [0x4cf428]
0044f695  add        ecx, ecx
0044f697  add        ecx, ecx
0044f699  xor        ecx, eax
0044f69b  and        ecx, 0xc
0044f69e  xor        eax, ecx
0044f6a0  mov        dword ptr [0x4cf428], eax
0044f6a5  call       0x4516a0

// block 0044f6aa
0044f6aa  push       0x4c
0044f6ac  call       0x46c9ea

// block 0044f6b1
0044f6b1  add        esp, 4
0044f6b4  cmp        eax, ebx
0044f6b6  je         0x44f6f3

// block 0044f6b8
0044f6b8  mov        dword ptr [eax + 8], ebx
0044f6bb  mov        dword ptr [eax + 0xc], ebx
0044f6be  mov        dword ptr [eax + 0x10], ebx
0044f6c1  mov        dword ptr [eax], ebx
0044f6c3  lea        ecx, [eax + 0x24]
0044f6c6  mov        edx, 0xfffffffe
0044f6cb  and        dword ptr [eax + 4], edx
0044f6ce  mov        dword ptr [eax + 0x14], eax
0044f6d1  mov        dword ptr [eax + 0x18], ebx
0044f6d4  mov        dword ptr [eax + 0x1c], ebx
0044f6d7  and        dword ptr [ecx + 4], edx
0044f6da  mov        dword ptr [ecx + 8], ebx
0044f6dd  mov        dword ptr [ecx + 0xc], ebx
0044f6e0  mov        dword ptr [ecx + 0x10], ebx
0044f6e3  mov        dword ptr [ecx], ebx
0044f6e5  mov        dword ptr [ecx + 0x14], ecx
0044f6e8  mov        dword ptr [ecx + 0x18], ebx
0044f6eb  mov        dword ptr [ecx + 0x1c], ebx
0044f6ee  mov        dword ptr [eax + 0x48], ebx
0044f6f1  jmp        0x44f6f5

// block 0044f6f3
0044f6f3  xor        eax, eax

// block 0044f6f5
0044f6f5  push       0x20
0044f6f7  mov        dword ptr [0x4ce89c], eax
0044f6fc  call       0x46c6b6

// block 0044f701
0044f701  mov        dword ptr [0x4ce8ec], eax
0044f706  cmp        eax, ebx
0044f708  jne        0x44f721

// block 0044f70a
0044f70a  push       0x4a2664
0044f70f  mov        edi, 0x4b0ec8
0044f714  call       0x464300

// block 0044f719
0044f719  add        esp, 4
0044f71c  jmp        0x44fb99

// block 0044f721
0044f721  mov        ebx, esi
0044f723  call       0x450a70

// block 0044f728
0044f728  test       eax, eax
0044f72a  jne        0x44fb97

// block 0044f730
0044f730  call       0x4629b0

// block 0044f735
0044f735  call       0x463980

// block 0044f73a
0044f73a  call       0x451d20

// block 0044f73f
0044f73f  mov        esi, 0x4b0e6c
0044f744  call       0x464c40

// block 0044f749
0044f749  mov        edx, dword ptr [0x4cf3f0]
0044f74f  push       edx
0044f750  call       0x452fd0

// block 0044f755
0044f755  call       0x450cc0

// block 0044f75a
0044f75a  test       eax, eax
0044f75c  jne        0x44fb97

// block 0044f762
0044f762  push       0x88ed54
0044f767  call       0x46c9ea

// block 0044f76c
0044f76c  xor        ebx, ebx
0044f76e  add        esp, 4
0044f771  cmp        eax, ebx
0044f773  je         0x44f77d

// block 0044f775
0044f775  push       eax
0044f776  call       0x45dfa0

// block 0044f77b
0044f77b  jmp        0x44f77f

// block 0044f77d
0044f77d  xor        eax, eax

// block 0044f77f
0044f77f  mov        dword ptr [0x4ce8cc], eax
0044f784  cmp        dword ptr [0x4ce9fc], ebx
0044f78a  jne        0x44f7a5

// block 0044f78c
0044f78c  push       ebx
0044f78d  push       ebx
0044f78e  call       0x46c830

// block 0044f793
0044f793  push       ebx
0044f794  call       dword ptr [0x498240]

// block 0044f79a
0044f79a  test       eax, eax
0044f79c  jge        0x44f793

// block 0044f79e
0044f79e  push       ebx
0044f79f  call       dword ptr [0x49822c]

// block 0044f7a5
0044f7a5  fldz       
0044f7a7  fstp       qword ptr [0x4cf448]
0044f7ad  call       0x4508b0

// block 0044f7b2
0044f7b2  fst        qword ptr [0x4cf440]
0044f7b8  fst        qword ptr [0x4cf430]
0044f7be  fstp       qword ptr [0x4cf438]
0044f7c4  call       0x4508b0

// block 0044f7c9
0044f7c9  fst        qword ptr [0x4cf458]
0044f7cf  mov        eax, dword ptr [0x4cf3f0]
0044f7d4  fstp       qword ptr [0x4cf450]
0044f7da  push       eax
0044f7db  call       dword ptr [0x498234]

// block 0044f7e1
0044f7e1  call       0x42f8c0

// block 0044f7e6
0044f7e6  mov        dword ptr [esp + 0xc], eax
0044f7ea  cmp        eax, ebx
0044f7ec  je         0x44f804

// block 0044f7ee
0044f7ee  cmp        eax, -1
0044f7f1  je         0x44fb2d

// block 0044f7f7
0044f7f7  mov        dword ptr [esp + 0xc], 2
0044f7ff  jmp        0x44fb2d

// block 0044f804
0044f804  mov        esi, 1
0044f809  or         dword ptr [0x4cf428], esi
0044f80f  mov        dword ptr [esp + 0xc], ebx
0044f813  mov        byte ptr [0x4cf404], 0xfc
0044f81a  cmp        dword ptr [0x4cf3f4], ebx
0044f820  jne        0x44fb2d

// block 0044f826
0044f826  jmp        0x44f82d

// block 0044f828
0044f828  mov        esi, 1

// block 0044f82d
0044f82d  push       esi
0044f82e  push       ebx
0044f82f  push       ebx
0044f830  push       ebx
0044f831  lea        ecx, [esp + 0x24]
0044f835  push       ecx
0044f836  call       dword ptr [0x498254]

// block 0044f83c
0044f83c  test       eax, eax
0044f83e  je         0x44f85b

// block 0044f840
0044f840  lea        edx, [esp + 0x14]
0044f844  push       edx
0044f845  call       dword ptr [0x49823c]

// block 0044f84b
0044f84b  lea        eax, [esp + 0x14]
0044f84f  push       eax
0044f850  call       dword ptr [0x498270]

// block 0044f856
0044f856  jmp        0x44fb21

// block 0044f85b
0044f85b  mov        eax, dword ptr [0x4ce8f0]
0044f860  mov        ecx, dword ptr [eax]
0044f862  mov        edx, dword ptr [ecx + 0xc]
0044f865  push       eax
0044f866  call       edx

// block 0044f868
0044f868  mov        cl, byte ptr [0x4cf428]
0044f86e  cmp        eax, ebx
0044f870  jne        0x44f8c7

// block 0044f872
0044f872  test       cl, 2
0044f875  jne        0x44f8d2

// block 0044f877
0044f877  test       cl, 0x10
0044f87a  je         0x44f888

// block 0044f87c
0044f87c  push       0x4cf3f0
0044f881  call       0x450080

// block 0044f886
0044f886  jmp        0x44f8af

// block 0044f888
0044f888  cmp        dword ptr [0x4cea10], esi
0044f88e  jne        0x44f8a5

// block 0044f890
0044f890  cmp        byte ptr [0x4ceace], 0
0044f897  jne        0x44f8a5

// block 0044f899
0044f899  push       0x4cf3f0
0044f89e  call       0x450600

// block 0044f8a3
0044f8a3  jmp        0x44f8af

// block 0044f8a5
0044f8a5  push       0x4cf3f0
0044f8aa  call       0x4503f0

// block 0044f8af
0044f8af  mov        dword ptr [esp + 0xc], eax
0044f8b3  cmp        eax, ebx
0044f8b5  jne        0x44fb2d

// block 0044f8bb
0044f8bb  and        dword ptr [0x4cee78], 0xffffffef
0044f8c2  jmp        0x44fb21

// block 0044f8c7
0044f8c7  cmp        eax, 0x88760869
0044f8cc  jne        0x44fb21

// block 0044f8d2
0044f8d2  mov        dword ptr [0x4cf42c], 0xa
0044f8dc  test       cl, 2
0044f8df  je         0x44f98f

// block 0044f8e5
0044f8e5  test       cl, 0xc
0044f8e8  jne        0x44f94e

// block 0044f8ea
0044f8ea  mov        eax, dword ptr [0x4cf3f0]
0044f8ef  push       0x4ce8f8
0044f8f4  push       eax
0044f8f5  call       dword ptr [0x498230]

// block 0044f8fb
0044f8fb  mov        ecx, 0xe
0044f900  mov        esi, 0x4ce9dc
0044f905  mov        edi, 0x4cea14

// block 0044f90a
0044f90a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0044f90c
0044f90c  mov        cl, byte ptr [0x4cf428]
0044f912  and        cl, 0x10
0044f915  movzx      edx, cl
0044f918  neg        edx
0044f91a  sbb        edx, edx
0044f91c  and        edx, 0x7fffffff
0044f922  xor        eax, eax
0044f924  inc        edx
0044f925  cmp        byte ptr [0x4ceaca], al
0044f92b  mov        dword ptr [0x4cea0c], 0x3c
0044f935  setne      al
0044f938  mov        dword ptr [0x4cea10], edx
0044f93e  mov        dword ptr [0x4ce9fc], ebx
0044f944  add        eax, 0x16
0044f947  mov        dword ptr [0x4ce9e4], eax
0044f94c  jmp        0x44f98f

// block 0044f94e
0044f94e  mov        edx, dword ptr [0x4cea90]
0044f954  mov        dword ptr [0x4ce9e4], edx
0044f95a  mov        dword ptr [0x4cea0c], ebx
0044f960  test       cl, 0x10
0044f963  je         0x44f971

// block 0044f965
0044f965  mov        dword ptr [0x4cea10], 0x80000000
0044f96f  jmp        0x44f989

// block 0044f971
0044f971  mov        eax, dword ptr [0x4cea8c]
0044f976  sub        eax, 0x3c
0044f979  neg        eax
0044f97b  sbb        eax, eax
0044f97d  and        eax, 0x7fffffff
0044f982  add        eax, esi
0044f984  mov        dword ptr [0x4cea10], eax

// block 0044f989
0044f989  mov        dword ptr [0x4ce9fc], esi

// block 0044f98f
0044f98f  call       0x431700

// block 0044f994
0044f994  call       0x44f370

// block 0044f999
0044f999  mov        eax, dword ptr [0x4ce8f0]
0044f99e  mov        ecx, dword ptr [eax]
0044f9a0  mov        edx, dword ptr [ecx + 0x40]
0044f9a3  push       0x4ce9dc
0044f9a8  push       eax
0044f9a9  call       edx

// block 0044f9ab
0044f9ab  cmp        eax, ebx
0044f9ad  je         0x44f9bf

// block 0044f9af
0044f9af  cmp        eax, 0x88760868
0044f9b4  jne        0x44fb2d

// block 0044f9ba
0044f9ba  jmp        0x44fb21

// block 0044f9bf
0044f9bf  call       0x451200

// block 0044f9c4
0044f9c4  call       0x44f400

// block 0044f9c9
0044f9c9  or         dword ptr [0x4cee78], 0x10
0044f9d0  test       byte ptr [0x4cf428], 2
0044f9d7  mov        dword ptr [0x4cee5c], 3
0044f9e1  je         0x44fb0e

// block 0044f9e7
0044f9e7  mov        eax, dword ptr [0x4cf428]
0044f9ec  shr        eax, 2
0044f9ef  and        eax, 3
0044f9f2  jbe        0x44faae

// block 0044f9f8
0044f9f8  mov        esi, dword ptr [0x498278]
0044f9fe  push       7
0044fa00  cmp        eax, 3
0044fa03  jne        0x44fa1b

// block 0044fa05
0044fa05  call       esi

// block 0044fa07
0044fa07  push       8
0044fa09  lea        edi, [eax + eax + 0x500]
0044fa10  call       esi

// block 0044fa12
0044fa12  lea        ebx, [eax + eax + 0x3c0]
0044fa19  jmp        0x44fa4a

// block 0044fa1b
0044fa1b  cmp        eax, 2
0044fa1e  jne        0x44fa36

// block 0044fa20
0044fa20  call       esi

// block 0044fa22
0044fa22  push       8
0044fa24  lea        edi, [eax + eax + 0x3c0]
0044fa2b  call       esi

// block 0044fa2d
0044fa2d  lea        ebx, [eax + eax + 0x2d0]
0044fa34  jmp        0x44fa4a

// block 0044fa36
0044fa36  call       esi

// block 0044fa38
0044fa38  push       8
0044fa3a  lea        edi, [eax + eax + 0x280]
0044fa41  call       esi

// block 0044fa43
0044fa43  lea        ebx, [eax + eax + 0x1e0]

// block 0044fa4a
0044fa4a  push       4
0044fa4c  call       esi

// block 0044fa4e
0044fa4e  push       0x10cb0000
0044fa53  mov        esi, eax
0044fa55  mov        eax, dword ptr [0x4cf3f0]
0044fa5a  push       -0x10
0044fa5c  push       eax
0044fa5d  add        esi, ebx
0044fa5f  call       dword ptr [0x498248]

// block 0044fa65
0044fa65  mov        ecx, dword ptr [0x4ce8fc]
0044fa6b  mov        edx, dword ptr [0x4ce8f8]
0044fa71  mov        eax, dword ptr [0x4cf3f0]
0044fa76  push       0x60
0044fa78  push       esi
0044fa79  push       edi
0044fa7a  push       ecx
0044fa7b  push       edx
0044fa7c  push       0
0044fa7e  push       eax
0044fa7f  call       dword ptr [0x498264]

// block 0044fa85
0044fa85  mov        ecx, dword ptr [0x4cf3f0]
0044fa8b  push       1
0044fa8d  push       ecx
0044fa8e  call       dword ptr [0x498268]

// block 0044fa94
0044fa94  push       1
0044fa96  push       0
0044fa98  call       0x46c830

// block 0044fa9d
0044fa9d  lea        ecx, [ecx]

// block 0044faa0
0044faa0  push       1
0044faa2  call       dword ptr [0x498240]

// block 0044faa8
0044faa8  test       eax, eax
0044faaa  jl         0x44faa0

// block 0044faac
0044faac  jmp        0x44fb0e

// block 0044faae
0044faae  mov        edx, dword ptr [0x4cf3f0]
0044fab4  push       0x90000000
0044fab9  push       -0x10
0044fabb  push       edx
0044fabc  call       dword ptr [0x498248]

// block 0044fac2
0044fac2  mov        eax, dword ptr [0x4cf3f0]
0044fac7  push       0x20
0044fac9  push       0x1e0
0044face  push       0x280
0044fad3  push       0
0044fad5  push       0
0044fad7  push       0
0044fad9  push       eax
0044fada  call       dword ptr [0x498264]

// block 0044fae0
0044fae0  push       0
0044fae2  push       0
0044fae4  call       0x46c830

// block 0044fae9
0044fae9  lea        esp, [esp]

// block 0044faf0
0044faf0  push       0
0044faf2  call       dword ptr [0x498240]

// block 0044faf8
0044faf8  test       eax, eax
0044fafa  jge        0x44faf0

// block 0044fafc
0044fafc  push       0
0044fafe  call       dword ptr [0x49822c]

// block 0044fb04
0044fb04  mov        dword ptr [0x4cf400], 0

// block 0044fb0e
0044fb0e  mov        eax, 0x4ce8e8
0044fb13  call       0x431630

// block 0044fb18
0044fb18  and        dword ptr [0x4cf428], 0xfffffffd
0044fb1f  xor        ebx, ebx

// block 0044fb21
0044fb21  cmp        dword ptr [0x4cf3f4], ebx
0044fb27  je         0x44f828

// block 0044fb2d
0044fb2d  mov        ecx, dword ptr [0x4cf428]
0044fb33  shr        ecx, 2
0044fb36  and        cl, 3
0044fb39  mov        byte ptr [0x4ceacd], cl
0044fb3f  je         0x44fb69

// block 0044fb41
0044fb41  mov        edx, dword ptr [0x4cf3f0]
0044fb47  push       0x4ce8f8
0044fb4c  push       edx
0044fb4d  call       dword ptr [0x498230]

// block 0044fb53
0044fb53  mov        eax, dword ptr [0x4ce8f8]
0044fb58  mov        ecx, dword ptr [0x4ce8fc]
0044fb5e  mov        dword ptr [0x4cead8], eax
0044fb63  mov        dword ptr [0x4ceadc], ecx

// block 0044fb69
0044fb69  mov        ebx, 0x4ce8e8
0044fb6e  call       0x42f650

// block 0044fb73
0044fb73  mov        eax, dword ptr [0x4ce89c]
0044fb78  mov        esi, eax
0044fb7a  test       eax, eax
0044fb7c  je         0x44fb8d

// block 0044fb7e
0044fb7e  push       eax
0044fb7f  call       0x44f200

// block 0044fb84
0044fb84  push       esi
0044fb85  call       0x46ca4f

// block 0044fb8a
0044fb8a  add        esp, 4

// block 0044fb8d
0044fb8d  mov        dword ptr [0x4ce89c], 0

// block 0044fb97
0044fb97  xor        ebx, ebx

// block 0044fb99
0044fb99  mov        edi, 2
0044fb9e  mov        dword ptr [0x4d4770], edi
0044fba4  call       0x453030

// block 0044fba9
0044fba9  call       0x453500

// block 0044fbae
0044fbae  mov        eax, dword ptr [0x4ce8cc]
0044fbb3  mov        esi, eax
0044fbb5  cmp        eax, ebx
0044fbb7  je         0x44fbc8

// block 0044fbb9
0044fbb9  push       eax
0044fbba  call       0x45ea40

// block 0044fbbf
0044fbbf  push       esi
0044fbc0  call       0x46ca4f

// block 0044fbc5
0044fbc5  add        esp, 4

// block 0044fbc8
0044fbc8  mov        eax, dword ptr [0x4cea94]
0044fbcd  mov        dword ptr [0x4ce8cc], ebx
0044fbd3  cmp        eax, ebx
0044fbd5  je         0x44fbe5

// block 0044fbd7
0044fbd7  mov        edx, dword ptr [eax]
0044fbd9  push       eax
0044fbda  mov        eax, dword ptr [edx + 8]
0044fbdd  call       eax

// block 0044fbdf
0044fbdf  mov        dword ptr [0x4cea94], ebx

// block 0044fbe5
0044fbe5  mov        eax, dword ptr [0x4cea98]
0044fbea  cmp        eax, ebx
0044fbec  je         0x44fbfc

// block 0044fbee
0044fbee  mov        ecx, dword ptr [eax]
0044fbf0  mov        edx, dword ptr [ecx + 8]
0044fbf3  push       eax
0044fbf4  call       edx

// block 0044fbf6
0044fbf6  mov        dword ptr [0x4cea98], ebx

// block 0044fbfc
0044fbfc  mov        eax, dword ptr [0x4cea9c]
0044fc01  cmp        eax, ebx
0044fc03  je         0x44fc13

// block 0044fc05
0044fc05  mov        ecx, dword ptr [eax]
0044fc07  mov        edx, dword ptr [ecx + 8]
0044fc0a  push       eax
0044fc0b  call       edx

// block 0044fc0d
0044fc0d  mov        dword ptr [0x4cea9c], ebx

// block 0044fc13
0044fc13  mov        eax, dword ptr [0x4ce8f0]
0044fc18  cmp        eax, ebx
0044fc1a  je         0x44fc2a

// block 0044fc1c
0044fc1c  mov        ecx, dword ptr [eax]
0044fc1e  mov        edx, dword ptr [ecx + 8]
0044fc21  push       eax
0044fc22  call       edx

// block 0044fc24
0044fc24  mov        dword ptr [0x4ce8f0], ebx

// block 0044fc2a
0044fc2a  mov        eax, dword ptr [0x4ce8ec]
0044fc2f  cmp        eax, ebx
0044fc31  je         0x44fc41

// block 0044fc33
0044fc33  mov        ecx, dword ptr [eax]
0044fc35  mov        edx, dword ptr [ecx + 8]
0044fc38  push       eax
0044fc39  call       edx

// block 0044fc3b
0044fc3b  mov        dword ptr [0x4ce8ec], ebx

// block 0044fc41
0044fc41  mov        eax, dword ptr [0x4cf3f0]
0044fc46  cmp        eax, ebx
0044fc48  je         0x44fc76

// block 0044fc4a
0044fc4a  push       ebx
0044fc4b  push       eax
0044fc4c  call       dword ptr [0x498268]

// block 0044fc52
0044fc52  mov        eax, dword ptr [0x4cf3f0]
0044fc57  push       ebx
0044fc58  push       ebx
0044fc59  push       ebx
0044fc5a  push       ebx
0044fc5b  push       ebx
0044fc5c  push       eax
0044fc5d  call       dword ptr [0x49820c]

// block 0044fc63
0044fc63  mov        ecx, dword ptr [0x4cf3f0]
0044fc69  push       ecx
0044fc6a  call       dword ptr [0x498228]

// block 0044fc70
0044fc70  mov        dword ptr [0x4cf3f0], ebx

// block 0044fc76
0044fc76  push       1
0044fc78  call       dword ptr [0x498240]

// block 0044fc7e
0044fc7e  test       eax, eax
0044fc80  jl         0x44fc76

// block 0044fc82
0044fc82  cmp        dword ptr [esp + 0xc], edi
0044fc86  jne        0x44fcff

// block 0044fc88
0044fc88  mov        ecx, 0x4b0ec8
0044fc8d  push       0x4a260c
0044fc92  mov        dword ptr [0x4b2ec8], ecx
0044fc98  mov        byte ptr [0x4b0ec8], bl
0044fc9e  call       0x464220

// block 0044fca3
0044fca3  add        esp, 4
0044fca6  cmp        dword ptr [0x4ce9fc], ebx
0044fcac  jne        0x44fcb6

// block 0044fcae
0044fcae  push       1
0044fcb0  push       ebx
0044fcb1  call       0x46c830

// block 0044fcb6
0044fcb6  mov        edi, dword ptr [0x498254]
0044fcbc  mov        esi, 0x3c

// block 0044fcc1
0044fcc1  push       1
0044fcc3  push       ebx
0044fcc4  push       ebx
0044fcc5  push       ebx
0044fcc6  lea        edx, [esp + 0x24]
0044fcca  push       edx
0044fccb  call       edi

// block 0044fccd
0044fccd  test       eax, eax
0044fccf  je         0x44fce7

// block 0044fcd1
0044fcd1  lea        eax, [esp + 0x14]
0044fcd5  push       eax
0044fcd6  call       dword ptr [0x49823c]

// block 0044fcdc
0044fcdc  lea        ecx, [esp + 0x14]
0044fce0  push       ecx
0044fce1  call       dword ptr [0x498270]

// block 0044fce7
0044fce7  sub        esi, 1
0044fcea  jne        0x44fcc1

// block 0044fcec
0044fcec  and        dword ptr [0x4cee78], 0xfffffe7f
0044fcf6  mov        esi, dword ptr [esp + 0x10]
0044fcfa  jmp        0x44f6aa

// block 0044fcff
0044fcff  push       0x4ceab0
0044fd04  mov        ebx, 0x3c
0044fd09  mov        edi, 0x4a250c
0044fd0e  call       0x463e80

// block 0044fd13
0044fd13  push       1
0044fd15  call       dword ptr [0x498290]

// block 0044fd1b
0044fd1b  call       0x44f2c0

// block 0044fd20
0044fd20  and        dword ptr [0x4cee78], 0xffff7fff
0044fd2a  lea        edi, [ebx - 0x30]
0044fd2d  mov        ebx, dword ptr [0x498094]
0044fd33  mov        esi, 0x4cf0f8

// block 0044fd38
0044fd38  push       esi
0044fd39  call       ebx

// block 0044fd3b
0044fd3b  add        esi, 0x18
0044fd3e  sub        edi, 1
0044fd41  jne        0x44fd38

// block 0044fd43
0044fd43  mov        edx, dword ptr [0x4cf41c]
0044fd49  mov        esi, dword ptr [0x498274]
0044fd4f  push       2
0044fd51  push       edi
0044fd52  push       edx
0044fd53  push       0x11
0044fd55  call       esi

// block 0044fd57
0044fd57  mov        eax, dword ptr [0x4cf420]
0044fd5c  push       2
0044fd5e  push       edi
0044fd5f  push       eax
0044fd60  push       0x55
0044fd62  call       esi

// block 0044fd64
0044fd64  mov        ecx, dword ptr [0x4cf424]
0044fd6a  push       2
0044fd6c  push       edi
0044fd6d  push       ecx
0044fd6e  push       0x56
0044fd70  call       esi

// block 0044fd72
0044fd72  push       1
0044fd74  push       edi
0044fd75  call       0x46c830

// block 0044fd7a
0044fd7a  mov        eax, dword ptr [0x4ce898]
0044fd7f  test       eax, eax
0044fd81  je         0x44fd8c

// block 0044fd83
0044fd83  push       eax
0044fd84  call       0x46ca4f

// block 0044fd89
0044fd89  add        esp, 4

// block 0044fd8c
0044fd8c  mov        ecx, dword ptr [esp + 0x134]
0044fd93  pop        edi
0044fd94  pop        esi
0044fd95  pop        ebx
0044fd96  xor        ecx, esp
0044fd98  xor        eax, eax
0044fd9a  call       0x46c932

// block 0044fd9f
0044fd9f  mov        esp, ebp
0044fda1  pop        ebp
0044fda2  ret        0x10
