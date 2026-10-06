
// block 00435900
00435900  push       ebx
00435901  push       ebp
00435902  mov        ebp, dword ptr [esp + 0xc]
00435906  and        dword ptr [ebp + 0xc414], 0xfffffff7
0043590d  push       esi
0043590e  push       edi
0043590f  lea        esi, [ebp + 0x8314]
00435915  mov        edi, 8
0043591a  lea        ebx, [ebx]

// block 00435920
00435920  mov        eax, dword ptr [esi - 4]
00435923  push       eax
00435924  mov        ebx, 2
00435929  call       0x461970

// block 0043592e
0043592e  mov        ecx, dword ptr [esi]
00435930  push       ecx
00435931  call       0x461970

// block 00435936
00435936  add        esi, 0xe4
0043593c  sub        edi, 1
0043593f  jne        0x435920

// block 00435941
00435941  mov        dword ptr [ebp + 0xc418], edi
00435947  pop        edi
00435948  pop        esi
00435949  pop        ebp
0043594a  pop        ebx
0043594b  ret        4
