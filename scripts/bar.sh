#!/bin/dash

# ^c$var^ = fg color
# ^b$var^ = bg color

interval=0

. ~/.config/chadwm/scripts/bar_themes/catppuccin

cpu() {
  cpu_val=$(grep -o "^[^ ]*" /proc/loadavg)

  printf "^c$base^ ^b$green^  "
  printf "^c$green^ ^b$surface0^ $cpu_val"
}

pkg_updates() {
  updates=$({ timeout 20 checkupdates 2>/dev/null || true; } | wc -l)

  if [ -z "$updates" ] || [ "$updates" -eq 0 ]; then
    printf "  ^c$green^    Fully Updated"
  else
    printf "  ^c$yellow^    $updates updates"
  fi
}

battery() {
  for bat in /sys/class/power_supply/BAT*; do
    [ -f "$bat/capacity" ] && get_capacity="$(cat "$bat/capacity")" && break
  done
  if [ -z "$get_capacity" ]; then
    printf "^c$base^ ^b$blue^   N/A"
  else
    printf " ^c$blue^ ^b$base^   $get_capacity%%"
  fi
}

brightness() {
  screen_bright=$(brightnessctl get)
  max_bright=$(brightnessctl m)
  percent=$((100 * screen_bright / max_bright))
  percent_5=$((percent - (percent % 5)))

  printf "^c$base^ ^b$red^  "
  printf "^c$red^ ^b$base^ $percent_5%%"
}

mem() {
  printf "^c$base^ ^b$blue^  "
  printf "^c$blue^ ^b$base^ $(free -h | awk '/^Mem/ { print $3 }' | sed s/i//g)"
}

wlan() {
  iface=$(ls /sys/class/net/ | grep -E '^wl' | head -n 1)
  if [ -z "$iface" ]; then
    printf "^c$base^ ^b$sky^ 󰤭 ^d^%s" " ^c$sky^No WiFi"
    return
  fi

  state=$(cat "/sys/class/net/$iface/operstate" 2>/dev/null)
  case "$state" in
  up) printf "^c$base^ ^b$sky^ 󰤨  ^d^%s" " ^c$sky^Connected" ;;
  down) printf "^c$base^ ^b$sky^ 󰤭  ^d^%s" " ^c$sky^Disconnected" ;;
  *) printf "^c$base^ ^b$sky^ 󰤭  ^d^%s" " ^c$sky^Unknown" ;;
  esac
}

clock() {
  printf "^c$base^ ^b$sapphire^ 󱑆 "
  printf "^c$base^^b$sapphire^ $(date '+%H:%M')  "
}

while true; do
  [ $interval = 0 ] || [ $(($interval % 3600)) = 0 ] && updates=$(pkg_updates)
  interval=$((interval + 1))

  sleep 1 && xsetroot -name "$(battery) $(brightness) $(cpu) $(mem) $(wlan) $(clock)"
done
