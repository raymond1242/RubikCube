#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

float Colors[7][4] = {
    { 1.0f, 0.0f, 0.0f, 1.0f },//0 red
    { 0.4f, 0.0f, 0.8f, 0.7f },//1 purple
    { 1.0f, 1.0f, 0.0f, 1.0f },//2 yellow
    { 0.0f, 1.0f, 0.0f, 1.0f },//3 green
    { 1.0f, 1.0f, 1.0f, 1.0f },//4 white
    { 0.0f, 0.0f, 1.0f, 1.0f },//5 blue
    { 0.1f, 0.1f, 0.1f, 1.0f }//6 black
};

enum { FRONT, RIGHT, BACK, LEFT, DOWN, UP };

bool PaintedFaces[27][6] = 
{
    {0,0,1,1,1,0},{0,0,1,1,0,0},{0,0,1,1,0,1},{0,0,1,0,1,0},{0,0,1,0,0,0},{0,0,1,0,0,1},{0,1,1,0,1,0},{0,1,1,0,0,0},{0,1,1,0,0,1},
    {0,0,0,1,1,0},{0,0,0,1,0,0},{0,0,0,1,0,1},{0,0,0,0,1,0},{0,0,0,0,0,0},{0,0,0,0,0,1},{0,1,0,0,1,0},{0,1,0,0,0,0},{0,1,0,0,0,1},
    {1,0,0,1,1,0},{1,0,0,1,0,0},{1,0,0,1,0,1},{1,0,0,0,1,0},{1,0,0,0,0,0},{1,0,0,0,0,1},{1,1,0,0,1,0},{1,1,0,0,0,0},{1,1,0,0,0,1}
};

float r[3][3] = { {0.15f, 0.2f, 0.25f}, {0.3f, 0.2f, 0.1f}, {0.35f, 0.2f, 0.05f} };

float speed = 0.5f;

struct Texture {
    int width, height, BitsPerPixel;
    unsigned int renderedID;
    string FilePath;
    unsigned char* LocalBuffer;
    float imgPos[6][4];
    Texture() : FilePath(""), renderedID(0), LocalBuffer(nullptr), width(0), height(0), BitsPerPixel(0), 
                imgPos{{0.008f, 0.15f, 0.15f, 0.9f}, {0.18f, 0.15f, 0.315f, 0.9f}, {0.345f, 0.15f, 0.475f, 0.9f}, 
                       {0.5f, 0.15f, 0.635f, 0.9f},  {0.665f, 0.15f, 0.8f, 0.9f}, {0.83f, 0.15f, 0.98f, 0.9f}}
    {
        FilePath = "C://Users//Raymond//Desktop//CG-Library//src//Rubikv4//CuboRubik.png";
        glGenTextures(1, &renderedID);
        glBindTexture(GL_TEXTURE_2D, renderedID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        stbi_set_flip_vertically_on_load(1);
        
        LocalBuffer = stbi_load(FilePath.c_str(), &width, &height, &BitsPerPixel, 0);
        if (LocalBuffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, LocalBuffer);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(LocalBuffer);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture()
    {
        glDeleteTextures(1, &renderedID);
    }
    void Bind()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedID);
    }
    void Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

struct Cubee 
{
    float radio[3];
    Texture Letters;
    unsigned int VBO, VAO, renderedID;
    int numFaces, Shader, vertexLocation, transformLoc;
    float vertexs[192];
    unsigned int indexs[6][6];
    float o[3];
    unsigned int IBO[6];
    bool ColorFaces[6];
    glm::mat4 trans;

    Cubee() : renderedID(0), vertexLocation(0), VBO(0), VAO(0), transformLoc(0), Shader(0), radio{0, 0, 0}, numFaces(6), Letters(),
             indexs{ { 0, 1, 3, 1, 2, 3 }, { 4, 5, 7, 5, 6, 7 }, { 8, 9, 11, 9, 10, 11 }, { 12, 13, 15, 13, 14, 15 }, 
                     {16,17,19,17,18,19 }, {20,21,23,21,22,23 }}, o{0,0,0}, IBO{0,0,0,0,0,0}, ColorFaces{0,0,0,0,0,0}, trans(1.0f)
    { 
        memset(vertexs, 0, 192 * sizeof(float));
    }

    void pasteColor(bool temp[6]) { for (int i = 0; i < numFaces; ++i) { ColorFaces[i] = temp[i]; } }

    void UpdateBuffers() 
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs[i]), indexs[i], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    int getSection(int i, int flip)
    {
        int t = 0;
        if(o[i] > 0){ (flip) ? t = 0 : t = 2; }
        else if(o[i] == 0){ t = 1; }
        else{ (flip) ? t = 2 : t = 0; }
        return t;
    }

