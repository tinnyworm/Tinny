# .bashrc

# if not logging in interactively do nothing 
[ -z "$PS1" ] && return 

# Source global definitions
if [ -f /etc/bashrc ]; then
    . /etc/bashrc
fi

# User specific aliases
alias cp='cp -i'                                # interactive copy
alias mv='mv -i'                                # interactive move
alias rm='rm -i'                                # interactive remove
alias grep='grep --color'                       # show differences in colour
alias clean='rm -f *~'                          # clean temporary
alias less='less -S'                            # long line display

# VNC Server
alias vnc='vncserver -geometry 1220x900 -depth 24' # VNC Server

SYS=`uname -s`
UFAL='/work/price/falcon'

if [ "$SYS" = "Linux" ]; then

    # User specific commands directory
    PATH=${PATH}:~/tinny/bin:/usr/local/bin:/etc:/usr/etc:/usr/bin/X11:/usr/local/etc:/usr/local/bin/X11:/work/gold/linux/bin:/work/gold/projects/bin:${UFAL}/Linux/bin:${UFAL}/Linux/local/bin:${UFAL}/bin:/fo1/bin/Linux:/fo1/R/R-2.4.0/bin:/home/krh/tools

fi

if [ "$SYS" = "SunOS" ]; then

    # User specific commands directory
    PATH=${PATH}:~/tinny/bin:/usr/local/bin:/etc:/usr/etc:/usr/bin/X11:/usr/local/etc:/usr/local/bin/X11:/work/gold/bin:/work/gold/projects/bin:${UFAL}/bin:${UFAL}/local/bin:${UFAL}/bin:/fo1/bin/SunOS:/home/dxl/tools/perl:/home/krh/tools

fi

# uwe's utilities
export PATH=/ana/tda6/Tools/`uname`/bin:${PATH}
export PATH=/ana/tda6/Tools/bin:${PATH}

# java path
export PATH=/ana/tda6/j2sdk1.4.2_09/bin/:${PATH}
export CLASSPATH=.:/ana/tda6/j2sdk1.4.2_09

# ant path
export PATH=/ana/tda6/apache/apache-ant-1.6.5/bin/:${PATH}
export ANT_HOME=/ana/tda6/apache/apache-ant-1.6.5/

# printer
export PRINTER=sdob0312

# permission
umask 0007

# default group
[ $(id -gn) = falcon ] || exec newgrp falcon
