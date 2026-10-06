
// block 0044bab0
0044bab0  push       -1
0044bab2  push       0x496edb
0044bab7  mov        eax, dword ptr fs:[0]
0044babd  push       eax
0044babe  sub        esp, 8
0044bac1  push       ebp
0044bac2  push       esi
0044bac3  push       edi
0044bac4  mov        eax, dword ptr [0x4ad138]
0044bac9  xor        eax, esp
0044bacb  push       eax
0044bacc  lea        eax, [esp + 0x18]
0044bad0  mov        dword ptr fs:[0], eax
0044bad6  mov        ebp, dword ptr [esp + 0x2c]
0044bada  xor        ecx, ecx
0044badc  lea        esi, [ebp + 1]
0044badf  mov        eax, esi
0044bae1  mov        edx, 0x10
0044bae6  mul        edx
0044bae8  seto       cl
0044baeb  neg        ecx
0044baed  or         ecx, eax
0044baef  xor        eax, eax
0044baf1  add        ecx, 4
0044baf4  setb       al
0044baf7  neg        eax
0044baf9  or         eax, ecx
0044bafb  push       eax
0044bafc  call       0x46c9ea

// block 0044bb01
0044bb01  add        esp, 4
0044bb04  mov        dword ptr [esp + 0x14], eax
0044bb08  mov        dword ptr [esp + 0x20], 0
0044bb10  test       eax, eax
0044bb12  je         0x44bb34

// block 0044bb14
0044bb14  push       0x44bc50
0044bb19  push       0x44bcc0
0044bb1e  push       esi
0044bb1f  lea        edi, [eax + 4]
0044bb22  push       0x10
0044bb24  push       edi
0044bb25  mov        dword ptr [eax], esi
0044bb27  mov        dword ptr [esp + 0x24], edi
0044bb2b  call       0x46ce49

// block 0044bb30
0044bb30  test       edi, edi
0044bb32  jne        0x44bb4b

// block 0044bb34
0044bb34  xor        eax, eax
0044bb36  mov        ecx, dword ptr [esp + 0x18]
0044bb3a  mov        dword ptr fs:[0], ecx
0044bb41  pop        ecx
0044bb42  pop        edi
0044bb43  pop        esi
0044bb44  pop        ebp
0044bb45  add        esp, 0x14
0044bb48  ret        0xc

// block 0044bb4b
0044bb4b  mov        esi, dword ptr [esp + 0x28]
0044bb4f  test       ebp, ebp
0044bb51  jle        0x44bbf0

// block 0044bb57
0044bb57  mov        ecx, dword ptr [esp + 0x2c]
0044bb5b  lea        ebp, [edi + 8]
0044bb5e  mov        dword ptr [esp + 0x28], ecx

// block 0044bb62
0044bb62  mov        eax, esi
0044bb64  lea        edx, [eax + 1]

// block 0044bb67
0044bb67  mov        cl, byte ptr [eax]
0044bb69  inc        eax
0044bb6a  test       cl, cl
0044bb6c  jne        0x44bb67

// block 0044bb6e
0044bb6e  sub        eax, edx
0044bb70  inc        eax
0044bb71  push       eax
0044bb72  call       0x46d04a

// block 0044bb77
0044bb77  add        esp, 4
0044bb7a  test       eax, eax
0044bb7c  je         0x44bb92

// block 0044bb7e
0044bb7e  mov        edi, eax
0044bb80  mov        ecx, esi
0044bb82  sub        edi, esi

// block 0044bb84
0044bb84  mov        dl, byte ptr [ecx]
0044bb86  mov        byte ptr [edi + ecx], dl
0044bb89  inc        ecx
0044bb8a  test       dl, dl
0044bb8c  jne        0x44bb84

// block 0044bb8e
0044bb8e  mov        edi, dword ptr [esp + 0x10]

// block 0044bb92
0044bb92  mov        dword ptr [ebp - 8], eax
0044bb95  mov        eax, esi
0044bb97  lea        edx, [eax + 1]
0044bb9a  lea        ebx, [ebx]

// block 0044bba0
0044bba0  mov        cl, byte ptr [eax]
0044bba2  inc        eax
0044bba3  test       cl, cl
0044bba5  jne        0x44bba0

// block 0044bba7
0044bba7  sub        eax, edx
0044bba9  inc        eax
0044bbaa  mov        ecx, eax
0044bbac  and        ecx, 0x80000003
0044bbb2  jns        0x44bbb9

// block 0044bbb4
0044bbb4  dec        ecx
0044bbb5  or         ecx, 0xfffffffc
0044bbb8  inc        ecx

// block 0044bbb9
0044bbb9  je         0x44bbc4

// block 0044bbbb
0044bbbb  mov        edx, 4
0044bbc0  sub        edx, ecx
0044bbc2  add        eax, edx

// block 0044bbc4
0044bbc4  add        esi, eax
0044bbc6  mov        eax, dword ptr [esi]
0044bbc8  add        esi, 4
0044bbcb  mov        dword ptr [ebp - 4], eax
0044bbce  mov        ecx, dword ptr [esi]
0044bbd0  add        esi, 4
0044bbd3  mov        dword ptr [ebp], ecx
0044bbd6  mov        edx, dword ptr [esi]
0044bbd8  mov        dword ptr [ebp + 4], edx
0044bbdb  add        esi, 4
0044bbde  add        ebp, 0x10
0044bbe1  sub        dword ptr [esp + 0x28], 1
0044bbe6  jne        0x44bb62

// block 0044bbec
0044bbec  mov        ebp, dword ptr [esp + 0x2c]

// block 0044bbf0
0044bbf0  mov        ecx, dword ptr [esp + 0x30]
0044bbf4  shl        ebp, 4
0044bbf7  lea        eax, [edi + ebp]
0044bbfa  mov        dword ptr [eax + 4], ecx
0044bbfd  mov        dword ptr [eax + 8], 0
0044bc04  mov        eax, edi
0044bc06  mov        ecx, dword ptr [esp + 0x18]
0044bc0a  mov        dword ptr fs:[0], ecx
0044bc11  pop        ecx
0044bc12  pop        edi
0044bc13  pop        esi
0044bc14  pop        ebp
0044bc15  add        esp, 0x14
0044bc18  ret        0xc
