
// block 004907fa
004907fa  mov        edi, edi
004907fc  push       ebp
004907fd  mov        ebp, esp
004907ff  sub        esp, 0x2c
00490802  mov        eax, dword ptr [0x4ad138]
00490807  xor        eax, ebp
00490809  mov        dword ptr [ebp - 4], eax
0049080c  mov        eax, dword ptr [ebp + 0x14]
0049080f  push       ebx
00490810  push       esi
00490811  push       edi
00490812  xor        edi, edi
00490814  mov        ebx, edx
00490816  mov        esi, ecx
00490818  mov        dword ptr [ebp - 0x24], ebx
0049081b  mov        dword ptr [ebp - 0x1c], eax
0049081e  cmp        dword ptr [0x4b43b0], edi
00490824  jne        0x49085e

// block 00490826
00490826  push       1
00490828  mov        eax, 0x49e1c8
0049082d  push       eax
0049082e  push       1
00490830  push       eax
00490831  push       edi
00490832  push       edi
00490833  call       dword ptr [0x49817c]

// block 00490839
00490839  test       eax, eax
0049083b  je         0x490849

// block 0049083d
0049083d  mov        dword ptr [0x4b43b0], 1
00490847  jmp        0x49085e

// block 00490849
00490849  call       dword ptr [0x4980e4]

// block 0049084f
0049084f  cmp        eax, 0x78
00490852  jne        0x49085e

// block 00490854
00490854  mov        dword ptr [0x4b43b0], 2

// block 0049085e
0049085e  cmp        dword ptr [ebp + 0x10], edi
00490861  jle        0x490889

// block 00490863
00490863  push       dword ptr [ebp + 0x10]
00490866  mov        eax, ebx
00490868  call       0x4907dc

// block 0049086d
0049086d  pop        ecx
0049086e  mov        dword ptr [ebp + 0x10], eax

// block 00490871
00490871  mov        edx, dword ptr [ebp + 0x18]
00490874  cmp        edx, edi
00490876  jle        0x490896

// block 00490878
00490878  mov        eax, dword ptr [ebp - 0x1c]
0049087b  push       edx
0049087c  call       0x4907dc

// block 00490881
00490881  mov        edx, eax
00490883  pop        ecx
00490884  mov        dword ptr [ebp + 0x18], edx
00490887  jmp        0x49089b

// block 00490889
00490889  cmp        dword ptr [ebp + 0x10], -1
0049088d  jge        0x490871

// block 0049088f
0049088f  xor        eax, eax
00490891  jmp        0x490b58

// block 00490896
00490896  cmp        edx, -1
00490899  jl         0x49088f

// block 0049089b
0049089b  mov        ecx, dword ptr [0x4b43b0]
004908a1  cmp        ecx, 2
004908a4  je         0x490aa3

// block 004908aa
004908aa  cmp        ecx, edi
004908ac  je         0x490aa3

// block 004908b2
004908b2  xor        eax, eax
004908b4  inc        eax
004908b5  cmp        ecx, eax
004908b7  jne        0x49088f

// block 004908b9
004908b9  mov        dword ptr [ebp - 0x28], edi
004908bc  cmp        dword ptr [ebp + 0x1c], edi
004908bf  jne        0x4908c9

// block 004908c1
004908c1  mov        ecx, dword ptr [esi]
004908c3  mov        ecx, dword ptr [ecx + 4]
004908c6  mov        dword ptr [ebp + 0x1c], ecx

// block 004908c9
004908c9  cmp        dword ptr [ebp + 0x10], edi
004908cc  je         0x4908d6

// block 004908ce
004908ce  cmp        edx, edi
004908d0  jne        0x490970

// block 004908d6
004908d6  cmp        dword ptr [ebp + 0x10], edx
004908d9  jne        0x4908e3

// block 004908db
004908db  push       2

// block 004908dd
004908dd  pop        eax
004908de  jmp        0x490b58

// block 004908e3
004908e3  cmp        edx, eax
004908e5  jg         0x490b58

