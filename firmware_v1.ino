#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>

// Configuramos os pinos do LED RGB
const int pinR = 15;
const int pinG = 2;
const int pinB = 4;

// Realizei configuração de Wi-Fi e OTA
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const String versaoAtual = "1.0";
// URL Raw do seu repositório no GitHub
const char* urlManifesto = "https://raw.githubusercontent.com/moreiraRoberto/CP02--Projeto-Motiva-Atualiza-o-Remota-de-Firmware-OTA-/main/version.json";

// Setup da temporização e lógica
unsigned long tempoUltimaSessao = 0;
const unsigned long intervaloSessao = 48000;
unsigned long tempoUltimaLeitura = 0;
const unsigned long intervaloLeitura = 2000;

int leituras[5];
int contagemLeituras = 0;
bool sessaoAtiva = false;
int sessoesConcluidas = 0;
bool otaVerificada = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  // Acender o LED Azul
  digitalWrite(pinR, LOW);
  digitalWrite(pinG, LOW);
  digitalWrite(pinB, HIGH);

  Serial.println("\nMONITORAMENTO DE VEGETACAO - FW 1.0");

  conectarWiFi();
  
  // Inicia a primeira sessão imediatamente após o boot
  tempoUltimaSessao = millis();
  sessaoAtiva = true;
}

void loop() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoUltimaSessao >= intervaloSessao) {
    tempoUltimaSessao = tempoAtual;
    contagemLeituras = 0;
    sessaoAtiva = true;
  }

  if (sessaoAtiva && (tempoAtual - tempoUltimaLeitura >= intervaloLeitura)) {
    tempoUltimaLeitura = tempoAtual;
    leituras[contagemLeituras] = random(10, 21);
    
    Serial.print("Leitura ");
    Serial.print(contagemLeituras + 1);
    Serial.print(": ");
    Serial.print(leituras[contagemLeituras]);
    Serial.println(" cm");
    
    contagemLeituras++;

    if (contagemLeituras >= 5) {
      sessaoAtiva = false;
      processarSessao();
    }
  }

  // Vamos realizar 3 ciclos de leitura
  if (sessoesConcluidas >= 3 && !otaVerificada) {
    otaVerificada = true;
    verificarAtualizacaoOTA();
  }
}

void conectarWiFi() {
  Serial.print("Conectando ao Wi-Fi Wokwi-GUEST");
  WiFi.begin(ssid, password, 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!\n");
}

void processarSessao() {
  float soma = 0;
  for (int i = 0; i < 5; i++) {
    soma += leituras[i];
  }
  float media = soma / 5.0;
  Serial.print("Media da sessao: ");
  Serial.print(media, 1);
  Serial.println(" cm");
  Serial.println("Proxima sessao em 48 segundos.");
  Serial.println("-----------------------------------");
  
  sessoesConcluidas++;
}

void verificarAtualizacaoOTA() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Erro: Sem conexao Wi-Fi para consultar o manifesto.");
    return;
  }

  Serial.println("\nConsultando manifesto de versao...");
  HTTPClient http;
  http.begin(urlManifesto);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.println("Erro ao analisar o arquivo version.json.");
      http.end();
      return;
    }

    String versaoDisponivel = doc["version"].as<String>();
    String urlFirmware = doc["url"].as<String>();

    Serial.print("Versao instalada: ");
    Serial.println(versaoAtual);
    Serial.print("Versao disponivel: ");
    Serial.println(versaoDisponivel);

    if (versaoDisponivel != versaoAtual) {
      Serial.println("Atualizacao encontrada! Iniciando download e gravacao...");
      executarOTA(urlFirmware);
    } else {
      Serial.println("A versao instalada ja e a mais recente.");
    }
  } else {
    Serial.print("Erro: O manifesto nao pode ser acessado. Codigo HTTP: ");
    Serial.println(httpCode);
  }
  http.end();
}

void executarOTA(String url) {
  HTTPClient http;
  http.begin(url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();

  if (httpCode == 200) {
    int contentLength = http.getSize();
    Serial.print("Tamanho reportado pelo servidor: ");
    Serial.println(contentLength);
    
    size_t tamanhoAtualizacao = (contentLength > 0) ? contentLength : 350000;
    
    bool canBegin = Update.begin(tamanhoAtualizacao);

    if (canBegin) {
      WiFiClient* client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);

      if (written > 0) {
        Serial.println("Download concluido. Gravacao na memoria flash bem-sucedida.");
      } else {
        Serial.println("Erro: Nenhum byte foi gravado na memoria.");
      }

      if (Update.end()) {
        Serial.println("Atualizacao OTA finalizada com exito. Reiniciando o equipamento...");
        delay(1000);
        ESP.restart();
      } else {
        Serial.print("Erro critico no processo de atualizacao OTA: ");
        Serial.println(Update.errorString());
      }
    } else {
      Serial.print("Erro no Update.begin(): ");
      Serial.println(Update.errorString());
    }
  } else {
    Serial.print("Erro: Arquivo de firmware nao pode ser baixado. Codigo HTTP: ");
    Serial.println(httpCode);
  }
  http.end();
}

