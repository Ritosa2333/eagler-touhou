// Keep previously shared runtime links working after introducing the launcher.
const query=new URLSearchParams(location.search);
if(['rtc','relay'].includes(query.get('net'))){
  location.replace(`local.html${location.search}${location.hash}`);
}
