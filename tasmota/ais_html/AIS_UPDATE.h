const char AIS_UPDATE[] PROGMEM =
"  <h2 style=\"margin-top: 1em;\">Upgrade</h2>"
"<span class=\"mdi mdi-update\" style=\"font-size: 7em; color:#5b0566\"></span>"
"  <div id=\"f2\" style=\"display:none;text-align:center;\"><b>Upload started...</b></div>"
"  <form id=\"f1\" style=\"text-align: left;padding: 2em;\" method=\"post\" action=\"u2?fsz=\" enctype=\"multipart/form-data\">"
"    <div>"
"      <label for=\"formFileLg\" class=\"form-label\">Upload the upgrade file and click upgrade</label>"
"      <input class=\"form-control form-control-lg\" id=\"formFileLg\" type=\"file\" name=\"u2\">"
"    </div>"
"    <br/><br/>"
"    <div class=\"d-grid gap-2 d-md-flex justify-content-md-end\">"
"      <button class=\"btn btn-lg btn-primary\" name=\"save\" type=\"submit\" class=\"button bgrn\""
"onclick=\"document.getElementById('f1').style.display='none';document.getElementById('f2')"
".style.display='block';this.form.action+=this.form['u2'].files[0].size;this.form.submit();\">Upgrade</button>"
"    </div>"
"  </form>"    
;
