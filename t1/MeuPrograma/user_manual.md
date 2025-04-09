# BIMP - Manual de Uso

Bem-vindo ao BIMP - o Brizzi Image Manipulation Program. O nome se trata de uma cópia do GIMP, programa análogo do GNU.

## Compilando

### Code::Blocks
Tendo Code::Blocks com mingw, basta abrir o `cbp.cbp` e executar a aplicação.

### Terminal
Tendo mingw32 instalado e as dependências do Canvas2D na pasta superior à do programa, use:

```shell
g++ $args\src\*.cpp -L"./lib" -I"./include" -o ./main.exe -lfreeglut32 -lglfw3 -lglfw3dll -lglu32 -lopengl32
```

## Início

Estamos na tela inicial do BIMP. Aqui você verá as seguintes opções:

- New Project
- Load from file
- Load quick demo

Para continuar seu guia rápido, clique em `Load quick demo`.

## Editor
Agora, você já está no editor do BIMP. 

### Camadas
Se você carregou a demo, estará vendo à sua direita o seletor de camadas. Note que você tem duas camadas, mas pode ter quantas quiser, é só clicar no botão de adicionar camada abaixo. Veja que em cada camada há algumas informações e botões:

- Índice
- Nome
- Miniatura
- Olho (controla a visibilidade)
- X (apaga)
- Setas (controlam a ordem das camadas)

Ademais, uma camada está selecionada quando há um retângulo em sua borda dando destaque. Para selecionar uma camada, basta clicar sob ela no seletor.

Com mais de 3 camadas, use as setas grandes para navegar dentro da lista.

### Seletor de cores
Acima do seu gerenciador de camadas, há dois botões: ali, você seleciona se quer ver as camadas ou seu seletor de cores. Clique no ícone da paleta de cores para abrir o seletor.

Agora, você estará vendo o seletor de cores. Basta mexer os sliders para selecionar a cor desejada. Prefere usar valores RGB? Basta usar os botões de `+`/`-` e inserir o valor desejado com precisão.


### Ferramentas
À sua esquerda, estão as ferramentas. Clique com o botão esquerdo para usar cada uma delas. Clique com o botão direito em algumas ferramentas para ver opções adicionais. Segure o mouse sobre uma ferramenta para ver seu nome. Ferramentas selecionadas ficam com um destaque em sua borda.

Vamos testar? Com uma camada ativa selecionada, clique na ferramenta `Pencil`. Agora, clique na área da imagem e pinte uma região. Que obra de arte! :)

Tente clicar com o botão direito na ferramenta lápis. Um popup apareceu com as opções dessa ferramenta. Selecione agora o modo `Pixel Art (Grid)` e use o slider de tamanho para chegar ao tamanho desejado. Faça um belo desenho!

Muita cor para você? Que tal clicar na ferramenta `Effects` e selecionar `Sepia`, para dar um tom retrô para seu desenho?

Há muitas outras funcionalidades, mas não vamos cobrir todas aqui. De qualquer modo, o BIMP te ajuda a chegar no que sua imaginação deixar. Obrigado por seguir o tutorial!


## Conclusão
Este manual é uma breve introdução, mas não deixe de explorar tudo que o BIMP oferece para sua criatividade. Verifique também o [README](./README.md) caso ainda não tenha lido.


---
BIMP is powered by Canvas2D.