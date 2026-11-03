/** Character artwork: plasticparakeet, CC BY-NC-SA 3.0. */
export const SITE_BRAND_FACES=Object.freeze(["face-F001","face-F022","face-F01B","face-F02F","face-F040","face-F051","face-F025","face-F023","face-F027","face-F028","face-F02A","face-F029","face-F02B","face-F02C","face-F02E","face-F02D","face-F031","face-F020","face-F037","face-F024","face-F038","face-F039","face-F03D","face-F03A","face-F03F","face-F021","face-F026","face-F046","face-F032","face-F047","face-F03C","face-F044","face-F052","face-F030","face-F050","face-F055","face-F054","face-F04F","face-F04C","face-F04E","face-F042","face-F041","face-F056","face-F057","face-F058","face-F059","face-F05A","face-F05D","face-F04D","face-F053","face-F05E","face-F05B","face-F060","face-F062","face-F063","face-F05F","face-F067","face-F068","face-F069","face-F06B","face-F06A","face-F06C","face-F06D","face-F070","face-F071","face-F073","face-F074","face-F076","face-F077","face-F016","face-F06F","face-F078","face-F079","face-F07A","face-F07B","face-F07C","face-F07F","face-F07D","face-F07E","face-F088","face-F081","face-F082","face-F083","face-F085","face-F087","face-F086","face-F080"]);
export const SITE_BRAND_DEFAULT="face-F02B";
export const SITE_BRAND_RATIO=3.125;
const ordinary=SITE_BRAND_FACES.filter(id=>id!=="face-F02B"&&id!=="face-F055");
/** The two preferred characters each have 10% weight. */
export function chooseSiteBrandFace(random=Math.random){const roll=random();return roll<.1?"face-F02B":roll<.2?"face-F055":ordinary[Math.min(ordinary.length-1,Math.floor(random()*ordinary.length))];}
export const SITE_BRAND_SESSION_KEY="eagler.site.brand.face";
const sessionFaces=new WeakMap();
export function sessionSiteBrandFace(view,random=Math.random){
  if(!view)return chooseSiteBrandFace(random);
  if(sessionFaces.has(view))return sessionFaces.get(view);
  let face=SITE_BRAND_DEFAULT;
  try{
    const storage=view.sessionStorage,saved=storage.getItem(SITE_BRAND_SESSION_KEY);
    if(SITE_BRAND_FACES.includes(saved))face=saved;
    else{face=chooseSiteBrandFace(random);storage.setItem(SITE_BRAND_SESSION_KEY,face);}
  }catch{face=SITE_BRAND_DEFAULT;}
  sessionFaces.set(view,face);return face;
}
export function resetSiteBrandFace(view){
  if(view){sessionFaces.set(view,SITE_BRAND_DEFAULT);try{view.sessionStorage.setItem(SITE_BRAND_SESSION_KEY,SITE_BRAND_DEFAULT);}catch{}}
  return SITE_BRAND_DEFAULT;
}
export function siteBrandPath(face=SITE_BRAND_DEFAULT){if(!SITE_BRAND_FACES.includes(face))throw Error("Unknown site brand character");return "assets/brand/"+face+".svg";}
export function initializeStaticSiteBrand(document,moduleUrl){
  const face=sessionSiteBrandFace(document.defaultView);
  for(const mark of document.querySelectorAll('img[data-site-brand]')){
    const fallback=mark.src;
    mark.src=new URL('../'+siteBrandPath(face),moduleUrl).href;
    mark.onerror=()=>{mark.onerror=null;mark.src=fallback;mark.dataset.brandFace=resetSiteBrandFace(document.defaultView);};
    mark.dataset.brandFace=face;
  }
}

initializeStaticSiteBrand(document,import.meta.url);
