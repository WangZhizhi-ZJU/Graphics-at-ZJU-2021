#include "MeshModel.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <QMessageBox>
using namespace std;

// 模型类构造函数，构造时同时依obj文件地址读入模型数据
MeshModel::MeshModel(const char* objFileName)
{
    meshModelPath = QString(objFileName);
    ifstream objString(objFileName);
    string str;
    Face* faces;
    Float3* position;
    Vector3* normal;
    Float2* texture;
    // 从obj文件中读取出模型信息
    while (getline(objString, str))
    {
        if (str.length() < 2)
            continue;
        if (str[0] == 'v')
        {
            if (str[1] == 't')          // "vt"后记录的是材质
            {
                istringstream in(str);
                texture = new Float2();
                string head;
                in >> head >> texture->u >> texture->v;
                Texture.push_back(*texture);
                textures++;
            }
            else if (str[1] == 'n')     // "vn"后记录的是法向量
            {
                istringstream in(str);
                normal = new Vector3();
                string head;
                in >> head >> normal->x >> normal->y >> normal->z;
                Normal.push_back(*normal);
                normals++;
            }
            else                        // "v"后记录的是位置
            {
                istringstream in(str);
                position = new Float3();
                string head;
                in >> head >> position->x >> position->y >> position->z;
                Position.push_back(*position);
                vertices++;
            }
        }
        else if (str[0] == 'f')
        {
            for (int k = str.size() - 1; k >= 0; k--)
            {
                if (str[k] == '/')
                    str[k] = ' ';
            }
            istringstream in(str);
            faces = new Face();
            string head;
            in >> head;
            int i = 0;
            while (i < 3)
            {
                if (Position.size() != 0)
                {
                    in >> faces->Position[i];
                    faces->Position[i] -= 1;
                }
                if (Texture.size() != 0)
                {
                    in >> faces->Texture[i];
                    faces->Texture[i] -= 1;
                }
                if (Normal.size() != 0)
                {
                    in >> faces->Normal[i];
                    faces->Normal[i] -= 1;
                }
                i++;
            }
            Faces.push_back(*faces);
            faces++;
        }
    }
}

unsigned char* MeshModel::LoadFileContent(const char* path, int& filesize)
{
    unsigned char* fileContent = nullptr;
    filesize = 0;
    FILE* pFile = fopen(path, "rb");
    if (pFile)
    {
        fseek(pFile, 0, SEEK_END);
        int nLen = ftell(pFile);
        if (nLen > 0)
        {
            rewind(pFile);
            fileContent = new unsigned char[nLen + 1];
            fread(fileContent, sizeof(unsigned char), nLen, pFile);
            fileContent[nLen] = '\0';
            filesize = nLen;
        }
        fclose(pFile);
    }
    return fileContent;
}

GLuint MeshModel::createTexture2D(unsigned char* piexlData, int width, int height, GLenum type)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, piexlData);
    return texture;
}

GLuint MeshModel::CreateTexture2DFromBMP(const char* bmpPath)
{
    int nFileSize = 0;
    unsigned char* bmpFileContent = LoadFileContent(bmpPath, nFileSize);
    if (bmpFileContent == nullptr)
    {
        return 0;
    }
    int bmpWidth = 0, bmpHeight = 0;
    unsigned char* pixelData = DecodeBMP(bmpFileContent, bmpWidth, bmpHeight);
    if (bmpWidth == 0)
    {
        delete bmpFileContent;
        return 0;
    }
    GLuint texture = createTexture2D(pixelData, bmpWidth, bmpHeight, GL_RGB);
    delete bmpFileContent;
    return texture;
}

unsigned char* MeshModel::DecodeBMP(unsigned char* bmpFileData, int& width, int& height)
{
    if (0x4D42 == *((unsigned short*)bmpFileData))
    {
        int pixelDataOffset = *((int*)(bmpFileData + 10));
        width = *((int*)(bmpFileData + 18));
        height = *((int*)(bmpFileData + 22));
        unsigned char* pixelData = bmpFileData + pixelDataOffset;
        for (int i = 0; i < width * height * 3; i += 3)
        {
            unsigned char temp = pixelData[i];
            pixelData[i] = pixelData[i + 2];
            pixelData[i + 2] = temp;
        }
        return pixelData;
    }
    return nullptr;
}

