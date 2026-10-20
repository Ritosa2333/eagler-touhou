
// block 00454580
00454580  sub        esp, 0x54
00454583  mov        eax, dword ptr [0x4ad138]
00454588  xor        eax, esp
0045458a  mov        dword ptr [esp + 0x50], eax
0045458e  cmp        dword ptr [0x4cf4f8], 0
00454595  push       ebp
00454596  mov        ebp, dword ptr [esp + 0x5c]
0045459a  mov        dword ptr [esp + 8], ebp
0045459e  jne        0x4545b4

// block 004545a0
004545a0  xor        eax, eax
004545a2  pop        ebp
004545a3  mov        ecx, dword ptr [esp + 0x50]
004545a7  xor        ecx, esp
004545a9  call       0x46c932

// block 004545ae
004545ae  add        esp, 0x54
004545b1  ret        4

// block 004545b4
004545b4  mov        eax, dword ptr [esi]
004545b6  test       eax, eax
004545b8  je         0x4545c8

// block 004545ba
004545ba  mov        ecx, dword ptr [eax]
004545bc  mov        edx, dword ptr [ecx + 8]
004545bf  push       eax
004545c0  call       edx

// block 004545c2
004545c2  mov        dword ptr [esi], 0

// block 004545c8
004545c8  xor        eax, eax
004545ca  cmp        dword ptr [esi + 0xc], eax
004545cd  push       edi
004545ce  jle        0x4545f4

// block 004545d0
004545d0  mov        ecx, dword ptr [esi + 8]
004545d3  mov        edx, dword ptr [ecx + 4]
004545d6  mov        ecx, 0x4d0e78
004545db  jmp        0x4545e0

// block 004545e0
004545e0  mov        edi, dword ptr [ecx]
004545e2  cmp        dword ptr [edi + 4], edx
004545e5  je         0x454669

// block 004545eb
004545eb  inc        eax
004545ec  add        ecx, 0x18
004545ef  cmp        eax, dword ptr [esi + 0xc]
004545f2  jl         0x4545e0

// block 004545f4
004545f4  mov        edx, dword ptr [esi + 8]
004545f7  mov        eax, dword ptr [edx + 4]
004545fa  cmp        dword ptr [eax*4 + 0x4d1410], 0
00454602  push       ebx
00454603  jne        0x454630

// block 00454605
00454605  mov        edi, dword ptr [0x498084]
0045460b  mov        ebx, 2

// block 00454610
00454610  push       0xa
00454612  call       edi

// block 00454614
00454614  cmp        dword ptr [0x4d4770], ebx
0045461a  je         0x454886

// block 00454620
00454620  mov        ecx, dword ptr [esi + 8]
00454623  mov        edx, dword ptr [ecx + 4]
00454626  cmp        dword ptr [edx*4 + 0x4d1410], 0
0045462e  je         0x454610

// block 00454630
00454630  mov        eax, dword ptr [esi + 8]
00454633  mov        ecx, dword ptr [eax + 4]
00454636  mov        ebx, dword ptr [ecx*4 + 0x4d1410]
0045463d  push       4
0045463f  push       0x4a3134
00454644  push       ebx
00454645  call       0x46dc03

// block 0045464a
0045464a  add        esp, 0xc
0045464d  test       eax, eax
0045464f  je         0x454698

// block 00454651
00454651  push       ebp
00454652  push       0x4a313c
00454657  mov        ecx, 0x4b0ec8
0045465c  call       0x464220

// block 00454661
00454661  add        esp, 8
00454664  jmp        0x4547b9

// block 00454669
00454669  mov        ecx, dword ptr [0x4cf4e8]
0045466f  mov        edx, dword ptr [ecx]
00454671  lea        eax, [eax + eax*2]
00454674  mov        eax, dword ptr [eax*8 + 0x4d0e70]
0045467b  push       esi
0045467c  push       eax
0045467d  push       ecx
0045467e  mov        ecx, dword ptr [edx + 0x14]
00454681  call       ecx