    float *getFragmentImage(int index)
    {
        float *x = new float[4];

        float fragmentX = (Letters.imgPos[index][2] - Letters.imgPos[index][0]) / 3;
        float fragmentY = (Letters.imgPos[index][3] - Letters.imgPos[index][1]) / 3;
        int iPos, jPos;

        switch(index){
            case 0:
                iPos = getSection(0, 0);
                jPos = getSection(1, 0);
                break;
            case 1:
                iPos = getSection(2, 1);
                jPos = getSection(1, 0);
                break;
            case 2:
                iPos = getSection(0, 1);
                jPos = getSection(1, 0);
                break;
            case 3:
                iPos = getSection(2, 0);
                jPos = getSection(1, 0);
                break;
            case 4:
                iPos = getSection(0, 0);
                jPos = getSection(2, 0);
                break;
            case 5:
                iPos = getSection(0, 0);
                jPos = getSection(2, 1);
                break;
        }

        x[0] = Letters.imgPos[index][0] + (fragmentX * iPos);
        x[1] = Letters.imgPos[index][1] + (fragmentY * jPos);
        x[2] = Letters.imgPos[index][0] + (fragmentX * (iPos + 1));
        x[3] = Letters.imgPos[index][1] + (fragmentY * (jPos + 1));

        return x;
    }

    void getMirror(float x, float y, float z, bool t)
    {
        o[0] = x; o[1] = y; o[2] = z;

        if(t)
        {
            int tempCoords[3] = { getSection(0, 0), getSection(1, 0), getSection(2, 0) };
            o[0] = -0.5f; o[1] = -0.8f; o[2] = -0.9f;

            for(int k = 0 ; k < 3; ++k)
            {
                for (int i = 0; i < tempCoords[k]; ++i)
                {
                    o[k] += (r[k][i] * 2);
                }
                
                o[k] += r[k][tempCoords[k]];
                radio[k] = r[k][tempCoords[k]];
            }
        }
        else
        {
            radio[0] = 0.2f; radio[1] = 0.2f; radio[2] = 0.2f;
        }
    }

    void SetUp(float x, float y, float z, bool fColors[6], int ShaderProgram) 
    {
        getMirror(x, y, z, 1);

        Shader = ShaderProgram;
        pasteColor(fColors);
        transformLoc = glGetUniformLocation(Shader, "transform");
        vertexLocation = glGetUniformLocation(Shader, "Color");

        float vertices[] = 
        {
            o[0] + radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,// FRONT
            o[0] + radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,

            o[0] + radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0,// RIGHT
            o[0] + radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] + radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] + radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,