bool MeshModel::LoadBMPTexture(const char* bmpPath) {
    try { this->bitmapTexture = CreateTexture2DFromBMP(bmpPath); }
    catch (...) { return false; }
    isTextureLoaded = true;
    return true;
}

// 显示模型方法，naturalMag可以调整模型在GLArea中的固有放大率
void MeshModel::displayMeshModel(float naturalMag, int renderMethod, float red, float blue, float green)
{
    // 更新：带材质图形的绘制
    if (this->Texture.size() != 0 && isTextureLoaded)
    {
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bitmapTexture);
        float ambientLight[] = {32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    }
    // 模型由若干个三角形小面元构成，遍历其中的面元，将每个面元的三个点依次绘制出来
    for (int i = 0; i < this->Faces.size(); i++)
    {
        glBegin(renderMethod);
        // 绘制第一个顶点
        if (this->Texture.size() != 0)
            glTexCoord2f(this->Texture[this->Faces[i].Texture[0]].u, this->Texture[this->Faces[i].Texture[0]].v);  // 加载纹理
        if (this->Normal.size() != 0)
            glNormal3f(this->Normal[this->Faces[i].Normal[0]].x, this->Normal[this->Faces[i].Normal[0]].y, this->Normal[this->Faces[i].Normal[0]].z);  //加载法向量
        glVertex3f(this->Position[this->Faces[i].Position[0]].x / naturalMag, this->Position[this->Faces[i].Position[0]].y / naturalMag, this->Position[this->Faces[i].Position[0]].z / naturalMag);  // 绘画顶点
        // 绘制第二个顶点
        if (this->Texture.size() != 0)
            glTexCoord2f(this->Texture[this->Faces[i].Texture[1]].u, this->Texture[this->Faces[i].Texture[1]].v);
        if (this->Normal.size() != 0)
            glNormal3f(this->Normal[this->Faces[i].Normal[1]].x, this->Normal[this->Faces[i].Normal[1]].y, this->Normal[this->Faces[i].Normal[1]].z);
        glVertex3f(this->Position[this->Faces[i].Position[1]].x / naturalMag, this->Position[this->Faces[i].Position[1]].y / naturalMag, this->Position[this->Faces[i].Position[1]].z / naturalMag);
        // 绘制第三个顶点
        if (this->Texture.size() != 0)
            glTexCoord2f(this->Texture[this->Faces[i].Texture[2]].u, this->Texture[this->Faces[i].Texture[2]].v);
        if (this->Normal.size() != 0)
            glNormal3f(this->Normal[this->Faces[i].Normal[2]].x, this->Normal[this->Faces[i].Normal[2]].y, this->Normal[this->Faces[i].Normal[2]].z);
        glVertex3f(this->Position[this->Faces[i].Position[2]].x / naturalMag, this->Position[this->Faces[i].Position[2]].y / naturalMag, this->Position[this->Faces[i].Position[2]].z / naturalMag);
        glEnd();
    }
    if (this->Texture.size() != 0 && isTextureLoaded)
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        float reAmbientLight[] = {red, blue, green, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, reAmbientLight);
    }
}

