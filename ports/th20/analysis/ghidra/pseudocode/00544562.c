/* Entry: 0x00544562; symbol: _JumpToContinuation; body bytes: 43 */

/* Library Function - Single Match
    void __stdcall _JumpToContinuation(void *,struct EHRegistrationNode *)
   
   Library: Visual Studio */

void _JumpToContinuation(void *param_1,EHRegistrationNode *param_2)

{
  undefined4 *unaff_FS_OFFSET;
  
  *unaff_FS_OFFSET = *(undefined4 *)*unaff_FS_OFFSET;
                    /* WARNING: Could not recover jumptable at 0x0054458b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)param_1)();
  return;
}