            o[0] - radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0,// BACK
            o[0] - radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] + radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] + radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0,

            o[0] - radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,// LEFT
            o[0] - radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0,

            o[0] + radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,// DOWN
            o[0] + radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] - radio[1], o[2] - radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] - radio[1], o[2] + radio[2], 0,0,0,0,0,

            o[0] + radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0,// UP
            o[0] + radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] + radio[1], o[2] + radio[2], 0,0,0,0,0,
            o[0] - radio[0], o[1] + radio[1], o[2] - radio[2], 0,0,0,0,0
        };

        for (int i = 0; i < numFaces; ++i) 
        {
            if(!ColorFaces[i])
            {
                int j = 3;
                vertices[(32 * i) + j] = Colors[6][0]; vertices[(32 * i) + j + 1] = Colors[6][1]; vertices[(32 * i) + j + 2] = Colors[6][2]; vertices[(32 * i) + j + 3] = 0; vertices[(32 * i) + j + 4] = 0;
                j += 8;
                vertices[(32 * i) + j] = Colors[6][0]; vertices[(32 * i) + j + 1] = Colors[6][1]; vertices[(32 * i) + j + 2] = Colors[6][2]; vertices[(32 * i) + j + 3] = 0; vertices[(32 * i) + j + 4] = 0;
                j += 8;
                vertices[(32 * i) + j] = Colors[6][0]; vertices[(32 * i) + j + 1] = Colors[6][1]; vertices[(32 * i) + j + 2] = Colors[6][2]; vertices[(32 * i) + j + 3] = 0; vertices[(32 * i) + j + 4] = 0;
                j += 8;
                vertices[(32 * i) + j] = Colors[6][0]; vertices[(32 * i) + j + 1] = Colors[6][1]; vertices[(32 * i) + j + 2] = Colors[6][2]; vertices[(32 * i) + j + 3] = 0; vertices[(32 * i) + j + 4] = 0;
            }
            else
            {   
                float *temp = getFragmentImage(i);
                int j = 3;
                vertices[(32 * i) + j] = Colors[i][0]; vertices[(32 * i) + j + 1] = Colors[i][1]; vertices[(32 * i) + j + 2] = Colors[i][2]; vertices[(32 * i) + j + 3] = temp[2]; vertices[(32 * i) + j + 4] = temp[3];
                j += 8;
                vertices[(32 * i) + j] = Colors[i][0]; vertices[(32 * i) + j + 1] = Colors[i][1]; vertices[(32 * i) + j + 2] = Colors[i][2]; vertices[(32 * i) + j + 3] = temp[2]; vertices[(32 * i) + j + 4] = temp[1];
                j += 8;
                vertices[(32 * i) + j] = Colors[i][0]; vertices[(32 * i) + j + 1] = Colors[i][1]; vertices[(32 * i) + j + 2] = Colors[i][2]; vertices[(32 * i) + j + 3] = temp[0]; vertices[(32 * i) + j + 4] = temp[1];
                j += 8;
                vertices[(32 * i) + j] = Colors[i][0]; vertices[(32 * i) + j + 1] = Colors[i][1]; vertices[(32 * i) + j + 2] = Colors[i][2]; vertices[(32 * i) + j + 3] = temp[0]; vertices[(32 * i) + j + 4] = temp[3];
                delete temp;
            }
        }

        memcpy(vertexs, vertices, 192 * sizeof(float));
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(6, IBO);
        UpdateBuffers();
    }

    void prueba() {
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    }

    void rotarxpos(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(velo), glm::vec3(1.0, 0.0, 0.0));
        trans = xd * trans;
    }
    void rotarypos(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(velo), glm::vec3(0.0, 1.0, 0.0));
        trans = xd * trans;
    }
    void rotarzpos(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(velo), glm::vec3(0.0, 0.0, 1.0));
        trans = xd*trans;
    }
    void rotarxneg(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(360.0f - velo), glm::vec3(1.0, 0.0, 0.0));
        trans = xd*trans;
    }
    void rotaryneg(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(360.0f - velo), glm::vec3(0.0, 1.0, 0.0));
        trans = xd * trans;
    }
    void rotarzneg(float velo) {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(360.0f - velo), glm::vec3(0.0, 0.0, 1.0));
        trans = xd * trans;
    }

    void draw() 
    {
        glBindVertexArray(VAO);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        Letters.Bind();

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        Letters.Unbind();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Cubee() 
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(6, IBO);
    }
};

struct Rubik {
    int numCubes;
    float o[3];
    vector<Cubee*> Cubos;
    int Shader, dislocpos;
    float distance;
    glm::mat4 disloc[27];
    float cambio = 0.0f;
    int movimientos = 0;

    Rubik(): Cubos(27, 0), numCubes(27), Shader(0), distance(0.42f), o{0,0,0}
    {
        for (int i = 0; i < 27; ++i) { Cubos[i] = new Cubee; }
    }

    void dislocar(float x) 
    {
        glm::mat4 xd;
        int c1 = 0;
        for (int i = 1; i > -2; --i) {
            int c2 = 0;
            for (int j = -1; j < 2; ++j) {
                int c3 = 0;
                for (int k = -1; k < 2; ++k) {
                    xd = glm::translate(glm::mat4(1), glm::vec3(x * j, x * k, x * -i));
                    disloc[c1 + c2 + c3] = xd;

                    c3 += 1;
                }
                c2 += 3;
            }
            c1 += 9;
        }
    }

    void SetUp(int ShaderProgram) 
    {
        dislocar(0.0f);
        Shader = ShaderProgram;
        dislocpos = glGetUniformLocation(Shader, "disloc");

        float t1 = -distance;
        for (unsigned int i = 0; i < 3; ++i) {
            float t2 = -distance;
            for (unsigned int j = 0; j < 3; ++j) {
                float t3 = -distance;
                for (unsigned int k = 0; k < 3; ++k) {
                    Cubos[(i * 9) + (j * 3) + k]->SetUp(t2, t3, t1, PaintedFaces[(i * 9) + (j * 3) + k], Shader);
                    t3 += distance;
                }
                t2 += distance;
            }
            t1 += distance;
        }
    }

    void draw() 
    {
        for (int i = 0; i < numCubes; ++i) { 
            glUniformMatrix4fv(dislocpos, 1, GL_FALSE, glm::value_ptr(disloc[i]));
            Cubos[i]->draw(); 
        }
    }

