
// block 00413230
00413230  push       ebx
00413231  push       ebp
00413232  xor        ebx, ebx
00413234  push       esi
00413235  mov        dword ptr [edi + 0x1010], ebx
0041323b  mov        dword ptr [edi + 0x1014], ebx
00413241  lea        esi, [edi + 0x1040]
00413247  mov        dword ptr [edi], 0x49fc50
0041324d  call       0x4123c0

// block 00413252
00413252  push       0x1774
00413257  push       ebx
00413258  push       esi
00413259  call       0x477420

// block 0041325e
0041325e  fldz       
00413260  and        dword ptr [edi + 0x1028], 0xfffffffe
00413267  fstp       dword ptr [edi + 8]
0041326a  lea        eax, [edi + 8]
0041326d  mov        dword ptr [edi + 4], eax
00413270  mov        dword ptr [edi + 0xc], ebx
00413273  mov        dword ptr [edi + 0x101c], edi
00413279  or         esi, 0xffffffff
0041327c  mov        dword ptr [edi + 0x1018], esi
00413282  mov        dword ptr [edi + 0x1020], ebx
00413288  mov        dword ptr [edi + 0x1030], eax
0041328e  mov        dword ptr [edi + 0x1034], ebx
00413294  mov        dword ptr [edi + 0x1038], ebx
0041329a  push       0x34
0041329c  lea        eax, [edi + 0x10dc]
004132a2  push       ebx
004132a3  push       eax
004132a4  mov        dword ptr [edi + 0x278c], edi
004132aa  mov        dword ptr [edi + 0x1310], ebx
004132b0  mov        dword ptr [edi + 0x135c], ebx
004132b6  mov        dword ptr [edi + 0x1398], ebx
004132bc  mov        dword ptr [edi + 0x13d4], ebx
004132c2  mov        dword ptr [edi + 0x1410], ebx
004132c8  mov        dword ptr [edi + 0x144c], ebx
004132ce  mov        dword ptr [edi + 0x1488], ebx
004132d4  mov        dword ptr [edi + 0x14c4], ebx
004132da  mov        dword ptr [edi + 0x26cc], esi
004132e0  call       0x477420

// block 004132e5
004132e5  push       0x34
004132e7  lea        ecx, [edi + 0x10a8]
004132ed  push       ebx
004132ee  push       ecx
004132ef  call       0x477420

// block 004132f4
004132f4  push       0x34
004132f6  lea        edx, [edi + 0x1074]
004132fc  push       ebx
004132fd  push       edx
004132fe  call       0x477420

// block 00413303
00413303  fld        dword ptr [0x4a3e00]
00413309  fst        dword ptr [edi + 0x1110]
0041330f  push       0x58
00413311  fst        dword ptr [edi + 0x1114]
00413317  mov        dword ptr [edi + 0x2704], esi
0041331d  fst        dword ptr [edi + 0x1118]
00413323  lea        ebp, [edi + 0x2660]
00413329  fstp       dword ptr [edi + 0x111c]
0041332f  push       ebx
00413330  push       ebp
00413331  mov        dword ptr [edi + 0x12c0], edi
00413337  mov        dword ptr [edi + 0x12c4], ebx
0041333d  mov        dword ptr [edi + 0x12c8], ebx
00413343  call       0x477420

// block 00413348
00413348  fld        dword ptr [0x4a3d78]
0041334e  fst        dword ptr [ebp + 0x54]
00413351  mov        dword ptr [ebp], ebx
00413354  fstp       dword ptr [ebp + 0x50]
00413357  mov        eax, dword ptr [edi + 0x12bc]
0041335d  fldz       
0041335f  add        esp, 0x3c
00413362  mov        edx, 0xfff0bdc1
00413367  mov        ecx, 0x4b2ed0
0041336c  test       al, 1
0041336e  jne        0x413391

// block 00413370
00413370  or         eax, 1
00413373  fst        dword ptr [edi + 0x12b4]
00413379  mov        dword ptr [edi + 0x12b0], ebx
0041337f  mov        dword ptr [edi + 0x12ac], edx
00413385  mov        dword ptr [edi + 0x12b8], ecx
0041338b  mov        dword ptr [edi + 0x12bc], eax

// block 00413391
00413391  fst        dword ptr [edi + 0x12b4]
00413397  mov        dword ptr [edi + 0x12b0], ebx
0041339d  mov        dword ptr [edi + 0x12ac], esi
004133a3  mov        eax, dword ptr [edi + 0x26e0]
004133a9  test       al, 1
004133ab  jne        0x4133ce

// block 004133ad
004133ad  or         eax, 1
004133b0  fst        dword ptr [edi + 0x26d8]
004133b6  mov        dword ptr [edi + 0x26d4], ebx
004133bc  mov        dword ptr [edi + 0x26d0], edx
004133c2  mov        dword ptr [edi + 0x26dc], ecx
004133c8  mov        dword ptr [edi + 0x26e0], eax

