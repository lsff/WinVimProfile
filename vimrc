
command! Reloadsrc  source $VIM/winvimrc/vimrc
let mapleader = ","
set number "设置行号

set guioptions-=T "gvim 减掉工具栏
set guioptions-=r "gvim 减掉右滚动条
set guioptions-=m "gvim 减掉菜单栏
set guioptions-=L "gvim 减掉菜单栏

set cursorline "高亮当前行
set noerrorbells "输入错误的提示声
set novisualbell
set t_vb= "close visual bell
set linebreak "独立单词不分行

set autoindent "自动缩进
set smartindent

set expandtab
set tabstop=4 
set shiftwidth=4
set softtabstop=4

set laststatus=2 "窗口底下状态栏常在
set hlsearch "高亮选中内容
set incsearch 

set guifont=InputMono:h10:cANSI "字体可以通过set guifont=* 选择适当的字体

language messages en_US.utf-8 "提示信息乱码,改成中文显示

filetype plugin on
filetype indent on "智能缩进

set foldmethod=syntax "按语法折叠
"set nofoldenable "启动vim关闭折叠

colorscheme desert

autocmd FileType ruby,html,scss,css set shiftwidth=2 | set tabstop=2 | set softtabstop=2 | set expandtab

set encoding=utf-8
set fileencodings=ucs-bom,utf-8,cp936,gb18030,gb2312,big5,euc-jp,euc-kr,latin1

source $VIM/winvimrc/plugins_config/cfg_vundle.vim
source $VIM/winvimrc/plugins_config/cfg_vimtweak.vim
source $VIM/winvimrc/plugins_config/cfg_airline.vim
source $VIM/winvimrc/plugins_config/cfg_nerdtree.vim
source $VIM/winvimrc/plugins_config/cfg_unite.vim
source $VIM/winvimrc/plugins_config/cfg_vim_online_thesaurus.vim
source $VIM/winvimrc/plugins_config/cfg_youcompleteme.vim
source $VIM/winvimrc/plugins_config/cfg_cscope.vim

source $VIM/winvimrc/key.vim

colorscheme molokai
