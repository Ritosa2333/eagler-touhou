
// block 0047d64b
0047d64b  mov        ecx, dword ptr [0x4b4318]
0047d651  push       ebx
0047d652  push       esi
0047d653  mov        edx, 0x4000
0047d658  push       edi

// block 0047d659
0047d659  xor        esi, esi
0047d65b  cmp        byte ptr [ecx], 0x5f
0047d65e  jne        0x47d669

// block 0047d660
0047d660  inc        ecx
0047d661  mov        esi, edx
0047d663  mov        dword ptr [0x4b4318], ecx

// block 0047d669
0047d669  mov        al, byte ptr [ecx]
0047d66b  cmp        al, 0x41
0047d66d  jl         0x47d677

// block 0047d66f
0047d66f  cmp        al, 0x5a
0047d671  jle        0x47d700

// block 0047d677
0047d677  cmp        al, 0x24
0047d679  jne        0x47d9b5

// block 0047d67f
0047d67f  xor        bl, bl
0047d681  inc        ecx
0047d682  mov        dword ptr [0x4b4318], ecx
0047d688  movsx      eax, byte ptr [ecx]
0047d68b  cmp        eax, 0x42
0047d68e  jg         0x47d884

// block 0047d694
0047d694  je         0x47d879

// block 0047d69a
0047d69a  test       eax, eax
0047d69c  je         0x47d86e

// block 0047d6a2
0047d6a2  cmp        eax, 0x24
0047d6a5  jne        0x47d802

// block 0047d6ab
0047d6ab  lea        eax, [ecx + 1]
0047d6ae  cmp        byte ptr [eax], 0x50
0047d6b1  jne        0x47d6b5

// block 0047d6b3
0047d6b3  mov        ecx, eax

// block 0047d6b5
0047d6b5  inc        ecx
0047d6b6  mov        dword ptr [0x4b4318], ecx
0047d6bc  movsx      eax, byte ptr [ecx]
0047d6bf  cmp        eax, 0x4a
0047d6c2  jg         0x47d6dc

// block 0047d6c4
0047d6c4  je         0x47d838

// block 0047d6ca
0047d6ca  sub        eax, 0
0047d6cd  je         0x47d82e

// block 0047d6d3
0047d6d3  sub        eax, 0x46
0047d6d6  je         0x47d6f4

// block 0047d6d8
0047d6d8  dec        eax
0047d6d9  dec        eax
0047d6da  jmp        0x47d6f2

// block 0047d6dc
0047d6dc  cmp        eax, 0x4c
0047d6df  jl         0x47d75a

// block 0047d6e1
0047d6e1  cmp        eax, 0x4d
0047d6e4  jle        0x47d6f4

// block 0047d6e6
0047d6e6  cmp        eax, 0x4f
0047d6e9  jle        0x47d838

// block 0047d6ef
0047d6ef  cmp        eax, 0x51

// block 0047d6f2
0047d6f2  jne        0x47d75a

// block 0047d6f4
0047d6f4  inc        ecx
0047d6f5  mov        dword ptr [0x4b4318], ecx
0047d6fb  jmp        0x47d659

// block 0047d700
0047d700  movsx      eax, byte ptr [ecx]
0047d703  sub        eax, 0x41
0047d706  mov        edx, 0x8000
0047d70b  inc        ecx
0047d70c  or         esi, edx
0047d70e  mov        dword ptr [0x4b4318], ecx
0047d714  test       al, 1
0047d716  je         0x47d720

// block 0047d718
0047d718  or         esi, 0x2000
0047d71e  jmp        0x47d726

// block 0047d720
0047d720  and        esi, 0xffffdfff

// block 0047d726
0047d726  cmp        eax, 0x18
0047d729  jge        0x47db0c

// block 0047d72f
0047d72f  mov        ebx, 0xffff9fff
0047d734  mov        edi, 0x800
0047d739  test       edx, esi
0047d73b  je         0x47d747

// block 0047d73d
0047d73d  and        esi, 0xffffefff
0047d743  or         esi, edi
0047d745  jmp        0x47d749

// block 0047d747
0047d747  and        esi, ebx

// block 0047d749
0047d749  mov        ecx, eax
0047d74b  and        ecx, 0x18
0047d74e  je         0x47d795

// block 0047d750
0047d750  cmp        ecx, 8
0047d753  je         0x47d778