    void animacion(int op,float velo) {

        int xd = 0; 
        switch (op) {
        case 0:
            break;
        case 1:
            Cubos[20 - xd]->rotarzpos(velo);            Cubos[23 - xd]->rotarzpos(velo);            Cubos[26 - xd]->rotarzpos(velo);            Cubos[19 - xd]->rotarzpos(velo);
            Cubos[22 - xd]->rotarzpos(velo);            Cubos[25 - xd]->rotarzpos(velo);            Cubos[18 - xd]->rotarzpos(velo);            Cubos[21 - xd]->rotarzpos(velo);
            Cubos[24 - xd]->rotarzpos(velo);            break;
        case 2:
            xd = 9;
            Cubos[20 - xd]->rotarzpos(velo);            Cubos[23 - xd]->rotarzpos(velo);            Cubos[26 - xd]->rotarzpos(velo);            Cubos[19 - xd]->rotarzpos(velo);
            Cubos[22 - xd]->rotarzpos(velo);            Cubos[25 - xd]->rotarzpos(velo);            Cubos[18 - xd]->rotarzpos(velo);            Cubos[21 - xd]->rotarzpos(velo);
            Cubos[24 - xd]->rotarzpos(velo);            break;
        case 3:
            xd = 18;
            Cubos[20 - xd]->rotarzpos(velo);            Cubos[23 - xd]->rotarzpos(velo);            Cubos[26 - xd]->rotarzpos(velo);            Cubos[19 - xd]->rotarzpos(velo);
            Cubos[22 - xd]->rotarzpos(velo);            Cubos[25 - xd]->rotarzpos(velo);            Cubos[18 - xd]->rotarzpos(velo);            Cubos[21 - xd]->rotarzpos(velo);
            Cubos[24 - xd]->rotarzpos(velo);            break;
        case 4:
            xd = 0;
            Cubos[2 + xd]->rotarxneg(velo);            Cubos[11 + xd]->rotarxneg(velo);            Cubos[20 + xd]->rotarxneg(velo);            Cubos[1 + xd]->rotarxneg(velo);
            Cubos[10 + xd]->rotarxneg(velo);            Cubos[19 + xd]->rotarxneg(velo);            Cubos[0 + xd]->rotarxneg(velo);            Cubos[9 + xd]->rotarxneg(velo);
            Cubos[18 + xd]->rotarxneg(velo);            break;
        case 5:
            xd = 3;
            Cubos[2 + xd]->rotarxneg(velo);            Cubos[11 + xd]->rotarxneg(velo);            Cubos[20 + xd]->rotarxneg(velo);            Cubos[1 + xd]->rotarxneg(velo);
            Cubos[10 + xd]->rotarxneg(velo);            Cubos[19 + xd]->rotarxneg(velo);            Cubos[0 + xd]->rotarxneg(velo);            Cubos[9 + xd]->rotarxneg(velo);
            Cubos[18 + xd]->rotarxneg(velo);            break;
        case 6:
            xd = 6;
            Cubos[2 + xd]->rotarxneg(velo);            Cubos[11 + xd]->rotarxneg(velo);            Cubos[20 + xd]->rotarxneg(velo);            Cubos[1 + xd]->rotarxneg(velo);
            Cubos[10 + xd]->rotarxneg(velo);            Cubos[19 + xd]->rotarxneg(velo);            Cubos[0 + xd]->rotarxneg(velo);            Cubos[9 + xd]->rotarxneg(velo);
            Cubos[18 + xd]->rotarxneg(velo);            break;
        case 7:
            xd = 0;
            Cubos[18 + xd]->rotaryneg(velo);            Cubos[21 + xd]->rotaryneg(velo);            Cubos[24 + xd]->rotaryneg(velo);            Cubos[9 + xd]->rotaryneg(velo);            Cubos[12 + xd]->rotaryneg(velo);
            Cubos[15 + xd]->rotaryneg(velo);            Cubos[0 + xd]->rotaryneg(velo);           
            Cubos[3 + xd]->rotaryneg(velo);             Cubos[6 + xd]->rotaryneg(velo);            break;
        case 8:
            xd = 1;
            Cubos[18 + xd]->rotaryneg(velo);            Cubos[21 + xd]->rotaryneg(velo);            Cubos[24 + xd]->rotaryneg(velo);            Cubos[9 + xd]->rotaryneg(velo);
            Cubos[12 + xd]->rotaryneg(velo);            Cubos[15 + xd]->rotaryneg(velo);            Cubos[0 + xd]->rotaryneg(velo);            Cubos[3 + xd]->rotaryneg(velo);
            Cubos[6 + xd]->rotaryneg(velo);            break;
        case 9:
            xd = 2;
            Cubos[18 + xd]->rotaryneg(velo);            Cubos[21 + xd]->rotaryneg(velo);            Cubos[24 + xd]->rotaryneg(velo);            Cubos[9 + xd]->rotaryneg(velo);
            Cubos[12 + xd]->rotaryneg(velo);            Cubos[15 + xd]->rotaryneg(velo);            Cubos[0 + xd]->rotaryneg(velo);            Cubos[3 + xd]->rotaryneg(velo);
            Cubos[6 + xd]->rotaryneg(velo);            break;
        case 10:
            xd = 0;
            Cubos[20 - xd]->rotarzneg(velo);            Cubos[23 - xd]->rotarzneg(velo);            Cubos[26 - xd]->rotarzneg(velo);            Cubos[19 - xd]->rotarzneg(velo);
            Cubos[22 - xd]->rotarzneg(velo);            Cubos[25 - xd]->rotarzneg(velo);            Cubos[18 - xd]->rotarzneg(velo);            Cubos[21 - xd]->rotarzneg(velo);
            Cubos[24 - xd]->rotarzneg(velo);            break;
        case 11:
            xd = 9;
            Cubos[20 - xd]->rotarzneg(velo);            Cubos[23 - xd]->rotarzneg(velo);
            Cubos[26 - xd]->rotarzneg(velo);            Cubos[19 - xd]->rotarzneg(velo);            Cubos[22 - xd]->rotarzneg(velo);            Cubos[25 - xd]->rotarzneg(velo);
            Cubos[18 - xd]->rotarzneg(velo);            Cubos[21 - xd]->rotarzneg(velo);            Cubos[24 - xd]->rotarzneg(velo);            break;
        case 12:
            xd = 18;
            Cubos[20 - xd]->rotarzneg(velo);            Cubos[23 - xd]->rotarzneg(velo);            Cubos[26 - xd]->rotarzneg(velo);
            Cubos[19 - xd]->rotarzneg(velo);            Cubos[22 - xd]->rotarzneg(velo);            Cubos[25 - xd]->rotarzneg(velo);
            Cubos[18 - xd]->rotarzneg(velo);            Cubos[21 - xd]->rotarzneg(velo);            Cubos[24 - xd]->rotarzneg(velo);            break;
        case 13:
            xd = 0;
            Cubos[2 + xd]->rotarxpos(velo);            Cubos[11 + xd]->rotarxpos(velo);
            Cubos[20 + xd]->rotarxpos(velo);            Cubos[1 + xd]->rotarxpos(velo);            Cubos[10 + xd]->rotarxpos(velo);            Cubos[19 + xd]->rotarxpos(velo);            Cubos[0 + xd]->rotarxpos(velo);            Cubos[9 + xd]->rotarxpos(velo);
            Cubos[18 + xd]->rotarxpos(velo);            break;
        case 14:
            xd = 3;
            Cubos[2 + xd]->rotarxpos(velo);            Cubos[11 + xd]->rotarxpos(velo);            Cubos[20 + xd]->rotarxpos(velo);            Cubos[1 + xd]->rotarxpos(velo);            Cubos[10 + xd]->rotarxpos(velo);            Cubos[19 + xd]->rotarxpos(velo);            Cubos[0 + xd]->rotarxpos(velo);
            Cubos[9 + xd]->rotarxpos(velo);            Cubos[18 + xd]->rotarxpos(velo);            break;
        case 15:
            xd = 6;
            Cubos[2 + xd]->rotarxpos(velo);            Cubos[11 + xd]->rotarxpos(velo);            Cubos[20 + xd]->rotarxpos(velo);            Cubos[1 + xd]->rotarxpos(velo);            Cubos[10 + xd]->rotarxpos(velo);            Cubos[19 + xd]->rotarxpos(velo);
            Cubos[0 + xd]->rotarxpos(velo);            Cubos[9 + xd]->rotarxpos(velo);
            Cubos[18 + xd]->rotarxpos(velo);            break;
        case 16:
            xd = 0;
            Cubos[18 + xd]->rotarypos(velo);            Cubos[21 + xd]->rotarypos(velo);            Cubos[24 + xd]->rotarypos(velo);            Cubos[9 + xd]->rotarypos(velo);            Cubos[12 + xd]->rotarypos(velo);
            Cubos[15 + xd]->rotarypos(velo);            Cubos[0 + xd]->rotarypos(velo);
            Cubos[3 + xd]->rotarypos(velo);            Cubos[6 + xd]->rotarypos(velo);            break;
        case 17:
            xd = 1;
            Cubos[18 + xd]->rotarypos(velo);            Cubos[21 + xd]->rotarypos(velo);            Cubos[24 + xd]->rotarypos(velo);
            Cubos[9 + xd]->rotarypos(velo);            Cubos[12 + xd]->rotarypos(velo);            Cubos[15 + xd]->rotarypos(velo);
            Cubos[0 + xd]->rotarypos(velo);            Cubos[3 + xd]->rotarypos(velo);            Cubos[6 + xd]->rotarypos(velo);            break;
        case 18:
            xd = 2;
            Cubos[18 + xd]->rotarypos(velo);            Cubos[21 + xd]->rotarypos(velo);            Cubos[24 + xd]->rotarypos(velo);            Cubos[9 + xd]->rotarypos(velo);            Cubos[12 + xd]->rotarypos(velo);
            Cubos[15 + xd]->rotarypos(velo);            Cubos[0 + xd]->rotarypos(velo);            Cubos[3 + xd]->rotarypos(velo);
            Cubos[6 + xd]->rotarypos(velo);            break;
        }
        return;
    }

