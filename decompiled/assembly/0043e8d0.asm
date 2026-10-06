
// block 0043e8d0
0043e8d0  push       ebp
0043e8d1  mov        ebp, esp
0043e8d3  and        esp, 0xfffffff8
0043e8d6  sub        esp, 0x14c
0043e8dc  mov        eax, dword ptr [0x4ad138]
0043e8e1  xor        eax, esp
0043e8e3  mov        dword ptr [esp + 0x148], eax
0043e8ea  push       ebx
0043e8eb  push       esi
0043e8ec  mov        esi, ecx
0043e8ee  mov        eax, dword ptr [esi + 0x30]
0043e8f1  xor        ebx, ebx
0043e8f3  sub        eax, ebx
0043e8f5  push       edi
0043e8f6  je         0x43eae4

// block 0043e8fc
0043e8fc  sub        eax, 1
0043e8ff  je         0x43e950

// block 0043e901
0043e901  sub        eax, 3
0043e904  jne        0x43ec5c

// block 0043e90a
0043e90a  fld1       
0043e90c  mov        edi, dword ptr [esi + 0x2dc]
0043e912  push       ecx
0043e913  fstp       dword ptr [esp]
0043e916  call       0x468d90

// block 0043e91b
0043e91b  mov        ecx, dword ptr [esi + 0x2dc]
0043e921  cmp        ecx, ebx
0043e923  je         0x43e93e

// block 0043e925
0043e925  mov        eax, dword ptr [ecx + 4]
0043e928  cmp        dword ptr [eax + 4], ebx
0043e92b  jne        0x43ec5c

// block 0043e931
0043e931  cmp        ecx, ebx
0043e933  je         0x43e93e

// block 0043e935
0043e935  mov        edx, dword ptr [ecx]
0043e937  mov        eax, dword ptr [edx + 0x14]
0043e93a  push       1
0043e93c  call       eax

// block 0043e93e
0043e93e  mov        dword ptr [esi + 0x2dc], ebx
0043e944  mov        dword ptr [esi + 0x30], 1
0043e94b  jmp        0x43ec5c

// block 0043e950
0043e950  mov        ecx, dword ptr [esi + 0x3c]
0043e953  lea        edi, [esi + 0x3c]
0043e956  mov        al, 0x20
0043e958  mov        dword ptr [edi + 4], ecx
0043e95b  test       byte ptr [0x4d48c4], al
0043e961  jne        0x43e96b

// block 0043e963
0043e963  test       byte ptr [0x4d48c0], al
0043e969  je         0x43e974

// block 0043e96b
0043e96b  push       1
0043e96d  mov        eax, edi
0043e96f  call       0x464970

// block 0043e974
0043e974  mov        al, 0x10
0043e976  test       byte ptr [0x4d48c4], al
0043e97c  jne        0x43e986

// block 0043e97e
0043e97e  test       byte ptr [0x4d48c0], al
0043e984  je         0x43e98f

// block 0043e986
0043e986  push       -1
0043e988  mov        eax, edi
0043e98a  call       0x464970

// block 0043e98f
0043e98f  mov        edi, dword ptr [edi]
0043e991  sub        edi, ebx
0043e993  je         0x43ea6e

// block 0043e999
0043e999  sub        edi, 1
0043e99c  je         0x43e9c9

// block 0043e99e
0043e99e  sub        edi, 1
0043e9a1  jne        0x43ec5c

// block 0043e9a7
0043e9a7  test       dword ptr [0x4d48c4], 0x80001
0043e9b1  je         0x43ec5c

// block 0043e9b7
0043e9b7  lea        ecx, [edi + 3]
0043e9ba  mov        eax, 0x4ce8e8
0043e9bf  call       0x40f720

// block 0043e9c4
0043e9c4  jmp        0x43ec5c

// block 0043e9c9
0043e9c9  cmp        dword ptr [esi + 0x2d8], ebx
0043e9cf  je         0x43ec5c

// block 0043e9d5
0043e9d5  mov        edx, dword ptr [esi + 0x1ec]
0043e9db  lea        edi, [esi + 0x1ec]
0043e9e1  mov        al, 0x80
0043e9e3  mov        dword ptr [edi + 4], edx
0043e9e6  test       byte ptr [0x4d48c4], al
0043e9ec  jne        0x43e9f6

// block 0043e9ee
0043e9ee  test       byte ptr [0x4d48c0], al
0043e9f4  je         0x43e9ff

// block 0043e9f6
0043e9f6  push       1
0043e9f8  mov        eax, edi
0043e9fa  call       0x464970

// block 0043e9ff
0043e9ff  mov        al, 0x40
0043ea01  test       byte ptr [0x4d48c4], al
0043ea07  jne        0x43ea11

// block 0043ea09
0043ea09  test       byte ptr [0x4d48c0], al
0043ea0f  je         0x43ea1a

// block 0043ea11
0043ea11  push       -1
0043ea13  mov        eax, edi
0043ea15  call       0x464970