// block 00454683
00454683  pop        edi
00454684  xor        eax, eax
00454686  pop        ebp
00454687  mov        ecx, dword ptr [esp + 0x50]
0045468b  xor        ecx, esp
0045468d  call       0x46c932

// block 00454692
00454692  add        esp, 0x54
00454695  ret        4

// block 00454698
00454698  mov        edi, dword ptr [ebx + 4]
0045469b  add        ebx, 4
0045469e  push       4
004546a0  add        ebx, 4
004546a3  push       0x4a3158
004546a8  push       ebx
004546a9  call       0x46dc03

// block 004546ae
004546ae  add        esp, 0xc
004546b1  test       eax, eax
004546b3  je         0x4546b8

// block 004546b5
004546b5  push       ebp
004546b6  jmp        0x45471c

// block 004546b8
004546b8  lea        ebp, [edi - 0xc]
004546bb  add        ebx, 4
004546be  push       0x4a317c
004546c3  mov        eax, ebp
004546c5  lea        edi, [esp + 0x20]
004546c9  mov        ecx, ebx
004546cb  call       0x453620

// block 004546d0
004546d0  test       eax, eax
004546d2  jne        0x4546db

// block 004546d4
004546d4  mov        edx, dword ptr [esp + 0x10]
004546d8  push       edx
004546d9  jmp        0x45471c

// block 004546db
004546db  mov        ecx, dword ptr [eax]
004546dd  mov        dword ptr [esp + 0x24], ecx
004546e1  mov        edx, dword ptr [eax + 4]
004546e4  mov        dword ptr [esp + 0x28], edx
004546e8  mov        ecx, dword ptr [eax + 8]
004546eb  mov        dword ptr [esp + 0x2c], ecx
004546ef  mov        edx, dword ptr [eax + 0xc]
004546f2  mov        dword ptr [esp + 0x30], edx
004546f6  mov        ax, word ptr [eax + 0x10]
004546fa  mov        word ptr [esp + 0x34], ax
004546ff  push       0x4a3184
00454704  mov        eax, ebp
00454706  lea        edi, [esp + 0x20]
0045470a  mov        ecx, ebx
0045470c  call       0x453620

// block 00454711
00454711  mov        edi, eax
00454713  test       edi, edi
00454715  jne        0x454733

// block 00454717
00454717  mov        ecx, dword ptr [esp + 0x10]
0045471b  push       ecx

// block 0045471c
0045471c  push       0x4a3160
00454721  mov        ecx, 0x4b0ec8
00454726  call       0x464220

// block 0045472b
0045472b  add        esp, 8
0045472e  jmp        0x4547b9

// block 00454733
00454733  mov        ebx, dword ptr [esp + 0x1c]
00454737  xor        eax, eax
00454739  mov        dword ptr [esp + 0x48], eax
0045473d  push       0
0045473f  mov        dword ptr [esp + 0x3c], eax
00454743  mov        dword ptr [esp + 0x40], eax
00454747  mov        dword ptr [esp + 0x44], eax
0045474b  lea        edx, [esp + 0x28]
0045474f  mov        dword ptr [esp + 0x48], eax
00454753  mov        dword ptr [esp + 0x50], eax
00454757  mov        dword ptr [esp + 0x54], eax
0045475b  mov        dword ptr [esp + 0x58], eax
0045475f  mov        dword ptr [esp + 0x5c], eax
00454763  mov        eax, dword ptr [0x4cf4e8]
00454768  mov        dword ptr [esp + 0x4c], edx
0045476c  push       esi
0045476d  lea        edx, [esp + 0x40]
00454771  mov        dword ptr [esp + 0x40], 0x24
00454779  mov        dword ptr [esp + 0x44], 0x80c8
00454781  mov        dword ptr [esp + 0x48], ebx
00454785  mov        ecx, dword ptr [eax]
00454787  push       edx
00454788  push       eax
00454789  mov        eax, dword ptr [ecx + 0xc]
0045478c  call       eax

// block 0045478e
0045478e  test       eax, eax
00454790  jl         0x4547b9

