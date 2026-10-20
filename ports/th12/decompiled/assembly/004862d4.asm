
// block 004862d4
004862d4  mov        edi, edi
004862d6  push       esi
004862d7  push       edi
004862d8  mov        edi, eax
004862da  jmp        0x48673d

// block 004862df
004862df  mov        eax, dword ptr [ecx]
004862e1  cmp        eax, dword ptr [edx]
004862e3  je         0x48635d

// block 004862e5
004862e5  movzx      esi, al
004862e8  movzx      eax, byte ptr [edx]
004862eb  sub        esi, eax
004862ed  je         0x486302

// block 004862ef
004862ef  xor        eax, eax
004862f1  test       esi, esi
004862f3  setg       al
004862f6  lea        eax, [eax + eax - 1]
004862fa  test       eax, eax
004862fc  jne        0x486b14

// block 00486302
00486302  movzx      esi, byte ptr [ecx + 1]
00486306  movzx      eax, byte ptr [edx + 1]
0048630a  sub        esi, eax
0048630c  je         0x486321

// block 0048630e
0048630e  xor        eax, eax
00486310  test       esi, esi
00486312  setg       al
00486315  lea        eax, [eax + eax - 1]
00486319  test       eax, eax
0048631b  jne        0x486b14

// block 00486321
00486321  movzx      esi, byte ptr [ecx + 2]
00486325  movzx      eax, byte ptr [edx + 2]
00486329  sub        esi, eax
0048632b  je         0x486340

// block 0048632d
0048632d  xor        eax, eax
0048632f  test       esi, esi
00486331  setg       al
00486334  lea        eax, [eax + eax - 1]
00486338  test       eax, eax
0048633a  jne        0x486b14

// block 00486340
00486340  movzx      esi, byte ptr [ecx + 3]
00486344  movzx      eax, byte ptr [edx + 3]
00486348  sub        esi, eax
0048634a  je         0x486359

// block 0048634c
0048634c  xor        eax, eax
0048634e  test       esi, esi
00486350  setg       al
00486353  lea        eax, [eax + eax - 1]
00486357  mov        esi, eax

// block 00486359
00486359  mov        eax, esi
0048635b  jmp        0x48635f

// block 0048635d
0048635d  xor        eax, eax

// block 0048635f
0048635f  test       eax, eax
00486361  jne        0x486b14

// block 00486367
00486367  mov        eax, dword ptr [ecx + 4]
0048636a  cmp        eax, dword ptr [edx + 4]
0048636d  je         0x4863e8

// block 0048636f
0048636f  movzx      esi, al
00486372  movzx      eax, byte ptr [edx + 4]
00486376  sub        esi, eax
00486378  je         0x48638d

// block 0048637a
0048637a  xor        eax, eax
0048637c  test       esi, esi
0048637e  setg       al
00486381  lea        eax, [eax + eax - 1]
00486385  test       eax, eax
00486387  jne        0x486b14

// block 0048638d
0048638d  movzx      esi, byte ptr [ecx + 5]
00486391  movzx      eax, byte ptr [edx + 5]
00486395  sub        esi, eax
00486397  je         0x4863ac

// block 00486399
00486399  xor        eax, eax
0048639b  test       esi, esi
0048639d  setg       al
004863a0  lea        eax, [eax + eax - 1]
004863a4  test       eax, eax
004863a6  jne        0x486b14

// block 004863ac
004863ac  movzx      esi, byte ptr [ecx + 6]
004863b0  movzx      eax, byte ptr [edx + 6]
004863b4  sub        esi, eax
004863b6  je         0x4863cb

// block 004863b8
004863b8  xor        eax, eax
004863ba  test       esi, esi
004863bc  setg       al
004863bf  lea        eax, [eax + eax - 1]
004863c3  test       eax, eax
004863c5  jne        0x486b14

// block 004863cb
004863cb  movzx      esi, byte ptr [ecx + 7]
004863cf  movzx      eax, byte ptr [edx + 7]
004863d3  sub        esi, eax
004863d5  je         0x4863e4

// block 004863d7
004863d7  xor        eax, eax
004863d9  test       esi, esi
004863db  setg       al
004863de  lea        eax, [eax + eax - 1]
004863e2  mov        esi, eax

// block 004863e4
004863e4  mov        eax, esi
004863e6  jmp        0x4863ea

// block 004863e8
004863e8  xor        eax, eax

// block 004863ea
004863ea  test       eax, eax
004863ec  jne        0x486b14

// block 004863f2
004863f2  mov        eax, dword ptr [ecx + 8]
004863f5  cmp        eax, dword ptr [edx + 8]
004863f8  je         0x486473

// block 004863fa
004863fa  movzx      esi, al
004863fd  movzx      eax, byte ptr [edx + 8]
00486401  sub        esi, eax
00486403  je         0x486418

// block 00486405
00486405  xor        eax, eax
00486407  test       esi, esi
00486409  setg       al
0048640c  lea        eax, [eax + eax - 1]
00486410  test       eax, eax
00486412  jne        0x486b14

// block 00486418
00486418  movzx      esi, byte ptr [ecx + 9]
0048641c  movzx      eax, byte ptr [edx + 9]
00486420  sub        esi, eax
00486422  je         0x486437

// block 00486424
00486424  xor        eax, eax
00486426  test       esi, esi
00486428  setg       al
0048642b  lea        eax, [eax + eax - 1]
0048642f  test       eax, eax
00486431  jne        0x486b14

// block 00486437
00486437  movzx      esi, byte ptr [ecx + 0xa]
0048643b  movzx      eax, byte ptr [edx + 0xa]
0048643f  sub        esi, eax
00486441  je         0x486456

// block 00486443
00486443  xor        eax, eax
00486445  test       esi, esi
00486447  setg       al
0048644a  lea        eax, [eax + eax - 1]
0048644e  test       eax, eax
00486450  jne        0x486b14

// block 00486456
00486456  movzx      esi, byte ptr [ecx + 0xb]
0048645a  movzx      eax, byte ptr [edx + 0xb]
0048645e  sub        esi, eax
00486460  je         0x48646f

// block 00486462
00486462  xor        eax, eax
00486464  test       esi, esi
00486466  setg       al
00486469  lea        eax, [eax + eax - 1]
0048646d  mov        esi, eax

// block 0048646f
0048646f  mov        eax, esi
00486471  jmp        0x486475

// block 00486473
00486473  xor        eax, eax

// block 00486475
00486475  test       eax, eax
00486477  jne        0x486b14

// block 0048647d
0048647d  mov        eax, dword ptr [ecx + 0xc]
00486480  cmp        eax, dword ptr [edx + 0xc]
00486483  je         0x4864fe

// block 00486485
00486485  movzx      esi, al
00486488  movzx      eax, byte ptr [edx + 0xc]
0048648c  sub        esi, eax
0048648e  je         0x4864a3

// block 00486490
00486490  xor        eax, eax
00486492  test       esi, esi
00486494  setg       al
00486497  lea        eax, [eax + eax - 1]
0048649b  test       eax, eax
0048649d  jne        0x486b14

// block 004864a3
004864a3  movzx      esi, byte ptr [ecx + 0xd]
004864a7  movzx      eax, byte ptr [edx + 0xd]
004864ab  sub        esi, eax
004864ad  je         0x4864c2

// block 004864af
004864af  xor        eax, eax
004864b1  test       esi, esi
004864b3  setg       al
004864b6  lea        eax, [eax + eax - 1]
004864ba  test       eax, eax
004864bc  jne        0x486b14

// block 004864c2
004864c2  movzx      esi, byte ptr [ecx + 0xe]
004864c6  movzx      eax, byte ptr [edx + 0xe]
004864ca  sub        esi, eax
004864cc  je         0x4864e1

// block 004864ce
004864ce  xor        eax, eax
004864d0  test       esi, esi
004864d2  setg       al
004864d5  lea        eax, [eax + eax - 1]
004864d9  test       eax, eax
004864db  jne        0x486b14

// block 004864e1
004864e1  movzx      esi, byte ptr [ecx + 0xf]
004864e5  movzx      eax, byte ptr [edx + 0xf]
004864e9  sub        esi, eax
004864eb  je         0x4864fa

// block 004864ed
004864ed  xor        eax, eax
004864ef  test       esi, esi
004864f1  setg       al
004864f4  lea        eax, [eax + eax - 1]
004864f8  mov        esi, eax

// block 004864fa
004864fa  mov        eax, esi
004864fc  jmp        0x486500

// block 004864fe
004864fe  xor        eax, eax

// block 00486500
00486500  test       eax, eax
00486502  jne        0x486b14

// block 00486508
00486508  mov        eax, dword ptr [ecx + 0x10]
0048650b  cmp        eax, dword ptr [edx + 0x10]
0048650e  je         0x486589

// block 00486510
00486510  movzx      esi, al
00486513  movzx      eax, byte ptr [edx + 0x10]
00486517  sub        esi, eax
00486519  je         0x48652e

// block 0048651b
0048651b  xor        eax, eax
0048651d  test       esi, esi
0048651f  setg       al
00486522  lea        eax, [eax + eax - 1]
00486526  test       eax, eax
00486528  jne        0x486b14

// block 0048652e
0048652e  movzx      esi, byte ptr [ecx + 0x11]
00486532  movzx      eax, byte ptr [edx + 0x11]
00486536  sub        esi, eax
00486538  je         0x48654d

// block 0048653a
0048653a  xor        eax, eax
0048653c  test       esi, esi
0048653e  setg       al
00486541  lea        eax, [eax + eax - 1]
00486545  test       eax, eax
00486547  jne        0x486b14

// block 0048654d
0048654d  movzx      esi, byte ptr [ecx + 0x12]
00486551  movzx      eax, byte ptr [edx + 0x12]
00486555  sub        esi, eax
00486557  je         0x48656c

// block 00486559
00486559  xor        eax, eax
0048655b  test       esi, esi
0048655d  setg       al
00486560  lea        eax, [eax + eax - 1]
00486564  test       eax, eax
00486566  jne        0x486b14

