# Projeto Motiva | Atualização Remota de Firmware (OTA)

**S2-CP02 · Edge Computing · FIAP · Turma 2CCPW · Grupo 26**
> **Professor:** Marcelo Fernando Morgantini
---

O projeto tem como objetivo desenvolver uma solução de atualização remota de firmware (OTA - Over The Air) para um dispositivo ESP32 simulado no Wokwi. No cenário proposto, um nó IoT responsável por monitorar a altura da vegetação em campo deve ser capaz de receber novas versões de software sem a necessidade de acesso físico ao equipamento.

Além da implementação da atualização OTA, o trabalho busca integrar conceitos de programação embarcada, tratamento de dados, comunicação via HTTP, versionamento de software e gerenciamento de firmware.

## Integrantes

| Nome | RM |
| ---- | -- |
| Bruna Queiroz | RM565648 |
| Felipe Santos | RM565274 |
| Gabriel Coutinho | RM565441 |
| Manoela Oliveira | RM563952 |
| Miguel Silva | RM565141 |

---

## Links do Projeto
* **Simulação no Wokwi:** https://wokwi.com/projects/475814605798574081
* **Repositório GitHub:** https://github.com/manoela-oliveira/cp2-firmware-edge-computing

---

## Objetivo do Projeto
O objetivo deste projeto é simular um nó de sensoriamento de vegetação (Projeto Motiva) utilizando um ESP32 no ambiente Wokwi. O dispositivo realiza medições periódicas (pseudoaleatórias) da altura da vegetação, processa os dados e, o mais importante, realiza uma **Atualização Remota de Firmware (OTA - Over-The-Air)** de forma autônoma, sem intervenção manual.

---

## Arquitetura da Solução
1. **Dispositivo:** ESP32 simulado no Wokwi.
2. **Conectividade:** Rede virtual `Wokwi-GUEST`.
3. **Servidor de Atualização:** Repositório no GitHub hospedando os arquivos de controle (`version.json`) e o arquivo binário compilado da nova versão (`firmware_v2.bin`).
4. **Fluxo OTA:** O ESP32 (rodando FW 1.0) acessa o manifesto JSON via HTTP. Ao detectar que a versão online ("2.0") é superior à instalada ("1.0"), ele realiza o download do `.bin`, grava na memória flash e reinicia automaticamente rodando o novo sistema.

---

## Funcionalidades Implementadas

### Firmware 1.0 (Versão Inicial)
* **Indicador Visual:** LED RGB aceso na cor **Azul**.
* **Coleta de Dados:** Gera 5 leituras pseudoaleatórias (10 a 20 cm) por sessão. Intervalo de 2 segundos entre leituras.
* **Processamento:** Calcula e exibe no Serial Monitor a média aritmética da sessão.
* **Temporização:** Utiliza `millis()` para garantir que uma nova sessão inicie exatamente a cada 48 segundos.
* **Gatilho OTA:** Após concluir 3 sessões completas de medição, o ESP32 se conecta ao servidor, baixa e instala a versão 2.0.

### Firmware 2.0 (Versão Atualizada)
* **Indicador Visual de Estado:** 
  * **Verde:** Estado NORMAL (Mediana <= 14 cm).
  * **Vermelho:** Estado ALERTA (Mediana >= 16 cm).
* **Tratamento de Dados Avançado:** Mantém a coleta de dados, mas agora ordena o vetor de leituras em ordem crescente (utilizando lógica *Bubble Sort*).
* **Cálculo de Mediana:** Extrai a mediana (3º elemento do vetor ordenado) para uma análise mais robusta.
* **Histerese:** Implementa lógica de histerese para evitar transições bruscas de estado. Se a mediana estiver entre 14 cm e 16 cm, o sistema mantém o estado anterior (evitando o efeito *piscando* entre alertas).

---

## Instruções de Execução e Teste

Para validar o funcionamento do projeto, siga os passos abaixo:

1. Acesse o link do Wokwi fornecido acima
2. Inicie a simulação (botão *Play*).
3. **Observação do FW 1.0:** 
   * Note que o Serial Monitor indicará `FW 1.0` e o LED estará **Azul**.
   * Aguarde o sistema realizar **3 sessões completas** (aprox. 2 minutos e meio).
4. **Processo de Atualização (OTA):**
   * Após a 3ª sessão, o Serial Monitor exibirá a consulta ao manifesto `version.json`.
   * Ele identificará a versão `2.0`, fará o download, indicará sucesso na gravação e o ESP32 será reiniciado.
5. **Observação do FW 2.0:**
   * Após o *reboot*, o Serial Monitor exibirá `FW 2.0`.
   * O sistema passará a exibir o vetor ordenado, a média, a mediana e o estado atual (ALERTA, NORMAL ou MANTIDO).
   * O LED mudará de cor dinamicamente para **Verde** ou **Vermelho** com base na histerese da mediana.

---

## Estrutura de Arquivos
* `firmware_v1.ino`: Código-fonte da versão inicial (com lógica OTA).
* `firmware_v2.ino`: Código-fonte da versão evoluída (com ordenação, mediana e histerese).
* `version.json`: Manifesto de versão consumido pelo ESP32.
* `firmware_v2.bin`: Arquivo binário compilado da versão 2.0 hospedado para download.
* `README.md` / `Relatorio.pdf`: Este documento de documentação.
