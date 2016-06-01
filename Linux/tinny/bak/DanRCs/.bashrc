# .bashrc

# User specific aliases and functions

# global definitions

sys=`uname`
umask 002

if [ "x$sys" = "xLinux" ]; then

# are we an interactive shell?
#  if [ "$PS1" ]; then
   
    if [ "x$SHLVL" != "x1" ]; then # We're not a login shell
       if [ -f $HOME/.dircolors ]; then
                eval `dircolors --sh $HOME/.dircolors` && COLORS=$HOME/.dircolors
                if echo $SHELL |grep bash 2>&1 >/dev/null; then # aliases are bash only
                        if ! egrep -qi "^COLOR.*none" $COLORS &>/dev/null; then
                                alias ll='ls -l --color'
                                alias l.='ls -d .[a-zA-Z]* --color'
                                alias ls='ls --color'
                        else
                                alias ll='ls -l'
                                alias l.='ls -d .[a-zA-Z]*'
                        fi
                fi

       fi
       if [ -x /etc/profile.d/mc.sh ]; then
	        . /etc/profile.d/mc.sh
       fi
       if [ -x /etc/profile.d/less.sh ]; then
	        . /etc/profile.d/less.sh
       fi
       if [ -x /etc/profile.d/which-2.sh ]; then
	        . /etc/profile.d/which-2.sh
       fi
       if [ "x$LANG" = "x" ]; then
            if [ -x /etc/profile.d/lang.sh ]; then
	        . /etc/profile.d/lang.sh
           fi
       fi
    fi
#  fi
#  if [ "x$DISPLAY" = "x" ]; then
#      export DISPLAY=10.32.2.206:9.0
# fi
  export MANPAGER="$HOME/bin/shell/pman"
else
  if [ "x$HOSTNAME" = "xux165" ]; then
    alias ls='/usr/local/hnc/bin/ls --color'
    alias zcat='/usr/local/hnc/bin/zcat'
    alias ln='/usr/local/hnc/bin/ln'
    alias cp='/usr/local/hnc/bin/cp'
  else
    alias ls='/usr/local/bin/ls --color'
    alias zcat='/usr/local/bin/zcat'
    alias ln='/usr/local/bin/ln'
    alias cp='/usr/local/bin/cp'
  fi
  alias ps='/usr/ucb/ps'
  export LS_COLORS=$'no=00:fi=00:di=01;33:ln=01;36:pi=40;33:so=01;35:bd=40;33;01:cd=40;33;01:or=01;05;37;41:mi=01;05;37;41:ex=01;32:*.cmd=01;32:*.exe=01;32:*.com=01;32:*.btm=01;32:*.bat=01;32:*.sh=01;32:*.csh=01;32:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.gz=01;31:*.bz2=01;31:*.bz=01;31:*.tz=01;31:*.rpm=01;31:*.cpio=01;31:*.jpg=01;35:*.gif=01;35:*.bmp=01;35:*.xbm=01;35:*.xpm=01;35:*.png=01;35:*.tif=01;35:'
  export PATH="/usr/openwin/bin:$PATH"
  export TERMINFO="$HOME/.terminfo"
  export PAGER="$HOME/bin/shell/pman"
  export MANPATH="/usr/man:/usr/local/man:/usr/openwin/man:/win/X11/man:/opt/gnu/man:/opt/SUNWspro/man:/home/splus/man:/Applications/math/man:/admn/man:/usr/share/catman:/sw/vim/man"
fi 

if [ "$PS1" ]; then
  stty erase `tput kbs`
  [ "$PS1" = "\\s-\\v\\\$ " ] && PS1="[\u@\h \W]\\$ "
  case $TERM in
    xterm*)
      PROMPT_COMMAND='echo -ne "\033]0;${USER}@${HOSTNAME%%.*}:${PWD/$HOME/~}\007"'
      ;;
    *)
      [ -e /etc/sysconfig/bash-prompt-default ] && PROMPT_COMMAND=/etc/sysconfig/bash-prompt-default
      ;;
  esac
fi

# Myself configurations
export PATH="$PATH:/usr/X11R6/bin:/usr/ucb:/usr/bsd:/usr/local/hnc/bin:/usr/local/bin/X11:/usr/sbin:/sgiusr/sbin:/usr/kvm:/vol/bin:/usr/lib/netls/bin:/sw/vim/bin"
export LD_LIBRARY_PATH="/usr/local/lib:/usr/X11R6/lib"
export PATH="/work/price/falcon/${sys}/bin:/work/price/falcon/jas/perl:/work/price/falcon/dvl/perl:/work/iosdistrib/${sys}/bin:${PATH}"
export PATH="${HOME}/bin:${HOME}/bin/perl:${HOME}/bin/c:${HOME}/bin/shell:${HOME}/fvwm2/bin:${PATH}"
export PATH="${HOME}/vim/bin.${sys}:$PATH"
case $HOSTNAME in
  snap* ) export PRINTER='sdoprt05@yak' ;;
  * )     export PRINTER='sdoprt05' ;;
esac
#   export LD_LIBRARY_PATH="${HOME}/lib:$LD_LIBRARY_PATH"
#export USER=`whoami`

alias rm='rm -i'
if [ "x$HOSTNAME" = "xux165" ]; then
  alias vim=/home/bxz/vim/bin.SunOS/vim.ux165
fi
alias vi='vim'
alias vr='vim -R -'

#alias less='less -r'
alias rxvt='rxvt -bg black -fg white -vb -tn xterm'

stcvs()
{
export CVS_RSH=ssh
export CVSROOT=:ext:bozhang@cvs.sourceforge.net:/cvsroot/minichinput
alias cvs="cvs -z3"
}

#p52()
#{
#echo "Printing to the default printer of ux52 ............"
#ssh ux52 "lpr $PWD/$*"
#}

#setting for modeling
export PYTHONPATH='/work/gold/projects/lib'
export MSG="/work/gold/bin"
if [ "x$LANG" = "xzh_CN.GB2312" ]; then
  export LC_COLLATE=zh_CN.GB2312
  export LC_CTYPE=zh_CN.GB2312
else
  export LC_COLLATE=C
  export LC_CTYPE=C
fi
. $HOME/.dirsettings
