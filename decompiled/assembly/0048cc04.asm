
// block 0048cc04
0048cc04  push       0x18
0048cc06  push       0x4ab078
0048cc0b  call       0x46fcec

// block 0048cc10
0048cc10  or         dword ptr [ebp - 0x1c], 0xffffffff
0048cc14  xor        edi, edi
0048cc16  mov        dword ptr [ebp - 0x24], edi
0048cc19  push       0xb
0048cc1b  call       0x46ebd7

// block 0048cc20
0048cc20  pop        ecx
0048cc21  test       eax, eax
0048cc23  jne        0x48cc2d

// block 0048cc25
0048cc25  or         eax, 0xffffffff
0048cc28  jmp        0x48cd8f

// block 0048cc2d
0048cc2d  push       0xb
0048cc2f  call       0x46ec9a

// block 0048cc34
0048cc34  pop        ecx
0048cc35  mov        dword ptr [ebp - 4], edi

// block 0048cc38
0048cc38  mov        dword ptr [ebp - 0x28], edi
0048cc3b  cmp        edi, 0x40
0048cc3e  jge        0x48cd80

// block 0048cc44
0048cc44  mov        esi, dword ptr [edi*4 + 0x4d6320]
0048cc4b  test       esi, esi
0048cc4d  je         0x48cd0d

// block 0048cc53
0048cc53  mov        dword ptr [ebp - 0x20], esi
0048cc56  mov        eax, dword ptr [edi*4 + 0x4d6320]
0048cc5d  add        eax, 0x800
0048cc62  cmp        esi, eax
0048cc64  jae        0x48cd01

// block 0048cc6a
0048cc6a  test       byte ptr [esi + 4], 1
0048cc6e  jne        0x48cccc

// block 0048cc70
0048cc70  cmp        dword ptr [esi + 8], 0
0048cc74  jne        0x48ccaf

// block 0048cc76
0048cc76  push       0xa
0048cc78  call       0x46ec9a

// block 0048cc7d
0048cc7d  pop        ecx
0048cc7e  xor        ebx, ebx
0048cc80  inc        ebx
0048cc81  mov        dword ptr [ebp - 4], ebx
0048cc84  cmp        dword ptr [esi + 8], 0
0048cc88  jne        0x48cca6

// block 0048cc8a
0048cc8a  push       0xfa0
0048cc8f  lea        eax, [esi + 0xc]
0048cc92  push       eax
0048cc93  call       0x47b416

// block 0048cc98
0048cc98  pop        ecx
0048cc99  pop        ecx
0048cc9a  test       eax, eax
0048cc9c  jne        0x48cca3

// block 0048cc9e
0048cc9e  mov        dword ptr [ebp - 0x24], ebx
0048cca1  jmp        0x48cca6

// block 0048cca3
0048cca3  inc        dword ptr [esi + 8]

// block 0048cca6
0048cca6  and        dword ptr [ebp - 4], 0
0048ccaa  call       0x48ccd7

// block 0048ccaf
0048ccaf  cmp        dword ptr [ebp - 0x24], 0
0048ccb3  jne        0x48cccc

// block 0048ccb5
0048ccb5  lea        ebx, [esi + 0xc]
0048ccb8  push       ebx
0048ccb9  call       dword ptr [0x498088]

// block 0048ccbf
0048ccbf  test       byte ptr [esi + 4], 1
0048ccc3  je         0x48cce0

// block 0048ccc5
0048ccc5  push       ebx
0048ccc6  call       dword ptr [0x49808c]

// block 0048cccc
0048cccc  add        esi, 0x40
0048cccf  jmp        0x48cc53

// block 0048cce0
0048cce0  cmp        dword ptr [ebp - 0x24], 0
0048cce4  jne        0x48cccc

// block 0048cce6
0048cce6  mov        byte ptr [esi + 4], 1
0048ccea  or         dword ptr [esi], 0xffffffff
0048cced  sub        esi, dword ptr [edi*4 + 0x4d6320]
0048ccf4  sar        esi, 6
0048ccf7  mov        eax, edi
0048ccf9  shl        eax, 5
0048ccfc  add        esi, eax
0048ccfe  mov        dword ptr [ebp - 0x1c], esi

// block 0048cd01
0048cd01  cmp        dword ptr [ebp - 0x1c], -1
0048cd05  jne        0x48cd80

// block 0048cd07
0048cd07  inc        edi
0048cd08  jmp        0x48cc38

// block 0048cd0d
0048cd0d  push       0x40
0048cd0f  push       0x20
0048cd11  call       0x477813

// block 0048cd16
0048cd16  pop        ecx
0048cd17  pop        ecx
0048cd18  mov        dword ptr [ebp - 0x20], eax
0048cd1b  test       eax, eax
0048cd1d  je         0x48cd80

// block 0048cd1f
0048cd1f  lea        ecx, [edi*4 + 0x4d6320]
0048cd26  mov        dword ptr [ecx], eax
0048cd28  add        dword ptr [0x4d6308], 0x20

// block 0048cd2f
0048cd2f  mov        edx, dword ptr [ecx]
0048cd31  add        edx, 0x800
0048cd37  cmp        eax, edx
0048cd39  jae        0x48cd52

// block 0048cd3b
0048cd3b  mov        byte ptr [eax + 4], 0
0048cd3f  or         dword ptr [eax], 0xffffffff
0048cd42  mov        byte ptr [eax + 5], 0xa
0048cd46  and        dword ptr [eax + 8], 0
0048cd4a  add        eax, 0x40
0048cd4d  mov        dword ptr [ebp - 0x20], eax
0048cd50  jmp        0x48cd2f

// block 0048cd52
0048cd52  shl        edi, 5
0048cd55  mov        dword ptr [ebp - 0x1c], edi
0048cd58  mov        eax, edi
0048cd5a  sar        eax, 5
0048cd5d  mov        ecx, edi
0048cd5f  and        ecx, 0x1f
0048cd62  shl        ecx, 6
0048cd65  mov        eax, dword ptr [eax*4 + 0x4d6320]
0048cd6c  mov        byte ptr [eax + ecx + 4], 1
0048cd71  push       edi
0048cd72  call       0x48cb3d

// block 0048cd77
0048cd77  pop        ecx
0048cd78  test       eax, eax
0048cd7a  jne        0x48cd80

// block 0048cd7c
0048cd7c  or         dword ptr [ebp - 0x1c], 0xffffffff

// block 0048cd80
0048cd80  mov        dword ptr [ebp - 4], 0xfffffffe
0048cd87  call       0x48cd95

// block 0048cd8c
0048cd8c  mov        eax, dword ptr [ebp - 0x1c]

// block 0048cd8f
0048cd8f  call       0x46fd31

// block 0048cd94
0048cd94  ret        
