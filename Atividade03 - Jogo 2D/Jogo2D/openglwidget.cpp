#include "openglwidget.h"
#include <stdlib.h>
#include <QApplication>
#include <QKeyEvent>

//Construtor da classe OpenGLWidget.
OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
}

//Destrutor da classe OpenGLWidget.
OpenGLWidget::~OpenGLWidget()
{

    destroyVBOs();
    destroyShaders();

}

//Função chamada uma vez, na inicialização do programa. Correspondente ao onCreate().
void OpenGLWidget::initializeGL()
{
   initializeOpenGLFunctions();

   //Inicializa a cor de fundo do widget do openGL. A estrutura segue o formato float (red, green, blue, alpha).
   glClearColor(0,0.8,0.9,1);

   createShaders();
   //createVBOs();
}

void OpenGLWidget::resizeGL(int w, int h)
{
}

void OpenGLWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT);

    //Sinaliza a possibilidade do uso dos shaders.
    glUseProgram(shaderProgram);

    drawHouseBase();
    glBindVertexArray(vaoHB);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    drawHouseRoof();
    glBindVertexArray(vaoHF);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    drawHouseWindow();
    glBindVertexArray(vaoHW);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //glDrawArrays(GL_TRIANGLE_FAN, 0, steps);


}

//Criação dos shaders, usando os resources vertex shaders e fragment shaders.
void OpenGLWidget::createShaders()
{
    makeCurrent();
    destroyShaders();

    //Lê vertex shader e fragment shaders.
    QFile vs(":/shaders/vshader1.glsl");
    QFile fs(":/shaders/fshader1.glsl");

    //Apresenta errors caso os arquivos não sejam encontrados.
    if(!vs.open(QFile::ReadOnly | QFile::Text)) return;
    if(!fs.open(QFile::ReadOnly | QFile::Text)) return;


    auto byteArrayVs{vs.readAll()};
    auto byteArrayFs{fs.readAll()};
    const char *c_strVs{byteArrayVs};
    const char *c_strFs{byteArrayFs};

    //Fecha os arquivos abertos anteriormente.
    vs.close();
    fs.close();

    //Cria vertex shader a partir dos dados lidos do resource vertex shader.
    GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(vertexShader, 1, &c_strVs,0);
    glCompileShader(vertexShader);
    GLint isCompiled{0};

    glGetShaderiv(vertexShader,GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE)
    {
        GLint maxLength{0};
        glGetShaderiv(vertexShader,GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(vertexShader);
        return;
    }

    //Cria fragment shader a partir dos dados lidos do resource fragment shader.
    GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &c_strFs, 0);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);

    if(isCompiled == GL_FALSE)
    {
        GLint maxLength{0};
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    //Cria shader program, inteiro que identifica o programa de shader composto pelo par de vertex/fragment shader.
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint isLinked{0};
    glGetProgramiv(shaderProgram,GL_LINK_STATUS, &isLinked);

    if(isLinked == GL_FALSE)
    {
        GLint maxLength{0};
        glGetProgramiv(shaderProgram,GL_INFO_LOG_LENGTH,&maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram,maxLength,&maxLength, &infoLog[0]);
        qDebug("%s",&infoLog[0]);
        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }


    glDetachShader(shaderProgram,vertexShader);
    glDetachShader(shaderProgram,fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

//Destrói os shaders.
void OpenGLWidget::destroyShaders()
{
    makeCurrent();
    glDeleteProgram(shaderProgram);
}

//Cria VBOs
void OpenGLWidget::createVBOs(GLuint &theVAO, GLuint &theVBO, GLuint &theColors, GLuint &theEBO)
{
    makeCurrent();

    //Libera os VBOs atuais, caso já tenham sido criados. Importante para não consumir toda memória da GPU.
    destroyVBOs();

    //Cria VAO
    glGenVertexArrays(1, &theVAO);
    //Associa atributos vertex ao VAO atual
    glBindVertexArray(theVAO);

    //Gera VBO de posições (vértices)
    glGenBuffers(1, &theVBO);
    glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(QVector4D), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    //Gera VBO de cores
    glGenBuffers (1, &theColors);
    glBindBuffer (GL_ARRAY_BUFFER, theColors);
    glBufferData (GL_ARRAY_BUFFER, colors.size()*sizeof(QVector4D), colors.data(), GL_STATIC_DRAW);

    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray (1);

    glGenBuffers (1, &theEBO);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, theEBO);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

//Destrói VBOs
void OpenGLWidget::destroyVBOs()
{
    makeCurrent ();

    //Deleta buffers.
    glDeleteBuffers(1, &vboHB);
    glDeleteBuffers(1, &vboColorHB);
    glDeleteBuffers(1, &eboHB);
    //Deleta vértices.
    glDeleteVertexArrays(1, &vaoHB);

    //Deleta buffers.
    glDeleteBuffers(1, &vboHF);
    glDeleteBuffers(1, &vboColorHF);
    glDeleteBuffers(1, &eboHF);
    //Deleta vértices.
    glDeleteVertexArrays(1, &vaoHF);

    //Deleta buffers.
    glDeleteBuffers(1, &vboHW);
    glDeleteBuffers(1, &vboColorHW);
    glDeleteBuffers(1, &eboHW);
    //Deleta vértices.
    glDeleteVertexArrays(1, &vaoHW);
}

//Altera alguns elementos da cena, como céu sol/lua e a janela da casa.
void OpenGLWidget::toggleDarkMode(bool changeToDarkMode)
{
    makeCurrent();
    isDay = !changeToDarkMode;


    if(changeToDarkMode)
    {
        //Céu escuro. Noite.
        glClearColor(0,0.1,0.3,1);
    }
    else
    {
        //Céu claro. Dia.
        glClearColor(0,0.8,0.9,1);
    }
    update();
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
            QApplication::quit();
            break;
    }
}

//Desenha a base da casa.
void OpenGLWidget::drawHouseBase()
{
    //reconfigura o array de vértices, colors e índices.
    vertices.resize(4);
    colors.resize(4);
    indices.resize(6); //2*3 -> dois triângulos.

    //Define a posição dos vértices, inserindo-as no array.
    vertices[0] = QVector4D(0, -0.9, 0, 1);
    vertices[1] = QVector4D(-0.9, -0.9, 0, 1);
    vertices[2] = QVector4D(-0.9, 0, 0, 1);
    vertices[3] = QVector4D(0, 0, 0, 1);


    //Cria as cores para os vértices.
    colors[0] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[1] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[2] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[3] = QVector4D(0.5, 0.5, 0.5, 1);

    //Gera a topologia do retângulo.
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 2; indices[4] = 3; indices[5] = 0;

    createVBOs(vaoHB, vboHB, vboColorHB, eboHB);

}

//Desenha o telhado da casa.
void OpenGLWidget::drawHouseRoof()
{
    //reconfigura o array de vértices, colors e índices.
    vertices.resize(3);
    colors.resize(3);
    indices.resize(3); //1*3 -> um triângulo.

    //Define a posição dos vértices, inserindo-as no array.
    vertices[0] = QVector4D(-0.9, 0.01, 0, 1);
    vertices[1] = QVector4D(0, 0.01, 0, 1);
    vertices[2] = QVector4D(-0.425, 0.5, 0, 1);


    //Cria as cores para os vértices.
    colors[0] = QVector4D(1, 0.5, 0.5, 1);
    colors[1] = QVector4D(1, 0.5, 0.5, 1);
    colors[2] = QVector4D(1, 0.5, 0.5, 1);

    //Gera a topologia do retângulo.
    indices[0] = 0; indices[1] = 1; indices[2] = 2;

    createVBOs(vaoHF, vboHF, vboColorHF, eboHF);

}

void OpenGLWidget::drawSunMoon()
{

    //reconfigura o array de vértices, colors e índices.
    vertices.resize(4);
    colors.resize(4);
    indices.resize(6); //3*3 -> três triângulos.

    //Define a posição dos vértices, inserindo-as no array.
    vertices[0] = QVector4D(0, -0.9, 0, 1);
    vertices[1] = QVector4D(0.9, -0.9, 0, 1);
    vertices[2] = QVector4D(0.9, 0, 0, 1);
    vertices[3] = QVector4D(0, 0, 0, 1);


    //Cria as cores para os vértices.
    colors[0] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[1] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[2] = QVector4D(0.5, 0.5, 0.5, 1);
    colors[3] = QVector4D(0.5, 0.5, 0.5, 1);

    //Gera a topologia do retângulo.
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 2; indices[4] = 3; indices[5] = 0;

    createVBOs(vao2, vboVertices2, vboColors2, eboIndices2);
}

void OpenGLWidget::drawHouseWindow()
{
    //reconfigura o array de vértices, colors e índices.
    vertices.resize(4);
    colors.resize(4);
    indices.resize(6); //2*3 -> dois triângulos.

    //Define a posição dos vértices, inserindo-as no array.
    vertices[0] = QVector4D(-0.1, -0.5, 0, 1);
    vertices[1] = QVector4D(-0.3, -0.5, 0, 1);
    vertices[2] = QVector4D(-0.3, -0.2, 0, 1);
    vertices[3] = QVector4D(-0.1, -0.2, 0, 1);


    if (isDay) {
        //Cria as cores para os vértices.
        colors[0] = QVector4D(1, 1, 1, 1);
        colors[1] = QVector4D(1, 1, 1, 1);
        colors[2] = QVector4D(1, 1, 1, 1);
        colors[3] = QVector4D(1, 1, 1, 1);
    } else {
        //Cria as cores para os vértices.
        colors[0] = QVector4D(1, 1, 0, 1);
        colors[1] = QVector4D(1, 1, 0, 1);
        colors[2] = QVector4D(1, 1, 0, 1);
        colors[3] = QVector4D(1, 1, 0, 1);
    }


    //Gera a topologia do retângulo.
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 2; indices[4] = 3; indices[5] = 0;

    createVBOs(vaoHW, vboHW, vboColorHW, eboHW);

}