void MeshModel::getBoundingBox()
{
    for (int i = 0; i < this->Faces.size(); i++)
    {
        if (xMax < this->Position[this->Faces[i].Position[0]].x)
            xMax = this->Position[this->Faces[i].Position[0]].x;
        if (xMax < this->Position[this->Faces[i].Position[1]].x)
            xMax = this->Position[this->Faces[i].Position[1]].x;
        if (xMax < this->Position[this->Faces[i].Position[2]].x)
            xMax = this->Position[this->Faces[i].Position[2]].x;
        if (yMax < this->Position[this->Faces[i].Position[0]].y)
            yMax = this->Position[this->Faces[i].Position[0]].y;
        if (yMax < this->Position[this->Faces[i].Position[1]].y)
            yMax = this->Position[this->Faces[i].Position[1]].y;
        if (yMax < this->Position[this->Faces[i].Position[2]].y)
            yMax = this->Position[this->Faces[i].Position[2]].y;
        if (zMax < this->Position[this->Faces[i].Position[0]].z)
            zMax = this->Position[this->Faces[i].Position[0]].z;
        if (zMax < this->Position[this->Faces[i].Position[1]].z)
            zMax = this->Position[this->Faces[i].Position[1]].z;
        if (zMax < this->Position[this->Faces[i].Position[2]].z)
            zMax = this->Position[this->Faces[i].Position[2]].z;
        if (xMin > this->Position[this->Faces[i].Position[0]].x)
            xMin = this->Position[this->Faces[i].Position[0]].x;
        if (xMin > this->Position[this->Faces[i].Position[1]].x)
            xMin = this->Position[this->Faces[i].Position[1]].x;
        if (xMin > this->Position[this->Faces[i].Position[2]].x)
            xMin = this->Position[this->Faces[i].Position[2]].x;
        if (yMin > this->Position[this->Faces[i].Position[0]].y)
            yMin = this->Position[this->Faces[i].Position[0]].y;
        if (yMin > this->Position[this->Faces[i].Position[1]].y)
            yMin = this->Position[this->Faces[i].Position[1]].y;
        if (yMin > this->Position[this->Faces[i].Position[2]].y)
            yMin = this->Position[this->Faces[i].Position[2]].y;
        if (zMin > this->Position[this->Faces[i].Position[0]].z)
            zMin = this->Position[this->Faces[i].Position[0]].z;
        if (zMin > this->Position[this->Faces[i].Position[1]].z)
            zMin = this->Position[this->Faces[i].Position[1]].z;
        if (zMin > this->Position[this->Faces[i].Position[2]].z)
            zMin = this->Position[this->Faces[i].Position[2]].z;
    }
}

void MeshModel::displayMeshModel(int FilterMode, float naturalMag, int renderMethod, float red, float blue, float green, int ignore)
{
    xMax = FLT_MIN;
    yMax = FLT_MIN;
    zMax = FLT_MIN;
    xMin = FLT_MAX;
    yMin = FLT_MAX;
    zMin = FLT_MAX;
    if (FilterMode == 0x030)
    {
        glEnable(GL_COLOR_MATERIAL);
        for (int i = 0; i < this->Faces.size(); i++)
        {
            glBegin(renderMethod);
            for (int j = 0; j < 3; j++)
            {
                if (this->Normal.size() != 0)
                    glNormal3f(this->Normal[this->Faces[i].Normal[j]].x, this->Normal[this->Faces[i].Normal[j]].y, this->Normal[this->Faces[i].Normal[j]].z);
                glColor4f(max(1.0f, fabs(this->Position[this->Faces[i].Position[j]].x / xMax)),
                          max(1.0f, fabs(this->Position[this->Faces[i].Position[j]].y / yMax)),
                          max(1.0f, fabs(this->Position[this->Faces[i].Position[j]].z / zMax)), 1.0f);
                glVertex3f(this->Position[this->Faces[i].Position[j]].x / naturalMag, this->Position[this->Faces[i].Position[j]].y / naturalMag, this->Position[this->Faces[i].Position[j]].z / naturalMag);
            }
            glEnd();
        }
    }

    else if (FilterMode == 0x031)
    {
        glEnable(GL_COLOR_MATERIAL);
        for (int i = 0; i < this->Faces.size(); i++)
        {
            glBegin(renderMethod);
            for (int j = 0; j < 3; j++)
            {
                if (this->Normal.size() != 0)
                    glNormal3f(this->Normal[this->Faces[i].Normal[j]].x, this->Normal[this->Faces[i].Normal[j]].y, this->Normal[this->Faces[i].Normal[j]].z);
                glColor4f(max(1.0f, fabs((this->Position[this->Faces[i].Position[j]].x - (xMax + xMin) / 2) / (xMax - xMin) * 2)),
                          max(1.0f, fabs((this->Position[this->Faces[i].Position[j]].y - (yMax + yMin) / 2) / (yMax - yMin) * 2)),
                          max(1.0f, fabs((this->Position[this->Faces[i].Position[j]].z - (zMax + zMin) / 2) / (zMax - zMin) * 2)), 1.0f);
                glVertex3f(this->Position[this->Faces[i].Position[j]].x / naturalMag, this->Position[this->Faces[i].Position[j]].y / naturalMag, this->Position[this->Faces[i].Position[j]].z / naturalMag);
            }
            glEnd();
        }
    }

    else if (FilterMode == 0x032)
    {
        getBoundingBox();
        glEnable(GL_COLOR_MATERIAL);
        for (int i = 0; i < this->Faces.size(); i++)
        {
            glBegin(renderMethod);
            for (int j = 0; j < 3; j++)
            {
                if (this->Normal.size() != 0)
                    glNormal3f(this->Normal[this->Faces[i].Normal[j]].x, this->Normal[this->Faces[i].Normal[j]].y, this->Normal[this->Faces[i].Normal[j]].z);
                glColor4f(fabs((this->Position[this->Faces[i].Position[j]].x - (xMax + xMin) / 2.0f) / (xMax - xMin) * 2.0f),
                          fabs((this->Position[this->Faces[i].Position[j]].y - (yMax + yMin) / 2.0f) / (yMax - yMin) * 2.0f),
                          fabs((this->Position[this->Faces[i].Position[j]].z - (zMax + zMin) / 2.0f) / (zMax - zMin) * 2.0f), 1.0f);
                glVertex3f(this->Position[this->Faces[i].Position[j]].x / naturalMag, this->Position[this->Faces[i].Position[j]].y / naturalMag, this->Position[this->Faces[i].Position[j]].z / naturalMag);
            }
            glEnd();
        }
    }

    else if (FilterMode == 0x033)
    {
        getBoundingBox();
        float ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
        glEnable(GL_COLOR_MATERIAL);
        for (int i = 0; i < this->Faces.size(); i++)
        {
            glBegin(renderMethod);
            for (int j = 0; j < 3; j++)
            {
                if (this->Normal.size() != 0)
                    glNormal3f(this->Normal[this->Faces[i].Normal[j]].x, this->Normal[this->Faces[i].Normal[j]].y, this->Normal[this->Faces[i].Normal[j]].z);
                glColor4f(fabs((this->Position[this->Faces[i].Position[j]].x - (xMax + xMin) / 2.0f) / (xMax - xMin) * 2.0f),
                          fabs((this->Position[this->Faces[i].Position[j]].y - (yMax + yMin) / 2.0f) / (yMax - yMin) * 2.0f),
                          fabs((this->Position[this->Faces[i].Position[j]].z - (zMax + zMin) / 2.0f) / (zMax - zMin) * 2.0f), 1.0f);
                glVertex3f(this->Position[this->Faces[i].Position[j]].x / naturalMag, this->Position[this->Faces[i].Position[j]].y / naturalMag, this->Position[this->Faces[i].Position[j]].z / naturalMag);
            }
            glEnd();
        }
        float reAmbientLight[] = {red, blue, green, 1.0f};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, reAmbientLight);
    }
}

