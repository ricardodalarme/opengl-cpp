## Descrição

Esse é um trabalho desenvolvido para a disciplina *CIC270 - Computação Gráfica*. O objetivo é desenvolver um programa que realiza a leitura de uma imagem no formato *PGM* e, através da utilização da biblioteca *OpenGL*, implementar a imagem como malha de triângulos ou como uma nuvem de pontos, bem como funcionalidades para aplicar transformações geométricas ao modelo bidimensional.

## Malha Triangular e Nuvem de Pontos

Um modelo bidimensional é obtido a partir da leitura de uma imagem. Este modelo pode ser visualizado como malha triangular planar, como na seguinte figura à esquerda, que possui pontos relacionados com os pixels da imagem e arestas conectando pontos adjacentes (lado a lado). Também pode ser mostrado os pontos relacionados aos pixels da imagem como uma nuvem de pontos (figura abaixo à direita)

<p align="center">
 <img src="https://i.ibb.co/kyq6SSN/Untitled.png">
</p>

## Comandos
### Visualização
- **v:** alterna entre as visualizações de malha triangular e nuvem de ponto.

### Translação
- **w:** deslocamento positivo em y;
- **s:** deslocamento negativo em y;
- **d:** deslocamento positivo em x;
- **a:** deslocamento negativo em x.

### Rotação
- **p:** rotação positiva (sentido anti-horário);
- **n:** rotação negativa (sentido horário).

### Escala
- **i:** fator de escala maior que 1 (aumenta o objeto em ambas as direções x e y);
- **o:** fator de escala menor que 1 (diminui o objeto em ambas as direções x e y).