// block 004908eb
004908eb  cmp        dword ptr [ebp + 0x10], eax
004908ee  jle        0x4908f4

// block 004908f0
004908f0  push       3
004908f2  jmp        0x4908dd

// block 004908f4
004908f4  lea        eax, [ebp - 0x18]
004908f7  push       eax
004908f8  push       dword ptr [ebp + 0x1c]
004908fb  call       dword ptr [0x498164]

// block 00490901
00490901  test       eax, eax
00490903  je         0x49088f

// block 00490905
00490905  cmp        dword ptr [ebp + 0x10], edi
00490908  jle        0x490933

// block 0049090a
0049090a  cmp        dword ptr [ebp - 0x18], 2
0049090e  jb         0x4908f0

// block 00490910
00490910  cmp        byte ptr [ebp - 0x12], 0
00490914  lea        eax, [ebp - 0x12]
00490917  je         0x4908f0

// block 00490919
00490919  mov        dl, byte ptr [eax + 1]
0049091c  test       dl, dl
0049091e  je         0x4908f0

// block 00490920
00490920  mov        cl, byte ptr [ebx]
00490922  cmp        cl, byte ptr [eax]
00490924  jb         0x49092a

// block 00490926
00490926  cmp        cl, dl
00490928  jbe        0x4908db

// block 0049092a
0049092a  inc        eax
0049092b  inc        eax
0049092c  cmp        byte ptr [eax], 0
0049092f  jne        0x490919

// block 00490931
00490931  jmp        0x4908f0

// block 00490933
00490933  cmp        dword ptr [ebp + 0x18], edi
00490936  jle        0x490970

// block 00490938
00490938  cmp        dword ptr [ebp - 0x18], 2
0049093c  jae        0x490946

// block 0049093e
0049093e  xor        eax, eax
00490940  inc        eax
00490941  jmp        0x490b58

// block 00490946
00490946  cmp        byte ptr [ebp - 0x12], 0
0049094a  lea        eax, [ebp - 0x12]
0049094d  je         0x49093e

// block 0049094f
0049094f  mov        dl, byte ptr [eax + 1]
00490952  test       dl, dl
00490954  je         0x49093e

// block 00490956
00490956  mov        ecx, dword ptr [ebp - 0x1c]
00490959  mov        cl, byte ptr [ecx]
0049095b  cmp        cl, byte ptr [eax]
0049095d  jb         0x490967

// block 0049095f
0049095f  cmp        cl, dl
00490961  jbe        0x4908db

// block 00490967
00490967  inc        eax
00490968  inc        eax
00490969  cmp        byte ptr [eax], 0
0049096c  jne        0x49094f

// block 0049096e
0049096e  jmp        0x49093e

// block 00490970
00490970  mov        esi, dword ptr [0x4980dc]
00490976  push       edi
00490977  push       edi
00490978  push       dword ptr [ebp + 0x10]
0049097b  push       ebx
0049097c  push       9
0049097e  push       dword ptr [ebp + 0x1c]
00490981  call       esi

// block 00490983
00490983  mov        ebx, eax
00490985  mov        dword ptr [ebp - 0x2c], ebx
00490988  cmp        ebx, edi
0049098a  je         0x49088f

// block 00490990
00490990  mov        edi, 0x400
00490995  test       ebx, ebx
00490997  jle        0x4909d9

// block 00490999
00490999  push       -0x20
0049099b  xor        edx, edx
0049099d  pop        eax
0049099e  div        ebx
004909a0  cmp        eax, 2
004909a3  jb         0x4909d9

// block 004909a5
004909a5  lea        eax, [ebx + ebx + 8]
004909a9  cmp        eax, edi
004909ab  ja         0x4909c0

// block 004909ad
004909ad  call       0x48d830

// block 004909b2
004909b2  mov        eax, esp
004909b4  test       eax, eax
004909b6  je         0x4909d4

