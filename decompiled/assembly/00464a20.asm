
// block 00464a20
00464a20  mov        eax, dword ptr [esi + 4]
00464a23  mov        ecx, dword ptr [esi + 0xc]
00464a26  mov        dword ptr [esi], eax
00464a28  fld        dword ptr [ecx]
00464a2a  fcomp      qword ptr [0x4a3db0]
00464a30  fnstsw     ax
00464a32  test       ah, 0x41
00464a35  jne        0x464a5e

// block 00464a37
00464a37  fld        dword ptr [0x4a3dac]
00464a3d  fcomp      dword ptr [ecx]
00464a3f  fnstsw     ax
00464a41  test       ah, 0x41
00464a44  jne        0x464a5e

// block 00464a46
00464a46  fld        dword ptr [esi + 8]
00464a49  fadd       dword ptr [esp + 4]
00464a4d  fstp       dword ptr [esi + 8]
00464a50  fld        dword ptr [esi + 8]
00464a53  call       0x4931e0

// block 00464a58
00464a58  mov        dword ptr [esi + 4], eax
00464a5b  ret        4

// block 00464a5e
00464a5e  fld        dword ptr [ecx]
00464a60  fmul       dword ptr [esp + 4]
00464a64  fstp       dword ptr [esp + 4]
00464a68  fld        dword ptr [esp + 4]
00464a6c  fadd       dword ptr [esi + 8]
00464a6f  fstp       dword ptr [esi + 8]
00464a72  fld        dword ptr [esi + 8]
00464a75  call       0x4931e0

// block 00464a7a
00464a7a  mov        dword ptr [esi + 4], eax
00464a7d  ret        4
