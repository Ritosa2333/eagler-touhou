
// block 00438370
00438370  sub        esp, 0x18
00438373  mov        ecx, dword ptr [0x4b43c8]
00438379  mov        dword ptr [esi + 0xa28], 4
00438383  fld        dword ptr [esi + 0x97c]
00438389  fadd       qword ptr [0x4a3d70]
0043838f  mov        edx, dword ptr [ecx + 0x4debdc]
00438395  push       ebx
00438396  push       ebp
00438397  fadd       qword ptr [0x4a3d28]
0043839d  push       edi
0043839e  xor        ebp, ebp
004383a0  push       ebp
004383a1  fstp       dword ptr [esp + 0x1c]
004383a5  push       0x4f
004383a7  fld        dword ptr [esi + 0x980]
004383ad  lea        eax, [esp + 0x18]
004383b1  fadd       qword ptr [0x4a3d68]
004383b7  push       eax
004383b8  push       edx
004383b9  xor        eax, eax
004383bb  fstp       dword ptr [esp + 0x2c]
004383bf  xor        ecx, ecx
004383c1  fld        dword ptr [esi + 0x984]
004383c7  fstp       dword ptr [esp + 0x30]
004383cb  call       0x4615a0

// block 004383d0
004383d0  mov        eax, dword ptr [esp + 0x10]
004383d4  mov        edx, dword ptr [0x4ce8cc]
004383da  push       eax
004383db  call       0x461920

// block 004383e0
004383e0  mov        edi, dword ptr [esp + 0x20]
004383e4  cmp        eax, ebp
004383e6  je         0x438402

// block 004383e8
004383e8  mov        ecx, dword ptr [esp + 0x18]
004383ec  mov        edx, dword ptr [esp + 0x1c]
004383f0  mov        dword ptr [eax + 0x430], ecx
004383f6  mov        dword ptr [eax + 0x434], edx
004383fc  mov        dword ptr [eax + 0x438], edi

// block 00438402
00438402  mov        dword ptr [esp + 0x10], 0x20
0043840a  lea        ebx, [ebx]

// block 00438410
00438410  mov        ecx, dword ptr [0x4b43c8]
00438416  mov        edx, dword ptr [ecx + 0x4debdc]
0043841c  push       ebp
0043841d  push       0x50
0043841f  lea        eax, [esp + 0x1c]
00438423  push       eax
00438424  push       edx
00438425  xor        eax, eax
00438427  xor        ecx, ecx
00438429  call       0x4615a0

// block 0043842e
0043842e  mov        ecx, dword ptr [esp + 0x14]
00438432  cmp        ecx, ebp
00438434  je         0x438493

// block 00438436
00438436  mov        edx, dword ptr [0x4ce8cc]
0043843c  mov        eax, dword ptr [edx + 0x8856b8]
00438442  cmp        eax, ebp
00438444  je         0x438453

// block 00438446
00438446  mov        ebx, dword ptr [eax]
00438448  cmp        dword ptr [ebx], ecx
0043844a  je         0x43846f

// block 0043844c
0043844c  mov        eax, dword ptr [eax + 4]
0043844f  cmp        eax, ebp
00438451  jne        0x438446

// block 00438453
00438453  mov        eax, dword ptr [edx + 0x8856c0]
00438459  cmp        eax, ebp
0043845b  je         0x438493

// block 0043845d
0043845d  lea        ecx, [ecx]

// block 00438460
00438460  mov        edx, dword ptr [eax]
00438462  cmp        dword ptr [edx], ecx
00438464  je         0x438473

// block 00438466
00438466  mov        eax, dword ptr [eax + 4]
00438469  cmp        eax, ebp
0043846b  jne        0x438460

// block 0043846d
0043846d  jmp        0x438493

// block 0043846f
0043846f  mov        eax, ebx
00438471  jmp        0x438475

// block 00438473
00438473  mov        eax, edx

