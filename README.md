# EmbarcaTech_Unidade4_Comunicacao_Serial
Repositório criado com o intuito de armazenas projetos da Capacitação do EmbarcaTech


# README - Projeto BitDogLab RP2040

## Descrição
Este projeto implementa um sistema interativo utilizando a placa BitDogLab RP2040. Ele combina comunicação serial, controle de LEDs, botões com interrupções, exibição de caracteres em um display OLED SSD1306 e controle de uma matriz de LEDs WS2812B.

## Funcionalidades
1. **Exibição de Caracteres no Display SSD1306**
   - O usuário pode enviar caracteres via USB/Serial.
   - O caractere digitado será exibido no display SSD1306.
   
2. **Representação Numérica na Matriz de LEDs WS2812B**
   - Se um número entre 0 e 9 for digitado, ele será representado na matriz 5x5 de LEDs.
   
3. **Interação com os Botões**
   - **Botão A**: Alterna o estado do LED verde e exibe uma mensagem no display SSD1306.
   - **Botão B**: Alterna o estado do LED azul e exibe uma mensagem no display SSD1306.

## Hardware Utilizado
- **Placa BitDogLab RP2040**
- **Display OLED SSD1306** (I2C, pinos GPIO 14 e 15)
- **Matriz de LEDs WS2812B** (pino GPIO 7)
- **LED RGB** (pinos GPIO 11, 12 e 13)
- **Botões** (GPIO 5 e 6)

## Configuração e Execução
### 1. Conectar o Hardware
- Certifique-se de conectar todos os componentes conforme os pinos especificados.

### 2. Compilar e Carregar o Código
- Compile o código usando o SDK do RP2040.
- Faça o upload do binário para a placa via USB.

### 3. Interação
- Abra um terminal serial (baud rate 115200).
- Digite um caractere para exibi-lo no display.
- Pressione os botões para alternar os LEDs e exibir mensagens.

## Observações
- O código inclui debounce para evitar leituras falsas nos botões.
- A matriz de LEDs segue um padrão específico de endereçamento.

## Autor
Este projeto foi desenvolvido por **Maria Clara Simoes de Jesus** para consolidar conhecimentos sobre microcontroladores e comunicação serial com o RP2040.