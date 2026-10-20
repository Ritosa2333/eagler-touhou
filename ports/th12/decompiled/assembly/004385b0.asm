
// block 004385b0
004385b0  sub        esp, 0x38
004385b3  mov        ecx, dword ptr [0x4b0c90]
004385b9  mov        eax, dword ptr [0x4b0c94]
004385be  lea        eax, [eax + ecx*2]
004385c1  mov        ecx, dword ptr [0x4b0c48]
004385c7  shl        eax, 6
004385ca  push       ebx
004385cb  add        eax, 0x4b31d8
004385d0  push       ebp
004385d1  mov        ebp, dword ptr [esp + 0x44]
004385d5  mov        dword ptr [esp + 0x44], eax
004385d9  mov        eax, ecx
004385db  cdq        
004385dc  idiv       dword ptr [0x4b0cd4]
004385e2  cmp        ecx, dword ptr [0x4b0cd0]
004385e8  push       esi
004385e9  push       edi
004385ea  mov        esi, eax
004385ec  mov        dword ptr [esp + 0x10], esi
004385f0  jl         0x438716

// block 004385f6
004385f6  test       esi, esi
004385f8  jle        0x43873c

// block 004385fe
004385fe  lea        ebx, [ebp + 0x8314]
00438604  mov        dword ptr [esp + 0x14], esi

// block 00438608
00438608  mov        ecx, dword ptr [ebx]
0043860a  test       ecx, ecx
0043860c  je         0x43867f

// block 0043860e
0043860e  mov        edx, dword ptr [0x4ce8cc]
00438614  mov        eax, dword ptr [edx + 0x8856b8]
0043861a  test       eax, eax
0043861c  je         0x43862d

// block 0043861e
0043861e  mov        edi, edi

// block 00438620
00438620  mov        edi, dword ptr [eax]
00438622  cmp        dword ptr [edi], ecx
00438624  je         0x438646

// block 00438626
00438626  mov        eax, dword ptr [eax + 4]
00438629  test       eax, eax
0043862b  jne        0x438620

// block 0043862d
0043862d  mov        eax, dword ptr [edx + 0x8856c0]
00438633  test       eax, eax
00438635  je         0x43867f

// block 00438637
00438637  mov        edx, dword ptr [eax]
00438639  cmp        dword ptr [edx], ecx
0043863b  je         0x43864a

// block 0043863d
0043863d  mov        eax, dword ptr [eax + 4]
00438640  test       eax, eax
00438642  jne        0x438637

// block 00438644
00438644  jmp        0x43867f

// block 00438646
00438646  mov        eax, edi
00438648  jmp        0x43864c

// block 0043864a
0043864a  mov        eax, edx

// block 0043864c
0043864c  test       eax, eax
0043864e  je         0x43867f

// block 00438650
00438650  mov        edx, 0x10000000
00438655  or         dword ptr [eax + 0x47c], edx
0043865b  cmp        dword ptr [eax + 0x18], 0
0043865f  jne        0x43867f

// block 00438661
00438661  mov        eax, dword ptr [eax + 0x14]
00438664  test       eax, eax
00438666  je         0x43867f

// block 00438668
00438668  jmp        0x438670

// block 00438670
00438670  mov        ecx, dword ptr [eax]
00438672  or         dword ptr [ecx + 0x47c], edx
00438678  mov        eax, dword ptr [eax + 4]
0043867b  test       eax, eax
0043867d  jne        0x438670

// block 0043867f
0043867f  mov        dword ptr [ebx], 0
00438685  mov        eax, dword ptr [0x4b0c94]
0043868a  mov        ecx, dword ptr [0x4b0c90]
00438690  lea        eax, [eax + ecx*2]
00438693  cmp        eax, 5
00438696  ja         0x438703

// block 00438698
00438698  jmp        dword ptr [eax*4 + 0x438d30]

// block 00438703
00438703  add        ebx, 0xe4
00438709  sub        dword ptr [esp + 0x14], 1
0043870e  jne        0x438608

// block 00438714
00438714  jmp        0x43873c

// block 00438716
00438716  lea        edi, [ebp + 0x8314]
0043871c  mov        esi, 8

// block 00438721
00438721  mov        edx, dword ptr [edi]
00438723  push       edx
00438724  mov        ebx, 1
00438729  call       0x461970

// block 0043872e
0043872e  add        edi, 0xe4
00438734  sub        esi, ebx
00438736  jne        0x438721

// block 00438738
00438738  mov        esi, dword ptr [esp + 0x10]

// block 0043873c
0043873c  cmp        dword ptr [ebp + 0xc41c], 0
00438743  jne        0x438756

// block 00438745
00438745  fldz       
00438747  push       ecx
00438748  fstp       dword ptr [esp]
0043874b  call       0x4646e0

// block 00438750
00438750  fstp       dword ptr [ebp + 0xc48c]

// block 00438756
00438756  cmp        dword ptr [ebp + 0xc41c], esi
0043875c  je         0x438d24

// block 00438762
00438762  xor        edi, edi
00438764  test       esi, esi
00438766  jle        0x438ca8