// block 00438475
00438475  cmp        eax, ebp
00438477  je         0x438493

// block 00438479
00438479  mov        ecx, dword ptr [esp + 0x18]
0043847d  mov        edx, dword ptr [esp + 0x1c]
00438481  mov        dword ptr [eax + 0x430], ecx
00438487  mov        dword ptr [eax + 0x434], edx
0043848d  mov        dword ptr [eax + 0x438], edi

// block 00438493
00438493  sub        dword ptr [esp + 0x10], 1
00438498  jne        0x438410

// block 0043849e
0043849e  mov        eax, dword ptr [esi + 0xa40]
004384a4  fldz       
004384a6  mov        ebx, 0xfff0bdc1
004384ab  mov        edi, 0x4b2ed0
004384b0  test       al, 1
004384b2  jne        0x4384d5

// block 004384b4
004384b4  or         eax, 1
004384b7  fst        dword ptr [esi + 0xa38]
004384bd  mov        dword ptr [esi + 0xa34], ebp
004384c3  mov        dword ptr [esi + 0xa30], ebx
004384c9  mov        dword ptr [esi + 0xa3c], edi
004384cf  mov        dword ptr [esi + 0xa40], eax

// block 004384d5
004384d5  mov        eax, dword ptr [0x4b44e8]
004384da  fst        dword ptr [esi + 0xa38]
004384e0  mov        dword ptr [esi + 0xa34], ebp
004384e6  mov        dword ptr [esi + 0xa30], 0xffffffff
004384f0  cmp        eax, ebp
004384f2  je         0x438507

// block 004384f4
004384f4  test       dword ptr [eax + 0x60], 0x200
004384fb  jne        0x438507

// block 004384fd
004384fd  mov        edx, 2
00438502  call       0x453d90

// block 00438507
00438507  mov        eax, dword ptr [esi + 0xc410]
0043850d  test       al, 1
0043850f  jne        0x438534

// block 00438511
00438511  or         eax, 1
00438514  fstp       dword ptr [esi + 0xc408]
0043851a  mov        dword ptr [esi + 0xc404], ebp
00438520  mov        dword ptr [esi + 0xc400], ebx
00438526  mov        dword ptr [esi + 0xc40c], edi
0043852c  mov        dword ptr [esi + 0xc410], eax
00438532  jmp        0x438536

// block 00438534
00438534  fstp       st(0)

// block 00438536
00438536  fld        dword ptr [0x4a4250]
0043853c  push       ebp
0043853d  lea        eax, [esi + 0x14]
00438540  fstp       dword ptr [esi + 0xc408]
00438546  mov        dword ptr [esi + 0xc404], 6
00438550  mov        dword ptr [esi + 0xc400], 5
0043855a  mov        ecx, dword ptr [esi + 0x10]
0043855d  push       eax
0043855e  call       0x454d10

// block 00438563
00438563  mov        ecx, dword ptr [0x4b43cc]
00438569  mov        eax, dword ptr [ecx + 0x7c]
0043856c  test       al, 1
0043856e  je         0x43859a

// block 00438570
00438570  cmp        dword ptr [ecx + 0x28], 0x3c
00438574  jl         0x438589

// block 00438576
00438576  and        eax, 0xffffffdd
00438579  mov        dword ptr [ecx + 0x80], ebp
0043857f  mov        dword ptr [ecx + 0x7c], eax
00438582  pop        edi
00438583  pop        ebp
00438584  pop        ebx
00438585  add        esp, 0x18
00438588  ret        

// block 00438589
00438589  mov        edx, dword ptr [0x4b43c4]
0043858f  cmp        dword ptr [edx + 0x3c], ebp
00438592  je         0x43859a

// block 00438594
00438594  or         eax, 0x20
00438597  mov        dword ptr [ecx + 0x7c], eax

// block 0043859a
0043859a  pop        edi
0043859b  pop        ebp
0043859c  pop        ebx
0043859d  add        esp, 0x18
004385a0  ret        
