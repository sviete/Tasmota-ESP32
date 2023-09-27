const char AIS_CONSOLE[] PROGMEM =
"<script>"
"var x=null,lt,to,tp,pc='';eb=s=>document.getElementById(s);qs=s=>document.querySelector(s);sp=i=>eb(i).type=(eb(i).type==='text'?'password':'text');wl=f=>window.addEventListener('load',f);var sn=0,id=0,ft,ltm=2345;function l(p){var c,o='';clearTimeout(lt);clearTimeout(ft);t=eb('t1');if(p==1){c=eb('c1');o='&c1='+encodeURIComponent(c.value);c.value='';t.scrollTop=1e8;sn=t.scrollTop;}if(t.scrollTop>=sn){if(x!=null){x.abort();}x=new XMLHttpRequest();x.onreadystatechange=()=>{if(x.readyState==4&&x.status==200){var z,d;d=x.responseText.split(/}1/);id=d.shift();if(d.shift()==0){t.value='';}z=d.shift();if(z.length>0){t.value+=z;}t.scrollTop=1e8;sn=t.scrollTop;clearTimeout(ft);lt=setTimeout(l,ltm);}};x.open('GET','cs?c2='+id+o,true);x.send();ft=setTimeout(l,2e4);}else{lt=setTimeout(l,ltm);}return false;}wl(l);var hc=[],cn=0;function h(){eb('c1').addEventListener('keydown',e=>{var b=eb('c1'),c=e.keyCode;if(38==c||40==c){b.autocomplete='off';setTimeout(b=>{b.focus();b.setSelectionRange(1e9,1e9)},0,b)}38==c?(++cn>hc.length&&(cn=hc.length),b.value=hc[cn-1]||''):40==c?(0>--cn&&(cn=0),b.value=hc[cn-1]||''):13==c&&(hc.length>19&&hc.pop(),hc.unshift(b.value),cn=0)});}wl(h);function jd(){var t=0,i=document.querySelectorAll('input,button,textarea,select');while(i.length>=t){if(i[t]){i[t]['name']=(i[t].hasAttribute('id')&&(!i[t].hasAttribute('name')))?i[t]['id']:i[t]['name'];}t++;}}function sf(s){var t=0,i=document.querySelectorAll('.hf');while(i.length>=t){if(i[t]){i[t].style.display=s?'block':'none';}t++;}}wl(jd);"
"</script>"  
"<style>"
".fakeButtons {"
"  height: 10px;"
"  width: 10px;"
"  border-radius: 50%%;"
"  border: 1px solid #000;"
"  position: relative;"
"  left: 6px;"
"  background-color: #ff3b47;"
"  border-color: #9d252b;"
"  display: inline-block;"
"}"
".fakeMinimize {"
"  left: 11px;"
"  background-color: #ffc100;"
"  border-color: #9d802c;"
"}"
".fakeZoom {"
"  left: 16px;"
"  background-color: #00d742;"
"  border-color: #049931;"
"}"
".fakeMenu {"
"  max-width: 1200px;"
"  box-sizing: border-box;"
"  height: 25px;"
"  background-color: #bbb;"
"  margin: 0 auto;"
"  border-top-right-radius: 5px;"
"  border-top-left-radius: 5px;"
"  margin-top: 2em;"
"}"
".fakeScreen {"
"  background-color: #151515;"
"  box-sizing: border-box;"
"  max-width: 1200px;"
"  margin: 0 auto;"
"  padding: 20px;"
"  border-bottom-left-radius: 5px;"
"  border-bottom-right-radius: 5px;"
"}"
"p {"
"  position: relative;"
"  left: 50%;"
"  margin-left: -8.5em;"
"  text-align: left;"
"  font-size: 1.25em;"
"  font-family: monospace;"
"  white-space: normal;"
"  overflow: hidden;"
"  width: 0;"
"}"
"span {"
"  color: #fff;"
"  font-weight: bold;"
"}"
"</style>"
"<body >"
"<div class=fakeMenu>"
"  <div class=\"fakeButtons fakeClose\"></div>"
"  <div class=\"fakeButtons fakeMinimize\"></div>"
"  <div class=\"fakeButtons fakeZoom\"></div>"
"</div>"
"<div class=\"fakeScreen\">"
"  <textarea readonly=\"\" id=\"t1\" wrap=\"off\" name=\"t1\" style=\"width: 100%%;background-color:#151515;border:0px;height:500px;color:#afe50c;font-family: monospace;\"></textarea>"
"  <form method=\"get\" onsubmit=\"return l(1);\"><br>"
"  <div class=\"input-group input-group-lg\">"
"      <span class=\"input-group-text\" id=\"inputGroup-sizing-lg\"><span style=\"color: black;\" class=\"mdi mdi-console-line\"></span></span>"
"      <input type=\"text\" class=\"form-control\" id=\"c1\" placeholder=\"Enter command\" autofocus=\"\" name=\"c1\">"
"    </div>"
"  </form>"
"  <br/><br/><button onclick=\"window.location.href='/'\" type=\"button\" class=\"btn btn-primary btn-lg\" style=\"width: 100%%;\">Main Menu</button>"
"  </div>"
;
