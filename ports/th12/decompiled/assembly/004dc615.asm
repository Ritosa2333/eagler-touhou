
// block 004dc615
004dc615  push       ecx
004dc616  mov        edx, ecx
004dc618  push       esi
004dc619  mov        ecx, 8
004dc61e  push       edi
004dc61f  cmp        dword ptr [edx + 4], ecx
004dc622  jb         0x4dc659

// block 004dc624
004dc624  push       ebx
004dc625  mov        esi, 0xfffffff8

// block 004dc62a
004dc62a  mov        eax, dword ptr [edx]
004dc62c  mov        bl, byte ptr [eax]
004dc62e  inc        eax
004dc62f  mov        byte ptr [esp + 0xc], bl
004dc633  mov        dword ptr [edx], eax
004dc635  mov        eax, dword ptr [edx + 8]
004dc638  mov        edi, dword ptr [esp + 0xc]
004dc63c  shl        eax, 8
004dc63f  and        edi, 0xff
004dc645  or         eax, edi
004dc647  mov        edi, dword ptr [edx + 4]
004dc64a  add        edi, esi
004dc64c  mov        dword ptr [edx + 8], eax
004dc64f  mov        eax, edi
004dc651  mov        dword ptr [edx + 4], edi
004dc654  cmp        eax, ecx
004dc656  jae        0x4dc62a

// block 004dc658
004dc658  pop        ebx

// block 004dc659
004dc659  mov        esi, dword ptr [edx + 4]
004dc65c  mov        eax, dword ptr [edx + 8]
004dc65f  mov        edi, dword ptr [esp + 0x10]
004dc663  sub        ecx, esi
004dc665  shr        eax, cl
004dc667  mov        ecx, 0x18
004dc66c  sub        ecx, edi
004dc66e  and        eax, 0xffffff
004dc673  shr        eax, cl
004dc675  add        esi, edi
004dc677  pop        edi
004dc678  mov        dword ptr [edx + 4], esi
004dc67b  pop        esi
004dc67c  pop        ecx
004dc67d  ret        4