// block 0048656c
0048656c  movzx      esi, byte ptr [ecx + 0x13]
00486570  movzx      eax, byte ptr [edx + 0x13]
00486574  sub        esi, eax
00486576  je         0x486585

// block 00486578
00486578  xor        eax, eax
0048657a  test       esi, esi
0048657c  setg       al
0048657f  lea        eax, [eax + eax - 1]
00486583  mov        esi, eax

// block 00486585
00486585  mov        eax, esi
00486587  jmp        0x48658b

// block 00486589
00486589  xor        eax, eax

// block 0048658b
0048658b  test       eax, eax
0048658d  jne        0x486b14

// block 00486593
00486593  mov        eax, dword ptr [ecx + 0x14]
00486596  cmp        eax, dword ptr [edx + 0x14]
00486599  je         0x486614

// block 0048659b
0048659b  movzx      esi, al
0048659e  movzx      eax, byte ptr [edx + 0x14]
004865a2  sub        esi, eax
004865a4  je         0x4865b9

// block 004865a6
004865a6  xor        eax, eax
004865a8  test       esi, esi
004865aa  setg       al
004865ad  lea        eax, [eax + eax - 1]
004865b1  test       eax, eax
004865b3  jne        0x486b14

// block 004865b9
004865b9  movzx      esi, byte ptr [ecx + 0x15]
004865bd  movzx      eax, byte ptr [edx + 0x15]
004865c1  sub        esi, eax
004865c3  je         0x4865d8

// block 004865c5
004865c5  xor        eax, eax
004865c7  test       esi, esi
004865c9  setg       al
004865cc  lea        eax, [eax + eax - 1]
004865d0  test       eax, eax
004865d2  jne        0x486b14

// block 004865d8
004865d8  movzx      esi, byte ptr [ecx + 0x16]
004865dc  movzx      eax, byte ptr [edx + 0x16]
004865e0  sub        esi, eax
004865e2  je         0x4865f7

// block 004865e4
004865e4  xor        eax, eax
004865e6  test       esi, esi
004865e8  setg       al
004865eb  lea        eax, [eax + eax - 1]
004865ef  test       eax, eax
004865f1  jne        0x486b14

// block 004865f7
004865f7  movzx      esi, byte ptr [ecx + 0x17]
004865fb  movzx      eax, byte ptr [edx + 0x17]
004865ff  sub        esi, eax
00486601  je         0x486610

// block 00486603
00486603  xor        eax, eax
00486605  test       esi, esi
00486607  setg       al
0048660a  lea        eax, [eax + eax - 1]
0048660e  mov        esi, eax

// block 00486610
00486610  mov        eax, esi
00486612  jmp        0x486616

// block 00486614
00486614  xor        eax, eax

// block 00486616
00486616  test       eax, eax
00486618  jne        0x486b14

// block 0048661e
0048661e  mov        eax, dword ptr [ecx + 0x18]
00486621  cmp        eax, dword ptr [edx + 0x18]
00486624  je         0x48669f

// block 00486626
00486626  movzx      esi, al
00486629  movzx      eax, byte ptr [edx + 0x18]
0048662d  sub        esi, eax
0048662f  je         0x486644

// block 00486631
00486631  xor        eax, eax
00486633  test       esi, esi
00486635  setg       al
00486638  lea        eax, [eax + eax - 1]
0048663c  test       eax, eax
0048663e  jne        0x486b14

// block 00486644
00486644  movzx      esi, byte ptr [ecx + 0x19]
00486648  movzx      eax, byte ptr [edx + 0x19]
0048664c  sub        esi, eax
0048664e  je         0x486663

// block 00486650
00486650  xor        eax, eax
00486652  test       esi, esi
00486654  setg       al
00486657  lea        eax, [eax + eax - 1]
0048665b  test       eax, eax
0048665d  jne        0x486b14

// block 00486663
00486663  movzx      esi, byte ptr [ecx + 0x1a]
00486667  movzx      eax, byte ptr [edx + 0x1a]
0048666b  sub        esi, eax
0048666d  je         0x486682

// block 0048666f
0048666f  xor        eax, eax
00486671  test       esi, esi
00486673  setg       al
00486676  lea        eax, [eax + eax - 1]
0048667a  test       eax, eax
0048667c  jne        0x486b14

// block 00486682
00486682  movzx      esi, byte ptr [ecx + 0x1b]
00486686  movzx      eax, byte ptr [edx + 0x1b]
0048668a  sub        esi, eax
0048668c  je         0x48669b

// block 0048668e
0048668e  xor        eax, eax
00486690  test       esi, esi
00486692  setg       al
00486695  lea        eax, [eax + eax - 1]
00486699  mov        esi, eax

// block 0048669b
0048669b  mov        eax, esi
0048669d  jmp        0x4866a1

// block 0048669f
0048669f  xor        eax, eax

// block 004866a1
004866a1  test       eax, eax
004866a3  jne        0x486b14

// block 004866a9
004866a9  mov        eax, dword ptr [ecx + 0x1c]
004866ac  cmp        eax, dword ptr [edx + 0x1c]
004866af  je         0x48672a

// block 004866b1
004866b1  movzx      esi, al
004866b4  movzx      eax, byte ptr [edx + 0x1c]
004866b8  sub        esi, eax
004866ba  je         0x4866cf

// block 004866bc
004866bc  xor        eax, eax
004866be  test       esi, esi
004866c0  setg       al
004866c3  lea        eax, [eax + eax - 1]
004866c7  test       eax, eax
004866c9  jne        0x486b14

// block 004866cf
004866cf  movzx      esi, byte ptr [ecx + 0x1d]
004866d3  movzx      eax, byte ptr [edx + 0x1d]
004866d7  sub        esi, eax
004866d9  je         0x4866ee

// block 004866db
004866db  xor        eax, eax
004866dd  test       esi, esi
004866df  setg       al
004866e2  lea        eax, [eax + eax - 1]
004866e6  test       eax, eax
004866e8  jne        0x486b14

// block 004866ee
004866ee  movzx      esi, byte ptr [ecx + 0x1e]
004866f2  movzx      eax, byte ptr [edx + 0x1e]
004866f6  sub        esi, eax
004866f8  je         0x48670d

// block 004866fa
004866fa  xor        eax, eax
004866fc  test       esi, esi
004866fe  setg       al
00486701  lea        eax, [eax + eax - 1]
00486705  test       eax, eax
00486707  jne        0x486b14

// block 0048670d
0048670d  movzx      esi, byte ptr [ecx + 0x1f]
00486711  movzx      eax, byte ptr [edx + 0x1f]
00486715  sub        esi, eax
00486717  je         0x486726

// block 00486719
00486719  xor        eax, eax
0048671b  test       esi, esi
0048671d  setg       al
00486720  lea        eax, [eax + eax - 1]
00486724  mov        esi, eax

// block 00486726
00486726  mov        eax, esi
00486728  jmp        0x48672c

// block 0048672a
0048672a  xor        eax, eax

// block 0048672c
0048672c  test       eax, eax
0048672e  jne        0x486b14

// block 00486734
00486734  add        ecx, 0x20
00486737  add        edx, 0x20
0048673a  sub        edi, 0x20

// block 0048673d
0048673d  cmp        edi, 0x20
00486740  jae        0x4862df

// block 00486746
00486746  add        ecx, edi
00486748  add        edx, edi
0048674a  cmp        edi, 0x1f
0048674d  ja         0x486b12

// block 00486753
00486753  jmp        dword ptr [edi*4 + 0x4876e8]

// block 0048675a
0048675a  mov        eax, dword ptr [ecx - 0x1c]
0048675d  cmp        eax, dword ptr [edx - 0x1c]
00486760  je         0x4867dc

// block 00486762
00486762  movzx      eax, byte ptr [edx - 0x1c]
00486766  movzx      esi, byte ptr [ecx - 0x1c]
0048676a  sub        esi, eax
0048676c  je         0x486781

// block 0048676e
0048676e  xor        eax, eax
00486770  test       esi, esi
00486772  setg       al
00486775  lea        eax, [eax + eax - 1]
00486779  test       eax, eax
0048677b  jne        0x486b14

// block 00486781
00486781  movzx      esi, byte ptr [ecx - 0x1b]
00486785  movzx      eax, byte ptr [edx - 0x1b]
00486789  sub        esi, eax
0048678b  je         0x4867a0

// block 0048678d
0048678d  xor        eax, eax
0048678f  test       esi, esi
00486791  setg       al
00486794  lea        eax, [eax + eax - 1]
00486798  test       eax, eax
0048679a  jne        0x486b14

// block 004867a0
004867a0  movzx      esi, byte ptr [ecx - 0x1a]
004867a4  movzx      eax, byte ptr [edx - 0x1a]
004867a8  sub        esi, eax
004867aa  je         0x4867bf

// block 004867ac
004867ac  xor        eax, eax
004867ae  test       esi, esi
004867b0  setg       al
004867b3  lea        eax, [eax + eax - 1]
004867b7  test       eax, eax
004867b9  jne        0x486b14

// block 004867bf
004867bf  movzx      esi, byte ptr [ecx - 0x19]
004867c3  movzx      eax, byte ptr [edx - 0x19]
004867c7  sub        esi, eax
004867c9  je         0x4867d8

// block 004867cb
004867cb  xor        eax, eax
004867cd  test       esi, esi
004867cf  setg       al
004867d2  lea        eax, [eax + eax - 1]
004867d6  mov        esi, eax

// block 004867d8
004867d8  mov        eax, esi
004867da  jmp        0x4867de

// block 004867dc
004867dc  xor        eax, eax

// block 004867de
004867de  test       eax, eax
004867e0  jne        0x486b14

// block 004867e6
004867e6  mov        eax, dword ptr [ecx - 0x18]
004867e9  cmp        eax, dword ptr [edx - 0x18]
004867ec  je         0x486867

