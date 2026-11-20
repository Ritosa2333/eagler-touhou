#pragma once

namespace th09::desktop {

constexpr const char* roomDocument = R"rml(
<rml>
<head>
<style>
body { position: absolute; top: 0; left: 0; width: 100%; height: 100%; margin: 0; color: #ede4cb; font-family: Room; font-size: 14px; }
div { display: block; }
button, input { display: inline-block; font-family: Room; font-size: 13px; }
button { background-color: #4a5749; border: 1px solid #61715c; color: #f5ebd6; padding: 3px 7px; margin: 0 4px 0 0; }
button:hover { background-color: #5b6e52; }
button:disabled { background-color: #444943; color: #949686; }
button.primary { background-color: #81915f; border-color: #a6b47b; color: #21271c; }
input { background-color: #212a24; border: 1px solid #626d59; color: #f5ebd6; padding: 4px 6px; width: 68px; }
#panel { position: absolute; top: 4px; right: 4px; background-color: #343c32; border: 1px solid #8e9974; }
header { display: block; padding: 6px 9px; height: 22px; line-height: 22px; background-color: #45513e; }
header #title { font-size: 16px; color: #f8ecc9; }
header button { float: right; margin: 0; padding: 1px 5px; }
#main { position: absolute; top: 35px; left: 8px; overflow-y: auto; }
scrollbarvertical { width: 8px; }
scrollbarhorizontal { height: 8px; }
scrollbarvertical sliderbar, scrollbarhorizontal sliderbar { background-color: #61715c; }
#footer { position: absolute; bottom: 3px; left: 8px; height: 25px; display: flex; align-items: center; font-size: 12px; color: #e3c894; overflow: hidden; }
.actions { margin: 1px 0 6px; }
#direct-address { width: 140px; }
#startup-routes { margin: 3px 0 6px; }
#startup-routes .route-row { display: flex; align-items: center; height: 23px; }
#startup-routes button { min-width: 90px; text-align: left; padding: 1px 6px; }
.room-row, .seat { background-color: #3e493b; border-bottom: 1px solid #576450; margin-bottom: 5px; padding: 5px 7px; min-height: 23px; }
.room-row button, .seat button { float: right; margin: 0; }
.avatar { width: 18px; height: 18px; margin-right: 4px; vertical-align: middle; }
.name { color: #fbebc9; }
.sub { color: #b7bea4; font-size: 12px; }
#messages { overflow-y: auto; height: 44px; margin-top: 4px; padding: 3px 0; border-top: 1px solid #56634d; font-size: 13px; }
#composer input { width: 57%; margin-right: 4px; }
#progress { padding: 10px 2px; }
#progress span { display: block; margin-bottom: 8px; }
progress { width: 95%; height: 6px; display: block; margin: 0 0 9px; background-color: #53604b; }
progress fill { background-color: #c0c987; }
#chat-hud { position: absolute; bottom: 5px; left: 5px; width: 420px; max-width: 93%; padding: 3px; pointer-events: none; }
#chat-tools { width: 345px; max-width: 100%; pointer-events: auto; }
#chat-hud button { background-color: transparent; border: 0; color: #f5ebd6; font-effect: outline(1px #141814); }
#chat-hud button:hover { background-color: #3c4c39aa; }
#chat-hud .phrases { margin-top: 4px; }
#chat-hud .phrases button { font-size: 12px; margin-bottom: 3px; }
#game-messages { max-height: 80px; overflow: hidden; font-effect: outline(1px #141814); }
#connection-status { position: absolute; bottom: 11px; right: 13px; pointer-events: none; white-space: nowrap; color: #f2f5f0eb; font-effect: shadow(0px 1px #000000d2); }
#connection-status .peer-label { font-size: 12px; color: #c9c4ce; margin-right: 8px; }
#connection-status .peer-route { font-size: 10px; color: #c9c4ce; }
#connection-status .peer-rtt { font-size: 13px; font-weight: bold; margin-left: 8px; }
#connection-status small { font-size: 10px; font-weight: normal; margin-left: 1px; color: #f2f5f0b4; }
#connection-status .peer-variation { font-size: 7px; font-weight: normal; margin-left: 2px; color: #d3c5dd; }
#connection-status .peer-variation small { font-size: 6px; color: #d3c5dd; }
#route-switch { position: absolute; bottom: 31px; right: 13px; pointer-events: none; white-space: nowrap; color: #f2f5f0eb; font-size: 12px; font-effect: shadow(0px 1px #000000d2); }
#route-switch-button { pointer-events: auto; font-size: 12px; padding: 1px 5px; }
#notice { position: absolute; top: 5px; left: 5px; background-color: #333d33; color: #fae6af; border: 1px solid #899673; padding: 4px 6px; }
</style>
</head>
<body>
<div id="panel">
  <header><span id="title">東方花映塚</span><button data-action="hide">F2 收起</button></header>
  <div id="main">
    <div id="directory">
      <div class="actions">
        <button class="primary" data-action="create">创建房间</button>
        <input id="code" type="text" maxlength="8" placeholder="房间号"/>
        <button data-action="join-code">加入</button><button data-action="directory">刷新</button>
      </div>
      <div class="actions"><button id="open-direct" data-action="open-direct">IP 直连</button></div>
      <div id="rooms"></div>
    </div>
    <div id="direct">
      <div class="actions"><button data-action="close-direct">返回大厅</button></div>
      <div class="actions">
        <input id="direct-port" type="text" maxlength="5" value="10800"/>
        <button id="direct-listen" class="primary" data-action="direct-listen">监听 UDP</button>
      </div>
      <div class="actions">
        <input id="direct-address" type="text" maxlength="21" placeholder="对方 IPv4:端口"/>
        <button id="direct-join" data-action="direct-join">连接</button>
      </div>
      <div class="sub">跨网联机时，请房主在路由器转发所填的 UDP 端口。</div>
    </div>
    <div id="room">
      <div class="actions"><button data-action="leave">返回大厅</button> <span id="room-code"></span></div>
      <div id="seats"></div>
      <div id="room-actions" class="actions"></div>
      <div id="startup-routes"></div>
      <div id="messages"></div>
      <div id="composer"><input id="message" type="text" maxlength="160" placeholder="发言"/><button data-action="send-chat">发送</button></div>
    </div>
    <div id="progress"><span id="progress-text"></span><progress id="progress-meter" max="1"></progress><button id="cancel-room" data-action="return">取消并回房</button></div>
  </div>
  <div id="footer"><button id="account" data-action="open-account">登录</button><span id="status"></span></div>
</div>
<div id="chat-hud">
  <div id="game-messages"></div>
  <div id="chat-tools">
    <button data-action="hide">F2 收起</button><button id="open-phrases" data-action="phrases">快捷发言</button><button id="return-room" data-action="return">回房</button>
    <div id="phrases" class="phrases"></div>
  </div>
</div>
<div id="notice"></div>
<div id="connection-status"></div>
<div id="route-switch"><button id="route-switch-button">切换</button><span id="route-switch-text"></span></div>
</body></rml>
)rml";

} // namespace th09::desktop
