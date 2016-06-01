#!/bin/sh

if [ -f "$HOME/.Xdefaults" ] ; then
    xrdb -merge $HOME/.Xdefaults        # Load Users X11 resource database
fi

xterm -geometry 80x30 -ls -sb -fg green -bg black -fn 7x14&

#exec /home/dxl/fvwm-2.4.18/bin/fvwm2

#exec /home/bxz/fvwm2/bin/fvwm -display @d | /usr/bin/X11/xterm -ls -sb -geometry 80x30+0+0 -display @d

exec /home/bxz/fvwm2/bin/fvwm -display @d