    /*
    Giro 1 : Cara Morada en Sentido antihorario
    Giro 2 : Capa Intermedia entre Cara Morada y Azul en Sentido antihorario visto desde la cara Morada
    Giro 3 : Cara Azul en Sentido horario
    Giro 4 : Cara Verde en Sentido antihorario
    Giro 5 : Capa Intermedia entre Cara Verde y Rojo en Sentido antihorario visto desde la cara Verde
    Giro 6 : Cara Rojo en Sentido horario
    Giro 7 : Cara Blanca en Sentido antihorario
    Giro 8 : Capa Intermedia entre Cara Blanca y Amarilla en Sentido antihorario visto desde la cara Blanca
    Giro 9 : Cara Amarillo en Sentido horario
    Giro 1 inv: Cara Morada en Sentido horario
    Giro 2 inv: Capa Intermedia entre Cara Morada y Azul en Sentido horario visto desde la cara Morada
    Giro 3 inv: Cara Azul en Sentido antihorario
    Giro 4 inv: Cara Verde en Sentido horario
    Giro 5 inv: Capa Intermedia entre Cara Verde y Rojo en Sentido horario visto desde la cara Verde
    Giro 6 inv: Cara Rojo en Sentido antihorario
    Giro 7 inv: Cara Blanca en Sentido horario
    Giro 8 inv: Capa Intermedia entre Cara Blanca y Amarilla en Sentido horario visto desde la cara Blanca
    Giro 9 inv: Cara Amarillo en Sentido antihorario
    */

