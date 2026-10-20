
// block 0047460e
0047460e  mov        edi, edi
00474610  push       ebp
00474611  mov        ebp, esp
00474613  sub        esp, 0x14
00474616  push       ebx
00474617  xor        ebx, ebx
00474619  inc        ebx
0047461a  push       0x355
0047461f  mov        dword ptr [ebp - 0x10], ebx
00474622  call       0x4777ce

// block 00474627
00474627  pop        ecx
00474628  mov        dword ptr [ebp - 0x14], eax
0047462b  test       eax, eax
0047462d  je         0x474788

// block 00474633
00474633  push       edi
00474634  lea        edi, [eax + 4]
00474637  mov        byte ptr [edi], 0
0047463a  mov        dword ptr [eax], ebx
0047463c  mov        dword ptr [ebp - 8], ebx
0047463f  lea        ebx, [esi + 0x10]
00474642  lea        eax, [ebx + 0x48]
00474645  push       dword ptr [eax]
00474647  mov        dword ptr [ebp - 0xc], eax
0047464a  push       0x49d500
0047464f  push       dword ptr [0x49d43c]
00474655  push       3
00474657  push       0x351
0047465c  push       edi
0047465d  call       0x474438

// block 00474662
00474662  mov        eax, dword ptr [ebp - 0xc]
00474665  add        esp, 0x18
00474668  mov        dword ptr [ebp - 0xc], eax
0047466b  mov        dword ptr [ebp - 4], 0x49d43c

// block 00474672
00474672  push       0x49d4fc
00474677  push       0x351
0047467c  push       edi
0047467d  call       0x483089

// block 00474682
00474682  add        esp, 0xc
00474685  test       eax, eax
00474687  je         0x474698

// block 00474689
00474689  xor        eax, eax
0047468b  push       eax
0047468c  push       eax
0047468d  push       eax
0047468e  push       eax
0047468f  push       eax
00474690  call       0x470dc6

// block 00474695
00474695  add        esp, 0x14

// block 00474698
00474698  push       dword ptr [ebx + 0x58]
0047469b  mov        eax, dword ptr [ebp - 0xc]
0047469e  push       dword ptr [eax]
004746a0  call       0x472ac0

// block 004746a5
004746a5  pop        ecx
004746a6  pop        ecx
004746a7  test       eax, eax
004746a9  je         0x4746af

// block 004746ab
004746ab  and        dword ptr [ebp - 0x10], 0

// block 004746af
004746af  inc        dword ptr [ebp - 8]
004746b2  mov        eax, dword ptr [ebp - 8]
004746b5  add        dword ptr [ebp - 4], 0xc
004746b9  shl        eax, 4
004746bc  lea        ebx, [eax + esi]
004746bf  lea        eax, [ebx + 0x48]
004746c2  push       dword ptr [eax]
004746c4  mov        dword ptr [ebp - 0xc], eax
004746c7  mov        eax, dword ptr [ebp - 4]
004746ca  push       0x49d500
004746cf  push       dword ptr [eax]
004746d1  push       3
004746d3  push       0x351
004746d8  push       edi
004746d9  call       0x474438

// block 004746de
004746de  add        esp, 0x18
004746e1  cmp        dword ptr [ebp - 4], 0x49d46c
004746e8  jl         0x474672

// block 004746ea
004746ea  cmp        dword ptr [ebp - 0x10], 0
004746ee  jne        0x474739

// block 004746f0
004746f0  mov        eax, dword ptr [esi + 0x50]
004746f3  mov        ebx, dword ptr [0x49815c]
004746f9  test       eax, eax
004746fb  je         0x47470d

// block 004746fd
004746fd  push       eax
004746fe  call       ebx

// block 00474700
00474700  test       eax, eax
00474702  jne        0x47470d

// block 00474704
00474704  push       dword ptr [esi + 0x50]
00474707  call       0x46c941

// block 0047470c
0047470c  pop        ecx

// block 0047470d
0047470d  mov        eax, dword ptr [esi + 0x54]
00474710  test       eax, eax
00474712  je         0x474724

// block 00474714
00474714  push       eax
00474715  call       ebx

// block 00474717
00474717  test       eax, eax
00474719  jne        0x474724

// block 0047471b
0047471b  push       dword ptr [esi + 0x54]
0047471e  call       0x46c941

// block 00474723
00474723  pop        ecx

// block 00474724
00474724  mov        eax, dword ptr [ebp - 0x14]
00474727  and        dword ptr [esi + 0x54], 0
0047472b  and        dword ptr [esi + 0x4c], 0
0047472f  mov        dword ptr [esi + 0x50], eax
00474732  mov        dword ptr [esi + 0x48], edi
00474735  mov        eax, edi
00474737  jmp        0x474787

// block 00474739
00474739  push       dword ptr [ebp - 0x14]
0047473c  call       0x46c941

// block 00474741
00474741  mov        eax, dword ptr [esi + 0x50]
00474744  mov        edi, dword ptr [0x49815c]
0047474a  xor        ebx, ebx
0047474c  pop        ecx
0047474d  cmp        eax, ebx
0047474f  je         0x474761

// block 00474751
00474751  push       eax
00474752  call       edi

// block 00474754
00474754  test       eax, eax
00474756  jne        0x474761

// block 00474758
00474758  push       dword ptr [esi + 0x50]
0047475b  call       0x46c941

// block 00474760
00474760  pop        ecx

// block 00474761
00474761  mov        eax, dword ptr [esi + 0x54]
00474764  cmp        eax, ebx
00474766  je         0x474778

// block 00474768
00474768  push       eax
00474769  call       edi

// block 0047476b
0047476b  test       eax, eax
0047476d  jne        0x474778

// block 0047476f
0047476f  push       dword ptr [esi + 0x54]
00474772  call       0x46c941

// block 00474777
00474777  pop        ecx

// block 00474778
00474778  mov        eax, dword ptr [esi + 0x68]
0047477b  mov        dword ptr [esi + 0x54], ebx
0047477e  mov        dword ptr [esi + 0x4c], ebx
00474781  mov        dword ptr [esi + 0x50], ebx
00474784  mov        dword ptr [esi + 0x48], ebx

// block 00474787
00474787  pop        edi

// block 00474788
00474788  pop        ebx
00474789  leave      
0047478a  ret        