// block 0043ea1a
0043ea1a  test       dword ptr [0x4d48c8], 0x80001
0043ea24  je         0x43ec5c

// block 0043ea2a
0043ea2a  mov        ecx, dword ptr [esi + 0x2dc]
0043ea30  cmp        ecx, ebx
0043ea32  je         0x43ea43

// block 0043ea34
0043ea34  mov        eax, dword ptr [ecx]
0043ea36  mov        edx, dword ptr [eax + 0x14]
0043ea39  push       1
0043ea3b  call       edx

// block 0043ea3d
0043ea3d  mov        dword ptr [esi + 0x2dc], ebx

// block 0043ea43
0043ea43  mov        eax, dword ptr [esi + 0x2d8]
0043ea49  mov        ecx, dword ptr [edi]
0043ea4b  mov        edx, dword ptr [eax + 0x8c]
0043ea51  mov        ecx, dword ptr [edx + ecx*8]
0043ea54  push       ecx
0043ea55  mov        edi, eax
0043ea57  call       0x469570

// block 0043ea5c
0043ea5c  mov        dword ptr [esi + 0x2dc], eax
0043ea62  mov        dword ptr [esi + 0x30], 4
0043ea69  jmp        0x43ec5c

// block 0043ea6e
0043ea6e  mov        eax, dword ptr [esi + 0x114]
0043ea74  lea        edi, [esi + 0x114]
0043ea7a  mov        dword ptr [edi + 4], eax
0043ea7d  mov        al, 0x80
0043ea7f  test       byte ptr [0x4d48c4], al
0043ea85  jne        0x43ea8f

// block 0043ea87
0043ea87  test       byte ptr [0x4d48c0], al
0043ea8d  je         0x43ea98

// block 0043ea8f
0043ea8f  push       1
0043ea91  mov        eax, edi
0043ea93  call       0x464970

// block 0043ea98
0043ea98  mov        al, 0x40
0043ea9a  test       byte ptr [0x4d48c4], al
0043eaa0  jne        0x43eaaa

// block 0043eaa2
0043eaa2  test       byte ptr [0x4d48c0], al
0043eaa8  je         0x43eab3

// block 0043eaaa
0043eaaa  push       -1
0043eaac  mov        eax, edi
0043eaae  call       0x464970

// block 0043eab3
0043eab3  test       dword ptr [0x4d48c4], 0x80001
0043eabd  je         0x43ec5c

// block 0043eac3
0043eac3  and        dword ptr [esi + 0x2d4], 0xfffffffd
0043eaca  push       esi
0043eacb  lea        eax, [esi + 0x10]
0043eace  mov        edi, 0x43e710
0043ead3  call       0x464cb0

// block 0043ead8
0043ead8  mov        dword ptr [esi + 0x30], 2
0043eadf  jmp        0x43ec5c

// block 0043eae4
0043eae4  call       0x43e380

// block 0043eae9
0043eae9  lea        ecx, [esp + 0x10]
0043eaed  push       ecx
0043eaee  push       0x49f7d8
0043eaf3  call       dword ptr [0x498078]

// block 0043eaf9
0043eaf9  mov        edi, eax
0043eafb  cmp        edi, -1
0043eafe  je         0x43eb11

// block 0043eb00
0043eb00  lea        edx, [esp + 0x10]
0043eb04  push       edx
0043eb05  push       edi
0043eb06  inc        ebx
0043eb07  call       dword ptr [0x49807c]

// block 0043eb0d
0043eb0d  test       eax, eax
0043eb0f  jne        0x43eb00

// block 0043eb11
0043eb11  push       edi
0043eb12  call       dword ptr [0x498080]

// block 0043eb18
0043eb18  xor        edi, edi
0043eb1a  mov        dword ptr [esi + 0x38], ebx
0043eb1d  cmp        ebx, edi
0043eb1f  je         0x43ebb0

// block 0043eb25
0043eb25  lea        eax, [ebx*4]
0043eb2c  push       eax
0043eb2d  call       0x46d04a

// block 0043eb32
0043eb32  add        esp, 4
0043eb35  lea        ecx, [esp + 0x10]
0043eb39  push       ecx
0043eb3a  push       0x49f7d8
0043eb3f  mov        dword ptr [esi + 0x34], eax
0043eb42  call       dword ptr [0x498078]

// block 0043eb48
0043eb48  mov        dword ptr [esp + 0xc], eax
0043eb4c  test       ebx, ebx
0043eb4e  jle        0x43eba3

// block 0043eb50
0043eb50  lea        eax, [esp + 0x3c]
0043eb54  lea        edx, [eax + 1]

// block 0043eb57
0043eb57  mov        cl, byte ptr [eax]
0043eb59  inc        eax
0043eb5a  test       cl, cl
0043eb5c  jne        0x43eb57