// block 004909b8
004909b8  mov        dword ptr [eax], 0xcccc
004909be  jmp        0x4909d1

// block 004909c0
004909c0  push       eax
004909c1  call       0x46d04a

// block 004909c6
004909c6  pop        ecx
004909c7  test       eax, eax
004909c9  je         0x4909d4

// block 004909cb
004909cb  mov        dword ptr [eax], 0xdddd

// block 004909d1
004909d1  add        eax, 8

// block 004909d4
004909d4  mov        dword ptr [ebp - 0x20], eax
004909d7  jmp        0x4909dd

// block 004909d9
004909d9  and        dword ptr [ebp - 0x20], 0

// block 004909dd
004909dd  cmp        dword ptr [ebp - 0x20], 0
004909e1  je         0x49088f

// block 004909e7
004909e7  push       ebx
004909e8  push       dword ptr [ebp - 0x20]
004909eb  push       dword ptr [ebp + 0x10]
004909ee  push       dword ptr [ebp - 0x24]
004909f1  push       1
004909f3  push       dword ptr [ebp + 0x1c]
004909f6  call       esi

// block 004909f8
004909f8  test       eax, eax
004909fa  je         0x490a92

// block 00490a00
00490a00  push       0
00490a02  push       0
00490a04  push       dword ptr [ebp + 0x18]
00490a07  push       dword ptr [ebp - 0x1c]
00490a0a  push       9
00490a0c  push       dword ptr [ebp + 0x1c]
00490a0f  call       esi

// block 00490a11
00490a11  mov        ebx, eax
00490a13  test       ebx, ebx
00490a15  je         0x490a92

// block 00490a17
00490a17  jle        0x490a5b

// block 00490a19
00490a19  push       -0x20
00490a1b  xor        edx, edx
00490a1d  pop        eax
00490a1e  div        ebx
00490a20  cmp        eax, 2
00490a23  jb         0x490a5b

// block 00490a25
00490a25  lea        eax, [ebx + ebx + 8]
00490a29  cmp        eax, edi
00490a2b  ja         0x490a43

// block 00490a2d
00490a2d  call       0x48d830

// block 00490a32
00490a32  mov        edi, esp
00490a34  test       edi, edi
00490a36  je         0x490a92

// block 00490a38
00490a38  mov        dword ptr [edi], 0xcccc
00490a3e  add        edi, 8
00490a41  jmp        0x490a5d

// block 00490a43
00490a43  push       eax
00490a44  call       0x46d04a

// block 00490a49
00490a49  pop        ecx
00490a4a  test       eax, eax
00490a4c  je         0x490a57

// block 00490a4e
00490a4e  mov        dword ptr [eax], 0xdddd
00490a54  add        eax, 8

// block 00490a57
00490a57  mov        edi, eax
00490a59  jmp        0x490a5d

// block 00490a5b
00490a5b  xor        edi, edi

// block 00490a5d
00490a5d  test       edi, edi
00490a5f  je         0x490a92

// block 00490a61
00490a61  push       ebx
00490a62  push       edi
00490a63  push       dword ptr [ebp + 0x18]
00490a66  push       dword ptr [ebp - 0x1c]
00490a69  push       1
00490a6b  push       dword ptr [ebp + 0x1c]
00490a6e  call       esi

// block 00490a70
00490a70  test       eax, eax
00490a72  je         0x490a8b

// block 00490a74
00490a74  push       ebx
00490a75  push       edi
00490a76  push       dword ptr [ebp - 0x2c]
00490a79  push       dword ptr [ebp - 0x20]
00490a7c  push       dword ptr [ebp + 0xc]
00490a7f  push       dword ptr [ebp + 8]
00490a82  call       dword ptr [0x49817c]

// block 00490a88
00490a88  mov        dword ptr [ebp - 0x28], eax

// block 00490a8b
00490a8b  push       edi
00490a8c  call       0x48326a

// block 00490a91
00490a91  pop        ecx

