# 🌐📡 ESP32 ESP-NOW — Cliente/Servidor com Registro Automático + Telemetria 🔥

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-ESP%20NOW-blue?logo=espressif&style=for-the-badge)
![C++](https://img.shields.io/badge/Written%20in-C++-orange?style=for-the-badge)
![PlatformIO](https://img.shields.io/badge/Compatible-PlatformIO%20%7C%20ArduinoIDE-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Project-Active-success?style=for-the-badge)

</div>

---

## 🚀 Visão Geral

Este repositório implementa uma comunicação **ESP-NOW completa entre dois ESP32**, utilizando um modelo:

- **Cliente (Sensor)** → Descobre o servidor automaticamente por broadcast  
- **Servidor (Gateway)** → Registra clientes, responde handshake e recebe telemetria  
- Envio periódico de **temperatura** e **umidade**  
- Feedback visual usando **LED RGB (FastLED)**  
- Lista de peers conectados impressa em tempo real  
- Botão físico para ações nos dois dispositivos  

---

## 🧩 Arquitetura da Solução

```
┌─────────────┐       ESP-NOW (Broadcast)        ┌──────────────┐
│   CLIENTE    │  ───────────────────────────▶   │   SERVIDOR    │
│ (Sensor ESP) │  cmd = 0xAA  → Pedido de Link   │  (Gateway)    │
└─────────────┘                                  └──────────────┘
        ▲                                                 │
        │     Resposta Unicast cmd = 0x55                 │
        └─────────────────────────────────────────────────┘

Depois do handshake:

CLIENTE  ─── cmd = 0xBB + Telemetria ───▶  SERVIDOR
```

---

## 📡 Protocolos de Comando

| Código | Direção | Significado |
|-------|---------|-------------|
| **0xAA** | Cliente → Servidor | Pedido de registro / descoberta |
| **0x55** | Servidor → Cliente | Confirmação de registro |
| **0xBB** | Cliente → Servidor | Dados de telemetria |

---

## 🧱 Estrutura da Mensagem

⚠️ **IMPORTANTE:** cliente e servidor DEVEM usar a **mesma struct**.

```cpp
typedef struct struct_message {
    int id;
    char ssid[16];
    char password[16];
    uint32_t readingId;
    int cmd;
    bool btn;
    float temp;
    float hum;
} struct_message;
```

---

## 📦 Estrutura do Repositório

```
📁 root
 ├── client/
 │    └── main.cpp      
 ├── server/
 │    └── main.cpp      
 └── README.md          
```

---

## 🛰️ Funcionamento do Cliente

- Entra em modo `WIFI_STA`  
- Envia broadcast para buscar o servidor  
- Ao receber resposta (`0x55`), cadastra o MAC remoto  
- Envia dados a cada 50s  
- Usa botão no GPIO 0 para acionar descoberta  

### ✉️ Pacote enviado:

```
cmd = 0xBB
temp = random(25–30)
hum  = random(50–100)
readingId++
```

---

## 🛑 Funcionamento do Servidor

- Responde pedidos de cadastro  
- Mantém tabela de peers  
- Exibe MACs conectados  
- Recebe telemetria e mostra no Serial  
- Usa LED RGB para feedback (FastLED)  

### 💡 Indicações visuais

| Cor | Significado |
|-----|-------------|
| 🔴 Vermelho | Ação manual / teste |
| 🟢 Verde | Cliente conectado |
| 🔵 Azul piscando | Atividade / monitoramento |
| ⚪ Branco | Idle / estado neutro |

---

## 🔌 Pinagem Recomendada

| Função | Cliente | Servidor |
|--------|---------|----------|
| Botão | GPIO 0 | GPIO 0 |
| LED digital | — | GPIO 48 |
| LED RGB (WS2812) | — | GPIO 48 |

---

## 🛠️ Como Rodar

### 1️⃣ Carregue o Servidor
- Compile e envie para o ESP32  
- Abra o **Serial Monitor (115200)**  

### 2️⃣ Carregue o Cliente
- Compile e envie para outro ESP32  
- Pressione o botão **GPIO 0**  

### 3️⃣ Funcionamento
- Servidor pisca LEDs  
- Exibe clientes conectados  
- Recebe telemetria contínua  

---

## 🎯 Possíveis Extensões

- Integrar sensores reais (DHT22, BME280, SHT31)  
- Enviar dados via MQTT ou HTTP  
- Dashboard web para monitoramento  
- Suporte a múltiplos clientes  
- OTA por Wi-Fi  

---

## 📜 Licença

MIT ou outra de sua escolha.
