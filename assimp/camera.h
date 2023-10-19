#ifndef CAMERA_H
#define CAMERA_H

#include <QSet>
#include <QVector3D>
#include <QEvent>
#include <QWidget>
#include <QtMath>
#include <QMatrix4x4>
#include <QKeyEvent>
#include <QTime>


class Camera
{
public:
    Camera(QWidget *widget);

    float getMoveSpeed() const;
    void setMoveSpeed(float value);

    float getSensitivity() const;
    void setSensitivity(float value);

    float getYaw() const;
    void setYaw(float value);

    float getPitch() const;
    void setPitch(float value);

    QVector3D getCameraPos() const;
    void setCameraPos(const QVector3D &value);

    void init();                    //初始化摄像机

    void handle(QEvent *event);     //处理窗口事件

    QMatrix4x4 getView() const;     //获取观察矩阵

private:
    QWidget *widget;

    float yaw;                  //偏航角
    float pitch;                //俯视角
    float sensitivity;          //鼠标灵敏度

    QVector3D cameraPos;        //摄像机初始位置

    QVector3D cameraDirection;  //摄像机方向
    QVector3D cameraRight;      //摄像机右向量
    QVector3D cameraUp;         //摄像机上向量

    float moveSpeed;    //控制移动速度
    QSet<int> keys;     //记录当前被按下按键的集合

    int timeId;         //定时器id：此定时器用于完成键盘移动事件
    float deltaTime;    // 当前帧与上一帧的时间差
    float lastFrame;    // 上一帧的时间

    QMatrix4x4 view;    //观察矩阵

};

#endif // CAMERA_H