// 导出三维模型
void MeshModel::exportMesh(const char* objFileName)
{
    ofstream out(objFileName, ios::ate);
    out << fixed << setprecision(6);
    for (int i = 0; i < this->Position.size(); i++)
        out << "v " << this->Position[i].x << " " << this->Position[i].y << " " << this->Position[i].z << endl;
    for (int i = 0; i < this->Normal.size(); i++)
        out << "vn " << this->Normal[i].x << " " << this->Normal[i].y << " " << this->Normal[i].z << endl;
    for (int i = 0; i < this->Texture.size(); i++)
        out << "vt " << this->Texture[i].u << " " << this->Texture[i].v << endl;
    for (int i = 0; i < this->Faces.size(); i++)
    {
        out << "f";
        for (int j = 0; j < 3; j++)
        {
            out << " " << this->Faces[i].Position[j] + 1;
            if (this->Texture.size() != 0)
                out << "//" << this->Faces[i].Texture[j] + 1;
            if (this->Normal.size() != 0)
                out << "//" << this->Faces[i].Normal[j] + 1;
        }
        out << endl;
    }
}

// 重心保留法稀疏模型
void MeshModel::getSparseModel()
{
    vector<Float3> newPositions;
    vector<Vector3> newNormals;
    vector<Face> newFaces;
    int nVertices = 0;
    int nNorms = 0;
    int nFaces = 0;
    sort(Faces.begin(), Faces.end(), [&](Face x, Face y)
         { return x.Position[0] < y.Position[0]; });
    sort(Faces.begin(), Faces.end(), [&](Face x, Face y)
         { return x.Position[1] < y.Position[1]; });
    sort(Faces.begin(), Faces.end(), [&](Face x, Face y)
         { return x.Position[2] < y.Position[2]; });
    for (int i = 0; i < this->Faces.size(); i++) 
    {
        Float3 residuePoint;
        residuePoint.x = (this->Position[this->Faces[i].Position[0]].x + this->Position[this->Faces[i].Position[1]].x + this->Position[this->Faces[i].Position[2]].x) / 3;
        residuePoint.y = (this->Position[this->Faces[i].Position[0]].y + this->Position[this->Faces[i].Position[1]].y + this->Position[this->Faces[i].Position[2]].y) / 3;
        residuePoint.z = (this->Position[this->Faces[i].Position[0]].z + this->Position[this->Faces[i].Position[1]].z + this->Position[this->Faces[i].Position[2]].z) / 3;
        nVertices++;
        Vector3 residueNorm;
        if (this->Normal.size() != 0)
        {
            residueNorm.x = (this->Normal[this->Faces[i].Normal[0]].x + this->Normal[this->Faces[i].Normal[1]].x + this->Normal[this->Faces[i].Normal[2]].x) / 3;
            residueNorm.y = (this->Normal[this->Faces[i].Normal[0]].y + this->Normal[this->Faces[i].Normal[1]].y + this->Normal[this->Faces[i].Normal[2]].y) / 3;
            residueNorm.z = (this->Normal[this->Faces[i].Normal[0]].z + this->Normal[this->Faces[i].Normal[1]].z + this->Normal[this->Faces[i].Normal[2]].z) / 3;
            nNorms++;
        }
        newPositions.push_back(residuePoint);
        if (this->Normal.size() != 0)
            newNormals.push_back(residueNorm);
    }

    for (int i = 0; i + 2 < this->Faces.size(); i += 3)
    {
        if (this->Normal.size() != 0)
            newFaces.push_back({{i, i + 1, i + 2}, {0, 0, 0}, {i, i + 1, i + 2}});
        else
            newFaces.push_back({{i, i + 1, i + 2}, {0, 0, 0}, {0, 0, 0}});
        nFaces++;
    }
    faces = nFaces;
    normals = 3 * nFaces;
    vertices = 3 * nFaces;
    Faces = newFaces;
    Normal = newNormals;
    Position = newPositions;
    Texture.clear();
}