// block 004867ee
004867ee  movzx      esi, al
004867f1  movzx      eax, byte ptr [edx - 0x18]
004867f5  sub        esi, eax
004867f7  je         0x48680c

// block 004867f9
004867f9  xor        eax, eax
004867fb  test       esi, esi
004867fd  setg       al
00486800  lea        eax, [eax + eax - 1]
00486804  test       eax, eax
00486806  jne        0x486b14

// block 0048680c
0048680c  movzx      esi, byte ptr [ecx - 0x17]
00486810  movzx      eax, byte ptr [edx - 0x17]
00486814  sub        esi, eax
00486816  je         0x48682b

// block 00486818
00486818  xor        eax, eax
0048681a  test       esi, esi
0048681c  setg       al
0048681f  lea        eax, [eax + eax - 1]
00486823  test       eax, eax
00486825  jne        0x486b14

// block 0048682b
0048682b  movzx      esi, byte ptr [ecx - 0x16]
0048682f  movzx      eax, byte ptr [edx - 0x16]
00486833  sub        esi, eax
00486835  je         0x48684a

// block 00486837
00486837  xor        eax, eax
00486839  test       esi, esi
0048683b  setg       al
0048683e  lea        eax, [eax + eax - 1]
00486842  test       eax, eax
00486844  jne        0x486b14

// block 0048684a
0048684a  movzx      esi, byte ptr [ecx - 0x15]
0048684e  movzx      eax, byte ptr [edx - 0x15]
00486852  sub        esi, eax
00486854  je         0x486863

// block 00486856
00486856  xor        eax, eax
00486858  test       esi, esi
0048685a  setg       al
0048685d  lea        eax, [eax + eax - 1]
00486861  mov        esi, eax

// block 00486863
00486863  mov        eax, esi
00486865  jmp        0x486869

// block 00486867
00486867  xor        eax, eax

// block 00486869
00486869  test       eax, eax
0048686b  jne        0x486b14

// block 00486871
00486871  mov        eax, dword ptr [ecx - 0x14]
00486874  cmp        eax, dword ptr [edx - 0x14]
00486877  je         0x4868f2

// block 00486879
00486879  movzx      esi, al
0048687c  movzx      eax, byte ptr [edx - 0x14]
00486880  sub        esi, eax
00486882  je         0x486897

// block 00486884
00486884  xor        eax, eax
00486886  test       esi, esi
00486888  setg       al
0048688b  lea        eax, [eax + eax - 1]
0048688f  test       eax, eax
00486891  jne        0x486b14

// block 00486897
00486897  movzx      esi, byte ptr [ecx - 0x13]
0048689b  movzx      eax, byte ptr [edx - 0x13]
0048689f  sub        esi, eax
004868a1  je         0x4868b6

// block 004868a3
004868a3  xor        eax, eax
004868a5  test       esi, esi
004868a7  setg       al
004868aa  lea        eax, [eax + eax - 1]
004868ae  test       eax, eax
004868b0  jne        0x486b14

// block 004868b6
004868b6  movzx      esi, byte ptr [ecx - 0x12]
004868ba  movzx      eax, byte ptr [edx - 0x12]
004868be  sub        esi, eax
004868c0  je         0x4868d5

// block 004868c2
004868c2  xor        eax, eax
004868c4  test       esi, esi
004868c6  setg       al
004868c9  lea        eax, [eax + eax - 1]
004868cd  test       eax, eax
004868cf  jne        0x486b14

// block 004868d5
004868d5  movzx      esi, byte ptr [ecx - 0x11]
004868d9  movzx      eax, byte ptr [edx - 0x11]
004868dd  sub        esi, eax
004868df  je         0x4868ee

// block 004868e1
004868e1  xor        eax, eax
004868e3  test       esi, esi
004868e5  setg       al
004868e8  lea        eax, [eax + eax - 1]
004868ec  mov        esi, eax

// block 004868ee
004868ee  mov        eax, esi
004868f0  jmp        0x4868f4

// block 004868f2
004868f2  xor        eax, eax

// block 004868f4
004868f4  test       eax, eax
004868f6  jne        0x486b14

// block 004868fc
004868fc  mov        eax, dword ptr [ecx - 0x10]
004868ff  cmp        eax, dword ptr [edx - 0x10]
00486902  je         0x48697d

// block 00486904
00486904  movzx      esi, al
00486907  movzx      eax, byte ptr [edx - 0x10]
0048690b  sub        esi, eax
0048690d  je         0x486922

// block 0048690f
0048690f  xor        eax, eax
00486911  test       esi, esi
00486913  setg       al
00486916  lea        eax, [eax + eax - 1]
0048691a  test       eax, eax
0048691c  jne        0x486b14

// block 00486922
00486922  movzx      esi, byte ptr [ecx - 0xf]
00486926  movzx      eax, byte ptr [edx - 0xf]
0048692a  sub        esi, eax
0048692c  je         0x486941

// block 0048692e
0048692e  xor        eax, eax
00486930  test       esi, esi
00486932  setg       al
00486935  lea        eax, [eax + eax - 1]
00486939  test       eax, eax
0048693b  jne        0x486b14

// block 00486941
00486941  movzx      esi, byte ptr [ecx - 0xe]
00486945  movzx      eax, byte ptr [edx - 0xe]
00486949  sub        esi, eax
0048694b  je         0x486960

// block 0048694d
0048694d  xor        eax, eax
0048694f  test       esi, esi
00486951  setg       al
00486954  lea        eax, [eax + eax - 1]
00486958  test       eax, eax
0048695a  jne        0x486b14

// block 00486960
00486960  movzx      esi, byte ptr [ecx - 0xd]
00486964  movzx      eax, byte ptr [edx - 0xd]
00486968  sub        esi, eax
0048696a  je         0x486979

// block 0048696c
0048696c  xor        eax, eax
0048696e  test       esi, esi
00486970  setg       al
00486973  lea        eax, [eax + eax - 1]
00486977  mov        esi, eax

// block 00486979
00486979  mov        eax, esi
0048697b  jmp        0x48697f

// block 0048697d
0048697d  xor        eax, eax

// block 0048697f
0048697f  test       eax, eax
00486981  jne        0x486b14

// block 00486987
00486987  mov        eax, dword ptr [ecx - 0xc]
0048698a  cmp        eax, dword ptr [edx - 0xc]
0048698d  je         0x486a08

// block 0048698f
0048698f  movzx      esi, al
00486992  movzx      eax, byte ptr [edx - 0xc]
00486996  sub        esi, eax
00486998  je         0x4869ad

// block 0048699a
0048699a  xor        eax, eax
0048699c  test       esi, esi
0048699e  setg       al
004869a1  lea        eax, [eax + eax - 1]
004869a5  test       eax, eax
004869a7  jne        0x486b14

// block 004869ad
004869ad  movzx      esi, byte ptr [ecx - 0xb]
004869b1  movzx      eax, byte ptr [edx - 0xb]
004869b5  sub        esi, eax
004869b7  je         0x4869cc

// block 004869b9
004869b9  xor        eax, eax
004869bb  test       esi, esi
004869bd  setg       al
004869c0  lea        eax, [eax + eax - 1]
004869c4  test       eax, eax
004869c6  jne        0x486b14

// block 004869cc
004869cc  movzx      esi, byte ptr [ecx - 0xa]
004869d0  movzx      eax, byte ptr [edx - 0xa]
004869d4  sub        esi, eax
004869d6  je         0x4869eb

// block 004869d8
004869d8  xor        eax, eax
004869da  test       esi, esi
004869dc  setg       al
004869df  lea        eax, [eax + eax - 1]
004869e3  test       eax, eax
004869e5  jne        0x486b14

// block 004869eb
004869eb  movzx      esi, byte ptr [ecx - 9]
004869ef  movzx      eax, byte ptr [edx - 9]
004869f3  sub        esi, eax
004869f5  je         0x486a04

// block 004869f7
004869f7  xor        eax, eax
004869f9  test       esi, esi
004869fb  setg       al
004869fe  lea        eax, [eax + eax - 1]
00486a02  mov        esi, eax

// block 00486a04
00486a04  mov        eax, esi
00486a06  jmp        0x486a0a

// block 00486a08
00486a08  xor        eax, eax

// block 00486a0a
00486a0a  test       eax, eax
00486a0c  jne        0x486b14

// block 00486a12
00486a12  mov        eax, dword ptr [ecx - 8]
00486a15  cmp        eax, dword ptr [edx - 8]
00486a18  je         0x486a93

// block 00486a1a
00486a1a  movzx      esi, al
00486a1d  movzx      eax, byte ptr [edx - 8]
00486a21  sub        esi, eax
00486a23  je         0x486a38

// block 00486a25
00486a25  xor        eax, eax
00486a27  test       esi, esi
00486a29  setg       al
00486a2c  lea        eax, [eax + eax - 1]
00486a30  test       eax, eax
00486a32  jne        0x486b14

// block 00486a38
00486a38  movzx      esi, byte ptr [ecx - 7]
00486a3c  movzx      eax, byte ptr [edx - 7]
00486a40  sub        esi, eax
00486a42  je         0x486a57

// block 00486a44
00486a44  xor        eax, eax
00486a46  test       esi, esi
00486a48  setg       al
00486a4b  lea        eax, [eax + eax - 1]
00486a4f  test       eax, eax
00486a51  jne        0x486b14

// block 00486a57
00486a57  movzx      esi, byte ptr [ecx - 6]
00486a5b  movzx      eax, byte ptr [edx - 6]
00486a5f  sub        esi, eax
00486a61  je         0x486a76

// block 00486a63
00486a63  xor        eax, eax
00486a65  test       esi, esi
00486a67  setg       al
00486a6a  lea        eax, [eax + eax - 1]
00486a6e  test       eax, eax
00486a70  jne        0x486b14

// block 00486a76
00486a76  movzx      esi, byte ptr [ecx - 5]
00486a7a  movzx      eax, byte ptr [edx - 5]
00486a7e  sub        esi, eax
00486a80  je         0x486a8f

