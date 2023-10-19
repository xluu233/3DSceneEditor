//
// Created by xlu on 2023/10/19.
//
#ifndef OPENGL_TOOLS_ASSMODEL_H
#define OPENGL_TOOLS_ASSMODEL_H


#include "AssMesh.h"
#include "assimp_helper.h"

class AssModel
{
private:
    QOpenGLContext* context;                //opengl函数入口
    QOpenGLShaderProgram* shaderProgram;    //着色器程序

    /*  模型数据  */
    QVector<AssTexture*> textures_loaded;      //纹理
    QVector<AssMesh*> meshes;                  //网格
    QDir directory;                            //模型所在路径

public:
    AssModel(const QString& path,QOpenGLContext* context,QOpenGLShaderProgram* shaderProgram);
    ~AssModel();

    void draw();
    void destroy();

private:
    //递归遍历结点
    void processNode(aiNode *node, const aiScene *scene,aiMatrix4x4 mat4 = aiMatrix4x4());

    //加载网格
    AssMesh* processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 model);

    //加载材质纹理
    QVector<AssTexture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type,QString typeName);

};

#endif //OPENGL_TOOLS_ASSMODEL_H
