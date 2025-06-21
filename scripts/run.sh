#!/bin/sh

xrdb merge ~/.Xresources
# picom -b &
/usr/lib/polkit-kde-authentication-agent-1 &
udiskie -2 &
feh --bg-fill ~/Pictures/wallpaper/scenery_tower.jpg &
dunst &
xset r rate 200 50 &

dash ~/.config/chadwm/scripts/bar.sh &
while type chadwm >/dev/null; do chadwm && continue || break; done