// block 0047d755
0047d755  cmp        ecx, 0x10
0047d758  je         0x47d764

// block 0047d75a
0047d75a  mov        eax, 0xffff
0047d75f  jmp        0x47db0e

// block 0047d764
0047d764  test       edx, esi
0047d766  je         0x47d770

// block 0047d768
0047d768  and        esi, 0xffffff3f
0047d76e  jmp        0x47d7ac

// block 0047d770
0047d770  and        esi, 0xffffe7ff
0047d776  jmp        0x47d7ac

// block 0047d778
0047d778  test       edx, esi
0047d77a  je         0x47d787

// block 0047d77c
0047d77c  and        esi, 0xffffffbf
0047d77f  or         esi, 0x80
0047d785  jmp        0x47d7ac

// block 0047d787
0047d787  and        esi, 0xfffff7ff
0047d78d  or         esi, 0x1000
0047d793  jmp        0x47d7ac

// block 0047d795
0047d795  test       edx, esi
0047d797  je         0x47d7a4

// block 0047d799
0047d799  and        esi, 0xffffff7f
0047d79f  or         esi, 0x40
0047d7a2  jmp        0x47d7ac

// block 0047d7a4
0047d7a4  and        esi, 0xffffefff
0047d7aa  or         esi, edi

// block 0047d7ac
0047d7ac  and        eax, 6
0047d7af  sub        eax, 0
0047d7b2  je         0x47db0c

// block 0047d7b8
0047d7b8  dec        eax
0047d7b9  dec        eax
0047d7ba  je         0x47d7e6

// block 0047d7bc
0047d7bc  dec        eax
0047d7bd  dec        eax
0047d7be  je         0x47d7d5

// block 0047d7c0
0047d7c0  dec        eax
0047d7c1  dec        eax
0047d7c2  jne        0x47d75a

// block 0047d7c4
0047d7c4  and        esi, 0xfffffcff
0047d7ca  or         esi, 0x400
0047d7d0  jmp        0x47db0c

// block 0047d7d5
0047d7d5  and        esi, 0xfffff9ff
0047d7db  or         esi, 0x100
0047d7e1  jmp        0x47db0c

// block 0047d7e6
0047d7e6  test       edx, esi
0047d7e8  je         0x47d7fb

// block 0047d7ea
0047d7ea  and        esi, 0xfffffaff
0047d7f0  or         esi, 0x200
0047d7f6  jmp        0x47db0c

// block 0047d7fb
0047d7fb  and        esi, ebx
0047d7fd  jmp        0x47db0c

// block 0047d802
0047d802  cmp        eax, 0x2f
0047d805  jle        0x47d75a

// block 0047d80b
0047d80b  cmp        eax, 0x35
0047d80e  jle        0x47d8bf

// block 0047d814
0047d814  cmp        eax, 0x41
0047d817  jne        0x47d75a

// block 0047d81d
0047d81d  and        esi, 0xfffff4ff
0047d823  or         esi, 0x9000
0047d829  jmp        0x47d9a9

// block 0047d82e
0047d82e  mov        eax, 0xfffe
0047d833  jmp        0x47db0e

// block 0047d838
0047d838  inc        ecx
0047d839  mov        dword ptr [0x4b4318], ecx
0047d83f  mov        al, byte ptr [ecx]
0047d841  cmp        al, 0x30
0047d843  jl         0x47d864

// block 0047d845
0047d845  cmp        al, 0x39
0047d847  jg         0x47d864

// block 0047d849
0047d849  movsx      eax, al
0047d84c  lea        eax, [ecx + eax - 0x2f]
0047d850  mov        dword ptr [0x4b4318], eax
0047d855  call       0x47d64b

// block 0047d85a
0047d85a  or         eax, 0x10000
0047d85f  jmp        0x47db0e

// block 0047d864
0047d864  mov        esi, 0xffff
0047d869  jmp        0x47d9a9

// block 0047d86e
0047d86e  mov        esi, 0xfffe
0047d873  dec        ecx
0047d874  jmp        0x47d9a9

// block 0047d879
0047d879  or         esi, 0x9800
0047d87f  jmp        0x47d9a9

// block 0047d884
0047d884  sub        eax, 0x43
0047d887  je         0x47d9a3

// block 0047d88d
0047d88d  dec        eax
0047d88e  je         0x47d995

