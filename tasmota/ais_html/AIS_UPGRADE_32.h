const char AIS_UPGRADE_32[] PROGMEM =
"<script>"
 "function upl(t){"                            
    "var sl=t.form['u2'].files[0].slice(0,1);" 
    "var rd=new FileReader();"
    "rd.onload=()=>{"
      "var bb=new Uint8Array(rd.result);"
      "if(bb.length==1&&bb[0]==0xE9){"
        "fct(t);"      
      "}else{"
        "t.form.submit();"
      "};"
    "};"
    "rd.readAsArrayBuffer(sl);"
    "return false;"
  "};"
  "function fct(t){"
    "var x=new XMLHttpRequest();"
    "x.open('GET','/u4?u4=fct&api=',true);"
    "x.onreadystatechange=()=>{"
      "if(x.readyState==4&&x.status==200){"
        "var s=x.responseText;"
        "if(s=='false')setTimeout(()=>{fct(t);},6000);"
        "if(s=='true')su(t);"
      "}else if(x.readyState==4&&x.status==0){"
        "setTimeout(()=>{fct(t);},2000);"        
      "};"
    "};"
    "x.send();"
  "}" 
    "function su(t){"
    "document.getElementById('f3').style.display='none';"
    "document.getElementById('f2').style.display='block';"
    "t.form.submit();"
  "}"
"</script>"
"<body >"
"<center>"  
"  <div class=\"card\" style=\"width: 100%%;max-width: 600px;margin-top: 2em;\">"
"  <h2 style=\"margin-top: 1em;\">Upgrade</h2>"
"<span class=\"mdi mdi-update\" style=\"font-size: 7em; color:#5b0566\"></span>"
"  <div id=\"f2\" style=\"display:none;text-align:center;\"><b>Upload started...</b></div>"
"  <div id=\"f3\" style=\"display:none;text-align:center;\"><b>Switching to safeboot partition...</b></div>"
"  <form id=\"f1\" style=\"text-align: left;padding: 2em;\" method=\"post\" action=\"u2?fsz=\" enctype=\"multipart/form-data\">"
"    <div>"
"      <label for=\"formFileLg\" class=\"form-label\">Upload the upgrade file and click upgrade</label>"
"      <input class=\"form-control form-control-lg\" id=\"formFileLg\" type=\"file\" name=\"u2\">"
"    </div>"
"    <br/><br/>"
"    <div class=\"d-grid gap-2 d-md-flex justify-content-md-end\">"
"      <button class=\"btn btn-lg btn-primary\" name=\"save\" type=\"submit\" class=\"button bgrn\""
"onclick=\"document.getElementById('f1').style.display='none';document.getElementById('f3').style.display='block';"
"this.form.action+=this.form['u2'].files[0].size; return upl(this);\" style=\"width: 100%%;\">Upgrade</button>"
"    </div>"
"      <br/><br/><button onclick=\"window.location.href='/'\" type=\"button\" class=\"btn btn-primary btn-lg\" style=\"width: 100%%;\">Main Menu</button>"
"  </form>"   
;
