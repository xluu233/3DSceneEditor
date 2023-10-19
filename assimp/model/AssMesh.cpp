//
// Created by xlu on 2023/10/19.
//

#include "AssMesh.h"

AssMesh::AssMesh(QOpenGLExtraFunctions *glFunc, QOpenGLShaderProgram *shaderProgram, aiMatrix4x4 model) : glFunc(glFunc),
        shaderProgram(shaderProgram),
        VBO(QOpenGLBuffer::VertexBuffer),
        EBO(QOpenGLBuffer::IndexBuffer)
{
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            this->model(i,j)=model[i][j];
        }
    }
}

void AssMesh::Draw() {
    //定义各个纹理类型数量
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for (size_t i = 0; i < textures.size(); ++i) {
        //在绑定之前激活相应的纹理单元
        glFunc->glActiveTexture(GL_TEXTURE0 + i);

        QString texture_type = textures[i]->type;   //纹理类型：比如漫反射纹理，镜面光纹理
        QString texture_num;            //纹理编号（一个网格有多个个漫反射纹理，多个个镜面光纹理）

        if (texture_type == "texture_diffuse")
        {
            texture_num = QString::number(diffuseNr++);
        }
        else if (texture_type == "texture_specular")
        {
            texture_num = QString::number(specularNr++);
        }
        else if (texture_type == "texture_normal")
        {
            texture_num = QString::number(normalNr++);
        }
        else if (texture_type == "texture_height")
        {
            texture_num = QString::number(heightNr++);
        }
        textures[i]->texture.bind();
        shaderProgram->setUniformValue((texture_type+texture_num).toStdString().c_str(),static_cast<GLuint>(i));
    }

    // 绘制网格
    QOpenGLVertexArrayObject::Binder bind(&VAO);
    shaderProgram->setUniformValue("model",model);
    glFunc->glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void AssMesh::setupMesh() {
    VAO.create();
    VAO.bind();

    VBO.create();
    EBO.create();

    VBO.bind();
    VBO.allocate(vertices.data(),vertices.size() * sizeof(AssVertex));

    EBO.bind();
    EBO.allocate(indices.data(),indices.size() * sizeof(GLuint));


    shaderProgram->enableAttributeArray(0);
    shaderProgram->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(AssVertex));

    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(1,GL_FLOAT,offsetof(AssVertex,Normal),3,sizeof(AssVertex));

    shaderProgram->enableAttributeArray(2);
    shaderProgram->setAttributeBuffer(2,GL_FLOAT,offsetof(AssVertex,TexCoords),2,sizeof(AssVertex));

    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(3,GL_FLOAT,offsetof(AssVertex, Tangent),3,sizeof(AssVertex));

    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(4,GL_FLOAT,offsetof(AssVertex, Bitangent),3,sizeof(AssVertex));
    VAO.release();
    //VAO销毁之前，不能销毁VBO和EBO
}