// block 0047d894
0047d894  dec        eax
0047d895  je         0x47d987

// block 0047d89b
0047d89b  sub        eax, 0xd
0047d89e  jne        0x47d75a

// block 0047d8a4
0047d8a4  inc        ecx
0047d8a5  mov        dword ptr [0x4b4318], ecx
0047d8ab  mov        al, byte ptr [ecx]
0047d8ad  cmp        al, 0x30
0047d8af  mov        bl, 1
0047d8b1  jl         0x47d973

// block 0047d8b7
0047d8b7  cmp        al, 0x35
0047d8b9  jg         0x47d973

// block 0047d8bf
0047d8bf  movsx      eax, byte ptr [ecx]
0047d8c2  mov        edx, 0x8000
0047d8c7  or         esi, edx
0047d8c9  sub        eax, 0x30
0047d8cc  mov        edi, 0x800
0047d8d1  test       edx, esi
0047d8d3  je         0x47d8df

// block 0047d8d5
0047d8d5  and        esi, 0xffffefff
0047d8db  or         esi, edi
0047d8dd  jmp        0x47d8e5

// block 0047d8df
0047d8df  and        esi, 0xffff9fff

// block 0047d8e5
0047d8e5  test       bl, bl
0047d8e7  je         0x47d8f7

// block 0047d8e9
0047d8e9  and        esi, 0xfffffeff
0047d8ef  or         esi, 0x600
0047d8f5  jmp        0x47d903

// block 0047d8f7
0047d8f7  and        esi, 0xfffffdff
0047d8fd  or         esi, 0x500

// block 0047d903
0047d903  test       al, 1
0047d905  je         0x47d90f

// block 0047d907
0047d907  or         esi, 0x2000
0047d90d  jmp        0x47d915

// block 0047d90f
0047d90f  and        esi, 0xffffdfff

// block 0047d915
0047d915  and        eax, 6
0047d918  sub        eax, 0
0047d91b  je         0x47d95a

// block 0047d91d
0047d91d  dec        eax
0047d91e  dec        eax
0047d91f  je         0x47d93d

// block 0047d921
0047d921  dec        eax
0047d922  dec        eax
0047d923  jne        0x47d75a

// block 0047d929
0047d929  test       edx, esi
0047d92b  je         0x47d935

// block 0047d92d
0047d92d  and        esi, 0xffffff3f
0047d933  jmp        0x47d9a9

// block 0047d935
0047d935  and        esi, 0xffffe7ff
0047d93b  jmp        0x47d9a9

// block 0047d93d
0047d93d  test       edx, esi
0047d93f  je         0x47d94c

// block 0047d941
0047d941  and        esi, 0xffffffbf
0047d944  or         esi, 0x80
0047d94a  jmp        0x47d9a9

// block 0047d94c
0047d94c  and        esi, 0xfffff7ff
0047d952  or         esi, 0x1000
0047d958  jmp        0x47d9a9

// block 0047d95a
0047d95a  test       edx, esi
0047d95c  je         0x47d969

// block 0047d95e
0047d95e  and        esi, 0xffffff7f
0047d964  or         esi, 0x40
0047d967  jmp        0x47d9a9

// block 0047d969
0047d969  and        esi, 0xffffefff
0047d96f  or         esi, edi
0047d971  jmp        0x47d9a9

// block 0047d973
0047d973  xor        ecx, ecx
0047d975  test       al, al
0047d977  sete       cl
0047d97a  add        ecx, 0xfffe
0047d980  mov        eax, ecx
0047d982  jmp        0x47db0e

// block 0047d987
0047d987  and        esi, 0xfffff6ff
0047d98d  or         esi, 0x9200
0047d993  jmp        0x47d9a9

// block 0047d995
0047d995  and        esi, 0xfffff5ff
0047d99b  or         esi, 0x9100
0047d9a1  jmp        0x47d9a9

// block 0047d9a3
0047d9a3  or         esi, 0x7c00

// block 0047d9a9
0047d9a9  inc        ecx
0047d9aa  mov        dword ptr [0x4b4318], ecx
0047d9b0  jmp        0x47db0c

// block 0047d9b5
0047d9b5  mov        al, byte ptr [ecx]
0047d9b7  cmp        al, 0x30
0047d9b9  jl         0x47daeb

