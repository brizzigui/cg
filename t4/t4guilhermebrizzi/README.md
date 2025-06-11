# Trabalho 4 de Computação Gráfica
- Guilherme Brizzi
- Modelagem 3D com curvas de Bezier

## Funcionalidades implementadas

### Básicas

Todas as funcionalidades básicas foram implementadas.

### Extras

- Vetores normais
- Mais de 4 pontos de controle na curva (permite N pontos)
- Preenchimento de polígonos

## Guia de Uso

Toda interação com o programa acontece pelo mouse. Não há interação pelo teclado.

À esquerda, fica o editor de curva. À direita, o visualizador 3D.

### Editor

No editor, você constroi sua curva de Bezier.

Existem os seguintes botões, de cima para baixo:

- Mover: ativa o modo de mover pontos. Basta arrastar um ponto de controle para movê-lo.
- Adicionar: ativa o modo de adicionar pontos. Basta clicar para adicionar um ponto de controle.
- Remover: ativa o modo de remover pontos. Basta clicar em um ponto de controle para removê-lo.

- Aumentar segmentos de curva: aumenta a amostragem de segmentos da curva.
- Reduzir segmentos de curva: diminui a amostragem de segmentos da curva.

### Visualizador

No visualizador, você vê o resultado de sua modelagem.
Use o mouse para rotacionar a figura, de forma fácil e natural.
Use o scroll do mouse para dar zoom.

- Wireframe: Muda para o modo de visualização wireframe.
- Preenchimento: Muda para o modo de visualização preenchimento.
- Adicionar fatias de rotação: aumenta a amostragem de fatias de rotação.
- Reduzir fatias de rotação: diminui a amostragem de fatias de rotação.
- Aumento de y: ativa o modo de aumento de y. Essa função permite fazer sweep rotacional e translacional.
- Perspectiva: Ativa/desativa a visão em perspectiva. Estando desativada, você verá uma visão ortográfica. Ativada, em perspectiva.
- Ver normais: Ativa/desativa a visão dos vetores normais de cada face.