// block 00486a82
00486a82  xor        eax, eax
00486a84  test       esi, esi
00486a86  setg       al
00486a89  lea        eax, [eax + eax - 1]
00486a8d  mov        esi, eax

// block 00486a8f
00486a8f  mov        eax, esi
00486a91  jmp        0x486a95

// block 00486a93
00486a93  xor        eax, eax

// block 00486a95
00486a95  test       eax, eax
00486a97  jne        0x486b14

// block 00486a99
00486a99  mov        eax, dword ptr [ecx - 4]
00486a9c  cmp        eax, dword ptr [edx - 4]
00486a9f  je         0x486b0c

// block 00486aa1
00486aa1  movzx      esi, al
00486aa4  movzx      eax, byte ptr [edx - 4]
00486aa8  sub        esi, eax
00486aaa  je         0x486abb

// block 00486aac
00486aac  xor        eax, eax
00486aae  test       esi, esi
00486ab0  setg       al
00486ab3  lea        eax, [eax + eax - 1]
00486ab7  test       eax, eax
00486ab9  jne        0x486b14

// block 00486abb
00486abb  movzx      esi, byte ptr [ecx - 3]
00486abf  movzx      eax, byte ptr [edx - 3]
00486ac3  sub        esi, eax
00486ac5  je         0x486ad6

// block 00486ac7
00486ac7  xor        eax, eax
00486ac9  test       esi, esi
00486acb  setg       al
00486ace  lea        eax, [eax + eax - 1]
00486ad2  test       eax, eax
00486ad4  jne        0x486b14

// block 00486ad6
00486ad6  movzx      esi, byte ptr [ecx - 2]
00486ada  movzx      eax, byte ptr [edx - 2]
00486ade  sub        esi, eax
00486ae0  je         0x486af1

// block 00486ae2
00486ae2  xor        eax, eax
00486ae4  test       esi, esi
00486ae6  setg       al
00486ae9  lea        eax, [eax + eax - 1]
00486aed  test       eax, eax
00486aef  jne        0x486b14

// block 00486af1
00486af1  movzx      eax, byte ptr [ecx - 1]
00486af5  movzx      ecx, byte ptr [edx - 1]
00486af9  sub        eax, ecx
00486afb  je         0x486b0e

// block 00486afd
00486afd  xor        ecx, ecx
00486aff  test       eax, eax
00486b01  setg       cl
00486b04  lea        ecx, [ecx + ecx - 1]
00486b08  mov        eax, ecx
00486b0a  jmp        0x486b0e

// block 00486b0c
00486b0c  xor        eax, eax

// block 00486b0e
00486b0e  test       eax, eax
00486b10  jne        0x486b14

// block 00486b12
00486b12  xor        eax, eax

// block 00486b14
00486b14  pop        edi
00486b15  pop        esi
00486b16  ret        

// block 00486b17
00486b17  mov        eax, dword ptr [ecx - 0x1d]
00486b1a  cmp        eax, dword ptr [edx - 0x1d]
00486b1d  je         0x486b8c

// block 00486b1f
00486b1f  movzx      esi, al
00486b22  movzx      eax, byte ptr [edx - 0x1d]
00486b26  sub        esi, eax
00486b28  je         0x486b39

// block 00486b2a
00486b2a  xor        eax, eax
00486b2c  test       esi, esi
00486b2e  setg       al
00486b31  lea        eax, [eax + eax - 1]
00486b35  test       eax, eax
00486b37  jne        0x486b14

// block 00486b39
00486b39  movzx      esi, byte ptr [ecx - 0x1c]
00486b3d  movzx      eax, byte ptr [edx - 0x1c]
00486b41  sub        esi, eax
00486b43  je         0x486b54

// block 00486b45
00486b45  xor        eax, eax
00486b47  test       esi, esi
00486b49  setg       al
00486b4c  lea        eax, [eax + eax - 1]
00486b50  test       eax, eax
00486b52  jne        0x486b14

// block 00486b54
00486b54  movzx      esi, byte ptr [ecx - 0x1b]
00486b58  movzx      eax, byte ptr [edx - 0x1b]
00486b5c  sub        esi, eax
00486b5e  je         0x486b6f

// block 00486b60
00486b60  xor        eax, eax
00486b62  test       esi, esi
00486b64  setg       al
00486b67  lea        eax, [eax + eax - 1]
00486b6b  test       eax, eax
00486b6d  jne        0x486b14

// block 00486b6f
00486b6f  movzx      esi, byte ptr [ecx - 0x1a]
00486b73  movzx      eax, byte ptr [edx - 0x1a]
00486b77  sub        esi, eax
00486b79  je         0x486b88

// block 00486b7b
00486b7b  xor        eax, eax
00486b7d  test       esi, esi
00486b7f  setg       al
00486b82  lea        eax, [eax + eax - 1]
00486b86  mov        esi, eax

// block 00486b88
00486b88  mov        eax, esi
00486b8a  jmp        0x486b8e

// block 00486b8c
00486b8c  xor        eax, eax

// block 00486b8e
00486b8e  test       eax, eax
00486b90  jne        0x486b14

// block 00486b92
00486b92  mov        eax, dword ptr [ecx - 0x19]
00486b95  cmp        eax, dword ptr [edx - 0x19]
00486b98  je         0x486c14

// block 00486b9a
00486b9a  movzx      eax, byte ptr [edx - 0x19]
00486b9e  movzx      esi, byte ptr [ecx - 0x19]
00486ba2  sub        esi, eax
00486ba4  je         0x486bb9

// block 00486ba6
00486ba6  xor        eax, eax
00486ba8  test       esi, esi
00486baa  setg       al
00486bad  lea        eax, [eax + eax - 1]
00486bb1  test       eax, eax
00486bb3  jne        0x486b14

// block 00486bb9
00486bb9  movzx      esi, byte ptr [ecx - 0x18]
00486bbd  movzx      eax, byte ptr [edx - 0x18]
00486bc1  sub        esi, eax
00486bc3  je         0x486bd8

// block 00486bc5
00486bc5  xor        eax, eax
00486bc7  test       esi, esi
00486bc9  setg       al
00486bcc  lea        eax, [eax + eax - 1]
00486bd0  test       eax, eax
00486bd2  jne        0x486b14

// block 00486bd8
00486bd8  movzx      esi, byte ptr [ecx - 0x17]
00486bdc  movzx      eax, byte ptr [edx - 0x17]
00486be0  sub        esi, eax
00486be2  je         0x486bf7

// block 00486be4
00486be4  xor        eax, eax
00486be6  test       esi, esi
00486be8  setg       al
00486beb  lea        eax, [eax + eax - 1]
00486bef  test       eax, eax
00486bf1  jne        0x486b14

// block 00486bf7
00486bf7  movzx      esi, byte ptr [ecx - 0x16]
00486bfb  movzx      eax, byte ptr [edx - 0x16]
00486bff  sub        esi, eax
00486c01  je         0x486c10

// block 00486c03
00486c03  xor        eax, eax
00486c05  test       esi, esi
00486c07  setg       al
00486c0a  lea        eax, [eax + eax - 1]
00486c0e  mov        esi, eax

// block 00486c10
00486c10  mov        eax, esi
00486c12  jmp        0x486c16

// block 00486c14
00486c14  xor        eax, eax

// block 00486c16
00486c16  test       eax, eax
00486c18  jne        0x486b14

// block 00486c1e
00486c1e  mov        eax, dword ptr [ecx - 0x15]
00486c21  cmp        eax, dword ptr [edx - 0x15]
00486c24  je         0x486c9f

// block 00486c26
00486c26  movzx      esi, al
00486c29  movzx      eax, byte ptr [edx - 0x15]
00486c2d  sub        esi, eax
00486c2f  je         0x486c44

// block 00486c31
00486c31  xor        eax, eax
00486c33  test       esi, esi
00486c35  setg       al
00486c38  lea        eax, [eax + eax - 1]
00486c3c  test       eax, eax
00486c3e  jne        0x486b14

// block 00486c44
00486c44  movzx      esi, byte ptr [ecx - 0x14]
00486c48  movzx      eax, byte ptr [edx - 0x14]
00486c4c  sub        esi, eax
00486c4e  je         0x486c63

// block 00486c50
00486c50  xor        eax, eax
00486c52  test       esi, esi
00486c54  setg       al
00486c57  lea        eax, [eax + eax - 1]
00486c5b  test       eax, eax
00486c5d  jne        0x486b14

// block 00486c63
00486c63  movzx      esi, byte ptr [ecx - 0x13]
00486c67  movzx      eax, byte ptr [edx - 0x13]
00486c6b  sub        esi, eax
00486c6d  je         0x486c82

// block 00486c6f
00486c6f  xor        eax, eax
00486c71  test       esi, esi
00486c73  setg       al
00486c76  lea        eax, [eax + eax - 1]
00486c7a  test       eax, eax
00486c7c  jne        0x486b14

// block 00486c82
00486c82  movzx      esi, byte ptr [ecx - 0x12]
00486c86  movzx      eax, byte ptr [edx - 0x12]
00486c8a  sub        esi, eax
00486c8c  je         0x486c9b

// block 00486c8e
00486c8e  xor        eax, eax
00486c90  test       esi, esi
00486c92  setg       al
00486c95  lea        eax, [eax + eax - 1]
00486c99  mov        esi, eax

// block 00486c9b
00486c9b  mov        eax, esi
00486c9d  jmp        0x486ca1

// block 00486c9f
00486c9f  xor        eax, eax

// block 00486ca1
00486ca1  test       eax, eax
00486ca3  jne        0x486b14

// block 00486ca9
00486ca9  mov        eax, dword ptr [ecx - 0x11]
00486cac  cmp        eax, dword ptr [edx - 0x11]
00486caf  je         0x486d2a

// block 00486cb1
00486cb1  movzx      esi, al
00486cb4  movzx      eax, byte ptr [edx - 0x11]
00486cb8  sub        esi, eax
00486cba  je         0x486ccf