// block 00490a92
00490a92  push       dword ptr [ebp - 0x20]
00490a95  call       0x48326a

// block 00490a9a
00490a9a  mov        eax, dword ptr [ebp - 0x28]
00490a9d  pop        ecx
00490a9e  jmp        0x490b58

// block 00490aa3
00490aa3  xor        edi, edi
00490aa5  xor        ebx, ebx
00490aa7  cmp        dword ptr [ebp + 8], edi
00490aaa  jne        0x490ab4

// block 00490aac
00490aac  mov        eax, dword ptr [esi]
00490aae  mov        eax, dword ptr [eax + 0x14]
00490ab1  mov        dword ptr [ebp + 8], eax

// block 00490ab4
00490ab4  cmp        dword ptr [ebp + 0x1c], edi
00490ab7  jne        0x490ac1

// block 00490ab9
00490ab9  mov        eax, dword ptr [esi]
00490abb  mov        eax, dword ptr [eax + 4]
00490abe  mov        dword ptr [ebp + 0x1c], eax

// block 00490ac1
00490ac1  push       dword ptr [ebp + 8]
00490ac4  call       0x48d62f

// block 00490ac9
00490ac9  mov        esi, eax
00490acb  pop        ecx
00490acc  cmp        esi, -1
00490acf  je         0x49088f

// block 00490ad5
00490ad5  cmp        esi, dword ptr [ebp + 0x1c]
00490ad8  je         0x490b2a

// block 00490ada
00490ada  push       0
00490adc  push       0
00490ade  lea        eax, [ebp + 0x10]
00490ae1  push       eax
00490ae2  push       dword ptr [ebp - 0x24]
00490ae5  push       esi
00490ae6  push       dword ptr [ebp + 0x1c]
00490ae9  call       0x48d678

// block 00490aee
00490aee  mov        ebx, eax
00490af0  add        esp, 0x18
00490af3  test       ebx, ebx
00490af5  je         0x49088f

// block 00490afb
00490afb  push       0
00490afd  push       0
00490aff  lea        eax, [ebp + 0x18]
00490b02  push       eax
00490b03  push       dword ptr [ebp - 0x1c]
00490b06  push       esi
00490b07  push       dword ptr [ebp + 0x1c]
00490b0a  call       0x48d678

// block 00490b0f
00490b0f  mov        edi, eax
00490b11  add        esp, 0x18
00490b14  test       edi, edi
00490b16  jne        0x490b24

// block 00490b18
00490b18  push       ebx
00490b19  call       0x46c941

// block 00490b1e
00490b1e  pop        ecx
00490b1f  jmp        0x49088f

// block 00490b24
00490b24  mov        dword ptr [ebp - 0x24], ebx
00490b27  mov        dword ptr [ebp - 0x1c], edi

// block 00490b2a
00490b2a  push       dword ptr [ebp + 0x18]
00490b2d  push       dword ptr [ebp - 0x1c]
00490b30  push       dword ptr [ebp + 0x10]
00490b33  push       dword ptr [ebp - 0x24]
00490b36  push       dword ptr [ebp + 0xc]
00490b39  push       dword ptr [ebp + 8]
00490b3c  call       dword ptr [0x4981b8]

// block 00490b42
00490b42  mov        esi, eax
00490b44  test       ebx, ebx
00490b46  je         0x490b56

// block 00490b48
00490b48  push       ebx
00490b49  call       0x46c941

// block 00490b4e
00490b4e  push       edi
00490b4f  call       0x46c941

// block 00490b54
00490b54  pop        ecx
00490b55  pop        ecx

// block 00490b56
00490b56  mov        eax, esi

// block 00490b58
00490b58  lea        esp, [ebp - 0x38]
00490b5b  pop        edi
00490b5c  pop        esi
00490b5d  pop        ebx
00490b5e  mov        ecx, dword ptr [ebp - 4]
00490b61  xor        ecx, ebp
00490b63  call       0x46c932

// block 00490b68
00490b68  leave      
00490b69  ret        
