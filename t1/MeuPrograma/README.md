# Trabalho 1 de Computação Gráfica (2025/1)

"Photoshop caseiro" - Guilherme Brizzi


## Manual de Uso

Verifique o [Manual de Uso](./user_manual.md) antes de usar o programa.

## Funcionalidades desenvolvidas

### Básicas
Foram desevolvidas todas as funcionalidades básicas:

- Carregamento de imagens BMP de qualquer tamanho
- Menu de controle da aplicação
- Camadas ordenadas e com transparência
- Plano de fundo com padrão de transparência
- Desenho básico com o mouse
- Troca de cor
- Flip Horizontal e Vertical
- Controle de Brilho

### Extras

### Extras especificados
Foram desevolvidas todas as funcionalidades extras especificadas na descrição do trabalho:

* Efeitos extras
    - Tons de Cinza (até 0.5 pt)
    - Correção de Gama (até 0.5 pt)
    - Constraste (até 0.5 pt)
    - Desfoque Gaussiano (até 2 pt)

* Redimensionamento e movimentação das imagens (até 2 pts)

* Rotação de ângulo qualquer com o mouse (até 2 pts)
* Diferentes tipos de pincel/formas para desenhar com o mouse (até 2 pts). Implementou-se:
    - Lápis, nas formas quadrado e círculo, com controle de tamanho
    - Spray, com controle de tamanho
    - Marca-texto, com controle de tamanho
    - Flood-fill
    - Borracha, nas formas quadrado e círculo, com controle de tamanho

* Salvar e carregar estado atual do programa em arquivo (até 3 pts). Implementou-se:
    - Salvamento do estado do programa em arquivo binário .bimp. 
    - Ao restaurar, retornam a ordem e visibilidade das camadas, suas respectivas imagens, desenhos, posição e rotação, com efeitos e ajustes aplicados.


### Extras não especificados

* Conta-gotas:
    - Torna a cor ativa a cor do pixel clicado 
* Seletor de cor HSV: 
    - Mais que apenas RGB o seletor de cor usa o formato HSV para uma experiência mais intuitiva
* Modo Pixel Art (Grade): 
    - O Modo Pixel Art está disponível no lápis e na borracha e faz com que os desenhos sejam feitos seguindo uma grade, o que permite fazer Pixel Art com facilidade
* Exportação da imagem no formato .bmp:
    - Além de salvar e recuperar o estado do programa, também é possível exportar a imagem editada como um .bmp, com a aplicação de todos desenhos, ajustes, efeitos, etc.
* Ajustes adicionais:
    - Além dos ajustes pedidos, implementou-se também controle de saturação e temperatura
* Efeitos adicionais:
    - Além dos efeitos pedidos, implementou-se também sépia e vívido.
* Canal alpha completo:
    - Mais que apenas transparência binária, o programa tem suporte a canal alpha completo, tanto na edição, quanto na importação/exportação.
