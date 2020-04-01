//=============================================================================================
// Mintaprogram: Z�ld h�romsz�g. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!!
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    :
// Neptun :
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char* const vertexSource = R"(
    #version 330                // Shader 3.3
    precision highp float;        // normal floats, makes no difference on desktop computers

    uniform mat4 MVP;            // uniform variable, the Model-View-Projection transformation matrix
    layout(location = 0) in vec2 vp;    // Varying input: vp = vertex position is expected in attrib array 0

    void main() {
        gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;        // transform vp from modeling space to normalized device space
    }
)";

// fragment shader in GLSL
const char* const fragmentSource = R"(
    #version 330            // Shader 3.3
    precision highp float;    // normal floats, makes no difference on desktop computers
    
    uniform vec3 color;        // uniform variable, the color of the primitive
    out vec4 outColor;        // computed color of the current pixel

    void main() {
        outColor = vec4(color, 1);    // computed color is the color of the primitive
    }
)";

GPUProgram gpuProgram;
unsigned int vao;


class Circle {
    const static int numberOfVertices = 500;
    unsigned int vao, vbo;
    vec2 vertices[numberOfVertices];
public:
    void Create(vec2 center, float radius) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        for(int i = 0; i < numberOfVertices; i++) {
            float fi = i * 2 * M_PI / numberOfVertices;
            vertices[i] = vec2(cosf(fi) * radius, sinf(fi) * radius) + center;
        }
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * numberOfVertices, vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0)); 		     // stride, offset: tightly packed
    }

    void Draw(GLenum mode, vec3 color) {
        int location = glGetUniformLocation(gpuProgram.getId(), "color");
        glUniform3f(location, color.x, color.y, color.z); 
        float MVPtransf[4][4] = {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};

        location = glGetUniformLocation(gpuProgram.getId(), "MVP"); 
        glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, numberOfVertices);
    }
};

class SiriusTriangle
{
    vec2 points[3];
    Circle circles[3];
    int numberOfPoints = 0;
    bool calculated = false;

    vec2 FindCircleCenter(vec2 p1, vec2 p2) 
    {
        vec2 c;
        c.y = (
                0.5f - 
                (p2.x + p1.y * p1.y * p2.x) / (2.0f * p1.x) + 
                (p2.x * p2.x + p2.y * p2.y - p1.x * p2.x) / 2.0f
              ) / (p2.y - p1.y * p2.x / p1.x);
        c.x =  (1.0f + p1.y*p1.y) / (2.0f * p1.x) + p1.x / 2.0f - (p1.y * c.y) / p1.x;
        return c;
    }

public:
    void AddPoint(float cX, float cY)
    {
        if(numberOfPoints < 3) {
            points[numberOfPoints++] = vec2(cX, cY);
        }
        if (numberOfPoints == 3 && !calculated)
        {
            calculated = true;
            vec2 A = points[0];
            vec2 B = points[1];
            vec2 C = points[2];

            vec2 aCenter = FindCircleCenter(A,B);
            vec2 bCenter = FindCircleCenter(B,C);
            vec2 cCenter = FindCircleCenter(C,A);

            float aRadius = sqrt(length(aCenter)*length(aCenter)-1.0f);
            float bRadius = sqrt(length(bCenter)*length(bCenter)-1.0f);
            float cRadius = sqrt(length(cCenter)*length(cCenter)-1.0f);
            
            circles[0].Create(aCenter, aRadius);
            circles[1].Create(bCenter, bRadius);
            circles[2].Create(cCenter, cRadius);
        }
    }

    void Draw()
    {
        for(int i = 0; i < 3; i++)
        {
            circles[i].Draw(GL_LINE_STRIP, vec3(0.0f, 1.0f, 1.0f));
        }

    }    
};

Circle unitCircle;
SiriusTriangle siriusTriangle;

void onInitialization()
{
    glViewport(0, 0, windowWidth, windowHeight);
    unitCircle.Create(vec2(0.0f,0.0f), 1.0f);
    gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

void onDisplay()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    unitCircle.Draw(GL_TRIANGLE_FAN, vec3(1.0f, 0.0f, 0.0f));
    siriusTriangle.Draw();
    glutSwapBuffers();
}

void onMouse(int button, int state, int pX, int pY)
{ 
    float cX = 2.0f * pX / windowWidth - 1;
    float cY = 1.0f - 2.0f * pY / windowHeight;

    if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) 
    {
        siriusTriangle.AddPoint(cX, cY);
        glutPostRedisplay();
    }
}

void onKeyboard(unsigned char key, int pX, int pY) {}
void onKeyboardUp(unsigned char key, int pX, int pY) {}
void onMouseMotion(int pX, int pY) {}
void onIdle() {}
