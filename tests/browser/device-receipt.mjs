const form=document.querySelector('#device-form'),text=document.querySelector('#receipt'),status=document.querySelector('#receipt-status');
const button=form.querySelector('button');
form.addEventListener('submit',async event=>{
  event.preventDefault();if(button.disabled)return;button.disabled=true;
  try{
    const body=text.value;
    if(new TextEncoder().encode(body).length>49152)throw new Error('回执超出长度限制');
    if(!/^\/receipt-[a-f0-9-]{36}$/.test(location.pathname))throw new Error('没有对应的本机验收运行');
    const answer=await fetch(location.pathname,{method:'POST',headers:{'Content-Type':'application/json'},body,redirect:'error'});
    if(answer.status!==201||await answer.text()!=='accepted')throw new Error(`本机验收未接收：${answer.status}`);
    text.value='';status.textContent='accepted';
  }catch(error){status.textContent=String(error.message??error);button.disabled=false;}
});
