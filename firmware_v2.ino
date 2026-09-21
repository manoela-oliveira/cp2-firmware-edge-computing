// Definição dos pinos do LED RGB 
const int pinR = 15; 
const int pinG = 2; 
const int pinB = 4; 
 
// Variáveis de temporização (millis) 
unsigned long tempoUltimaSessao = 0; 
const unsigned long intervaloSessao = 48000; // 48 segundos 
unsigned long tempoUltimaLeitura = 0; 
const unsigned long intervaloLeitura = 2000; // 2 segundos 
 
int leituras[5]; 
int contagemLeituras = 0; 
bool sessaoAtiva = true; 
 
// Estados do sistema 
enum Estado { NORMAL, ALERTA }; 
Estado estadoAtual = NORMAL; 
 
void setup() { 
  Serial.begin(115200); 
   
  pinMode(pinR, OUTPUT); 
  pinMode(pinG, OUTPUT); 
  pinMode(pinB, OUTPUT); 
 
  // Inicia com o estado padrão 
  atualizarLED(); 
  Serial.println("MONITORAMENTO DE VEGETACAO - FW 2.0"); 
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
    contagemLeituras++; 
 
    if (contagemLeituras >= 5) { 
      sessaoAtiva = false; 
      processarDados(); // Chama função dedicada para não poluir o loop 
    } 
  } 
} 
 
// Função para processamento matemático e ordenação 
void processarDados() { 
  float soma = 0; 
  int leiturasOrdenadas[5]; 
 
  Serial.print("Leituras originais: "); 
  for (int i = 0; i < 5; i++) { 
    leiturasOrdenadas[i] = leituras[i]; // Copia para não alterar o vetor original 
    soma += leituras[i]; 
    Serial.print(leituras[i]); 
    if (i < 4) Serial.print(", "); 
  } 
  Serial.println(); 
 
  // Ordenação (Bubble Sort simples) 
  for (int i = 0; i < 4; i++) { 
    for (int j = i + 1; j < 5; j++) { 
      if (leiturasOrdenadas[i] > leiturasOrdenadas[j]) { 
        int temp = leiturasOrdenadas[i]; 
        leiturasOrdenadas[i] = leiturasOrdenadas[j]; 
        leiturasOrdenadas[j] = temp; 
      } 
    } 
  } 
 
  Serial.print("Valores ordenados:  "); 
  for (int i = 0; i < 5; i++) { 
    Serial.print(leiturasOrdenadas[i]); 
    if (i < 4) Serial.print(", "); 
  } 
  Serial.println(); 
 
  float media = soma / 5.0; 
  int mediana = leiturasOrdenadas[2]; // Mediana é o 3º elemento do array ordenado 
 
  Serial.print("Media da sessao: "); 
  Serial.print(media, 1); 
  Serial.println(" cm"); 
   
  Serial.print("Mediana da sessao: "); 
  Serial.print(mediana); 
  Serial.println(" cm"); 
 
  aplicarHisterese(mediana); 
 
  Serial.println("Proxima sessao em 48 segundos."); 
  Serial.println("-----------------------------------"); 
} 
 
// Função para definir o estado com base na mediana 
void aplicarHisterese(int mediana) { 
  if (mediana >= 16) { 
    estadoAtual = ALERTA; 
    Serial.println("Estado atual: ALERTA"); 
  } else if (mediana <= 14) { 
    estadoAtual = NORMAL; 
    Serial.println("Estado atual: NORMAL"); 
  } else { 
    // Entre 14 e 16 cm: mantém estado anterior 
    Serial.print("Estado atual: MANTIDO ("); 
    Serial.print(estadoAtual == NORMAL ? "NORMAL" : "ALERTA"); 
    Serial.println(")"); 
  } 
  atualizarLED(); 
} 
 
// Função para atualização do hardware 
void atualizarLED() { 
  digitalWrite(pinR, LOW); 
  digitalWrite(pinG, LOW); 
  digitalWrite(pinB, LOW); 
 
  if (estadoAtual == NORMAL) { 
    digitalWrite(pinG, HIGH); // Verde 
  } else if (estadoAtual == ALERTA) { 
    digitalWrite(pinR, HIGH); // Vermelho 
  } 
} 
