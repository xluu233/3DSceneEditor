//
// Created by xlu on 2023/10/19.
//

#ifndef OPENGL_TOOLS_ASSMESH_H
#define OPENGL_TOOLS_ASSMESH_H


#include "assimp_helper.h"

struct AssVertex
{
    QVector3D Position;     //顶点数据
    QVector3D Normal;       //法线数据
    QVector2D TexCoords;    //纹理坐标
    QVector3D Tangent;      //切线
    QVector3D Bitangent;    //位切线
};
//static const int VertexFloatCount = 20;

struct AssTexture {
    QOpenGLTexture texture;
    QString type;
    QString path;

    AssTexture() : texture(QOpenGLTexture::Target2D) {
        texture.create();
        texture.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
        texture.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
        texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
    }
};


class AssMesh {
public:
    /*  网格数据  */
    QVector<AssVertex> vertices;                //顶点数据
    QVector<AssTexture*> textures;              //纹理数据
    QVector<GLuint> indices;                    //索引数组
    QMatrix4x4 model;                           //模型矩阵
    QOpenGLFunctions *glFunc;                   //opengl函数入口
    QOpenGLShaderProgram *shaderProgram;        //着色器程序

    /*  函数  */
    AssMesh(QOpenGLExtraFunctions* glFunc,QOpenGLShaderProgram* shaderProgram,aiMatrix4x4 model);
    void Draw();
    void setupMesh();

private:
    /*  渲染数据  */
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO,EBO;
};



#endif //OPENGL_TOOLS_ASSMESH_H
