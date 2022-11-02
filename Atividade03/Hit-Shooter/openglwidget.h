#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>//Mac OS use _4_1_Core
#include <QTimer>
#include <QVector4D>
#include <vector>
#include <QFile>
#include <random>


#include "gamedata.h"
#include "player.h"
#include "enemy.h"
#include "bullets.h"
#include "power.h"
#include "starlayers.h"

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

protected:
   void initializeGL(); //Chamado uma vez antes das outras funções abaixo.
   void resizeGL(int w, int h);
   void paintGL();
   //bool event(QEvent *e) override;
   void keyPressEvent(QKeyEvent *event);
   void keyReleaseEvent(QKeyEvent *event);
   void update();
   void updateGameStatus();

   //onPaintUI: executado uma vez logo após a inicialização.
   //onPaint: executado a todo frame.

private:
    QTimer timer;    
    bool blinkFlag{false};
    std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};

    //Para jogo 2D.
    GLuint m_objectsProgram{0};  //Identificador do par de shaders objects frag e vert.

    GameData m_gameData;
    Player m_player;
    Enemy m_enemy;

    std::default_random_engine m_randomEngine;

    void restart(); //Para reiniciar o jogo.



public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    bool isDay{true};


    //Criemos apenas um VAO (vertex array object) para armazenar as configurações dos objetos.
    GLuint m_VAO{0};

    //House base.
    GLuint vboHB{0}, vboColorHB{0}, eboHB{0};

    //House roof.
    GLuint vboHF{0}, vboColorHF{0}, eboHF{0};

    //House window.
    GLuint vboHW{0}, vboColorHW{0}, eboHW{0};

    //Vector de cores auxiliar para que possamos setar as cores.
    std::vector<QVector4D> colors;

    std::vector<QVector4D> vertices;
    std::vector<GLuint> indices;

    //Cabeçalho das funções.
    void createVBOs(GLuint &theVAO, GLuint &theVBO, GLuint &theColors, GLuint &theEBO);
    void createShaders();
    void destroyVBOs();
    void destroy();

    //Funções para desenhos
    void setupModels();
    void drawHouseBase();
    void drawHouseRoof();
    void drawHouseWindow(); //A janela tem um gradiente de cor diferente dependendo do horário do dia.
};


#endif // OPENGLWIDGET_H