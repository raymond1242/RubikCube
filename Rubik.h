#include <iostream>
#include <vector>
#include <glm/glm.hpp>
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

unsigned int IndexColorsCube[27][6] = {
    {5,6,6,3,4,6},{5,6,6,3,6,6},{5,6,2,3,6,6},{5,6,6,6,4,6},{5,6,6,6,6,6},{5,6,2,6,6,6},{5,0,6,6,4,6},{5,0,6,6,6,6},{5,0,2,6,6,6},
    {6,6,6,3,4,6},{6,6,6,3,6,6},{6,6,2,3,6,6},{6,6,6,6,4,6},{6,6,6,6,6,6},{6,6,2,6,6,6},{6,0,6,6,4,6},{6,0,6,6,6,6},{6,0,2,6,6,6},
    {6,6,6,3,4,1},{6,6,6,3,6,1},{6,6,2,3,6,1},{6,6,6,6,4,1},{6,6,6,6,6,1},{6,6,2,6,6,1},{6,0,6,6,4,1},{6,0,6,6,6,1},{6,0,2,6,6,1}
};

struct Cube {
    unsigned int VBO, VAO;
    float radio;
    int numFaces;
    int Shader;
    int vertexLocation;
    unsigned int transformLoc;
    float vertexs[24];
    unsigned int indexs[6][6];
    float o[3] = { 0.0,0.0,0.0 };
    unsigned int IBO[6] = { 0,0,0,0,0,0 };
    unsigned int ColorFaces[6] = { 0,0,0,0,0,0 };
    glm::mat4 trans = glm::mat4(1.0f);

    Cube() :
        indexs{ {0, 1, 2, 0, 2, 3}, {4, 0, 3, 4, 3, 7}, {0, 5, 1, 0, 5, 4}, 
                {1, 2, 6, 1, 5, 6}, {2, 6, 7, 2, 3, 7}, {7, 5, 4, 7, 6, 5} }, 
        vertexs{ o[0] + radio, o[1] + radio, o[2] - radio,
                o[0] - radio, o[1] + radio, o[2] - radio,
                o[0] - radio, o[1] - radio, o[2] - radio,
                o[0] + radio, o[1] - radio, o[2] - radio,
                o[0] + radio, o[1] + radio, o[2] + radio,
                o[0] - radio, o[1] + radio, o[2] + radio,
                o[0] - radio, o[1] - radio, o[2] + radio,
                o[0] + radio, o[1] - radio, o[2] + radio }
    {
        radio = 0.2f; numFaces = 6; Shader = 0; vertexLocation = 0; VBO = 0; VAO = 0; transformLoc = 0;
    }

    void pasteColor(unsigned int temp[6]) {
        for (int i = 0; i < numFaces; ++i) {
            ColorFaces[i] = temp[i];
        }
    }

    void AllocateVertexsBuffers() {
        vertexs[0] = o[0] + radio; vertexs[1] = o[1] + radio; vertexs[2] = o[2] - radio;
        vertexs[3] = o[0] - radio; vertexs[4] = o[1] + radio; vertexs[5] = o[2] - radio;
        vertexs[6] = o[0] - radio; vertexs[7] = o[1] - radio; vertexs[8] = o[2] - radio;
        vertexs[9] = o[0] + radio; vertexs[10] = o[1] - radio; vertexs[11] = o[2] - radio;
        vertexs[12] = o[0] + radio; vertexs[13] = o[1] + radio; vertexs[14] = o[2] + radio;
        vertexs[15] = o[0] - radio; vertexs[16] = o[1] + radio; vertexs[17] = o[2] + radio;
        vertexs[18] = o[0] - radio; vertexs[19] = o[1] - radio; vertexs[20] = o[2] + radio;
        vertexs[21] = o[0] + radio; vertexs[22] = o[1] - radio; vertexs[23] = o[2] + radio;
    }

