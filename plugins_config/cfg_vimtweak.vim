"vimtweak https://github.com/mattn/vimtweak/blob/master/vimtweak.c
"编译出vimtweak.dll，放到gvim.exe同等目录下

map <leader>aw :call libcallnr("vimtweak.dll","SetAlpha",223)<cr>
map <leader>aW :call libcallnr("vimtweak.dll","SetAlpha",255)<cr>

map <leader>mw :call libcallnr("vimtweak.dll","EnableMaximize",1)<cr>
map <leader>mW :call libcallnr("vimtweak.dll","EnableMaximize",0)<cr>

au GUIEnter * call libcallnr("vimtweak.dll", "SetAlpha", 223)