// block 00454792
00454792  mov        eax, dword ptr [esi]
00454794  mov        ecx, dword ptr [eax]
00454796  push       0
00454798  lea        edx, [esp + 0x18]
0045479c  push       edx
0045479d  lea        edx, [esp + 0x28]
004547a1  push       edx
004547a2  lea        edx, [esp + 0x18]
004547a6  push       edx
004547a7  lea        edx, [esp + 0x28]
004547ab  push       edx
004547ac  push       ebx
004547ad  push       0
004547af  push       eax
004547b0  mov        eax, dword ptr [ecx + 0x2c]
004547b3  call       eax

// block 004547b5
004547b5  test       eax, eax
004547b7  jge        0x4547fb

// block 004547b9
004547b9  mov        ecx, dword ptr [esi + 8]
004547bc  mov        edx, dword ptr [ecx + 4]
004547bf  mov        eax, dword ptr [edx*4 + 0x4d1410]
004547c6  test       eax, eax
004547c8  je         0x4547e4

// block 004547ca
004547ca  push       eax
004547cb  call       0x46c941

// block 004547d0
004547d0  mov        eax, dword ptr [esi + 8]
004547d3  mov        ecx, dword ptr [eax + 4]
004547d6  add        esp, 4
004547d9  mov        dword ptr [ecx*4 + 0x4d1410], 0

// block 004547e4
004547e4  pop        ebx
004547e5  pop        edi
004547e6  or         eax, 0xffffffff
004547e9  pop        ebp
004547ea  mov        ecx, dword ptr [esp + 0x50]
004547ee  xor        ecx, esp
004547f0  call       0x46c932

// block 004547f5
004547f5  add        esp, 0x54
004547f8  ret        4

// block 004547fb
004547fb  mov        edx, dword ptr [esp + 0xc]
004547ff  mov        eax, dword ptr [esp + 0x18]
00454803  push       edx
00454804  push       edi
00454805  push       eax
00454806  call       0x47a330

// block 0045480b
0045480b  mov        ecx, dword ptr [esp + 0x20]
0045480f  add        esp, 0xc
00454812  test       ecx, ecx
00454814  je         0x45482f

// block 00454816
00454816  mov        edx, dword ptr [esp + 0x20]
0045481a  push       ecx
0045481b  mov        ecx, dword ptr [esp + 0x10]
0045481f  add        ecx, edi
00454821  push       ecx
00454822  push       edx
00454823  call       0x47a330

// block 00454828
00454828  mov        ecx, dword ptr [esp + 0x20]
0045482c  add        esp, 0xc

// block 0045482f
0045482f  mov        eax, dword ptr [esi]
00454831  mov        edx, dword ptr [eax]
00454833  mov        edx, dword ptr [edx + 0x4c]
00454836  push       ecx
00454837  mov        ecx, dword ptr [esp + 0x24]
0045483b  push       ecx
0045483c  mov        ecx, dword ptr [esp + 0x14]
00454840  push       ecx
00454841  mov        ecx, dword ptr [esp + 0x24]
00454845  push       ecx
00454846  push       eax
00454847  call       edx

// block 00454849
00454849  mov        eax, dword ptr [esi + 8]
0045484c  mov        ecx, dword ptr [eax + 4]
0045484f  mov        eax, dword ptr [ecx*4 + 0x4d1410]
00454856  test       eax, eax
00454858  je         0x454874

// block 0045485a
0045485a  push       eax
0045485b  call       0x46c941

// block 00454860
00454860  mov        edx, dword ptr [esi + 8]
00454863  mov        eax, dword ptr [edx + 4]
00454866  add        esp, 4
00454869  mov        dword ptr [eax*4 + 0x4d1410], 0

// block 00454874
00454874  mov        ecx, dword ptr [esp + 0x10]
00454878  push       ecx
00454879  push       0x4a318c
0045487e  call       0x454b00

// block 00454883
00454883  add        esp, 8

// block 00454886
00454886  mov        ecx, dword ptr [esp + 0x5c]
0045488a  pop        ebx
0045488b  pop        edi
0045488c  pop        ebp
0045488d  xor        ecx, esp
0045488f  xor        eax, eax
00454891  call       0x46c932

// block 00454896
00454896  add        esp, 0x54
00454899  ret        4
