
// block 004dc5a3
004dc5a3  add        byte ptr [ecx], al
004dc5a5  add        al, byte ptr [ebx]
004dc5a7  add        al, 5
004dc5a9  push       es
004dc5aa  pop        es
004dc5ab  or         byte ptr [edx], cl
004dc5ad  or         al, 0xe
004dc5af  adc        byte ptr [eax + ebx], dl
004dc5b2  sbb        al, 0x20
004dc5b4  sub        byte ptr [eax], dh
004dc5b6  cmp        byte ptr [eax + 0x50], al
004dc5b9  pushal     
004dc5ba  jo         0x4dc53c

// block 004dc5bc
004dc5bc  mov        al, byte ptr [0xe0c0]
004dc5c1  add        byte ptr [eax], al
004dc5c3  add        byte ptr [eax], al
004dc5c5  add        byte ptr [eax], al
004dc5c7  add        dword ptr [ecx], eax
004dc5c9  add        dword ptr [ecx], eax
004dc5cb  add        al, byte ptr [edx]
004dc5cd  add        al, byte ptr [edx]
004dc5cf  add        eax, dword ptr [ebx]
004dc5d1  add        eax, dword ptr [ebx]
004dc5d3  add        al, 4
004dc5d5  add        al, 4
004dc5d7  add        eax, 0x50505
004dc5dc  add        byte ptr [eax], al
004dc5de  add        byte ptr [ecx], al
004dc5e0  add        dword ptr [edx], eax
004dc5e2  add        al, byte ptr [ebx]
004dc5e4  add        eax, dword ptr [esp + eax]
004dc5e7  add        eax, 0x7060605
004dc5ec  pop        es
004dc5ed  or         byte ptr [eax], cl
004dc5ef  or         dword ptr [ecx], ecx
004dc5f1  or         cl, byte ptr [edx]
004dc5f3  or         ecx, dword ptr [ebx]
004dc5f5  or         al, 0xc
004dc5f7  or         eax, 0xf0e0e0d
004dc5fc  movups     xmm2, xmmword ptr [eax]
004dc5ff  adc        dword ptr [ecx], edx
004dc601  adc        dword ptr [ecx], edx
004dc603  adc        dword ptr [ecx], edx
004dc605  adc        dword ptr [ecx], edx
004dc607  adc        dword ptr [ecx], edx
004dc609  adc        dword ptr [ecx], edx
004dc60b  adc        dword ptr [ecx], edx
004dc60d  adc        dl, byte ptr [edx]
004dc60f  adc        dl, byte ptr [edx]
004dc611  adc        dl, byte ptr [edx]
004dc613  adc        dl, byte ptr [edx]