// block 0043eb5e
0043eb5e  sub        eax, edx
0043eb60  inc        eax
0043eb61  push       eax
0043eb62  call       0x46d04a

// block 0043eb67
0043eb67  mov        edx, dword ptr [esi + 0x34]
0043eb6a  mov        dword ptr [edx + edi*4], eax
0043eb6d  mov        eax, dword ptr [esi + 0x34]
0043eb70  mov        edx, dword ptr [eax + edi*4]
0043eb73  add        esp, 4
0043eb76  lea        ecx, [esp + 0x3c]
0043eb7a  lea        ebx, [ebx]

// block 0043eb80
0043eb80  mov        al, byte ptr [ecx]
0043eb82  mov        byte ptr [edx], al
0043eb84  inc        ecx
0043eb85  inc        edx
0043eb86  test       al, al
0043eb88  jne        0x43eb80

// block 0043eb8a
0043eb8a  mov        edx, dword ptr [esp + 0xc]
0043eb8e  lea        ecx, [esp + 0x10]
0043eb92  push       ecx
0043eb93  push       edx
0043eb94  call       dword ptr [0x49807c]

// block 0043eb9a
0043eb9a  test       eax, eax
0043eb9c  je         0x43eba3

// block 0043eb9e
0043eb9e  inc        edi
0043eb9f  cmp        edi, ebx
0043eba1  jl         0x43eb50

// block 0043eba3
0043eba3  mov        eax, dword ptr [esp + 0xc]
0043eba7  push       eax
0043eba8  call       dword ptr [0x498080]

// block 0043ebae
0043ebae  xor        edi, edi

// block 0043ebb0
0043ebb0  mov        ecx, 1
0043ebb5  mov        dword ptr [esi + 0x44], 3
0043ebbc  mov        dword ptr [esi + 0x10c], ecx
0043ebc2  mov        eax, dword ptr [esi + 0x44]
0043ebc5  cmp        eax, edi
0043ebc7  je         0x43ebd8

// block 0043ebc9
0043ebc9  jg         0x43ebd1

// block 0043ebcb
0043ebcb  dec        eax
0043ebcc  mov        dword ptr [esi + 0x3c], eax
0043ebcf  jmp        0x43ebdb

// block 0043ebd1
0043ebd1  xor        eax, eax
0043ebd3  mov        dword ptr [esi + 0x3c], eax
0043ebd6  jmp        0x43ebdb

// block 0043ebd8
0043ebd8  mov        dword ptr [esi + 0x3c], edi

// block 0043ebdb
0043ebdb  mov        eax, ebx
0043ebdd  mov        dword ptr [esi + 0x11c], ebx
0043ebe3  mov        dword ptr [esi + 0x1e4], ecx
0043ebe9  cmp        eax, edi
0043ebeb  je         0x43ec02

// block 0043ebed
0043ebed  jg         0x43ebf8

// block 0043ebef
0043ebef  dec        eax
0043ebf0  mov        dword ptr [esi + 0x114], eax
0043ebf6  jmp        0x43ec08

// block 0043ebf8
0043ebf8  xor        eax, eax
0043ebfa  mov        dword ptr [esi + 0x114], eax
0043ec00  jmp        0x43ec08

// block 0043ec02
0043ec02  mov        dword ptr [esi + 0x114], edi

// block 0043ec08
0043ec08  mov        eax, ecx
0043ec0a  mov        dword ptr [esi + 0x1f4], ecx
0043ec10  mov        dword ptr [esi + 0x2bc], ecx
0043ec16  cmp        eax, edi
0043ec18  je         0x43ec2f

// block 0043ec1a
0043ec1a  jg         0x43ec25

// block 0043ec1c
0043ec1c  dec        eax
0043ec1d  mov        dword ptr [esi + 0x1ec], eax
0043ec23  jmp        0x43ec35

// block 0043ec25
0043ec25  xor        eax, eax
0043ec27  mov        dword ptr [esi + 0x1ec], eax
0043ec2d  jmp        0x43ec35

// block 0043ec2f
0043ec2f  mov        dword ptr [esi + 0x1ec], edi

// block 0043ec35
0043ec35  fldz       
0043ec37  mov        dword ptr [esi + 0x30], ecx
0043ec3a  fst        dword ptr [esi + 0x2c4]
0043ec40  mov        dword ptr [esi + 0x2d0], 0x3e8
0043ec4a  fld        dword ptr [0x4a3d78]
0043ec50  fstp       dword ptr [esi + 0x2c8]
0043ec56  fstp       dword ptr [esi + 0x2cc]

// block 0043ec5c
0043ec5c  mov        ecx, dword ptr [esp + 0x154]
0043ec63  pop        edi
0043ec64  pop        esi
0043ec65  pop        ebx
0043ec66  xor        ecx, esp
0043ec68  mov        eax, 1
0043ec6d  call       0x46c932

// block 0043ec72
0043ec72  mov        esp, ebp
0043ec74  pop        ebp
0043ec75  ret        
