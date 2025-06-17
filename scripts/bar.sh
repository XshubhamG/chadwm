#!/bin/dash

# ^c$var^ = fg color
# ^b$var^ = bg color

interval=0

# load colors
. ~/.config/chadwm/scripts/bar_themes/catppuccin

cpu() {
  cpu_val=$(grep -o "^[^ ]*" /proc/loadavg)

  printf "^c$black^ ^b$green^ CPU"
  printf "^c$white^ ^b$grey^ $cpu_val"
}

pkg_updates() {
  #updates=$({ timeout 20 doas xbps-install -un 2>/dev/null || true; } | wc -l) # void
  updates=$({ timeout 20 checkupdates 2>/dev/null || true; } | wc -l) # arch
  # updates=$({ timeout 20 aptitude search '~U' 2>/dev/null || true; } | wc -l)  # apt (ubuntu, debian etc)

  if [ -z "$updates" ]; then
    printf "  ^c$green^    Fully Updated"
  else
    printf "  ^c$green^    $updates"" updates"
  fi
}

battery() {
  # Dynamically detect battery directory (BAT0 or BAT1)
  for bat in /sys/class/power_supply/BAT*; do
    [ -f "$bat/capacity" ] && get_capacity="$(cat "$bat/capacity")" && break
  done
  if [ -z "$get_capacity" ]; then
    printf "^c$blue^   N/A"
  else
    printf "^c$blue^   $get_capacity"
  fi
}

brightness() {
  screen_bright=$(brightnessctl get)
  max_bright=$(brightnessctl m)
  percent=$((100 * screen_bright / max_bright))
  percent_5=$((percent - (percent % 5)))

  printf "^c$red^  "
  printf "^c$red^ $percent_5"
}

mem() {
  printf "^c$blue^^b$black^  "
  printf "^c$blue^ $(free -h | awk '/^Mem/ { print $3 }' | sed s/i//g)"
}

wlan() {
  # Find the first wireless interface
  iface=$(ls /sys/class/net/ | grep -E '^wl' | head -n 1)
  if [ -z "$iface" ]; then
    printf "^c$black^ ^b$blue^ 󰤭 ^d^%s" " ^c$blue^No WiFi"
    return
  fi
  state=$(cat "/sys/class/net/$iface/operstate" 2>/dev/null)
  case "$state" in
  up) printf "^c$black^ ^b$blue^ 󰤨 ^d^%s" " ^c$blue^Connected" ;;
  down) printf "^c$black^ ^b$blue^ 󰤭 ^d^%s" " ^c$blue^Disconnected" ;;
  *) printf "^c$black^ ^b$blue^ 󰤭 ^d^%s" " ^c$blue^Unknown" ;;
  esac
}

clock() {
  printf "^c$black^ ^b$darkblue^ 󱑆 "
  printf "^c$black^^b$blue^ $(date '+%H:%M')  "
}

while true; do

  [ $interval = 0 ] || [ $(($interval % 3600)) = 0 ] && updates=$(pkg_updates)
  interval=$((interval + 1))

  sleep 1 && xsetroot -name "$updates $(battery) $(brightness) $(cpu) $(mem) $(wlan) $(clock)"
done
