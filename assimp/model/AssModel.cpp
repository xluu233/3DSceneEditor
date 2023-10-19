//
// Created by xlu on 2023/10/19.
//

#include "AssModel.h"

AssModel::AssModel(const QString &path, QOpenGLContext *context, QOpenGLShaderProgram *shaderProgram)
        : context(context), shaderProgram(shaderProgram), directory(path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(directory.absolutePath().toLocal8Bit(),
                                           aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        qDebug() << "ERROR::ASSIMP::" <<import.GetErrorString();
        return;
    }
    qDebug() << directory.absolutePath() << "加载成功";
    qDebug() << "网格：" << scene->mNumMeshes;
    qDebug() << "材质：" << scene->mNumMaterials;
    qDebug() << "纹理：" << scene->mNumTextures;
    qDebug() << "动画：" << scene->mNumAnimations;
    directory.cdUp();
    processNode(scene->mRootNode, scene);
}

AssModel::~AssModel() {
    //销毁对象
    for (auto &it: textures_loaded) {
        it->texture.destroy();
        delete it;
    }
    for (auto &it: meshes) {
        delete it;
    }
}

void AssModel::draw() {
    shaderProgram->bind();
    for (auto mesh: meshes) {
        mesh->Draw();
    }
}

void AssModel::destroy() {
    delete this;
    context->doneCurrent();
}

void AssModel::processNode(aiNode *node, const aiScene *scene, aiMatrix4x4 mat4) {
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, mat4));
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, mat4 * node->mChildren[i]->mTransformation);
    }
}

AssMesh *AssModel::processMesh(aiMesh *mesh, const aiScene *scene, aiMatrix4x4 model) {
    //  初始化网格
    AssMesh *pAssMesh = new AssMesh(context->extraFunctions(), shaderProgram, model);
    // 遍历网格的每个顶点
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        AssVertex vertex;
        QVector3D vector; //将assimp的数据转化为QtOpenGL支持的数据

        // 位置
        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;
        // 法向量
        if (mesh->mNormals) {
            vector.setX(mesh->mNormals[i].x);
            vector.setY(mesh->mNormals[i].y);
            vector.setZ(mesh->mNormals[i].z);
            vertex.Normal = vector;
        }
        // 纹理坐标
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            QVector2D vec;
            //一个顶点最多可以包含8个不同的纹理坐标。因此我们假设我们不用
            //使用一个顶点可以有多个纹理坐标的模型，所以我们总是取第一个集合(0)。
            vec.setX(mesh->mTextureCoords[0][i].x);
            vec.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = QVector2D(0, 0);
        }
        if (mesh->mTangents) {
            // tangent
            vector.setX(mesh->mTangents[i].x);
            vector.setY(mesh->mTangents[i].y);
            vector.setZ(mesh->mTangents[i].z);
            vertex.Tangent = vector;
        }
        if (mesh->mBitangents) {
            vector.setX(mesh->mBitangents[i].x);
            vector.setY(mesh->mBitangents[i].y);
            vector.setZ(mesh->mBitangents[i].z);
            vertex.Bitangent = vector;
        }
        // bitangent
        pAssMesh->vertices.push_back(vertex);
    }

    //Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // 将所有面的索引数据添加到索引数组中
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            pAssMesh->indices.push_back(face.mIndices[j]);
        }
    }

    // 处理材质
    if (mesh->mMaterialIndex > 0) {
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. 漫反射贴图
        QVector<AssTexture *> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        for (auto &it: diffuseMaps)
            pAssMesh->textures.push_back(it);

        // 2. 镜面贴图
        QVector<AssTexture *> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        for (auto &it: specularMaps)
            pAssMesh->textures.push_back(it);

        // 3. 法向量图
        QVector<AssTexture *> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        for (auto &it: normalMaps)
            pAssMesh->textures.push_back(it);

        // 4. 高度图
        QVector<AssTexture *> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        for (auto &it: heightMaps)
            pAssMesh->textures.push_back(it);
    }


    pAssMesh->setupMesh();
    return pAssMesh;
}

QVector<AssTexture *> AssModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName) {
    QVector<AssTexture *> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // 检查纹理是否在之前加载过，如果是，则继续到下一个迭代:跳过加载新纹理
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j]->path.toStdString().c_str(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true; //【优化】 带有相同filepath的纹理已经加载，继续到下一个
                break;
            }
        }
        if (!skip) {
            // 如果材质还没有加载，加载它
            AssTexture *texture = new AssTexture;
            QImage data(directory.filePath(str.C_Str()));
            if (!data.isNull()) {
                texture->texture.setData(data);
                texture->type = typeName;
                texture->path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(
                        texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            } else {
                qDebug() << "未能成功加载纹理：" << directory.filePath(str.C_Str());
            }
        }
    }
    return textures;
}

