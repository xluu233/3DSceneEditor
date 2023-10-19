#ifndef WIDGET_H
#define WIDGET_H

#include "camera.h"
#include "model/AssModel.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWidget>


class LoadModelWidget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    LoadModelWidget(QWidget *parent = 0);
    ~LoadModelWidget();
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w,int h) override;
    virtual bool event(QEvent *e) override;
    AssModel* model;
    Camera camera;
    QTimer timer;

    virtual void keyPressEvent(QKeyEvent * event) override;

private:
    QOpenGLShaderProgram shaderProgram;
};

#endif // WIDGET_H
