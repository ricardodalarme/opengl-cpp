#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "lib/utils.h"
#include "lib/cgImage.h"
using namespace std;

// Tamanho da janela
int win_width = 800;
int win_height = 600;

int wwidth;
int hheight;
int area;
int type_primitive = GL_TRIANGLES;
float *vertices;
int current = 0;

// Variáveis de configuração do OpenGL
int program;
unsigned int VAO;
unsigned int VBO;
/** Vertex shader. */
const char *vertex_code = "\n"
                          "#version 330 core\n"
                          "layout (location = 0) in vec3 position;\n"
                          "layout (location = 1) in vec3 color;\n"
                          "\n"
                          "out vec3 vColor;\n"
                          "\n"
                          "uniform mat4 transform;\n"
                          "\n"
                          "void main()\n"
                          "{\n"
                          "    gl_Position = transform * vec4(position, 1.0);\n"
                          "    vColor = color;\n"
                          "}\0";

/** Fragment shader. */
const char *fragment_code = "\n"
                            "#version 330 core\n"
                            "\n"
                            "in vec3 vColor;\n"
                            "out vec4 FragColor;\n"
                            "\n"
                            "void main()\n"
                            "{\n"
                            "    FragColor = vec4(vColor, 1.0f);\n"
                            "}\0";

// Para controlar rotação, translação e escala
float scale = 1.0;
int rotation = 0;
float translationX = 0.0;
float translationY = 0.0;

// Renderiza os vértices na tela
void display()
{
    glClearColor(0.241, 0.086, 0.206, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(VAO);

    // Translation.
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(translationX, translationY, 0.0f));
    // Rotation around z-axis.
    glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians((float)rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    // Scale.
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));

    // M = T*R*S.
    glm::mat4 M = T * Rz * S;

    // Retrieve location of tranform variable in shader.
    unsigned int loc = glGetUniformLocation(program, "transform");
    // Send matrix to shader.
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(M));

    glDrawArrays(type_primitive, 0, 6 * area);

    glutSwapBuffers();
}

// Ajusta o tamanho da tela caso necessário
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

// Faz a leitura das teclas do teclado
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        exit(0);
    case 'v':
        if (type_primitive == GL_POINTS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            type_primitive = GL_TRIANGLES;
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            type_primitive = GL_POINTS;
        }
        break;
    case 'p':
        rotation += 10;
        rotation = rotation % 360;
        break;
    case 'n':
        rotation -= 10;
        break;
    case 'w':
        translationY += 0.1;
        break;
    case 's':
        translationY -= 0.1;
        break;
    case 'd':
        translationX += 0.1;
        break;
    case 'a':
        translationX -= 0.1;
        break;
    case 'i':
        scale += 0.1;
        break;
    case 'o':
        if (scale > 0.1)
            scale -= 0.1;
        break;
    }

    glutPostRedisplay();
}

// Inicializa o vertex para renderização
void initData(float *vertices)
{
    // Vertex array.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * area * 36, vertices, GL_STATIC_DRAW);

    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);
}

// Cria o programa e inicializa os shaders
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}

// Normaliza o eixo Y
float mapRow2Y(int r, int h)
{
    return (((h - 1.0f - r) / (h - 1.0f)) * 2.0f - 1.0f);
}

// Normaliza o eixo X
float mapColumn2X(int c, int w)
{
    return ((c / (w - 1.0f)) * 2.0f - 1.0f);
}

// Adiciona um vértice ao conjunto
void addVertice(int j, int i, float color)
{
    // Adiciona todas as informações do vertice
    vertices[current++] = mapColumn2X(j, wwidth + 1);
    vertices[current++] = mapRow2Y(i, hheight + 1);
    vertices[current++] = 0.0f;
    for (int h = 0; h < 3; h++)
        vertices[current++] = color;
}

void readImage(char *fileName)
{
    // Lê a imagem
    cgMat2i img = cgReadPGMImage(fileName);
    wwidth = img->width;
    hheight = img->height;
    area = wwidth * hheight;

    // Cria os vértices com base nas informações da imagem
    vertices = (float *)malloc((36 * area) * sizeof(float));
    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++)
        {
            float color = img->val[i][j] / 255.0;

            // Primeiro triângulo
            addVertice(j, i, color);
            addVertice(j, i + 1, color);
            addVertice(j + 1, i, color);

            // Segundo triângulo
            addVertice(j, i + 1, color);
            addVertice(j + 1, i + 1, color);
            addVertice(j + 1, i, color);
        }
}

int main(int argc, char **argv)
{
    // Inicializa o opengGL
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow(argv[0]);
    glewExperimental = GL_TRUE;
    glewInit();

    // Lê a imagem
    readImage(argv[1]);

    // Inicializa o vertex
    initData(vertices);

    // Inicicializa os shaders.
    initShaders();
    glutReshapeFunc(reshape);

    // Desenha a malha triangular
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}
