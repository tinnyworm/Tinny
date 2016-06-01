# System-wide .cshrc file for the multiple systems.
# Set up search path, man pages path, environment, and user name.


###---------->Common env and path<---------------------------

setenv SYS `uname -s`
setenv UFAL /work/price/falcon
setenv GOLIVE /work/cantona/go-lives/
#setenv UFALDAT ${UFAL}/data
setenv SHELL $shell
setenv PRINTER sdob0203
setenv CVSROOT /fo1/CVSROOT
setenv PYTHONPATH /work/gold/projects/lib

# set sorting in ASCII order, see manpage for sort
setenv LC_COLLATE C 

	

###---------->different parts for different systems<----------

if ( $SYS == "Linux" ) then
	#---> environment variables for linux <---#
	setenv LD_LIBRARY_PATH 	/usr/lib/X11:/usr/local/lib:/lib:/opt/intel/compiler50/ia32/lib:/work/pele/jas/lib/Linux
	setenv MANPATH 	/usr/man:/usr/local/man:/usr/openwin/man:/win/X11/man:$HOME/manpage/man:/opt/gnu/man:/opt/SUNWspro/man:/home/splus/man:/Applications/math/man:/admn/man:/usr/share/man:/home/jas/sw/man:$UFAL/local/man:/usr/local/lib/perl5/man

   # by default LANG=en_US.UTF-8, but it causes slow start for xload with
   # error message 
   # Warning: Missing charsets in String to FontSet conversion
   setenv LANG 

	#---> path for Linux <---#
	set path = (\
		/usr/local/bin\
		/bin\
		/usr/bin\
		/etc\
		/usr/etc\
		/usr/bin/X11\
		/usr/local/etc\
		/usr/local/bin/X11\
		/usr/sbin\
		/work/gold/linux/bin\
		/work/gold/projects/bin\
		$UFAL/Linux/bin\
		$UFAL/Linux/local/bin\
		$UFAL/bin\
      /fo1/bin/Linux\
      /fo1/R/R-2.4.0/bin\
      $HOME/local/Linux\
	)

	alias gvim $HOME/bin/gvim
	alias vim  $HOME/bin/vim
   alias view $HOME/bin/vim -R

else if ( $SYS == "SunOS" ) then

	#---> environment variables for SunOS <---#
	setenv OPENWINHOME /usr/openwin
	setenv INFORMIXDIR /usr/informix
	setenv MSGBIN /work/gold/projects/bin
	setenv SASHOME /export/home/Applications/sas
	setenv DMW /export/home/Applications/DMW
	setenv XAPPLRESDIR $DMW/init
	setenv LD_LIBRARY_PATH /usr/lib/X11:/usr/openwin/lib:/usr/local/lib:/lib:/home/jas/sw/lib/pgplot:/work/pele/jas/lib/SunOS
	setenv MANPATH /usr/man:/usr/local/man:/usr/openwin/man:/win/X11/man:$HOME/manpage/man:/opt/gnu/man:/opt/SUNWspro/man:/home/splus/man:/Applications/math/man:/admn/man:/usr/share/catman:/home/jas/sw/man:$UFAL/local/man:/usr/local/lib/perl5/man

	#---> path for SunOS <---#
	set path = (\
		/usr/local/bin\
		/bin\
		/usr/bin\
		/etc\
		/usr/etc\
		/usr/bin/X11\
		/usr/local/etc\
		/usr/bsd\
		/usr/local/bin/X11\
		/usr/local/intertools/x\
		/usr/local/asap/bin\
		/win/X11/bin\
		/win/x11r5/sun4/bin\
		/vol/bin\
		/opt/gnu/bin\
		/usr/sbin\
		/Applications/math/bin\
		/sgiusr/sbin\
		/work/gold/bin\
		/work/gold/projects/bin\
		/work/pele/jas/sw/bin\
		/sw/fileutils/bin\
		$UFAL/bin\
		$UFAL/local/bin\
		/sw/perl/bin\
		/sw/workshop/SUNWspro/bin\
		$OPENWINHOME/bin\
		/opt/SUNWspro/bin\
		/usr/ucb\
		$INFORMIXDIR/bin\
		$SASHOME\
		/usr/asc/bin\
		/usr/dt/bin\
		$DMW/bin\
		/home/dxl/fvwm-2.4.18/bin\
      /usr/atria/bin\
      /fo1/bin/SunOS\
      $HOME/local/SunOS\
	)

   if ( $HOST != "ux165") then
	   alias gvim /home/bxz/vim/bin.SunOS/gvim
	   alias vim  /home/bxz/vim/bin.SunOS/vim
	   alias gview  /home/bxz/vim/bin.SunOS/gview
   endif
	alias sampstats /work/price/falcon/dvl/ccpp/utils/sampstats/pcd_tmp/sampstats