    int giro1() {
        movimientos++;int xd = 0;
        Cubee* aux1 = Cubos[20 - xd];        Cubee* aux2 = Cubos[23 - xd];        Cubee* aux3 = Cubos[26 - xd];        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];        Cubee* aux6 = Cubos[25 - xd];        Cubee* aux7 = Cubos[18 - xd];        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;        Cubos[23 - xd] = aux6;        Cubos[26 - xd] = aux9;        Cubos[19 - xd] = aux2;
        Cubos[22 - xd] = aux5;        Cubos[25 - xd] = aux8;        Cubos[18 - xd] = aux1;        Cubos[21 - xd] = aux4;
        Cubos[24 - xd] = aux7;         return 1;
    }

    int giro2() {
        movimientos++;int xd = 9;
        Cubee* aux1 = Cubos[20 - xd];        Cubee* aux2 = Cubos[23 - xd];        Cubee* aux3 = Cubos[26 - xd];        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];        Cubee* aux6 = Cubos[25 - xd];        Cubee* aux7 = Cubos[18 - xd];        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;        Cubos[23 - xd] = aux6;        Cubos[26 - xd] = aux9;
        Cubos[19 - xd] = aux2;        Cubos[22 - xd] = aux5;        Cubos[25 - xd] = aux8;        Cubos[18 - xd] = aux1;
        Cubos[21 - xd] = aux4;        Cubos[24 - xd] = aux7; return 2;
    }

    int giro3() {
        movimientos++;int xd = 18;
        Cubee* aux1 = Cubos[20 - xd];
        Cubee* aux2 = Cubos[23 - xd];
        Cubee* aux3 = Cubos[26 - xd];
        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];
        Cubee* aux6 = Cubos[25 - xd];
        Cubee* aux7 = Cubos[18 - xd];
        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;
        Cubos[23 - xd] = aux6;
        Cubos[26 - xd] = aux9;
        Cubos[19 - xd] = aux2;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux8;
        Cubos[18 - xd] = aux1;
        Cubos[21 - xd] = aux4;
        Cubos[24 - xd] = aux7; return 3;
    }
   
    int giro4() {
         movimientos++;int xd = 0;
         Cubee* aux1 = Cubos[2 + xd];
         Cubee* aux2 = Cubos[11 + xd];
         Cubee* aux3 = Cubos[20 + xd];
         Cubee* aux4 = Cubos[1 + xd];
         Cubee* aux5 = Cubos[10 + xd];
         Cubee* aux6 = Cubos[19 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[9 + xd];
         Cubee* aux9 = Cubos[18 + xd];
         Cubos[2 + xd] = aux3;
         Cubos[11 + xd] = aux6;
         Cubos[20 + xd] = aux9;
         Cubos[1 + xd] = aux2;
         Cubos[10 + xd] = aux5;
         Cubos[19 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[9 + xd] = aux4;
         Cubos[18 + xd] = aux7; return 4;
     }
        
    int giro5() {
         movimientos++;int xd = 3;
         Cubee* aux1 = Cubos[2 + xd];
         Cubee* aux2 = Cubos[11 + xd];
         Cubee* aux3 = Cubos[20 + xd];
         Cubee* aux4 = Cubos[1 + xd];
         Cubee* aux5 = Cubos[10 + xd];
         Cubee* aux6 = Cubos[19 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[9 + xd];
         Cubee* aux9 = Cubos[18 + xd];
         Cubos[2 + xd] = aux3;
         Cubos[11 + xd] = aux6;
         Cubos[20 + xd] = aux9;
         Cubos[1 + xd] = aux2;
         Cubos[10 + xd] = aux5;
         Cubos[19 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[9 + xd] = aux4;
         Cubos[18 + xd] = aux7; return 5;
     }
    
    int giro6() {
         movimientos++;int xd = 6;
         Cubee* aux1 = Cubos[2 + xd];
         Cubee* aux2 = Cubos[11 + xd];
         Cubee* aux3 = Cubos[20 + xd];
         Cubee* aux4 = Cubos[1 + xd];
         Cubee* aux5 = Cubos[10 + xd];
         Cubee* aux6 = Cubos[19 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[9 + xd];
         Cubee* aux9 = Cubos[18 + xd];
         Cubos[2 + xd] = aux3;
         Cubos[11 + xd] = aux6;
         Cubos[20 + xd] = aux9;
         Cubos[1 + xd] = aux2;
         Cubos[10 + xd] = aux5;
         Cubos[19 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[9 + xd] = aux4;
         Cubos[18 + xd] = aux7; return 6;
     }
    
    int giro7() {
         movimientos++;int xd = 0;
         Cubee* aux1 = Cubos[18 + xd];
         Cubee* aux2 = Cubos[21 + xd];
         Cubee* aux3 = Cubos[24 + xd];
         Cubee* aux4 = Cubos[9 + xd];
         Cubee* aux5 = Cubos[12 + xd];
         Cubee* aux6 = Cubos[15 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[3 + xd];
         Cubee* aux9 = Cubos[6 + xd];
         Cubos[18 + xd] = aux3;
         Cubos[21 + xd] = aux6;
         Cubos[24 + xd] = aux9;
         Cubos[9 + xd] = aux2;
         Cubos[12 + xd] = aux5;
         Cubos[15 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[3 + xd] = aux4;
         Cubos[6 + xd] = aux7; return 7;
     }
        
    int giro8() {
         movimientos++;int xd = 1;
         Cubee* aux1 = Cubos[18 + xd];
         Cubee* aux2 = Cubos[21 + xd];
         Cubee* aux3 = Cubos[24 + xd];
         Cubee* aux4 = Cubos[9 + xd];
         Cubee* aux5 = Cubos[12 + xd];
         Cubee* aux6 = Cubos[15 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[3 + xd];
         Cubee* aux9 = Cubos[6 + xd];
         Cubos[18 + xd] = aux3;
         Cubos[21 + xd] = aux6;
         Cubos[24 + xd] = aux9;
         Cubos[9 + xd] = aux2;
         Cubos[12 + xd] = aux5;
         Cubos[15 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[3 + xd] = aux4;
         Cubos[6 + xd] = aux7; return 8;
     }
        
    int giro9() {
         movimientos++;int xd = 2;
         Cubee* aux1 = Cubos[18 + xd];
         Cubee* aux2 = Cubos[21 + xd];
         Cubee* aux3 = Cubos[24 + xd];
         Cubee* aux4 = Cubos[9 + xd];
         Cubee* aux5 = Cubos[12 + xd];
         Cubee* aux6 = Cubos[15 + xd];
         Cubee* aux7 = Cubos[0 + xd];
         Cubee* aux8 = Cubos[3 + xd];
         Cubee* aux9 = Cubos[6 + xd];
         Cubos[18 + xd] = aux3;
         Cubos[21 + xd] = aux6;
         Cubos[24 + xd] = aux9;
         Cubos[9 + xd] = aux2;
         Cubos[12 + xd] = aux5;
         Cubos[15 + xd] = aux8;
         Cubos[0 + xd] = aux1;
         Cubos[3 + xd] = aux4;
         Cubos[6 + xd] = aux7; return 9;
     }
    
    int giro1inv() {
        movimientos++;int xd = 0;
        Cubee* aux1 = Cubos[20 - xd];
        Cubee* aux2 = Cubos[23 - xd];
        Cubee* aux3 = Cubos[26 - xd];
        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];
        Cubee* aux6 = Cubos[25 - xd];
        Cubee* aux7 = Cubos[18 - xd];
        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7; 
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
        return 10;
    }

    int giro2inv() {
        movimientos++;int xd = 9;
        Cubee* aux1 = Cubos[20 - xd];
        Cubee* aux2 = Cubos[23 - xd];
        Cubee* aux3 = Cubos[26 - xd];
        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];
        Cubee* aux6 = Cubos[25 - xd];
        Cubee* aux7 = Cubos[18 - xd];
        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7; 
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
        return 11;
    }

    int giro3inv() {
        movimientos++;int xd = 18;
        Cubee* aux1 = Cubos[20 - xd];
        Cubee* aux2 = Cubos[23 - xd];
        Cubee* aux3 = Cubos[26 - xd];
        Cubee* aux4 = Cubos[19 - xd];
        Cubee* aux5 = Cubos[22 - xd];
        Cubee* aux6 = Cubos[25 - xd];
        Cubee* aux7 = Cubos[18 - xd];
        Cubee* aux8 = Cubos[21 - xd];
        Cubee* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7; 
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
        return 12;
    }
    
    int giro4inv() {
        movimientos++;int xd = 0;
        Cubee* aux1 = Cubos[2 + xd];
        Cubee* aux2 = Cubos[11 + xd];
        Cubee* aux3 = Cubos[20 + xd];
        Cubee* aux4 = Cubos[1 + xd];
        Cubee* aux5 = Cubos[10 + xd];
        Cubee* aux6 = Cubos[19 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[9 + xd];
        Cubee* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] =  aux7;
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] =  aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] =  aux9;
        Cubos[9 + xd] =  aux6;
        Cubos[18 + xd] = aux3;
        return 13;
    }

    int giro5inv() {
        movimientos++;int xd = 3;
        Cubee* aux1 = Cubos[2 + xd];
        Cubee* aux2 = Cubos[11 + xd];
        Cubee* aux3 = Cubos[20 + xd];
        Cubee* aux4 = Cubos[1 + xd];
        Cubee* aux5 = Cubos[10 + xd];
        Cubee* aux6 = Cubos[19 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[9 + xd];
        Cubee* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux7; 
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] = aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[9 + xd] = aux6;
        Cubos[18 + xd] = aux3;
        return 14;
    }
    
    int giro6inv() {
        movimientos++;int xd = 6;
        Cubee* aux1 = Cubos[2 + xd];
        Cubee* aux2 = Cubos[11 + xd];
        Cubee* aux3 = Cubos[20 + xd];
        Cubee* aux4 = Cubos[1 + xd];
        Cubee* aux5 = Cubos[10 + xd];
        Cubee* aux6 = Cubos[19 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[9 + xd];
        Cubee* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux7; 
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] = aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[9 + xd] = aux6;
        Cubos[18 + xd] = aux3;
        return 15;
    }
    
    int giro7inv() {
        movimientos++;int xd = 0;
        Cubee* aux1 = Cubos[18 + xd];
        Cubee* aux2 = Cubos[21 + xd];
        Cubee* aux3 = Cubos[24 + xd];
        Cubee* aux4 = Cubos[9 + xd];
        Cubee* aux5 = Cubos[12 + xd];
        Cubee* aux6 = Cubos[15 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[3 + xd];
        Cubee* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7; 
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] =  aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] =  aux9;
        Cubos[3 + xd] =  aux6;
        Cubos[6 + xd] =  aux3;
        return 16;
    }

    int giro8inv() {
        movimientos++;int xd = 1;
        Cubee* aux1 = Cubos[18 + xd];
        Cubee* aux2 = Cubos[21 + xd];
        Cubee* aux3 = Cubos[24 + xd];
        Cubee* aux4 = Cubos[9 + xd];
        Cubee* aux5 = Cubos[12 + xd];
        Cubee* aux6 = Cubos[15 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[3 + xd];
        Cubee* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7; 
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] = aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[3 + xd] = aux6;
        Cubos[6 + xd] = aux3;
        return 17;
    }

    int giro9inv() {
        movimientos++;int xd = 2;
        Cubee* aux1 = Cubos[18 + xd];
        Cubee* aux2 = Cubos[21 + xd];
        Cubee* aux3 = Cubos[24 + xd];
        Cubee* aux4 = Cubos[9 + xd];
        Cubee* aux5 = Cubos[12 + xd];
        Cubee* aux6 = Cubos[15 + xd];
        Cubee* aux7 = Cubos[0 + xd];
        Cubee* aux8 = Cubos[3 + xd];
        Cubee* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7; 
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] = aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[3 + xd] = aux6;
        Cubos[6 + xd] = aux3;
        return 18;
    }

    ~Rubik() {
        for (int i = 0; i < numCubes; ++i) { delete Cubos[i]; }
    }
};
