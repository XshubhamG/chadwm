#!/bin/sh

xrdb merge ~/.Xresources
picom -b &
/usr/lib/polkit-kde-authentication-agent-1 &
/usr/lib/gpaste/gpaste-daemon &
udiskie -2 &
redshift -P -0 5000 &
dash ~/.fehbg &
dunst &
xset r rate 200 50 &

dash ~/.config/chadwm/scripts/bar.sh &
while type chadwm >/dev/null; do chadwm && continue || break; done
