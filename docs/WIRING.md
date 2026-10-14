# Cableado

| Luz      | Pin ESP32 | Duración | Resistencia |
|----------|-----------|----------|-------------|
| 🔴 Roja   | GPIO 2    | 20 s     | 220 Ω a GND |
| 🟢 Verde  | GPIO 5    | 15 s     | 220 Ω a GND |
| 🟡 Amarilla | GPIO 4  | 5 s      | 220 Ω a GND |

Secuencia: **ROJO → VERDE → AMARILLO → ROJO …**

> Si los pines 4 o 5 están ocupados, cambia `YELLOW_PIN` / `GREEN_PIN`
> en `firmware/traffic_light/traffic_light.ino`.