// block 0047d9bf
0047d9bf  cmp        al, 0x38
0047d9c1  jg         0x47daeb

// block 0047d9c7
0047d9c7  movsx      eax, al
0047d9ca  inc        ecx
0047d9cb  add        eax, -0x30
0047d9ce  and        esi, 0xffff7fff
0047d9d4  mov        dword ptr [0x4b4318], ecx
0047d9da  cmp        eax, 8
0047d9dd  ja         0x47d75a

// block 0047d9e3
0047d9e3  jmp        dword ptr [eax*4 + 0x47db13]

// block 0047d9ea
0047d9ea  mov        edx, 0x8000
0047d9ef  test       edx, esi
0047d9f1  je         0x47da01

// block 0047d9f3
0047d9f3  and        esi, 0xfffffaff
0047d9f9  or         esi, 0x200
0047d9ff  jmp        0x47da07

// block 0047da01
0047da01  and        esi, 0xffff9fff

// block 0047da07
0047da07  test       edx, esi
0047da09  je         0x47da19

// block 0047da0b
0047da0b  and        esi, 0xffffff7f
0047da11  or         esi, 0x40
0047da14  jmp        0x47db0c

// block 0047da19
0047da19  and        esi, 0xffffefff
0047da1f  or         esi, 0x800
0047da25  jmp        0x47db0c

// block 0047da2a
0047da2a  mov        edx, 0x8000
0047da2f  test       edx, esi
0047da31  je         0x47da41

// block 0047da33
0047da33  and        esi, 0xfffffaff
0047da39  or         esi, 0x200
0047da3f  jmp        0x47da47

// block 0047da41
0047da41  and        esi, 0xffff9fff

// block 0047da47
0047da47  test       edx, esi
0047da49  je         0x47da59

// block 0047da4b
0047da4b  and        esi, 0xffffffbf
0047da4e  or         esi, 0x80
0047da54  jmp        0x47db0c

// block 0047da59
0047da59  and        esi, 0xfffff7ff
0047da5f  or         esi, 0x1000
0047da65  jmp        0x47db0c

// block 0047da6a
0047da6a  mov        edx, 0x8000
0047da6f  test       edx, esi
0047da71  je         0x47da81

// block 0047da73
0047da73  and        esi, 0xfffffaff
0047da79  or         esi, 0x200
0047da7f  jmp        0x47da87

// block 0047da81
0047da81  and        esi, 0xffff9fff

// block 0047da87
0047da87  test       edx, esi
0047da89  je         0x47da93

// block 0047da8b
0047da8b  and        esi, 0xffffff3f
0047da91  jmp        0x47db0c

// block 0047da93
0047da93  and        esi, 0xffffe7ff
0047da99  jmp        0x47db0c

// block 0047da9b
0047da9b  and        esi, 0xffffdfff
0047daa1  or         esi, edx
0047daa3  jmp        0x47db0c

// block 0047daa5
0047daa5  and        esi, 0xffffe3ff
0047daab  or         esi, 0x6000
0047dab1  jmp        0x47db0c

// block 0047dab3
0047dab3  and        esi, 0xffffbfff
0047dab9  or         esi, 0x2000
0047dabf  jmp        0x47db0c

// block 0047dac1
0047dac1  and        esi, 0xffffebff
0047dac7  or         esi, 0x6800
0047dacd  jmp        0x47db0c

// block 0047dacf
0047dacf  and        esi, 0xfffff3ff
0047dad5  or         esi, 0x7000
0047dadb  jmp        0x47db0c

// block 0047dadd
0047dadd  and        esi, 0xfffffbff
0047dae3  or         esi, 0x7800
0047dae9  jmp        0x47db0c

// block 0047daeb
0047daeb  cmp        al, 0x39
0047daed  jne        0x47dafd

// block 0047daef
0047daef  inc        ecx
0047daf0  mov        dword ptr [0x4b4318], ecx
0047daf6  mov        esi, 0xfffd
0047dafb  jmp        0x47db0c

// block 0047dafd
0047dafd  xor        ecx, ecx
0047daff  test       al, al
0047db01  setne      cl
0047db04  add        ecx, 0xfffe
0047db0a  mov        esi, ecx

// block 0047db0c
0047db0c  mov        eax, esi

// block 0047db0e
0047db0e  pop        edi
0047db0f  pop        esi
0047db10  pop        ebx
0047db11  ret        
