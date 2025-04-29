# Ohmimetro
#### Autor:
* Roberto Vítor Lima Gomes Rodrigues

### Ohmímetro
Como segundo projeto, foi proposta a implementação de um ohmímetro na placa BitDogLab, utilizando as medições do ADC junto a um divisor de tensão com um resistor de referência para que fosse possível a mensuração do valor de resistência do outro.

#### Vídeo de funcionamento
    https://youtu.be/PgzQ43a0IY8?feature=shared


#### Instruções de compilação
Certifique-se de que você tem o ambiente configurado conforme abaixo:
* Pico SDK instalado e configurado.
* VSCode com todas as extensões configuradas, CMake e Make instalados.
* Clone o repositório e abra a pasta do projeto, a extensão Pi Pico criará a pasta build
* Clique em Compile na barra inferior, do lado direito (ao lado esquerdo de RUN | PICO SDK)
* Verifique se gerou o arquivo .uf2
* Conecte seu divisor de tensão à placa. Ground em GND, Vcc em 3,3V e o ADC no pino 28
* Conecte a placa BitDogLab e ponha-a em modo BOOTSEL
* Arraste o arquivo até a placa, que o programa se iniciará

#### Manual do programa
Ao executar o programa já conectado com o divisor de tensão, as seguintes características poderão ser verificadas:
* Os LEDs WS2812 se acenderão à medida que o circuito for ligado, msotrando as faixas do resistor de acordo com a série e24, e o display mostrará a medida de resistência real mensurada pelo ADC e a sequência de letras referente às faixas do resistor dessa série

