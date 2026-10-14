# 🚦 TRAFFIC-LIGHT-ESP32

Semáforo físico con ESP32 + página web en vivo que refleja la luz activa
y la cuenta regresiva para el cambio. Basado en el diseño
[TRAFFIC-LIGHT de Figma](https://www.figma.com/design/exTpyaShWrQcGDrMsf6Yi1/TRAFFIC-LIGHT).

![demo](assets/demo.png)

## ✨ Qué hace

- 🟢 La página muestra la luz activa del ESP32 con glow y la pastilla `STATUS`
  con cuenta regresiva (`STOP 12s`, `GO 8s`, `CAUTION 3s`).
- ⚡ Actualización instantánea por SSE + polling de respaldo.
- 🔌 Funciona por WiFi (HTTP POST) o por USB-Serial.
- 📴 Si el ESP32 deja de reportar ~4 s, la página muestra `DESCONECTADO`.

## 🗂️ Estructura

```text
├── firmware/traffic_light/traffic_light.ino  # sketch ESP32 (rojo GPIO2 20s, verde GPIO5 15s, amarillo GPIO4 5s)
├── server/server.py                           # servidor Python stdlib (sirve la web + API + SSE)
├── web/index.html                             # página (réplica del Figma)
├── docs/WIRING.md                             # cableado
└── assets/demo.png                            # captura
```

## 🚀 Quickstart

### 1. Firmware

1. Abrí `firmware/traffic_light/traffic_light.ino` en Arduino IDE (placa ESP32).
2. Configurá `WIFI_SSID`, `WIFI_PASS` y `SERVER_URL`
   (IP de este equipo, ej. `http://192.168.1.50:8000/api/status`).
3. Subí el sketch. Reporta por Serial (115200) y por WiFi.

### 2. Servidor

```bash
python3 server/server.py --port 8000
# o con el ESP32 por USB:
pip install -r requirements.txt
python3 server/server.py --port 8000 --serial /dev/cu.usbserial-0001  # o COM3 en Windows
```

### 3. Página

Abrí 👉 http://localhost:8000

## 🔌 API

| Método | Ruta          | Descripción                              |
|--------|---------------|------------------------------------------|
| `POST` | `/api/status` | El ESP32 reporta `{"light","remaining","total"}` |
| `GET`  | `/api/status` | Estado actual + `online`                 |
| `GET`  | `/events`     | Push SSE con cada reporte                |

Probar sin ESP32:

```bash
curl -X POST localhost:8000/api/status \
  -H 'Content-Type: application/json' \
  -d '{"light":"red","remaining":20,"total":20}'
```

## 📐 Diseño

Réplica minimalista del frame de Figma: barra horizontal con 3 luces
y pastilla `STATUS`. Luz activa a 100 % con glow, inactivas atenuadas,
`aria-live` para accesibilidad del conteo.
