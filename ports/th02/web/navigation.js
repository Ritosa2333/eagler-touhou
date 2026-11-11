// Local launcher and room server use separate ports. Keep remote origins intact.
if(location.hostname==='127.0.0.1'||location.hostname==='localhost'){
  if(location.port!=='19822')for(const a of document.querySelectorAll('[data-mode="lan"],[data-mode="public"]')){
    const url=new URL(a.href);url.port='19822';a.href=url.href;
  }
}