// block 00486cbc
00486cbc  xor        eax, eax
00486cbe  test       esi, esi
00486cc0  setg       al
00486cc3  lea        eax, [eax + eax - 1]
00486cc7  test       eax, eax
00486cc9  jne        0x486b14

// block 00486ccf
00486ccf  movzx      esi, byte ptr [ecx - 0x10]
00486cd3  movzx      eax, byte ptr [edx - 0x10]
00486cd7  sub        esi, eax
00486cd9  je         0x486cee

// block 00486cdb
00486cdb  xor        eax, eax
00486cdd  test       esi, esi
00486cdf  setg       al
00486ce2  lea        eax, [eax + eax - 1]
00486ce6  test       eax, eax
00486ce8  jne        0x486b14

// block 00486cee
00486cee  movzx      esi, byte ptr [ecx - 0xf]
00486cf2  movzx      eax, byte ptr [edx - 0xf]
00486cf6  sub        esi, eax
00486cf8  je         0x486d0d

// block 00486cfa
00486cfa  xor        eax, eax
00486cfc  test       esi, esi
00486cfe  setg       al
00486d01  lea        eax, [eax + eax - 1]
00486d05  test       eax, eax
00486d07  jne        0x486b14

// block 00486d0d
00486d0d  movzx      esi, byte ptr [ecx - 0xe]
00486d11  movzx      eax, byte ptr [edx - 0xe]
00486d15  sub        esi, eax
00486d17  je         0x486d26

// block 00486d19
00486d19  xor        eax, eax
00486d1b  test       esi, esi
00486d1d  setg       al
00486d20  lea        eax, [eax + eax - 1]
00486d24  mov        esi, eax

// block 00486d26
00486d26  mov        eax, esi
00486d28  jmp        0x486d2c

// block 00486d2a
00486d2a  xor        eax, eax

// block 00486d2c
00486d2c  test       eax, eax
00486d2e  jne        0x486b14

// block 00486d34
00486d34  mov        eax, dword ptr [ecx - 0xd]
00486d37  cmp        eax, dword ptr [edx - 0xd]
00486d3a  je         0x486db5

// block 00486d3c
00486d3c  movzx      esi, al
00486d3f  movzx      eax, byte ptr [edx - 0xd]
00486d43  sub        esi, eax
00486d45  je         0x486d5a

// block 00486d47
00486d47  xor        eax, eax
00486d49  test       esi, esi
00486d4b  setg       al
00486d4e  lea        eax, [eax + eax - 1]
00486d52  test       eax, eax
00486d54  jne        0x486b14

// block 00486d5a
00486d5a  movzx      esi, byte ptr [ecx - 0xc]
00486d5e  movzx      eax, byte ptr [edx - 0xc]
00486d62  sub        esi, eax
00486d64  je         0x486d79

// block 00486d66
00486d66  xor        eax, eax
00486d68  test       esi, esi
00486d6a  setg       al
00486d6d  lea        eax, [eax + eax - 1]
00486d71  test       eax, eax
00486d73  jne        0x486b14

// block 00486d79
00486d79  movzx      esi, byte ptr [ecx - 0xb]
00486d7d  movzx      eax, byte ptr [edx - 0xb]
00486d81  sub        esi, eax
00486d83  je         0x486d98

// block 00486d85
00486d85  xor        eax, eax
00486d87  test       esi, esi
00486d89  setg       al
00486d8c  lea        eax, [eax + eax - 1]
00486d90  test       eax, eax
00486d92  jne        0x486b14

// block 00486d98
00486d98  movzx      esi, byte ptr [ecx - 0xa]
00486d9c  movzx      eax, byte ptr [edx - 0xa]
00486da0  sub        esi, eax
00486da2  je         0x486db1

// block 00486da4
00486da4  xor        eax, eax
00486da6  test       esi, esi
00486da8  setg       al
00486dab  lea        eax, [eax + eax - 1]
00486daf  mov        esi, eax

// block 00486db1
00486db1  mov        eax, esi
00486db3  jmp        0x486db7

// block 00486db5
00486db5  xor        eax, eax

// block 00486db7
00486db7  test       eax, eax
00486db9  jne        0x486b14

// block 00486dbf
00486dbf  mov        eax, dword ptr [ecx - 9]
00486dc2  cmp        eax, dword ptr [edx - 9]
00486dc5  je         0x486e40

// block 00486dc7
00486dc7  movzx      esi, al
00486dca  movzx      eax, byte ptr [edx - 9]
00486dce  sub        esi, eax
00486dd0  je         0x486de5

// block 00486dd2
00486dd2  xor        eax, eax
00486dd4  test       esi, esi
00486dd6  setg       al
00486dd9  lea        eax, [eax + eax - 1]
00486ddd  test       eax, eax
00486ddf  jne        0x486b14

// block 00486de5
00486de5  movzx      esi, byte ptr [ecx - 8]
00486de9  movzx      eax, byte ptr [edx - 8]
00486ded  sub        esi, eax
00486def  je         0x486e04

// block 00486df1
00486df1  xor        eax, eax
00486df3  test       esi, esi
00486df5  setg       al
00486df8  lea        eax, [eax + eax - 1]
00486dfc  test       eax, eax
00486dfe  jne        0x486b14

// block 00486e04
00486e04  movzx      esi, byte ptr [ecx - 7]
00486e08  movzx      eax, byte ptr [edx - 7]
00486e0c  sub        esi, eax
00486e0e  je         0x486e23

// block 00486e10
00486e10  xor        eax, eax
00486e12  test       esi, esi
00486e14  setg       al
00486e17  lea        eax, [eax + eax - 1]
00486e1b  test       eax, eax
00486e1d  jne        0x486b14

// block 00486e23
00486e23  movzx      esi, byte ptr [ecx - 6]
00486e27  movzx      eax, byte ptr [edx - 6]
00486e2b  sub        esi, eax
00486e2d  je         0x486e3c

// block 00486e2f
00486e2f  xor        eax, eax
00486e31  test       esi, esi
00486e33  setg       al
00486e36  lea        eax, [eax + eax - 1]
00486e3a  mov        esi, eax

// block 00486e3c
00486e3c  mov        eax, esi
00486e3e  jmp        0x486e42

// block 00486e40
00486e40  xor        eax, eax

// block 00486e42
00486e42  test       eax, eax
00486e44  jne        0x486b14

// block 00486e4a
00486e4a  mov        eax, dword ptr [ecx - 5]
00486e4d  cmp        eax, dword ptr [edx - 5]
00486e50  je         0x486ecb

// block 00486e52
00486e52  movzx      esi, al
00486e55  movzx      eax, byte ptr [edx - 5]
00486e59  sub        esi, eax
00486e5b  je         0x486e70

// block 00486e5d
00486e5d  xor        eax, eax
00486e5f  test       esi, esi
00486e61  setg       al
00486e64  lea        eax, [eax + eax - 1]
00486e68  test       eax, eax
00486e6a  jne        0x486b14

// block 00486e70
00486e70  movzx      esi, byte ptr [ecx - 4]
00486e74  movzx      eax, byte ptr [edx - 4]
00486e78  sub        esi, eax
00486e7a  je         0x486e8f

// block 00486e7c
00486e7c  xor        eax, eax
00486e7e  test       esi, esi
00486e80  setg       al
00486e83  lea        eax, [eax + eax - 1]
00486e87  test       eax, eax
00486e89  jne        0x486b14

// block 00486e8f
00486e8f  movzx      esi, byte ptr [ecx - 3]
00486e93  movzx      eax, byte ptr [edx - 3]
00486e97  sub        esi, eax
00486e99  je         0x486eae

// block 00486e9b
00486e9b  xor        eax, eax
00486e9d  test       esi, esi
00486e9f  setg       al
00486ea2  lea        eax, [eax + eax - 1]
00486ea6  test       eax, eax
00486ea8  jne        0x486b14

// block 00486eae
00486eae  movzx      esi, byte ptr [ecx - 2]
00486eb2  movzx      eax, byte ptr [edx - 2]
00486eb6  sub        esi, eax
00486eb8  je         0x486ec7

// block 00486eba
00486eba  xor        eax, eax
00486ebc  test       esi, esi
00486ebe  setg       al
00486ec1  lea        eax, [eax + eax - 1]
00486ec5  mov        esi, eax

// block 00486ec7
00486ec7  mov        eax, esi
00486ec9  jmp        0x486ecd

// block 00486ecb
00486ecb  xor        eax, eax

// block 00486ecd
00486ecd  test       eax, eax
00486ecf  jne        0x486b14

// block 00486ed5
00486ed5  movzx      eax, byte ptr [ecx - 1]
00486ed9  movzx      ecx, byte ptr [edx - 1]
00486edd  sub        eax, ecx
00486edf  je         0x486b14

// block 00486ee5
00486ee5  xor        ecx, ecx
00486ee7  test       eax, eax
00486ee9  setg       cl
00486eec  lea        ecx, [ecx + ecx - 1]
00486ef0  mov        eax, ecx
00486ef2  jmp        0x486b14

// block 00486ef7
00486ef7  mov        eax, dword ptr [ecx - 0x1e]
00486efa  cmp        eax, dword ptr [edx - 0x1e]
00486efd  je         0x486f78

// block 00486eff
00486eff  movzx      esi, al
00486f02  movzx      eax, byte ptr [edx - 0x1e]
00486f06  sub        esi, eax
00486f08  je         0x486f1d

// block 00486f0a
00486f0a  xor        eax, eax
00486f0c  test       esi, esi
00486f0e  setg       al
00486f11  lea        eax, [eax + eax - 1]
00486f15  test       eax, eax
00486f17  jne        0x486b14

// block 00486f1d
00486f1d  movzx      esi, byte ptr [ecx - 0x1d]
00486f21  movzx      eax, byte ptr [edx - 0x1d]
00486f25  sub        esi, eax
00486f27  je         0x486f3c

