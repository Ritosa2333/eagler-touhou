/* Entry: 0x0055216e; symbol: _setlocale; body bytes: 56 */

/* Library Function - Single Match
    _setlocale
   
   Library: Visual Studio 2019 Release */

char * __cdecl _setlocale(int _Category,char *_Locale)

{
  char *pcVar1;
  void *in_ECX;
  int *local_18;
  char **local_14;
  undefined4 local_10;
  undefined4 local_c;
  __crt_seh_guarded_call<char*> local_5;
  
  ___acrt_eagerly_load_locale_apis(in_ECX);
  local_18 = &_Category;
  local_14 = &_Locale;
  local_c = 4;
  local_10 = 4;
  pcVar1 = __crt_seh_guarded_call<char*>::
           operator()<class_<lambda_f5952838e36297a1e2e9d6c748429792>,class_<lambda_2116bde18c9e5f34230805ea4a4660ed>&,class_<lambda_3c7107de1cc98bf330a3031f42c322a3>_>
                     (&local_5,(<lambda_f5952838e36297a1e2e9d6c748429792> *)&local_10,
                      (<lambda_2116bde18c9e5f34230805ea4a4660ed> *)&local_18,
                      (<lambda_3c7107de1cc98bf330a3031f42c322a3> *)&local_c);
  return pcVar1;
}

