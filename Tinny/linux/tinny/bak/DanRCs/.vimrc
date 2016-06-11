" deal with tab
set et	" turn on expand tab
set tabstop=3
set shiftwidth=4

" for Makefiles, no expand tab
autocmd BufEnter ?akefile* set noet 
 
" get rid of the toolbar which is pretty much useless
set guioptions-=T

" for source code
"autocmd BufEnter *.cpp,*.h,*.c,*.java,*.pl set et ts=4 sw=4 cindent 

" set the X11 font to use. See 'man xlsfonts' on unix/linux
set guifont=8x13bold

" backup
set writebackup 
set backup

" Set text width
set tw=76

" Set ruler
set ruler

" Show the matching bracket for the last ')'
set showmatch

" The opposite is 'set wrapscan' while searching for strings....
set nowrapscan
"
" The opposite is set ignorecase
set noignorecase

" Make command line one lines high
set ch=1

" Make backspace works after changing line
set bs=2

" Make shift-insert work like in Xterm
map <S-Insert> <MiddleMouse>
map! <S-Insert> <MiddleMouse>

" Only do this for Vim version 5.0 and later.
if version >= 500

  " I like highlighting strings inside C comments
  let c_comment_strings=1

  " Switch on syntax highlighting.
  syntax on

  " Switch on search pattern highlighting.
  set hlsearch

  " Hide the mouse pointer while typing
  set mousehide

  " Set nice colors
  " background for normal text is light grey
  " Text below the last line is darker grey
  " Cursor is green
  " Constants are not underlined but have a slightly lighter background
   highlight Normal guibg=grey90
   highlight Cursor guibg=brown guifg=NONE
   highlight NonText guibg=grey80
   highlight Constant gui=NONE guibg=grey95
   highlight Special gui=NONE guibg=grey95

endif

" Set abbreviation
source ~/.vim/abbreviation.vim

"" Set indent for Latex
"if version >=600
"	source ~/.vim/tex.vim
"endif

" Spell Checker is enabled by <F3>
map <F3> : so ~/.vim/engspchk.vim<CR>

" Set smart command
source ~/.vim/autocmd.vim

" Set keymap
imap <C-w> <ESC><C-w><C-w>i

" Set skeletons
autocmd BufNewFile *.c 0r ~/.vim/skeletons/skeleton.c
autocmd BufNewFile *.cpp 0r ~/.vim/skeletons/skeleton.cpp
autocmd BufNewFile *.h 0r ~/.vim/skeletons/skeleton.h
autocmd BufNewFile *.hh 0r ~/.vim/skeletons/skeleton.hh
autocmd BufNewFile *.hpp 0r ~/.vim/skeletons/skeleton.hpp

" Set hot keys for buffer
nmap <F5> :bp<CR>
nmap <F6> :bn<CR>

" turn on fal_syntax
nmap <F7> :setf fal<CR>

au BufRead,BufNewFile *.inc set filetype=fal
au BufRead,BufNewFile *.rule set filetype=fal
au BufRead,BufNewFile *.rul set filetype=fal
au BufRead,BufNewFile *.tmplt set filetype=fal
"au BufRead,BufNewFile *.cnf set filetype=fal
"au BufRead,BufNewFile *.config set filetype=fal

"au! Syntax fal source ~/.vim/syntax/fal.vim

"for html, turn off auto-wrap
au BufRead,BufNewFile *.html set fo-=t

" swith to nowrap
nmap <F1> :set nowrap<CR>