    void UpdateBuffers() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs[i]), indexs[i], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void SetUp(float x, float y, float z, unsigned int fColors[6], int ShaderProgram) {
        o[0] = x; o[1] = y; o[2] = z;
        // cout << x << " " << y << " " << z << endl;
        Shader = ShaderProgram;
        pasteColor(fColors);
        transformLoc = glGetUniformLocation(Shader, "transform");
        vertexLocation = glGetUniformLocation(Shader, "Color");

        AllocateVertexsBuffers();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(6, IBO);
        UpdateBuffers();
    }

    void SetDistance(float x, float y, float z) {
        o[0] = x; o[1] = y; o[2] = z;
        AllocateVertexsBuffers();
        UpdateBuffers();
    }

    void prueba() {
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    }

    void rotarxpos() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        trans = xd * trans;
    }
    void rotarypos() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        trans = xd * trans;
    }
    void rotarzpos() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        trans = xd * trans;
    }
    void rotarxneg() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(270.0f), glm::vec3(1.0, 0.0, 0.0));
        trans = xd * trans;
    }
    void rotaryneg() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
        trans = xd * trans;
    }
    void rotarzneg() {
        glm::mat4 xd(1.0);
        xd = glm::rotate(xd, glm::radians(270.0f), glm::vec3(0.0, 0.0, 1.0));
        trans = xd * trans;
    }


    void draw() {

        glBindVertexArray(VAO);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glUniform4f(vertexLocation, Colors[ColorFaces[i]][0], Colors[ColorFaces[i]][1], Colors[ColorFaces[i]][2], Colors[ColorFaces[i]][3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    void Delete() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(6, IBO);
    }
};

struct Rubik {
    double radio;
    int numCubes;
    float o[3] = { 0.0, 0.0, 0.0 };
    vector<Cube*> Cubos;
    int Shader;
    float distance;
    Rubik(): Cubos(27, 0) {
        radio = 0.6; numCubes = 27; Shader = 0; distance = 0.405f;
        for (int i = 0; i < 27; ++i) {
            //Cube* t = new Cube;
            Cubos[i] = new Cube;
        }
    }

    void SetUp(int ShaderProgram) {
        Shader = ShaderProgram;

        float t1 = -distance;
        for (int i = 0; i < 3; ++i) {
            float t2 = -distance;
            for (int j = 0; j < 3; ++j) {
                float t3 = -distance;
                for (int k = 0; k < 3; ++k) {
                    Cubos[(i * 9) + (j * 3) + k]->SetUp(t2, t3, t1, IndexColorsCube[(i * 9) + (j * 3) + k], Shader);
                    t3 += distance;
                }
                t2 += distance;
            }
            t1 += distance;
        }
    }

    void draw() {
        for (int i = 0; i < numCubes; ++i) {
            Cubos[i]->draw();
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

    void giro1() {
        int xd = 0;
        Cubos[20 - xd]->rotarzpos();
        Cubos[23 - xd]->rotarzpos();
        Cubos[26 - xd]->rotarzpos();
        Cubos[19 - xd]->rotarzpos();
        Cubos[22 - xd]->rotarzpos();
        Cubos[25 - xd]->rotarzpos();
        Cubos[18 - xd]->rotarzpos();
        Cubos[21 - xd]->rotarzpos();
        Cubos[24 - xd]->rotarzpos();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;
        Cubos[23 - xd] = aux6;
        Cubos[26 - xd] = aux9;
        Cubos[19 - xd] = aux2;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux8;
        Cubos[18 - xd] = aux1;
        Cubos[21 - xd] = aux4;
        Cubos[24 - xd] = aux7;
    }

    void giro2() {
        int xd = 9;
        Cubos[20 - xd]->rotarzpos();
        Cubos[23 - xd]->rotarzpos();
        Cubos[26 - xd]->rotarzpos();
        Cubos[19 - xd]->rotarzpos();
        Cubos[22 - xd]->rotarzpos();
        Cubos[25 - xd]->rotarzpos();
        Cubos[18 - xd]->rotarzpos();
        Cubos[21 - xd]->rotarzpos();
        Cubos[24 - xd]->rotarzpos();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;
        Cubos[23 - xd] = aux6;
        Cubos[26 - xd] = aux9;
        Cubos[19 - xd] = aux2;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux8;
        Cubos[18 - xd] = aux1;
        Cubos[21 - xd] = aux4;
        Cubos[24 - xd] = aux7;
    }

    void giro3() {
        int xd = 18;
        Cubos[20 - xd]->rotarzpos();
        Cubos[23 - xd]->rotarzpos();
        Cubos[26 - xd]->rotarzpos();
        Cubos[19 - xd]->rotarzpos();
        Cubos[22 - xd]->rotarzpos();
        Cubos[25 - xd]->rotarzpos();
        Cubos[18 - xd]->rotarzpos();
        Cubos[21 - xd]->rotarzpos();
        Cubos[24 - xd]->rotarzpos();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux3;
        Cubos[23 - xd] = aux6;
        Cubos[26 - xd] = aux9;
        Cubos[19 - xd] = aux2;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux8;
        Cubos[18 - xd] = aux1;
        Cubos[21 - xd] = aux4;
        Cubos[24 - xd] = aux7;
    }

    void giro4() {
        int xd = 0;
        Cubos[2 + xd]->rotarxneg();
        Cubos[11 + xd]->rotarxneg();
        Cubos[20 + xd]->rotarxneg();
        Cubos[1 + xd]->rotarxneg();
        Cubos[10 + xd]->rotarxneg();
        Cubos[19 + xd]->rotarxneg();
        Cubos[0 + xd]->rotarxneg();
        Cubos[9 + xd]->rotarxneg();
        Cubos[18 + xd]->rotarxneg();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux3;
        Cubos[11 + xd] = aux6;
        Cubos[20 + xd] = aux9;
        Cubos[1 + xd] = aux2;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[9 + xd] = aux4;
        Cubos[18 + xd] = aux7;
    }

    void giro5() {
        int xd = 3;
        Cubos[2 + xd]->rotarxneg();
        Cubos[11 + xd]->rotarxneg();
        Cubos[20 + xd]->rotarxneg();
        Cubos[1 + xd]->rotarxneg();
        Cubos[10 + xd]->rotarxneg();
        Cubos[19 + xd]->rotarxneg();
        Cubos[0 + xd]->rotarxneg();
        Cubos[9 + xd]->rotarxneg();
        Cubos[18 + xd]->rotarxneg();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux3;
        Cubos[11 + xd] = aux6;
        Cubos[20 + xd] = aux9;
        Cubos[1 + xd] = aux2;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[9 + xd] = aux4;
        Cubos[18 + xd] = aux7;
    }

    void giro6() {
        int xd = 6;
        Cubos[2 + xd]->rotarxneg();
        Cubos[11 + xd]->rotarxneg();
        Cubos[20 + xd]->rotarxneg();
        Cubos[1 + xd]->rotarxneg();
        Cubos[10 + xd]->rotarxneg();
        Cubos[19 + xd]->rotarxneg();
        Cubos[0 + xd]->rotarxneg();
        Cubos[9 + xd]->rotarxneg();
        Cubos[18 + xd]->rotarxneg();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux3;
        Cubos[11 + xd] = aux6;
        Cubos[20 + xd] = aux9;
        Cubos[1 + xd] = aux2;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[9 + xd] = aux4;
        Cubos[18 + xd] = aux7;
    }

    void giro7() {
        int xd = 0;
        Cubos[18 + xd]->rotaryneg();
        Cubos[21 + xd]->rotaryneg();
        Cubos[24 + xd]->rotaryneg();
        Cubos[9 + xd]->rotaryneg();
        Cubos[12 + xd]->rotaryneg();
        Cubos[15 + xd]->rotaryneg();
        Cubos[0 + xd]->rotaryneg();
        Cubos[3 + xd]->rotaryneg();
        Cubos[6 + xd]->rotaryneg();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux3;
        Cubos[21 + xd] = aux6;
        Cubos[24 + xd] = aux9;
        Cubos[9 + xd] = aux2;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[3 + xd] = aux4;
        Cubos[6 + xd] = aux7;
    }

    void giro8() {
        int xd = 1;
        Cubos[18 + xd]->rotaryneg();
        Cubos[21 + xd]->rotaryneg();
        Cubos[24 + xd]->rotaryneg();
        Cubos[9 + xd]->rotaryneg();
        Cubos[12 + xd]->rotaryneg();
        Cubos[15 + xd]->rotaryneg();
        Cubos[0 + xd]->rotaryneg();
        Cubos[3 + xd]->rotaryneg();
        Cubos[6 + xd]->rotaryneg();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux3;
        Cubos[21 + xd] = aux6;
        Cubos[24 + xd] = aux9;
        Cubos[9 + xd] = aux2;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[3 + xd] = aux4;
        Cubos[6 + xd] = aux7;
    }

    void giro9() {
        int xd = 2;
        Cubos[18 + xd]->rotaryneg();
        Cubos[21 + xd]->rotaryneg();
        Cubos[24 + xd]->rotaryneg();
        Cubos[9 + xd]->rotaryneg();
        Cubos[12 + xd]->rotaryneg();
        Cubos[15 + xd]->rotaryneg();
        Cubos[0 + xd]->rotaryneg();
        Cubos[3 + xd]->rotaryneg();
        Cubos[6 + xd]->rotaryneg();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux3;
        Cubos[21 + xd] = aux6;
        Cubos[24 + xd] = aux9;
        Cubos[9 + xd] = aux2;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux8;
        Cubos[0 + xd] = aux1;
        Cubos[3 + xd] = aux4;
        Cubos[6 + xd] = aux7;
    }

    void giro1inv() {
        int xd = 0;
        Cubos[20 - xd]->rotarzneg();
        Cubos[23 - xd]->rotarzneg();
        Cubos[26 - xd]->rotarzneg();
        Cubos[19 - xd]->rotarzneg();
        Cubos[22 - xd]->rotarzneg();
        Cubos[25 - xd]->rotarzneg();
        Cubos[18 - xd]->rotarzneg();
        Cubos[21 - xd]->rotarzneg();
        Cubos[24 - xd]->rotarzneg();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7;
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
    }

    void giro2inv() {
        int xd = 9;
        Cubos[20 - xd]->rotarzneg();
        Cubos[23 - xd]->rotarzneg();
        Cubos[26 - xd]->rotarzneg();
        Cubos[19 - xd]->rotarzneg();
        Cubos[22 - xd]->rotarzneg();
        Cubos[25 - xd]->rotarzneg();
        Cubos[18 - xd]->rotarzneg();
        Cubos[21 - xd]->rotarzneg();
        Cubos[24 - xd]->rotarzneg();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7;
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
    }

    void giro3inv() {
        int xd = 18;
        Cubos[20 - xd]->rotarzneg();
        Cubos[23 - xd]->rotarzneg();
        Cubos[26 - xd]->rotarzneg();
        Cubos[19 - xd]->rotarzneg();
        Cubos[22 - xd]->rotarzneg();
        Cubos[25 - xd]->rotarzneg();
        Cubos[18 - xd]->rotarzneg();
        Cubos[21 - xd]->rotarzneg();
        Cubos[24 - xd]->rotarzneg();
        Cube* aux1 = Cubos[20 - xd];
        Cube* aux2 = Cubos[23 - xd];
        Cube* aux3 = Cubos[26 - xd];
        Cube* aux4 = Cubos[19 - xd];
        Cube* aux5 = Cubos[22 - xd];
        Cube* aux6 = Cubos[25 - xd];
        Cube* aux7 = Cubos[18 - xd];
        Cube* aux8 = Cubos[21 - xd];
        Cube* aux9 = Cubos[24 - xd];
        Cubos[20 - xd] = aux7;
        Cubos[23 - xd] = aux4;
        Cubos[26 - xd] = aux1;
        Cubos[19 - xd] = aux8;
        Cubos[22 - xd] = aux5;
        Cubos[25 - xd] = aux2;
        Cubos[18 - xd] = aux9;
        Cubos[21 - xd] = aux6;
        Cubos[24 - xd] = aux3;
    }

    void giro4inv() {
        int xd = 0;
        Cubos[2 + xd]->rotarxpos();
        Cubos[11 + xd]->rotarxpos();
        Cubos[20 + xd]->rotarxpos();
        Cubos[1 + xd]->rotarxpos();
        Cubos[10 + xd]->rotarxpos();
        Cubos[19 + xd]->rotarxpos();
        Cubos[0 + xd]->rotarxpos();
        Cubos[9 + xd]->rotarxpos();
        Cubos[18 + xd]->rotarxpos();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux7;
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] = aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[9 + xd] = aux6;
        Cubos[18 + xd] = aux3;
    }

    void giro5inv() {
        int xd = 3;
        Cubos[2 + xd]->rotarxpos();
        Cubos[11 + xd]->rotarxpos();
        Cubos[20 + xd]->rotarxpos();
        Cubos[1 + xd]->rotarxpos();
        Cubos[10 + xd]->rotarxpos();
        Cubos[19 + xd]->rotarxpos();
        Cubos[0 + xd]->rotarxpos();
        Cubos[9 + xd]->rotarxpos();
        Cubos[18 + xd]->rotarxpos();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux7;
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] = aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[9 + xd] = aux6;
        Cubos[18 + xd] = aux3;
    }

    void giro6inv() {
        int xd = 6;
        Cubos[2 + xd]->rotarxpos();
        Cubos[11 + xd]->rotarxpos();
        Cubos[20 + xd]->rotarxpos();
        Cubos[1 + xd]->rotarxpos();
        Cubos[10 + xd]->rotarxpos();
        Cubos[19 + xd]->rotarxpos();
        Cubos[0 + xd]->rotarxpos();
        Cubos[9 + xd]->rotarxpos();
        Cubos[18 + xd]->rotarxpos();
        Cube* aux1 = Cubos[2 + xd];
        Cube* aux2 = Cubos[11 + xd];
        Cube* aux3 = Cubos[20 + xd];
        Cube* aux4 = Cubos[1 + xd];
        Cube* aux5 = Cubos[10 + xd];
        Cube* aux6 = Cubos[19 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[9 + xd];
        Cube* aux9 = Cubos[18 + xd];
        Cubos[2 + xd] = aux7;
        Cubos[11 + xd] = aux4;
        Cubos[20 + xd] = aux1;
        Cubos[1 + xd] = aux8;
        Cubos[10 + xd] = aux5;
        Cubos[19 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[9 + xd] = aux6;
        Cubos[18 + xd] = aux3;
    }

    void giro7inv() {
        int xd = 0;
        Cubos[18 + xd]->rotarypos();
        Cubos[21 + xd]->rotarypos();
        Cubos[24 + xd]->rotarypos();
        Cubos[9 + xd]->rotarypos();
        Cubos[12 + xd]->rotarypos();
        Cubos[15 + xd]->rotarypos();
        Cubos[0 + xd]->rotarypos();
        Cubos[3 + xd]->rotarypos();
        Cubos[6 + xd]->rotarypos();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7;
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] = aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[3 + xd] = aux6;
        Cubos[6 + xd] = aux3;
    }

    void giro8inv() {
        int xd = 1;
        Cubos[18 + xd]->rotarypos();
        Cubos[21 + xd]->rotarypos();
        Cubos[24 + xd]->rotarypos();
        Cubos[9 + xd]->rotarypos();
        Cubos[12 + xd]->rotarypos();
        Cubos[15 + xd]->rotarypos();
        Cubos[0 + xd]->rotarypos();
        Cubos[3 + xd]->rotarypos();
        Cubos[6 + xd]->rotarypos();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7;
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] = aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[3 + xd] = aux6;
        Cubos[6 + xd] = aux3;
    }

    void giro9inv() {
        int xd = 2;
        Cubos[18 + xd]->rotarypos();
        Cubos[21 + xd]->rotarypos();
        Cubos[24 + xd]->rotarypos();
        Cubos[9 + xd]->rotarypos();
        Cubos[12 + xd]->rotarypos();
        Cubos[15 + xd]->rotarypos();
        Cubos[0 + xd]->rotarypos();
        Cubos[3 + xd]->rotarypos();
        Cubos[6 + xd]->rotarypos();
        Cube* aux1 = Cubos[18 + xd];
        Cube* aux2 = Cubos[21 + xd];
        Cube* aux3 = Cubos[24 + xd];
        Cube* aux4 = Cubos[9 + xd];
        Cube* aux5 = Cubos[12 + xd];
        Cube* aux6 = Cubos[15 + xd];
        Cube* aux7 = Cubos[0 + xd];
        Cube* aux8 = Cubos[3 + xd];
        Cube* aux9 = Cubos[6 + xd];
        Cubos[18 + xd] = aux7;
        Cubos[21 + xd] = aux4;
        Cubos[24 + xd] = aux1;
        Cubos[9 + xd] = aux8;
        Cubos[12 + xd] = aux5;
        Cubos[15 + xd] = aux2;
        Cubos[0 + xd] = aux9;
        Cubos[3 + xd] = aux6;
        Cubos[6 + xd] = aux3;
    }



    void ModifyDistance(bool t) {
        if (t) {
            distance += 0.005f;
        }
        else {
            if (distance > 0.400f) distance -= 0.005f;
        }

        float t1 = -distance;
        for (int i = 0; i < 3; ++i) {
            float t2 = -distance;
            for (int j = 0; j < 3; ++j) {
                float t3 = -distance;
                for (int k = 0; k < 3; ++k) {
                    Cubos[(i * 9) + (j * 3) + k]->SetDistance(t2, t3, t1);
                    t3 += distance;
                }
                t2 += distance;
            }
            t1 += distance;
        }
    }

    void Delete() {
        for (int i = 0; i < numCubes; ++i) {
            Cubos[i]->Delete();
        }
        for (int i = 0; i < numCubes; ++i) {
            delete Cubos[i];
        }
    }
};
