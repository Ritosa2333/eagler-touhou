export function convertThkkdImage(bytes: Uint8Array,name: string,mask: Uint8Array|null): Promise<{blob:Blob;intrinsicAlpha:boolean}>;
export function decodeThkkdBmp(bytes: Uint8Array): {width:number;height:number;rgba:Uint8Array;intrinsicAlpha:boolean};