// block 00486f29
00486f29  xor        eax, eax
00486f2b  test       esi, esi
00486f2d  setg       al
00486f30  lea        eax, [eax + eax - 1]
00486f34  test       eax, eax
00486f36  jne        0x486b14

// block 00486f3c
00486f3c  movzx      esi, byte ptr [ecx - 0x1c]
00486f40  movzx      eax, byte ptr [edx - 0x1c]
00486f44  sub        esi, eax
00486f46  je         0x486f5b

// block 00486f48
00486f48  xor        eax, eax
00486f4a  test       esi, esi
00486f4c  setg       al
00486f4f  lea        eax, [eax + eax - 1]
00486f53  test       eax, eax
00486f55  jne        0x486b14

// block 00486f5b
00486f5b  movzx      esi, byte ptr [ecx - 0x1b]
00486f5f  movzx      eax, byte ptr [edx - 0x1b]
00486f63  sub        esi, eax
00486f65  je         0x486f74

// block 00486f67
00486f67  xor        eax, eax
00486f69  test       esi, esi
00486f6b  setg       al
00486f6e  lea        eax, [eax + eax - 1]
00486f72  mov        esi, eax

// block 00486f74
00486f74  mov        eax, esi
00486f76  jmp        0x486f7a

// block 00486f78
00486f78  xor        eax, eax

// block 00486f7a
00486f7a  test       eax, eax
00486f7c  jne        0x486b14

// block 00486f82
00486f82  mov        eax, dword ptr [ecx - 0x1a]
00486f85  cmp        eax, dword ptr [edx - 0x1a]
00486f88  je         0x487004

// block 00486f8a
00486f8a  movzx      eax, byte ptr [edx - 0x1a]
00486f8e  movzx      esi, byte ptr [ecx - 0x1a]
00486f92  sub        esi, eax
00486f94  je         0x486fa9

// block 00486f96
00486f96  xor        eax, eax
00486f98  test       esi, esi
00486f9a  setg       al
00486f9d  lea        eax, [eax + eax - 1]
00486fa1  test       eax, eax
00486fa3  jne        0x486b14

// block 00486fa9
00486fa9  movzx      esi, byte ptr [ecx - 0x19]
00486fad  movzx      eax, byte ptr [edx - 0x19]
00486fb1  sub        esi, eax
00486fb3  je         0x486fc8

// block 00486fb5
00486fb5  xor        eax, eax
00486fb7  test       esi, esi
00486fb9  setg       al
00486fbc  lea        eax, [eax + eax - 1]
00486fc0  test       eax, eax
00486fc2  jne        0x486b14

// block 00486fc8
00486fc8  movzx      esi, byte ptr [ecx - 0x18]
00486fcc  movzx      eax, byte ptr [edx - 0x18]
00486fd0  sub        esi, eax
00486fd2  je         0x486fe7

// block 00486fd4
00486fd4  xor        eax, eax
00486fd6  test       esi, esi
00486fd8  setg       al
00486fdb  lea        eax, [eax + eax - 1]
00486fdf  test       eax, eax
00486fe1  jne        0x486b14

// block 00486fe7
00486fe7  movzx      esi, byte ptr [ecx - 0x17]
00486feb  movzx      eax, byte ptr [edx - 0x17]
00486fef  sub        esi, eax
00486ff1  je         0x487000

// block 00486ff3
00486ff3  xor        eax, eax
00486ff5  test       esi, esi
00486ff7  setg       al
00486ffa  lea        eax, [eax + eax - 1]
00486ffe  mov        esi, eax

// block 00487000
00487000  mov        eax, esi
00487002  jmp        0x487006

// block 00487004
00487004  xor        eax, eax

// block 00487006
00487006  test       eax, eax
00487008  jne        0x486b14

// block 0048700e
0048700e  mov        eax, dword ptr [ecx - 0x16]
00487011  cmp        eax, dword ptr [edx - 0x16]
00487014  je         0x48708f

// block 00487016
00487016  movzx      esi, al
00487019  movzx      eax, byte ptr [edx - 0x16]
0048701d  sub        esi, eax
0048701f  je         0x487034

// block 00487021
00487021  xor        eax, eax
00487023  test       esi, esi
00487025  setg       al
00487028  lea        eax, [eax + eax - 1]
0048702c  test       eax, eax
0048702e  jne        0x486b14

// block 00487034
00487034  movzx      esi, byte ptr [ecx - 0x15]
00487038  movzx      eax, byte ptr [edx - 0x15]
0048703c  sub        esi, eax
0048703e  je         0x487053

// block 00487040
00487040  xor        eax, eax
00487042  test       esi, esi
00487044  setg       al
00487047  lea        eax, [eax + eax - 1]
0048704b  test       eax, eax
0048704d  jne        0x486b14

// block 00487053
00487053  movzx      esi, byte ptr [ecx - 0x14]
00487057  movzx      eax, byte ptr [edx - 0x14]
0048705b  sub        esi, eax
0048705d  je         0x487072

// block 0048705f
0048705f  xor        eax, eax
00487061  test       esi, esi
00487063  setg       al
00487066  lea        eax, [eax + eax - 1]
0048706a  test       eax, eax
0048706c  jne        0x486b14

// block 00487072
00487072  movzx      esi, byte ptr [ecx - 0x13]
00487076  movzx      eax, byte ptr [edx - 0x13]
0048707a  sub        esi, eax
0048707c  je         0x48708b

// block 0048707e
0048707e  xor        eax, eax
00487080  test       esi, esi
00487082  setg       al
00487085  lea        eax, [eax + eax - 1]
00487089  mov        esi, eax

// block 0048708b
0048708b  mov        eax, esi
0048708d  jmp        0x487091

// block 0048708f
0048708f  xor        eax, eax

// block 00487091
00487091  test       eax, eax
00487093  jne        0x486b14

// block 00487099
00487099  mov        eax, dword ptr [ecx - 0x12]
0048709c  cmp        eax, dword ptr [edx - 0x12]
0048709f  je         0x48711a

// block 004870a1
004870a1  movzx      esi, al
004870a4  movzx      eax, byte ptr [edx - 0x12]
004870a8  sub        esi, eax
004870aa  je         0x4870bf

// block 004870ac
004870ac  xor        eax, eax
004870ae  test       esi, esi
004870b0  setg       al
004870b3  lea        eax, [eax + eax - 1]
004870b7  test       eax, eax
004870b9  jne        0x486b14

// block 004870bf
004870bf  movzx      esi, byte ptr [ecx - 0x11]
004870c3  movzx      eax, byte ptr [edx - 0x11]
004870c7  sub        esi, eax
004870c9  je         0x4870de

// block 004870cb
004870cb  xor        eax, eax
004870cd  test       esi, esi
004870cf  setg       al
004870d2  lea        eax, [eax + eax - 1]
004870d6  test       eax, eax
004870d8  jne        0x486b14

// block 004870de
004870de  movzx      esi, byte ptr [ecx - 0x10]
004870e2  movzx      eax, byte ptr [edx - 0x10]
004870e6  sub        esi, eax
004870e8  je         0x4870fd

// block 004870ea
004870ea  xor        eax, eax
004870ec  test       esi, esi
004870ee  setg       al
004870f1  lea        eax, [eax + eax - 1]
004870f5  test       eax, eax
004870f7  jne        0x486b14

// block 004870fd
004870fd  movzx      esi, byte ptr [ecx - 0xf]
00487101  movzx      eax, byte ptr [edx - 0xf]
00487105  sub        esi, eax
00487107  je         0x487116

// block 00487109
00487109  xor        eax, eax
0048710b  test       esi, esi
0048710d  setg       al
00487110  lea        eax, [eax + eax - 1]
00487114  mov        esi, eax

// block 00487116
00487116  mov        eax, esi
00487118  jmp        0x48711c

// block 0048711a
0048711a  xor        eax, eax

// block 0048711c
0048711c  test       eax, eax
0048711e  jne        0x486b14

// block 00487124
00487124  mov        eax, dword ptr [ecx - 0xe]
00487127  cmp        eax, dword ptr [edx - 0xe]
0048712a  je         0x4871a5

// block 0048712c
0048712c  movzx      esi, al
0048712f  movzx      eax, byte ptr [edx - 0xe]
00487133  sub        esi, eax
00487135  je         0x48714a

// block 00487137
00487137  xor        eax, eax
00487139  test       esi, esi
0048713b  setg       al
0048713e  lea        eax, [eax + eax - 1]
00487142  test       eax, eax
00487144  jne        0x486b14

// block 0048714a
0048714a  movzx      esi, byte ptr [ecx - 0xd]
0048714e  movzx      eax, byte ptr [edx - 0xd]
00487152  sub        esi, eax
00487154  je         0x487169

// block 00487156
00487156  xor        eax, eax
00487158  test       esi, esi
0048715a  setg       al
0048715d  lea        eax, [eax + eax - 1]
00487161  test       eax, eax
00487163  jne        0x486b14

// block 00487169
00487169  movzx      esi, byte ptr [ecx - 0xc]
0048716d  movzx      eax, byte ptr [edx - 0xc]
00487171  sub        esi, eax
00487173  je         0x487188

// block 00487175
00487175  xor        eax, eax
00487177  test       esi, esi
00487179  setg       al
0048717c  lea        eax, [eax + eax - 1]
00487180  test       eax, eax
00487182  jne        0x486b14

// block 00487188
00487188  movzx      esi, byte ptr [ecx - 0xb]
0048718c  movzx      eax, byte ptr [edx - 0xb]
00487190  sub        esi, eax
00487192  je         0x4871a1

// block 00487194
00487194  xor        eax, eax
00487196  test       esi, esi
00487198  setg       al
0048719b  lea        eax, [eax + eax - 1]
0048719f  mov        esi, eax

// block 004871a1
004871a1  mov        eax, esi
004871a3  jmp        0x4871a7

// block 004871a5
004871a5  xor        eax, eax

// block 004871a7
004871a7  test       eax, eax
004871a9  jne        0x486b14

// block 004871af
004871af  mov        eax, dword ptr [ecx - 0xa]
004871b2  cmp        eax, dword ptr [edx - 0xa]
004871b5  je         0x487230

