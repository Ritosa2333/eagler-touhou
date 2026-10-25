export function nativeTabFixture(){
  const owner={targetId:'game-page',windowId:12,url:'https://fixture.invalid/host'};
  const document=(hidden,events=[])=>({url:'https://fixture.invalid/host',timeOrigin:100,hidden,visibilityState:hidden?'hidden':'visible',hasFocus:!hidden,shadowed:[],events});
  return{schema:'eagler-touhou/native-tab-suspension/1',kind:'same-window-tab',cleanupErrors:[],peer:{targetId:'peer-page',windowId:12},
    before:{owner,host:document(false),runtime:document(false)},hidden:{owner,host:document(true,[{trusted:true,hidden:true,visibilityState:'hidden'}]),runtime:document(true,[{trusted:true,hidden:true,visibilityState:'hidden'}])},
    restored:{owner,host:document(false,[{trusted:true,hidden:true,visibilityState:'hidden'},{trusted:true,hidden:false,visibilityState:'visible'}]),
      runtime:document(false,[{trusted:true,hidden:true,visibilityState:'hidden'},{trusted:true,hidden:false,visibilityState:'visible'}])}};
}
