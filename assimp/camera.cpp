
#include "camera.h"

Camera::Camera(QWidget *widget)
    : widget(widget)
    , yaw(0)
    , pitch(0)
    , sensitivity(0.005f)
    , cameraPos(0.0f,0.0f,5.0f)
    , cameraDirection(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch))
    , cameraRight(QVector3D::crossProduct({0.0f,1.0f,0.0f},cameraDirection))
    , cameraUp(QVector3D::crossProduct(cameraDirection,cameraRight))
    , moveSpeed(0.5f)
    , timeId(0)
{
}

float Camera::getMoveSpeed() const
{
    return moveSpeed;
}

void Camera::setMoveSpeed(float value)
{
    moveSpeed = value;
}

float Camera::getSensitivity() const
{
    return sensitivity;
}

void Camera::setSensitivity(float value)
{
    sensitivity = value;
}

float Camera::getYaw() const
{
    return yaw;
}

void Camera::setYaw(float value)
{
    yaw = value;
}

float Camera::getPitch() const
{
    return pitch;
}

void Camera::setPitch(float value)
{
    pitch = value;
}

QVector3D Camera::getCameraPos() const
{
    return cameraPos;
}

void Camera::setCameraPos(const QVector3D &value)
{
    cameraPos = value;
}

void Camera::handle(QEvent *e)
{
    if(e->type()==QEvent::MouseMove){
        QMouseEvent *event=static_cast<QMouseEvent*>(e);
        float xoffset = event->x() - widget->rect().center().x();
        float yoffset = widget->rect().center().y() - event->y(); // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        yaw   += xoffset;
        pitch += yoffset;
        if(pitch > 1.55)         //将俯视角限制到[-89°,89°]，89°约等于1.55
            pitch =  1.55;
        if(pitch < -1.55)
            pitch = -1.55;
        cameraDirection.setX(cos(yaw)*cos(pitch));
        cameraDirection.setY(sin(pitch));
        cameraDirection.setZ(sin(yaw)*cos(pitch));
        view.setToIdentity();
        view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
        QCursor::setPos(widget->geometry().center());       //将鼠标移动窗口中央
    }
    else if(e->type()==QEvent::Timer){
        float cameraSpeed = moveSpeed * deltaTime;
        if (keys.contains(Qt::Key_W))                           //前
            cameraPos+=cameraSpeed * cameraDirection;
        if (keys.contains(Qt::Key_S))                           //后
            cameraPos -= cameraSpeed * cameraDirection;
        if (keys.contains(Qt::Key_A))                           //左
            cameraPos-=QVector3D::crossProduct(cameraDirection,cameraUp)*cameraSpeed;
        if (keys.contains(Qt::Key_D))                           //右
            cameraPos+=QVector3D::crossProduct(cameraDirection,cameraUp)*cameraSpeed;
        if (keys.contains(Qt::Key_Space))                       //上浮
            cameraPos.setY(cameraPos.y()+cameraSpeed);
        if (keys.contains(Qt::Key_Shift))                       //下沉
            cameraPos.setY(cameraPos.y()-cameraSpeed);

        view.setToIdentity();
        view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
    }
    else if(e->type()==QEvent::KeyPress){
        //isAutoRepeat用于判断此按键的来源是否是长按
        QKeyEvent *event=static_cast<QKeyEvent*>(e);
        keys.insert(event->key());                              //添加按键
        if(!event->isAutoRepeat()&&timeId==0){                  //如果定时器未启动，则启动定时器
            timeId=widget->startTimer(1);
        }
    }
    else if(e->type()==QEvent::KeyRelease){
        QKeyEvent *event=static_cast<QKeyEvent*>(e);
        keys.remove(event->key());
        if(!event->isAutoRepeat()&&timeId!=0&&keys.empty()){    //当没有按键按下且定时器正在运行，才关闭定时器
             widget->killTimer(timeId);
             timeId=0;                                          //重置定时器id
        }
    }
    else if(e->type()==QEvent::UpdateRequest){
        float time=QTime::currentTime().msecsSinceStartOfDay()/1000.0;
        deltaTime = time - lastFrame;                           //在此处更新时间差
        lastFrame = time;
    }
    else if(e->type()==QEvent::FocusIn){
        widget->setCursor(Qt::BlankCursor);             //隐藏鼠标光标
        QCursor::setPos(widget->geometry().center());   //将鼠标移动窗口中央
        widget->setMouseTracking(true);                 //开启鼠标追踪
    }
    else if(e->type()==QEvent::FocusOut){
        widget->setCursor(Qt::ArrowCursor);   //恢复鼠标光标
        widget->setMouseTracking(false);      //关闭鼠标追踪
    }
}

void Camera::init()
{
    view.lookAt(cameraPos,cameraPos+cameraDirection,cameraUp);
    widget->activateWindow();                 //激活窗口
    widget->setFocus();

}

QMatrix4x4 Camera::getView() const
{
    return view;
}
