declare module 'virtual:document-history' {
  const history:Record<string,Record<string,{author?:string;date?:string;commit?:string;pending?:boolean}>>;
  export default history;
}
