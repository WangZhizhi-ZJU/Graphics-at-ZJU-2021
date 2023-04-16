#ifndef MESHMODEL_H
#define MESHMODEL_H

#include <vector>
#include <QString>
#include <GL/GL.h>
#include <GL/GLU.h>

// 空间坐标
struct Float3 {
    double x;
    double y;
    double z;
};

// 平面坐标
struct Float2 {
    double u;
    double v;
};

// 空间向量
struct Vector3 {
    double x;
    double y;
    double z;
};
struct Face {
    int Position[3];
    int Texture[3];
    int Normal[3];
};

// 模型类
class MeshModel {
public:
    std::vector<Float3> Position;   // 记录顶点
    std::vector<Float2> Texture;    // 记录材质
    std::vector<Vector3> Normal;    // 记录法向量
    std::vector<Face> Faces;        // 记录面：包含值为面的顶点、纹理、法向量的索引

    float xMax = FLT_MIN;
    float yMax = FLT_MIN;
    float zMax = FLT_MIN;
    float xMin = FLT_MAX;
    float yMin = FLT_MAX;
    float zMin = FLT_MAX;

    int vertices = 0;
    int faces = 0;
    int normals = 0;
    int textures = 0;
    QString meshModelPath;          // 模型的地址，用于重新载入模型

    MeshModel() {};
    MeshModel(const char* objFileName);
    ~MeshModel()
    {
        Position.clear();
        Texture.clear();
        Normal.clear();
        Faces.clear();
        Position.shrink_to_fit();
        Texture.shrink_to_fit();
        Normal.shrink_to_fit();
        Faces.shrink_to_fit();
    };
    void displayMeshModel(float naturalMag, int renderMethod, float red, float blue, float green);
    void displayMeshModel(int colorful, float natualMag, int renderMethod, float red, float green, float blue, int ignore);
    void getBoundingBox();
    void exportMesh(const char* objFileName);
    void getSparseModel();
    void getDenseModel();

    GLuint bitmapTexture;
    bool isTextureLoaded = false;
    GLuint CreateTexture2DFromBMP(const char* bmpPath);
    unsigned char* LoadFileContent(const char* path, int& filesize);
    unsigned char* DecodeBMP(unsigned char* bmpFileData, int& width, int& height);
    GLuint createTexture2D(unsigned char* piexlData, int width, int height, GLenum type);
    bool LoadBMPTexture(const char* bmpPath);
};

#endif