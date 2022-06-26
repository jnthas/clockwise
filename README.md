# Clockwise
DIY smart wall clock device

Clockwise foi uma ideia que tive enquanto trabalhava com matrizes de LED de 64x64.
Essas matrizes tem mais ou menos o tamanho de um relógio de parede e com o ESP32, além de controlar o conteúdo apresentado no display ainda ganhamos as funcionalidades de 
WiFi, Bluetooth, touch buttons e os mais diversos sensores o que nos leva a ter em mãos basicamente um relógio inteligente de parede. 
A partir daí, comecei a desenvolver uma plataforma para que sejam criadas as _Clockfaces_, ou skins que o relógio pode ter. As possibilidades são muitas e espero que com ajuda de contribuidores, podemos crescer ainda mais as opções.

Atualmente são três opção de clockfaces:

## Mario Bros. Clock

![Mario Bros. Clockface](https://github.com/jnthas/cw-cf-0x01/blob/main/cf_0x01_thumb.jpg "Mario Bros. Clockface")

https://github.com/jnthas/cw-cf-0x01


## Time in Words

![Time in Words Clockface](https://github.com/jnthas/cw-cf-0x02/blob/main/cf_0x02_thumb.jpg "Time in Words Clockface")

https://github.com/jnthas/cw-cf-0x02

## World Map Clock

![World Map Clockface](https://github.com/jnthas/cw-cf-0x03/blob/main/cf_0x03_thumb.jpg "World Map Clockface")

https://github.com/jnthas/cw-cf-0x03


## Mudar um clockface

Use o PlatformIO para executar o Clockwise. A estrutura do Clockwise é composta por principalmente três pastas
- clockfaces: contém a coleção de clockfaces disponiveis. Essa pasta não é incluida na compilação
- lib: contém o código básico para o Clockwise funcionar e além disso um link simbólico para o clockface atual
- src: contém o ponto de entrada do código do relógio

```
.
├── clockfaces
│   ├── cw-cf-0x01
│   ├── cw-cf-0x02
│   └── cw-cf-0x03
├── lib
│   ├── cw-commons
│   ├── cw-gfx-engine
│   └── timeinwords -> ../clockfaces/cw-cf-0x02/
└── src
    └── main.cpp

```
Para criar o link simbólico execute o comando na raiz do projeto:

`ln -s firmware/clockfaces/cw-cf-0x02/ firmware/lib/timeinwords`

Ou, se preferir, é possivel chegar no mesmo resultado apenas copiando a pasta do clockface desejado para lib/



