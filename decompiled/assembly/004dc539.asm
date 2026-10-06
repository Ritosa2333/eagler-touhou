
// block 004dc539

// block 004dc53c
004dc53c  adc        byte ptr [ecx + 0x354ec], al
004dc542  add        byte ptr [ebp + 0x5004244c], cl
004dc548  call       0x4dc8f5

// block 004dc54d
004dc54d  mov        ecx, dword ptr [esp + 0x35c]
004dc554  mov        edx, dword ptr [esp + 0x358]
004dc55b  push       ecx
004dc55c  push       edx
004dc55d  lea        ecx, [esp + 0xc]
004dc561  call       0x4dc973

// block 004dc566
004dc566  test       al, al
004dc568  jne        0x4dc574

// block 004dc56a
004dc56a  or         eax, 0xffffffff
004dc56d  add        esp, 0x354
004dc573  ret        

// block 004dc574
004dc574  mov        ecx, dword ptr [esp + 0x360]
004dc57b  lea        eax, [esp]
004dc57e  push       eax
004dc57f  push       ecx
004dc580  lea        ecx, [esp + 0xc]
004dc584  call       0x4dcb71

// block 004dc589
004dc589  test       al, al
004dc58b  jne        0x4dc597

// block 004dc58d
004dc58d  or         eax, 0xffffffff
004dc590  add        esp, 0x354
004dc596  ret        

// block 004dc597
004dc597  mov        eax, dword ptr [esp]
004dc59a  add        esp, 0x354
004dc5a0  ret        0x10
