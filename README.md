# espNowAutodisc
ESP32 ESP-NOW – Cliente/Servidor de Telemetria com Descoberta Automática

🚀 Visão Geral
Este repositório implementa uma comunicação ESP-NOW completa entre dois ESP32, utilizando um modelo:
Cliente (Sensor) → Descobre o servidor automaticamente por broadcast
Servidor (Gateway) → Registra clientes, responde handshake e recebe telemetria
Envio periódico de temperatura e umidade
Feedback visual usando LED RGB (FastLED)
Lista de peers conectados impressa em tempo real
Botão físico para ações nos dois dispositivos
Tudo isso usando um protocolo simples baseado em comandos (cmd) e uma struct compartilhada entre as placas.

🧩 Arquitetura da Solução
┌─────────────┐       ESP-NOW (Broadcast)        ┌──────────────┐
│   CLIENTE    │  ───────────────────────────▶   │   SERVIDOR    │
│ (Sensor ESP) │  cmd = 0xAA  → Pedido de Link   │  (Gateway)    │
└─────────────┘                                  └──────────────┘
        ▲                                                 │
        │     Resposta Unicast cmd = 0x55                 │
        └─────────────────────────────────────────────────┘

Depois do handshake:

CLIENTE  ─── cmd = 0xBB + Telemetria ───▶  SERVIDOR

📡 Protocolos de Comando
Código	Direção	Significado
0xAA	Cliente → Servidor	Pedido de registro / descoberta
0x55	Servidor → Cliente	Confirmação de registro
0xBB	Cliente → Servidor	Dados de telemetria
🧱 Estrutura da Mensagem

⚠️ IMPORTANTE: cliente e servidor DEVEM usar a mesma struct.
Se os tipos divergirem (ex.: char vs int), os dados chegam desalinhados e corrompidos.

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

📦 Conteúdo do Repositório
📁 root
 ├── client/
 │    └── main.cpp      ← Código do cliente ESP-NOW
 ├── server/
 │    └── main.cpp      ← Código do servidor ESP-NOW
 └── README.md          ← Este arquivo bonito 🥰

🛰️ Funcionamento do Cliente

✔ Entra em modo WIFI_STA
✔ Envia broadcast para buscar o servidor
✔ Ao receber resposta (0x55), cadastra o MAC remoto
✔ Envia dados a cada 50s
✔ Usa botão no GPIO 0 para acionar descoberta

✉️ Pacote enviado:
cmd = 0xBB
temp = random(25–30)
hum  = random(50–100)
readingId++

🛑 Funcionamento do Servidor

✔ Responde pedidos de cadastro
✔ Mantém tabela de peers
✔ Exibe MACs conectados
✔ Recebe telemetria e mostra no Serial
✔ Usa LED RGB para feedback (FastLED)

💡 Indicações visuais
Cor	Significado
🔴 Vermelho	Ação manual / teste
🟢 Verde	Cliente conectado
🔵 Azul piscando	Atividade / monitoramento
⚪ Branco	Idle / estado neutro
🔌 Pinagem Recomendada
Função	Cliente	Servidor
Botão	GPIO 0	GPIO 0
LED digital	—	GPIO 48
LED RGB (WS2812)	—	GPIO 48

Pode alterar conforme a placa usada (ESP32, ESP32-S3, etc.)

🛠️ Como Rodar
1️⃣ Carregue o Servidor

Abra o código do servidor
Compile e envie para o ESP32
Abra o Serial Monitor (115200)
Aguarde: ele entrará em modo monitoramento

2️⃣ Carregue o Cliente

Abra o código do cliente
Compile e envie para o segundo ESP32
Pressione o botão GPIO 0 → inicia a descoberta

3️⃣ Aproveite!

O servidor pisca LEDs
Mostra MACs conectados
Imprime telemetria recebida

🎯 Possíveis Extensões

Integrar sensores reais (DHT22, BME280, SHT31)
Encaminhar dados para MQTT, HTTP ou banco local
Criar dashboard web para visualização
Suporte a múltiplos clientes simultâneos
Implementar OTA por Wi-Fi

📜 Licença

Escolha a licença que desejar (MIT recomendado)
