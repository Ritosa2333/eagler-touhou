
// block 00408120
00408120  fld        dword ptr [0x4a3d78]
00408126  mov        eax, dword ptr [0x4b4514]
0040812b  fstp       dword ptr [edi + 0x514]
00408131  sub        esp, 0x10
00408134  fld        dword ptr [0x4a4114]
0040813a  push       ebx
0040813b  fstp       dword ptr [edi + 0x518]
00408141  mov        ecx, dword ptr [eax + 0x97c]
00408147  push       ebp
00408148  lea        ebp, [edi + 0x508]
0040814e  mov        dword ptr [ebp], ecx
00408151  mov        edx, dword ptr [eax + 0x980]
00408157  mov        dword ptr [ebp + 4], edx
0040815a  mov        eax, dword ptr [eax + 0x984]
00408160  push       esi
00408161  mov        edx, 0x33
00408166  mov        dword ptr [ebp + 8], eax
00408169  call       0x453d90

// block 0040816e
0040816e  mov        ecx, dword ptr [0x4b43cc]
00408174  mov        eax, dword ptr [ecx + 0x7c]
00408177  test       al, 1
00408179  je         0x4081a2

// block 0040817b
0040817b  cmp        dword ptr [ecx + 0x28], 0x3c
0040817f  jl         0x408190

// block 00408181
00408181  mov        dword ptr [ecx + 0x80], 0
0040818b  and        eax, 0xffffffdd
0040818e  jmp        0x40819f

// block 00408190
00408190  mov        edx, dword ptr [0x4b43c4]
00408196  cmp        dword ptr [edx + 0x3c], 0
0040819a  je         0x4081a2

// block 0040819c
0040819c  or         eax, 0x20

// block 0040819f
0040819f  mov        dword ptr [ecx + 0x7c], eax

// block 004081a2
004081a2  mov        eax, dword ptr [0x4b43dc]
004081a7  mov        esi, 1
004081ac  add        dword ptr [eax + 0x14], esi
004081af  mov        eax, dword ptr [edi + 0x500]
004081b5  test       eax, eax
004081b7  je         0x4081cc

// block 004081b9
004081b9  push       eax
004081ba  call       0x46c941

// block 004081bf
004081bf  add        esp, 4
004081c2  mov        dword ptr [edi + 0x500], 0

// block 004081cc
004081cc  push       0x5a0
004081d1  call       0x46d04a

// block 004081d6
004081d6  add        esp, 4
004081d9  push       0x5a0
004081de  push       0
004081e0  push       eax
004081e1  mov        dword ptr [edi + 0x500], eax
004081e7  call       0x477420

// block 004081ec
004081ec  mov        eax, dword ptr [0x4b43e4]
004081f1  mov        ebx, dword ptr [eax + 0x6d44]
004081f7  add        esp, 0xc
004081fa  test       dword ptr [0x4cee78], 0x8000
00408204  je         0x408217

// block 00408206
00408206  push       0x4cf1d0
0040820b  call       dword ptr [0x498088]

// block 00408211
00408211  inc        byte ptr [0x4cf221]

// block 00408217
00408217  add        dword ptr [ebx + 0x130], esi
0040821d  call       0x4621c0

// block 00408222
00408222  mov        esi, eax
00408224  or         dword ptr [esi + 0x480], 1
0040822b  mov        dword ptr [esi + 0x20], 0x17
00408232  test       ebp, ebp
00408234  jne        0x408264

// block 00408236
00408236  fldz       
00408238  fst        dword ptr [esp + 0x10]
0040823c  mov        ecx, dword ptr [esp + 0x10]
00408240  fst        dword ptr [esp + 0x14]
00408244  mov        edx, dword ptr [esp + 0x14]
00408248  fstp       dword ptr [esp + 0x18]
0040824c  mov        eax, dword ptr [esp + 0x18]
00408250  mov        dword ptr [esi + 0x430], ecx
00408256  mov        dword ptr [esi + 0x434], edx
0040825c  mov        dword ptr [esi + 0x438], eax
00408262  jmp        0x408291

// block 00408264
00408264  fld        dword ptr [ebp]
00408267  fadd       qword ptr [0x4a3d70]
0040826d  fadd       qword ptr [0x4a3d28]
00408273  fstp       dword ptr [esi + 0x430]
00408279  fld        dword ptr [ebp + 4]
0040827c  fadd       qword ptr [0x4a3d68]
00408282  fstp       dword ptr [esi + 0x434]
00408288  fld        dword ptr [ebp + 8]
0040828b  fstp       dword ptr [esi + 0x438]

// block 00408291
00408291  push       1
00408293  push       esi
00408294  mov        ecx, ebx
00408296  call       0x454d10

// block 0040829b
0040829b  mov        ebx, esi
0040829d  lea        eax, [esp + 0xc]
004082a1  call       0x461250

// block 004082a6
004082a6  test       dword ptr [0x4cee78], 0x8000
004082b0  mov        esi, dword ptr [eax]
004082b2  je         0x4082c5

// block 004082b4
004082b4  push       0x4cf1d0
004082b9  call       dword ptr [0x49808c]

// block 004082bf
004082bf  dec        byte ptr [0x4cf221]

// block 004082c5
004082c5  mov        dword ptr [edi + 0x48], esi
004082c8  pop        esi
004082c9  pop        ebp
004082ca  xor        eax, eax
004082cc  pop        ebx
004082cd  add        esp, 0x10
004082d0  ret        