// block 004871b7
004871b7  movzx      esi, al
004871ba  movzx      eax, byte ptr [edx - 0xa]
004871be  sub        esi, eax
004871c0  je         0x4871d5

// block 004871c2
004871c2  xor        eax, eax
004871c4  test       esi, esi
004871c6  setg       al
004871c9  lea        eax, [eax + eax - 1]
004871cd  test       eax, eax
004871cf  jne        0x486b14

// block 004871d5
004871d5  movzx      esi, byte ptr [ecx - 9]
004871d9  movzx      eax, byte ptr [edx - 9]
004871dd  sub        esi, eax
004871df  je         0x4871f4

// block 004871e1
004871e1  xor        eax, eax
004871e3  test       esi, esi
004871e5  setg       al
004871e8  lea        eax, [eax + eax - 1]
004871ec  test       eax, eax
004871ee  jne        0x486b14

// block 004871f4
004871f4  movzx      esi, byte ptr [ecx - 8]
004871f8  movzx      eax, byte ptr [edx - 8]
004871fc  sub        esi, eax
004871fe  je         0x487213

// block 00487200
00487200  xor        eax, eax
00487202  test       esi, esi
00487204  setg       al
00487207  lea        eax, [eax + eax - 1]
0048720b  test       eax, eax
0048720d  jne        0x486b14

// block 00487213
00487213  movzx      esi, byte ptr [ecx - 7]
00487217  movzx      eax, byte ptr [edx - 7]
0048721b  sub        esi, eax
0048721d  je         0x48722c

// block 0048721f
0048721f  xor        eax, eax
00487221  test       esi, esi
00487223  setg       al
00487226  lea        eax, [eax + eax - 1]
0048722a  mov        esi, eax

// block 0048722c
0048722c  mov        eax, esi
0048722e  jmp        0x487232

// block 00487230
00487230  xor        eax, eax

// block 00487232
00487232  test       eax, eax
00487234  jne        0x486b14

// block 0048723a
0048723a  mov        eax, dword ptr [ecx - 6]
0048723d  cmp        eax, dword ptr [edx - 6]
00487240  je         0x4872bb

// block 00487242
00487242  movzx      esi, al
00487245  movzx      eax, byte ptr [edx - 6]
00487249  sub        esi, eax
0048724b  je         0x487260

// block 0048724d
0048724d  xor        eax, eax
0048724f  test       esi, esi
00487251  setg       al
00487254  lea        eax, [eax + eax - 1]
00487258  test       eax, eax
0048725a  jne        0x486b14

// block 00487260
00487260  movzx      esi, byte ptr [ecx - 5]
00487264  movzx      eax, byte ptr [edx - 5]
00487268  sub        esi, eax
0048726a  je         0x48727f

// block 0048726c
0048726c  xor        eax, eax
0048726e  test       esi, esi
00487270  setg       al
00487273  lea        eax, [eax + eax - 1]
00487277  test       eax, eax
00487279  jne        0x486b14

// block 0048727f
0048727f  movzx      esi, byte ptr [ecx - 4]
00487283  movzx      eax, byte ptr [edx - 4]
00487287  sub        esi, eax
00487289  je         0x48729e

// block 0048728b
0048728b  xor        eax, eax
0048728d  test       esi, esi
0048728f  setg       al
00487292  lea        eax, [eax + eax - 1]
00487296  test       eax, eax
00487298  jne        0x486b14

// block 0048729e
0048729e  movzx      esi, byte ptr [ecx - 3]
004872a2  movzx      eax, byte ptr [edx - 3]
004872a6  sub        esi, eax
004872a8  je         0x4872b7

// block 004872aa
004872aa  xor        eax, eax
004872ac  test       esi, esi
004872ae  setg       al
004872b1  lea        eax, [eax + eax - 1]
004872b5  mov        esi, eax

// block 004872b7
004872b7  mov        eax, esi
004872b9  jmp        0x4872bd

// block 004872bb
004872bb  xor        eax, eax

// block 004872bd
004872bd  test       eax, eax
004872bf  jne        0x486b14

// block 004872c5
004872c5  mov        ax, word ptr [ecx - 2]
004872c9  cmp        ax, word ptr [edx - 2]
004872cd  je         0x486b12

// block 004872d3
004872d3  jmp        0x4876c5

// block 004872d8
004872d8  mov        eax, dword ptr [ecx - 0x1f]
004872db  cmp        eax, dword ptr [edx - 0x1f]
004872de  je         0x48735a

// block 004872e0
004872e0  movzx      eax, byte ptr [edx - 0x1f]
004872e4  movzx      esi, byte ptr [ecx - 0x1f]
004872e8  sub        esi, eax
004872ea  je         0x4872ff

// block 004872ec
004872ec  xor        eax, eax
004872ee  test       esi, esi
004872f0  setg       al
004872f3  lea        eax, [eax + eax - 1]
004872f7  test       eax, eax
004872f9  jne        0x486b14

// block 004872ff
004872ff  movzx      esi, byte ptr [ecx - 0x1e]
00487303  movzx      eax, byte ptr [edx - 0x1e]
00487307  sub        esi, eax
00487309  je         0x48731e

// block 0048730b
0048730b  xor        eax, eax
0048730d  test       esi, esi
0048730f  setg       al
00487312  lea        eax, [eax + eax - 1]
00487316  test       eax, eax
00487318  jne        0x486b14

// block 0048731e
0048731e  movzx      esi, byte ptr [ecx - 0x1d]
00487322  movzx      eax, byte ptr [edx - 0x1d]
00487326  sub        esi, eax
00487328  je         0x48733d

// block 0048732a
0048732a  xor        eax, eax
0048732c  test       esi, esi
0048732e  setg       al
00487331  lea        eax, [eax + eax - 1]
00487335  test       eax, eax
00487337  jne        0x486b14

// block 0048733d
0048733d  movzx      esi, byte ptr [ecx - 0x1c]
00487341  movzx      eax, byte ptr [edx - 0x1c]
00487345  sub        esi, eax
00487347  je         0x487356

// block 00487349
00487349  xor        eax, eax
0048734b  test       esi, esi
0048734d  setg       al
00487350  lea        eax, [eax + eax - 1]
00487354  mov        esi, eax

// block 00487356
00487356  mov        eax, esi
00487358  jmp        0x48735c

// block 0048735a
0048735a  xor        eax, eax

// block 0048735c
0048735c  test       eax, eax
0048735e  jne        0x486b14

// block 00487364
00487364  mov        eax, dword ptr [ecx - 0x1b]
00487367  cmp        eax, dword ptr [edx - 0x1b]
0048736a  je         0x4873e5

// block 0048736c
0048736c  movzx      esi, al
0048736f  movzx      eax, byte ptr [edx - 0x1b]
00487373  sub        esi, eax
00487375  je         0x48738a

// block 00487377
00487377  xor        eax, eax
00487379  test       esi, esi
0048737b  setg       al
0048737e  lea        eax, [eax + eax - 1]
00487382  test       eax, eax
00487384  jne        0x486b14

// block 0048738a
0048738a  movzx      esi, byte ptr [ecx - 0x1a]
0048738e  movzx      eax, byte ptr [edx - 0x1a]
00487392  sub        esi, eax
00487394  je         0x4873a9

// block 00487396
00487396  xor        eax, eax
00487398  test       esi, esi
0048739a  setg       al
0048739d  lea        eax, [eax + eax - 1]
004873a1  test       eax, eax
004873a3  jne        0x486b14

// block 004873a9
004873a9  movzx      esi, byte ptr [ecx - 0x19]
004873ad  movzx      eax, byte ptr [edx - 0x19]
004873b1  sub        esi, eax
004873b3  je         0x4873c8

// block 004873b5
004873b5  xor        eax, eax
004873b7  test       esi, esi
004873b9  setg       al
004873bc  lea        eax, [eax + eax - 1]
004873c0  test       eax, eax
004873c2  jne        0x486b14

// block 004873c8
004873c8  movzx      esi, byte ptr [ecx - 0x18]
004873cc  movzx      eax, byte ptr [edx - 0x18]
004873d0  sub        esi, eax
004873d2  je         0x4873e1

// block 004873d4
004873d4  xor        eax, eax
004873d6  test       esi, esi
004873d8  setg       al
004873db  lea        eax, [eax + eax - 1]
004873df  mov        esi, eax

// block 004873e1
004873e1  mov        eax, esi
004873e3  jmp        0x4873e7

// block 004873e5
004873e5  xor        eax, eax

// block 004873e7
004873e7  test       eax, eax
004873e9  jne        0x486b14

// block 004873ef
004873ef  mov        eax, dword ptr [ecx - 0x17]
004873f2  cmp        eax, dword ptr [edx - 0x17]
004873f5  je         0x487470

// block 004873f7
004873f7  movzx      esi, al
004873fa  movzx      eax, byte ptr [edx - 0x17]
004873fe  sub        esi, eax
00487400  je         0x487415

// block 00487402
00487402  xor        eax, eax
00487404  test       esi, esi
00487406  setg       al
00487409  lea        eax, [eax + eax - 1]
0048740d  test       eax, eax
0048740f  jne        0x486b14

// block 00487415
00487415  movzx      esi, byte ptr [ecx - 0x16]
00487419  movzx      eax, byte ptr [edx - 0x16]
0048741d  sub        esi, eax
0048741f  je         0x487434

// block 00487421
00487421  xor        eax, eax
00487423  test       esi, esi
00487425  setg       al
00487428  lea        eax, [eax + eax - 1]
0048742c  test       eax, eax
0048742e  jne        0x486b14

// block 00487434
00487434  movzx      esi, byte ptr [ecx - 0x15]
00487438  movzx      eax, byte ptr [edx - 0x15]
0048743c  sub        esi, eax
0048743e  je         0x487453

// block 00487440
00487440  xor        eax, eax
00487442  test       esi, esi
00487444  setg       al
00487447  lea        eax, [eax + eax - 1]
0048744b  test       eax, eax
0048744d  jne        0x486b14