// block 0043876c
0043876c  fld        qword ptr [0x4a3d48]
00438772  mov        ebx, dword ptr [esp + 0x10]
00438776  lea        esi, [ebp + 0x82d0]

// block 0043877c
0043877c  mov        eax, dword ptr [ebp + 0x988]
00438782  mov        dword ptr [esi - 0x14], eax
00438785  mov        ecx, dword ptr [ebp + 0x98c]
0043878b  mov        dword ptr [esi - 0x10], ecx
0043878e  mov        ecx, dword ptr [esi + 0x40]
00438791  test       ecx, ecx
00438793  je         0x4387fd

// block 00438795
00438795  mov        edx, dword ptr [0x4ce8cc]
0043879b  mov        eax, dword ptr [edx + 0x8856b8]
004387a1  test       eax, eax
004387a3  je         0x4387b2

// block 004387a5
004387a5  mov        edx, dword ptr [eax]
004387a7  cmp        dword ptr [edx], ecx
004387a9  je         0x4387d0

// block 004387ab
004387ab  mov        eax, dword ptr [eax + 4]
004387ae  test       eax, eax
004387b0  jne        0x4387a5

// block 004387b2
004387b2  mov        eax, dword ptr [0x4ce8cc]
004387b7  mov        eax, dword ptr [eax + 0x8856c0]
004387bd  test       eax, eax
004387bf  je         0x4387fd

// block 004387c1
004387c1  mov        edx, dword ptr [eax]
004387c3  cmp        dword ptr [edx], ecx
004387c5  je         0x4387d0

// block 004387c7
004387c7  mov        eax, dword ptr [eax + 4]
004387ca  test       eax, eax
004387cc  jne        0x4387c1

// block 004387ce
004387ce  jmp        0x4387fd

// block 004387d0
004387d0  mov        eax, edx
004387d2  mov        edx, 0x10000000
004387d7  test       eax, eax
004387d9  je         0x4387fd

// block 004387db
004387db  or         dword ptr [eax + 0x47c], edx
004387e1  cmp        dword ptr [eax + 0x18], 0
004387e5  jne        0x4387fd

// block 004387e7
004387e7  mov        eax, dword ptr [eax + 0x14]
004387ea  test       eax, eax
004387ec  je         0x4387fd

// block 004387ee
004387ee  mov        ecx, dword ptr [eax]
004387f0  or         dword ptr [ecx + 0x47c], edx
004387f6  mov        eax, dword ptr [eax + 4]
004387f9  test       eax, eax
004387fb  jne        0x4387ee

// block 004387fd
004387fd  mov        dword ptr [esi + 0x40], 0
00438804  mov        dword ptr [esi + 0x60], edi
00438807  mov        eax, dword ptr [0x4b0c94]
0043880c  mov        ecx, dword ptr [0x4b0c90]
00438812  lea        eax, [eax + ecx*2]
00438815  cmp        eax, 5
00438818  ja         0x438c8b

// block 0043881e
0043881e  jmp        dword ptr [eax*4 + 0x438d48]

// block 00438c8b
00438c8b  mov        dword ptr [esi - 0x70], 2
00438c92  inc        edi
00438c93  add        esi, 0xe4
00438c99  cmp        edi, ebx
00438c9b  jl         0x43877c

// block 00438ca1
00438ca1  cmp        edi, 8
00438ca4  fstp       st(0)
00438ca6  jge        0x438ce5

// block 00438ca8
00438ca8  mov        edx, edi
00438caa  imul       edx, edx, 0xe4
00438cb0  mov        eax, 8
00438cb5  sub        eax, edi
00438cb7  lea        esi, [edx + ebp + 0x8310]
00438cbe  mov        dword ptr [esp + 0x4c], eax
00438cc2  mov        edi, eax

// block 00438cc4
00438cc4  mov        eax, dword ptr [esi]
00438cc6  push       eax
00438cc7  mov        ebx, 1
00438ccc  mov        dword ptr [esi - 0xb0], 0
00438cd6  call       0x461970

// block 00438cdb
00438cdb  add        esi, 0xe4
00438ce1  sub        edi, ebx
00438ce3  jne        0x438cc4

// block 00438ce5
00438ce5  mov        ecx, dword ptr [esp + 0x10]
00438ce9  mov        eax, 1
00438cee  mov        dword ptr [ebp + 0xc41c], ecx
00438cf4  mov        dword ptr [ebp + 0x8334], eax
00438cfa  mov        dword ptr [ebp + 0x8418], eax
00438d00  mov        dword ptr [ebp + 0x84fc], eax
00438d06  mov        dword ptr [ebp + 0x85e0], eax
00438d0c  mov        dword ptr [ebp + 0x86c4], eax
00438d12  mov        dword ptr [ebp + 0x87a8], eax
00438d18  mov        dword ptr [ebp + 0x888c], eax
00438d1e  mov        dword ptr [ebp + 0x8970], eax

// block 00438d24
00438d24  pop        edi
00438d25  pop        esi
00438d26  pop        ebp
00438d27  pop        ebx
00438d28  add        esp, 0x38
00438d2b  ret        4