// block 004133ce
004133ce  fst        dword ptr [edi + 0x26d8]
004133d4  mov        dword ptr [edi + 0x26d4], ebx
004133da  mov        dword ptr [edi + 0x26d0], esi
004133e0  mov        eax, dword ptr [edi + 0x26f4]
004133e6  test       al, 1
004133e8  jne        0x41340b

// block 004133ea
004133ea  or         eax, 1
004133ed  fst        dword ptr [edi + 0x26ec]
004133f3  mov        dword ptr [edi + 0x26e8], ebx
004133f9  mov        dword ptr [edi + 0x26e4], edx
004133ff  mov        dword ptr [edi + 0x26f0], ecx
00413405  mov        dword ptr [edi + 0x26f4], eax

// block 0041340b
0041340b  mov        eax, dword ptr [0x4b43dc]
00413410  fst        dword ptr [edi + 0x26ec]
00413416  mov        ecx, dword ptr [esp + 0x10]
0041341a  mov        dword ptr [edi + 0x26e8], ebx
00413420  mov        dword ptr [edi + 0x26e4], esi
00413426  mov        dword ptr [edi + 0x1278], 1
00413430  mov        dword ptr [edi + 0x103c], ebx
00413436  mov        eax, dword ptr [eax + 0x64]
00413439  push       ecx
0041343a  mov        dword ptr [edi + 0x102c], eax
00413440  call       0x4694f0

// block 00413445
00413445  mov        edx, dword ptr [edi + 4]
00413448  mov        dword ptr [edx + 4], eax
0041344b  mov        eax, dword ptr [edi + 4]
0041344e  fstp       dword ptr [eax]
00413450  and        dword ptr [edi + 0x265c], 0xfffffffd
00413457  mov        dword ptr [edi + 0x2648], ebx
0041345d  mov        dword ptr [edi + 0x264c], ebx
00413463  mov        dword ptr [edi + 0x2650], ebx
00413469  mov        eax, esi
0041346b  mov        dword ptr [edi + 0x270c], esi
00413471  mov        dword ptr [edi + 0x2710], esi
00413477  mov        dword ptr [edi + 0x2714], ebx
0041347d  mov        dword ptr [edi + 0x271c], esi
00413483  mov        dword ptr [edi + 0x2720], esi
00413489  mov        dword ptr [edi + 0x2724], ebx
0041348f  mov        dword ptr [edi + 0x272c], esi
00413495  mov        dword ptr [edi + 0x2730], esi
0041349b  mov        dword ptr [edi + 0x2734], ebx
004134a1  mov        dword ptr [edi + 0x273c], esi
004134a7  mov        dword ptr [edi + 0x2740], esi
004134ad  mov        dword ptr [edi + 0x2744], ebx
004134b3  mov        dword ptr [edi + 0x274c], esi
004134b9  mov        dword ptr [edi + 0x2750], esi
004134bf  mov        dword ptr [edi + 0x2754], ebx
004134c5  mov        dword ptr [edi + 0x275c], esi
004134cb  mov        dword ptr [edi + 0x2760], esi
004134d1  mov        dword ptr [edi + 0x2764], ebx
004134d7  mov        dword ptr [edi + 0x276c], esi
004134dd  mov        dword ptr [edi + 0x2770], esi
004134e3  mov        dword ptr [edi + 0x2774], ebx
004134e9  mov        dword ptr [edi + 0x277c], esi
004134ef  mov        dword ptr [edi + 0x2780], esi
004134f5  mov        dword ptr [edi + 0x2784], ebx
004134fb  mov        dword ptr [edi + 0x1220], eax
00413501  mov        dword ptr [edi + 0x1224], eax
00413507  mov        dword ptr [edi + 0x1228], eax
0041350d  mov        dword ptr [edi + 0x122c], eax
00413513  mov        dword ptr [edi + 0x1230], eax
00413519  mov        dword ptr [edi + 0x1234], eax
0041351f  mov        dword ptr [edi + 0x1238], eax
00413525  mov        dword ptr [edi + 0x123c], eax
0041352b  mov        dword ptr [edi + 0x1240], eax
00413531  mov        dword ptr [edi + 0x1244], eax
00413537  mov        dword ptr [edi + 0x1248], eax
0041353d  mov        dword ptr [edi + 0x124c], eax
00413543  mov        dword ptr [edi + 0x1250], eax
00413549  mov        dword ptr [edi + 0x1254], eax
0041354f  pop        esi
00413550  mov        dword ptr [edi + 0x1258], eax
00413556  pop        ebp
00413557  mov        dword ptr [edi + 0x125c], eax
0041355d  mov        eax, edi
0041355f  pop        ebx
00413560  ret        4
