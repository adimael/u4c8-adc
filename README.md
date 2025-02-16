## Controle de LEDs RGB e Display OLED com Joystick no RP2040

Este projeto teve como objetivo consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab. Para isso, desenvolvi um sistema interativo utilizando um joystick, LEDs RGB e um display OLED SSD1306.

## Vídeo da Prática

Segue o link do vídeo dos resultados obtidos durante a simulação:

[Assista ao vídeo demonstrativo. Clique aqui!](https://youtu.be/NKpbVaHT9DI)

## 🎯 Objetivos

- Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040.
- Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick.
- Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
- Aplicar o protocolo de comunicação I2C na integração com o display.

## 🧩 Componentes Utilizados

- Placa BitDogLab (RP2040)
- LED RGB (GPIOs 11, 12 e 13)
- Joystick (GPIOs 26 e 27)
- Botão do Joystick (GPIO 22)
- Botão A (GPIO 5)
- Display OLED SSD1306 (I2C - GPIO 14 e GPIO 15)

📜 Estrutura do Código

O código está dividido em seções bem organizadas:

1. Configuração de Hardware: Inicialização dos pinos GPIO, ADC e PWM.
2. Leitura do Joystick: Captura dos valores analógicos dos eixos X e Y.
3. Controle dos LEDs RGB: Aplicação do PWM para variação de intensidade.
4. Manipulação do Display SSD1306: Desenho do quadrado móvel e bordas dinâmicas.
5. Interrupções e Debouncing: Gerenciamento dos botões e suas funcionalidades.

## 🚀 Como Executar o Projeto

1. **Clone o repositório**:
   ```bash
   git clone https://github.com/adimael/u4c8-adc.git
   ````
2. **Entre na pasta do projeto**:
   ````
   cd u4c8-adc
   
   ````

### **1️⃣ Configurar o Ambiente de Desenvolvimento**
Certifique-se de ter instalado o **Raspberry Pi Pico SDK** e o **compilador C/C++**. Se ainda não configurou, siga as etapas abaixo:

### Passo 1: Baixar o Ninja
   1. Acesse a página oficial de releases do Ninja:  
   [Ninja Releases no GitHub](https://github.com/ninja-build/ninja/releases).
   2. Baixe o arquivo binário mais recente para Windows (geralmente um `.zip`).
   3. Extraia o conteúdo do arquivo `.zip` (haverá um arquivo `ninja.exe`).

   ---

   ### Passo 2: Adicionar o Ninja ao Path do sistema
   Para que o `ninja` seja reconhecido como um comando em qualquer terminal, siga estas etapas:

   1. Clique com o botão direito no botão **Iniciar** e selecione **Configurações**.
   2. Vá em **Sistema** → **Sobre** → **Configurações avançadas do sistema** (no lado direito).
   3. Na aba **Avançado**, clique em **Variáveis de Ambiente**.
   4. Na seção **Variáveis do Sistema**, localize a variável `Path` e clique em **Editar**.
   5. Clique em **Novo** e adicione o caminho completo para o diretório onde você extraiu o `ninja.exe`. Por exemplo:
      ````
      C:\Users\SeuUsuario\Downloads\ninja-win
      ````
   6. Clique em **OK** em todas as janelas.

   ---

   ### Passo 3: Verificar se o Ninja está funcionando
   1. Abra um terminal (PowerShell ou Prompt de Comando).
   2. Digite o comando abaixo para verificar a instalação:
      ```bash
      ninja --version
      ````

   ### Passo 4: Configurar e Compilar o Projeto

   1. Volte ao diretório do projeto e abra um terminal.
   2. Crie ou limpe a pasta `build`:
      ```bash
      rmdir /s /q build
      mkdir build
      cd build
      
   3. Configure o projeto com o CMake:
      ```bash
      cmake -G Ninja ..
      ````
   4. Compile o projeto:

      ```bash
         ninja
      ````

   5. Após isso, o arquivo main.elf será gerado na pasta build.

### Pré-requisitos
- **Placa BitDogLab** configurada e conectada.
- Ambiente de desenvolvimento configurado com **SDK do RP2040**.

   ou

- Use a extensão do Wokwi no VSCode [https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode](https://marketplace.visualstudio.com/items?itemName=Wokwi.wokwi-vscode)
