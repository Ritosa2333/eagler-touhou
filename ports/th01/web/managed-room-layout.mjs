export function mountManagedRoom(){
 document.head.querySelectorAll('link[rel="stylesheet"],style').forEach(node=>node.remove());
 document.body.className='';
 document.body.innerHTML='<div id="screen"><canvas id="canvas" width="640" height="400" tabindex="0" aria-label="东方灵异传"></canvas></div>';
 const style=document.createElement('style');
 style.textContent='html,body,#screen{width:100%;height:100%;margin:0;padding:0;overflow:hidden;background:#000}#screen{position:fixed;inset:0;display:grid;place-items:center}#canvas{display:block;width:min(100vw,160vh);height:min(100vh,62.5vw);max-width:none;max-height:none;object-fit:contain;image-rendering:pixelated;outline:none}';
 document.head.append(style);
}