endif	
	
set path = (\
	$path\
	$HOME/bin\
   $HOME/local/bin\
	$HOME/tools/perl\
	$HOME/tools/shell\
	)

# My favorite prompt definition:
if ($?prompt) then
  if ($term == xterm || $term == xterml) then
    # the first part if the title appeared on the terminal
	# %B:  bold
	# %c2: Trailing component of cwd, replacing $HOME with ~. Show only the
	#      last two elements 
	#set prompt="%{\e]2\;%n@%m %~%#^g\e]1\;%n@%m^g\r%} %B%c2 > "
	set prompt="%{\e]2\;%m %c4%#^g\e]1\;%n@%m^g\r%} %B%c2 > "
  else
    set prompt="%B\! %S(%n@%m)%s %c03> "
  endif
endif

###---------->auto logout<---------------
unset autologout

###---------->history<----------
### Configuration of the history mechanism 
set history = 200
# save commands after logout
set savehist = (100 merge)

###---------->alias<----------
source $HOME/.alias

###---------->falcon alias<---------------
alias ufalcut /work/price/falcon/$SYS/bin/ufalcut

###---------->command line editor/misc<----------
### Configuration of the command line editor 
#set echo_style = both
	
###---------->command line editor/bindings<----------
### Bindings for the command line editor 

# using vi bindings
bindkey -v
#bindkey -k up history-search-backward
#bindkey -k down history-search-forward

###---------->completion/misc<----------
### Configuration on filename completion 
set autolist
set addsuffix
set autoexpand
set matchbeep = nomatch
set listmaxrows = 24

###---------->completion/userdefined<----------
### user defined completion 
complete ghostview  'p/*/f:*.ps/'
complete kill  'c/%/j/' 'c/-/S/' 'p/*/`awk \{print\ \$1\}`/'
complete pu  'c/+/`dirs`/'
complete fg  'c/%/j/' 'p/*/`ps -x | awk \{print\ \$1\}`/'
complete make  'p@*@`perl -ne if\ \(/^\(\[^.#\]\[^:\]+\):/\)\ \{print\ \"\$1\ \"\} Makefile`@'
complete xfig  'p/*/f:*.fig/'
complete dvips  'p/*/f:*.dvi/'
complete latex  'p/*/f:*.{tex,dtx,ins}/'
complete ftp  'p@1@`awk \{print\ \$2\} $HOME/.netrc`@'
complete xdvi  'p/*/f:*.dvi/'
complete cd  'p/1/d/'

###---------->files<----------
### configuration of files 
#set listlinks
#set noclobber
#set rmstar
#set symlinks ignore
umask 002

###---------->prompt/general<----------
### Configuration related to prompts 
#set ellipsis

###---------->dirs command<----------
### Configuration of directory stack 
#set dirsfile = /home/blackie/.cshdirs
#unset pushdsilent

###---------->jobs<----------
### Configuration on job related commands 
#set notify

####---------->limits<----------
### Limits 
#limit stacksize 8192k
#limit coredumpsize 0k
#limit descriptors 64
#

###---------->logout<----------
### Configuration of logout 
#set autologout = (0 0)
#set ignoreeof = 2

###---------->misc<----------
### misc 
#setenv EDITOR xemacs
#setenv VISUAL xemacs
#setenv PAGER less
#set correct = cmd