// block 00487453
00487453  movzx      esi, byte ptr [ecx - 0x14]
00487457  movzx      eax, byte ptr [edx - 0x14]
0048745b  sub        esi, eax
0048745d  je         0x48746c

// block 0048745f
0048745f  xor        eax, eax
00487461  test       esi, esi
00487463  setg       al
00487466  lea        eax, [eax + eax - 1]
0048746a  mov        esi, eax

// block 0048746c
0048746c  mov        eax, esi
0048746e  jmp        0x487472

// block 00487470
00487470  xor        eax, eax

// block 00487472
00487472  test       eax, eax
00487474  jne        0x486b14

// block 0048747a
0048747a  mov        eax, dword ptr [ecx - 0x13]
0048747d  cmp        eax, dword ptr [edx - 0x13]
00487480  je         0x4874fb

// block 00487482
00487482  movzx      esi, al
00487485  movzx      eax, byte ptr [edx - 0x13]
00487489  sub        esi, eax
0048748b  je         0x4874a0

// block 0048748d
0048748d  xor        eax, eax
0048748f  test       esi, esi
00487491  setg       al
00487494  lea        eax, [eax + eax - 1]
00487498  test       eax, eax
0048749a  jne        0x486b14

// block 004874a0
004874a0  movzx      esi, byte ptr [ecx - 0x12]
004874a4  movzx      eax, byte ptr [edx - 0x12]
004874a8  sub        esi, eax
004874aa  je         0x4874bf

// block 004874ac
004874ac  xor        eax, eax
004874ae  test       esi, esi
004874b0  setg       al
004874b3  lea        eax, [eax + eax - 1]
004874b7  test       eax, eax
004874b9  jne        0x486b14

// block 004874bf
004874bf  movzx      esi, byte ptr [ecx - 0x11]
004874c3  movzx      eax, byte ptr [edx - 0x11]
004874c7  sub        esi, eax
004874c9  je         0x4874de

// block 004874cb
004874cb  xor        eax, eax
004874cd  test       esi, esi
004874cf  setg       al
004874d2  lea        eax, [eax + eax - 1]
004874d6  test       eax, eax
004874d8  jne        0x486b14

// block 004874de
004874de  movzx      esi, byte ptr [ecx - 0x10]
004874e2  movzx      eax, byte ptr [edx - 0x10]
004874e6  sub        esi, eax
004874e8  je         0x4874f7

// block 004874ea
004874ea  xor        eax, eax
004874ec  test       esi, esi
004874ee  setg       al
004874f1  lea        eax, [eax + eax - 1]
004874f5  mov        esi, eax

// block 004874f7
004874f7  mov        eax, esi
004874f9  jmp        0x4874fd

// block 004874fb
004874fb  xor        eax, eax

// block 004874fd
004874fd  test       eax, eax
004874ff  jne        0x486b14

// block 00487505
00487505  mov        eax, dword ptr [ecx - 0xf]
00487508  cmp        eax, dword ptr [edx - 0xf]
0048750b  je         0x487586

// block 0048750d
0048750d  movzx      esi, al
00487510  movzx      eax, byte ptr [edx - 0xf]
00487514  sub        esi, eax
00487516  je         0x48752b

// block 00487518
00487518  xor        eax, eax
0048751a  test       esi, esi
0048751c  setg       al
0048751f  lea        eax, [eax + eax - 1]
00487523  test       eax, eax
00487525  jne        0x486b14

// block 0048752b
0048752b  movzx      esi, byte ptr [ecx - 0xe]
0048752f  movzx      eax, byte ptr [edx - 0xe]
00487533  sub        esi, eax
00487535  je         0x48754a

// block 00487537
00487537  xor        eax, eax
00487539  test       esi, esi
0048753b  setg       al
0048753e  lea        eax, [eax + eax - 1]
00487542  test       eax, eax
00487544  jne        0x486b14

// block 0048754a
0048754a  movzx      esi, byte ptr [ecx - 0xd]
0048754e  movzx      eax, byte ptr [edx - 0xd]
00487552  sub        esi, eax
00487554  je         0x487569

// block 00487556
00487556  xor        eax, eax
00487558  test       esi, esi
0048755a  setg       al
0048755d  lea        eax, [eax + eax - 1]
00487561  test       eax, eax
00487563  jne        0x486b14

// block 00487569
00487569  movzx      esi, byte ptr [ecx - 0xc]
0048756d  movzx      eax, byte ptr [edx - 0xc]
00487571  sub        esi, eax
00487573  je         0x487582

// block 00487575
00487575  xor        eax, eax
00487577  test       esi, esi
00487579  setg       al
0048757c  lea        eax, [eax + eax - 1]
00487580  mov        esi, eax

// block 00487582
00487582  mov        eax, esi
00487584  jmp        0x487588

// block 00487586
00487586  xor        eax, eax

// block 00487588
00487588  test       eax, eax
0048758a  jne        0x486b14

// block 00487590
00487590  mov        eax, dword ptr [ecx - 0xb]
00487593  cmp        eax, dword ptr [edx - 0xb]
00487596  je         0x487611

// block 00487598
00487598  movzx      esi, al
0048759b  movzx      eax, byte ptr [edx - 0xb]
0048759f  sub        esi, eax
004875a1  je         0x4875b6

// block 004875a3
004875a3  xor        eax, eax
004875a5  test       esi, esi
004875a7  setg       al
004875aa  lea        eax, [eax + eax - 1]
004875ae  test       eax, eax
004875b0  jne        0x486b14

// block 004875b6
004875b6  movzx      esi, byte ptr [ecx - 0xa]
004875ba  movzx      eax, byte ptr [edx - 0xa]
004875be  sub        esi, eax
004875c0  je         0x4875d5

// block 004875c2
004875c2  xor        eax, eax
004875c4  test       esi, esi
004875c6  setg       al
004875c9  lea        eax, [eax + eax - 1]
004875cd  test       eax, eax
004875cf  jne        0x486b14

// block 004875d5
004875d5  movzx      esi, byte ptr [ecx - 9]
004875d9  movzx      eax, byte ptr [edx - 9]
004875dd  sub        esi, eax
004875df  je         0x4875f4

// block 004875e1
004875e1  xor        eax, eax
004875e3  test       esi, esi
004875e5  setg       al
004875e8  lea        eax, [eax + eax - 1]
004875ec  test       eax, eax
004875ee  jne        0x486b14

// block 004875f4
004875f4  movzx      esi, byte ptr [ecx - 8]
004875f8  movzx      eax, byte ptr [edx - 8]
004875fc  sub        esi, eax
004875fe  je         0x48760d

// block 00487600
00487600  xor        eax, eax
00487602  test       esi, esi
00487604  setg       al
00487607  lea        eax, [eax + eax - 1]
0048760b  mov        esi, eax

// block 0048760d
0048760d  mov        eax, esi
0048760f  jmp        0x487613

// block 00487611
00487611  xor        eax, eax

// block 00487613
00487613  test       eax, eax
00487615  jne        0x486b14

// block 0048761b
0048761b  mov        eax, dword ptr [ecx - 7]
0048761e  cmp        eax, dword ptr [edx - 7]
00487621  je         0x48769c

// block 00487623
00487623  movzx      esi, al
00487626  movzx      eax, byte ptr [edx - 7]
0048762a  sub        esi, eax
0048762c  je         0x487641

// block 0048762e
0048762e  xor        eax, eax
00487630  test       esi, esi
00487632  setg       al
00487635  lea        eax, [eax + eax - 1]
00487639  test       eax, eax
0048763b  jne        0x486b14

// block 00487641
00487641  movzx      esi, byte ptr [ecx - 6]
00487645  movzx      eax, byte ptr [edx - 6]
00487649  sub        esi, eax
0048764b  je         0x487660

// block 0048764d
0048764d  xor        eax, eax
0048764f  test       esi, esi
00487651  setg       al
00487654  lea        eax, [eax + eax - 1]
00487658  test       eax, eax
0048765a  jne        0x486b14

// block 00487660
00487660  movzx      esi, byte ptr [ecx - 5]
00487664  movzx      eax, byte ptr [edx - 5]
00487668  sub        esi, eax
0048766a  je         0x48767f

// block 0048766c
0048766c  xor        eax, eax
0048766e  test       esi, esi
00487670  setg       al
00487673  lea        eax, [eax + eax - 1]
00487677  test       eax, eax
00487679  jne        0x486b14

// block 0048767f
0048767f  movzx      esi, byte ptr [ecx - 4]
00487683  movzx      eax, byte ptr [edx - 4]
00487687  sub        esi, eax
00487689  je         0x487698

// block 0048768b
0048768b  xor        eax, eax
0048768d  test       esi, esi
0048768f  setg       al
00487692  lea        eax, [eax + eax - 1]
00487696  mov        esi, eax

// block 00487698
00487698  mov        eax, esi
0048769a  jmp        0x48769e

// block 0048769c
0048769c  xor        eax, eax

// block 0048769e
0048769e  test       eax, eax
004876a0  jne        0x486b14

// block 004876a6
004876a6  movzx      esi, byte ptr [ecx - 3]
004876aa  movzx      eax, byte ptr [edx - 3]
004876ae  sub        esi, eax
004876b0  je         0x4876c5

// block 004876b2
004876b2  xor        eax, eax
004876b4  test       esi, esi
004876b6  setg       al
004876b9  lea        eax, [eax + eax - 1]
004876bd  test       eax, eax
004876bf  jne        0x486b14

// block 004876c5
004876c5  movzx      esi, byte ptr [ecx - 2]
004876c9  movzx      eax, byte ptr [edx - 2]
004876cd  sub        esi, eax
004876cf  je         0x486ed5

// block 004876d5
004876d5  xor        eax, eax
004876d7  test       esi, esi
004876d9  setg       al
004876dc  lea        eax, [eax + eax - 1]
004876e0  jmp        0x486ecd
