
// block 0048bed9
0048bed9  mov        edi, edi
0048bedb  push       ebp
0048bedc  mov        ebp, esp
0048bede  push       esi
0048bedf  mov        esi, dword ptr [ebp + 8]
0048bee2  push       edi
0048bee3  xor        edi, edi
0048bee5  cmp        esi, edi
0048bee7  jne        0x48bf06

// block 0048bee9
0048bee9  call       0x46e96f

// block 0048beee
0048beee  push       edi
0048beef  push       edi
0048bef0  push       edi
0048bef1  push       edi
0048bef2  push       edi
0048bef3  mov        dword ptr [eax], 0x16
0048bef9  call       0x470f2d

// block 0048befe
0048befe  add        esp, 0x14
0048bf01  jmp        0x48bffd

// block 0048bf06
0048bf06  mov        eax, dword ptr [esi + 0xc]
0048bf09  test       al, 0x83
0048bf0b  je         0x48bffd

// block 0048bf11
0048bf11  test       al, 0x40
0048bf13  jne        0x48bffd

// block 0048bf19
0048bf19  test       al, 2
0048bf1b  je         0x48bf28

// block 0048bf1d
0048bf1d  or         eax, 0x20
0048bf20  mov        dword ptr [esi + 0xc], eax
0048bf23  jmp        0x48bffd

// block 0048bf28
0048bf28  or         eax, 1
0048bf2b  mov        dword ptr [esi + 0xc], eax
0048bf2e  test       eax, 0x10c
0048bf33  jne        0x48bf3e

// block 0048bf35
0048bf35  push       esi
0048bf36  call       0x47bf78

// block 0048bf3b
0048bf3b  pop        ecx
0048bf3c  jmp        0x48bf43

// block 0048bf3e
0048bf3e  mov        eax, dword ptr [esi + 8]
0048bf41  mov        dword ptr [esi], eax

// block 0048bf43
0048bf43  push       dword ptr [esi + 0x18]
0048bf46  push       dword ptr [esi + 8]
0048bf49  push       esi
0048bf4a  call       0x47c1da

// block 0048bf4f
0048bf4f  pop        ecx
0048bf50  push       eax
0048bf51  call       0x48e9f9

// block 0048bf56
0048bf56  add        esp, 0xc
0048bf59  mov        dword ptr [esi + 4], eax
0048bf5c  cmp        eax, edi
0048bf5e  je         0x48bfed

// block 0048bf64
0048bf64  cmp        eax, -1
0048bf67  je         0x48bfed

// block 0048bf6d
0048bf6d  test       byte ptr [esi + 0xc], 0x82
0048bf71  jne        0x48bfc2

// block 0048bf73
0048bf73  push       esi
0048bf74  call       0x47c1da

// block 0048bf79
0048bf79  pop        ecx
0048bf7a  cmp        eax, -1
0048bf7d  je         0x48bfad

// block 0048bf7f
0048bf7f  push       esi
0048bf80  call       0x47c1da

// block 0048bf85
0048bf85  pop        ecx
0048bf86  cmp        eax, -2
0048bf89  je         0x48bfad

// block 0048bf8b
0048bf8b  push       esi
0048bf8c  call       0x47c1da

// block 0048bf91
0048bf91  sar        eax, 5
0048bf94  push       esi
0048bf95  lea        edi, [eax*4 + 0x4d6320]
0048bf9c  call       0x47c1da

// block 0048bfa1
0048bfa1  and        eax, 0x1f
0048bfa4  pop        ecx
0048bfa5  shl        eax, 6
0048bfa8  add        eax, dword ptr [edi]
0048bfaa  pop        ecx
0048bfab  jmp        0x48bfb2

// block 0048bfad
0048bfad  mov        eax, 0x4adbb0

// block 0048bfb2
0048bfb2  mov        al, byte ptr [eax + 4]
0048bfb5  and        al, 0x82
0048bfb7  cmp        al, 0x82
0048bfb9  jne        0x48bfc2

// block 0048bfbb
0048bfbb  or         dword ptr [esi + 0xc], 0x2000

// block 0048bfc2
0048bfc2  cmp        dword ptr [esi + 0x18], 0x200
0048bfc9  jne        0x48bfe0

// block 0048bfcb
0048bfcb  mov        eax, dword ptr [esi + 0xc]
0048bfce  test       al, 8
0048bfd0  je         0x48bfe0

// block 0048bfd2
0048bfd2  test       eax, 0x400
0048bfd7  jne        0x48bfe0

// block 0048bfd9
0048bfd9  mov        dword ptr [esi + 0x18], 0x1000

// block 0048bfe0
0048bfe0  mov        ecx, dword ptr [esi]
0048bfe2  dec        dword ptr [esi + 4]
0048bfe5  movzx      eax, byte ptr [ecx]
0048bfe8  inc        ecx
0048bfe9  mov        dword ptr [esi], ecx
0048bfeb  jmp        0x48c000

// block 0048bfed
0048bfed  neg        eax
0048bfef  sbb        eax, eax
0048bff1  and        eax, 0x10
0048bff4  add        eax, 0x10
0048bff7  or         dword ptr [esi + 0xc], eax
0048bffa  mov        dword ptr [esi + 4], edi

// block 0048bffd
0048bffd  or         eax, 0xffffffff

// block 0048c000
0048c000  pop        edi
0048c001  pop        esi
0048c002  pop        ebp
0048c003  ret        
