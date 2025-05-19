# Trabalho 3 de Computação Gráfica
Guilherme Brizzi

## Requisitos de Hardware
O jogo utiliza muitas imagens renderizadas a partir da API Canvas2D, que não é otimizada para tal. Dessa forma, em hardwares menos poderosos, o programa pode sofrer perda de FPS.
Nos hardwares testados (Ryzen 7 7700X, 4070 Super / Ryzen 7 6800HS, 3050), não ocorreu esse problema. 

Para mitigar essas complicações, o CBP foi configurado para compilar com flags de otimização.

## Funcionalidades básicas
Todas as funcionalidades básicas pedidas foram implementadas.

## Extras implementados

- Alvos com diferentes lógicas, atiram de volta, se movimentam etc
- Alvos com diferentes visuais e colisão apropriada
    * Implementou-se colisão baseada em pixel, permitindo detecção de colisões entre quaisquer formas
- Power-ups
    * Na forma de um powerup de vida para o tanque
- Efeitos de explosão bonitos
    * Utilizou-se várias imagens em sequência e mudando de escala para criar o efeito de explosão.
- Preenchimento colorido da pista com detalhes
    * A pista foi preenchida com textura costumizada de asfalto e foram adicionadas linhas ao meio.
- Diferentes níveis de dificuldade ao explodir todos os alvos.
    * Há 5 níveis de dificuldade, correspondentes às "estrelas de procurado".