// 中点插入法加密模型
void MeshModel::getDenseModel()
{
    vector<Float3> appendPositions;
    vector<Vector3> appendNormals;
    vector<Face> appendFaces;
    for (int i = 0; i < this->Faces.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Float3 insertionPoint;
            insertionPoint.x = (this->Position[this->Faces[i].Position[j]].x + this->Position[this->Faces[i].Position[(j + 1) % 3]].x) / 2;
            insertionPoint.y = (this->Position[this->Faces[i].Position[j]].y + this->Position[this->Faces[i].Position[(j + 1) % 3]].y) / 2;
            insertionPoint.z = (this->Position[this->Faces[i].Position[j]].z + this->Position[this->Faces[i].Position[(j + 1) % 3]].z) / 2;
            Vector3 insertionNorm;
            if (this->Normal.size() != 0)
            {
                insertionNorm.x = (this->Normal[this->Faces[i].Normal[j]].x + this->Normal[this->Faces[i].Normal[(j + 1) % 3]].x) / 2;
                insertionNorm.y = (this->Normal[this->Faces[i].Normal[j]].y + this->Normal[this->Faces[i].Normal[(j + 1) % 3]].y) / 2;
                insertionNorm.z = (this->Normal[this->Faces[i].Normal[j]].z + this->Normal[this->Faces[i].Normal[(j + 1) % 3]].z) / 2;
            }
            appendPositions.push_back(insertionPoint);
            if (this->Normal.size() != 0)
                appendNormals.push_back(insertionNorm);
        }
        if (this->Normal.size() != 0)
            appendFaces.push_back({{vertices++, vertices++, vertices++}, {0, 0, 0}, {normals++, normals++, normals++}});
        else
            appendFaces.push_back({{vertices++, vertices++, vertices++}, {0, 0, 0}, {0, 0, 0}});
        faces++;
    }
    for (int i = 0; i < appendPositions.size(); i++)
        Position.push_back(appendPositions[i]);
    if (this->Normal.size() != 0)
        for (int i = 0; i < appendNormals.size(); i++)
            Normal.push_back(appendNormals[i]);
    for (int i = 0; i < appendFaces.size(); i++)
        Faces.push_back(appendFaces[i]);
}