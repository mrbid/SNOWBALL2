/*
--------------------------------------------------
    James William Fletcher (james@voxdsp.com)
        October 2021 - esAux.h v2.0
--------------------------------------------------
    v2.0:
    - added support for fullbright texture mapping

    v1.0:
    An OpenGL ES auxiliary library, in an OpenGL ES style.
    I can't say I wrote every function in this file because
    I didn't, like esCramerInvert() was from some intel pdf
    that is no where to be found online anymore.

    Like anything in life this is an eclection of things that
    "just work" that I've scraped together over the years.

    The quaternion code is mostly from what Tyrone Davison
    taught me at Teesside University, the basic matrix
    functions are nothing special, most of it is just from
    the "OpenGL(R) ES 2.0 Programming Guide" book. Because
    that is what inspired me to make this, I thought the
    helper files provided by Aaftab Munshi, Dan Ginsburg
    and Dave Shreiner where good but incomplete.

    The shaders are the product of a little help from:
    http://www.cs.toronto.edu/~jacobson/phong-demo/
    https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/code/WebGLShaderLightMat/ShaderLightMat.html
    https://www.gsn-lib.org/docs/nodes/ShaderPluginNode.php
    https://www.khronos.org/registry/OpenGL/specs/es/2.0/GLSL_ES_Specification_1.00.pdf
    https://www.khronos.org/opengles/sdk/docs/reference_cards/OpenGL-ES-2_0-Reference-card.pdf
    https://www.khronos.org/registry/OpenGL-Refpages/es2.0/

    John Carmack's Quake3 engine's vector / matrix source
    files actually helped inspire how I decided to simplify
    this function set as well. InvSqrt() nostalgia.

    For a lot of functions I don't take pointers as inputs
    but for some I do, like esTranspose() and esCramerInvert()
    and that's only really because that's how the CramerInvert
    function came originally and that's the same with the
    esMatrixMultiply() function, when I extended it to multiply
    by point and vec I just kept to that standard in those cases.

    And so there we have it, one file with all the basics
    that I believe one needs when making OpenGL ES applications.

    The phong uses BlinnPhong by default, if you wish to use
    vanilla phong just specify the following definition in
    you source file: #define REGULAR_PHONG
*/

#ifndef AUX_H
#define AUX_H

//*************************************
// MATRIX
//*************************************

// structures
typedef struct
{
    GLfloat m[4][4];
} ESMatrix;

typedef struct
{
    GLfloat x,y,z,w;
} ESVector;

typedef struct
{
    GLuint vid; // Vertex Array Buffer ID
    GLuint iid;	// Index Array Buffer ID
    GLuint cid;	// Colour Array Buffer ID
    GLuint nid;	// Normal Array Buffer ID
    GLuint tid;	// TexCoord Array Buffer ID
} ESModel;

// defines
#define PI 3.141592741f         // PI
#define x2PI 6.283185482f       // PI * 2
#define d2PI 1.570796371f       // PI / 2
#define DEGREE 57.29578018f     // 1 Radian as Degrees
#define RADIAN 0.01745329238f   // PI / 180 (1 Degree as Radians

#define FLOAT_MAX 9223372036854775807.0f
#define INV_FLOAT_MAX 1.084202172e-19F

// vector
void vRuv(ESVector* v);   // Random Unit Vector
void vRuvN(ESVector* v);  // Normal Random Unit Vector
void vRuvBT(ESVector* v); // Brian Tung Random Unit Vector
void vRuvTA(ESVector* v); // T.Davison Trial & Error
void vRuvTD(ESVector* v); // T.Davison Random Unit Vector Sphere

void vCross(ESVector* r, const ESVector v1, const ESVector v2);
GLfloat vDot(const ESVector v1, const ESVector v2);

void vNorm(ESVector* v);
GLfloat vDist(const ESVector v1, const ESVector v2);
GLfloat vModulus(const ESVector v);
GLfloat vMagnitude(const ESVector v);
void vInvert(ESVector* v);
void vCopy(ESVector* r, const ESVector v);

void vRotX(ESVector* v, const GLfloat radians);
void vRotY(ESVector* v, const GLfloat radians);
void vRotZ(ESVector* v, const GLfloat radians);

void vAdd(ESVector* r, const ESVector v1, const ESVector v2);
void vSub(ESVector* r, const ESVector v1, const ESVector v2);
void vDiv(ESVector* r, const ESVector denominator, const ESVector numerator);
void vMul(ESVector* r, const ESVector v1, const ESVector v2);

void vAddScalar(ESVector* r, const ESVector v1, const GLfloat v2);
void vSubScalar(ESVector* r, const ESVector v1, const GLfloat v2);
void vDivScalar(ESVector* r, const ESVector v1, const GLfloat v2);
void vMulScalar(ESVector* r, const ESVector v1, const GLfloat v2);

// matrix
void esMatrixLoadIdentity(ESMatrix *result);
void esMatrixCopy(ESMatrix* r, const ESMatrix* v);
void esMatrixMultiply(ESMatrix *result, const ESMatrix *srcA, const ESMatrix *srcB);
void esMatrixMultiplyPoint(ESVector *r, const ESMatrix *srcA, const GLfloat x, const GLfloat y, const GLfloat z);
void esMatrixMultiplyVec(ESVector *r, const ESMatrix *srcA, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w);
void esScale(ESMatrix *result, const GLfloat sx, const GLfloat sy, const GLfloat sz);
void esTranslate(ESMatrix *result, const GLfloat tx, const GLfloat ty, const GLfloat tz);
void esRotate(ESMatrix *result, const GLfloat degree, GLfloat x, GLfloat y, GLfloat z);
void esFrustum(ESMatrix *result, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat nearZ, const GLfloat farZ);
void esPerspective(ESMatrix *result, const GLfloat fovy, const GLfloat aspect, const GLfloat nearZ, const GLfloat farZ);
void esOrtho(ESMatrix *result, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat nearZ, const GLfloat farZ);
void esLookAt(ESMatrix *result, const GLfloat origin_x, const GLfloat origin_y, const GLfloat origin_z, const GLfloat dirnorm_x, const GLfloat dirnorm_y, const GLfloat dirnorm_z);
void esCramerInvert(GLfloat *dst, const GLfloat *mat);
void esTranspose(ESMatrix *r, const ESMatrix* m);
void esSetDirection(ESMatrix *result, const ESVector dir_norm, const ESVector up_norm);
void esGetDirection(ESVector *result, const ESMatrix matrix);
void esGetDirectionX(ESVector *result, const ESMatrix matrix);
void esGetDirectionY(ESVector *result, const ESMatrix matrix);
void esGetDirectionZ(ESVector *result, const ESMatrix matrix);
void esGetViewDirection(ESVector *result, const ESMatrix matrix);

// utility functions
float randf();  // uniform
float randfn(); // box-muller normal
int ftoi(float f); // float to integer quantise
GLfloat esRandFloat(const GLfloat min, const GLfloat max);
void esBind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage);
void esBindModel(ESModel* model, const GLfloat* vertices, const GLsizei vertlen, const GLushort* indices, const GLsizei indlen);
GLuint esLoadTexture(const GLuint w, const GLuint h, const unsigned char* data);

// quaternion
void qMatrix(ESMatrix* r, const ESVector q);
void qCovert(ESVector* r, const ESMatrix m);
void qAngle(ESVector* r, const GLfloat x, const GLfloat y, const GLfloat z, GLfloat radians);
void qDirection(ESVector* rq, const ESVector vn);
void qMul(ESVector* r, const ESVector lhs, const ESVector rhs);
GLfloat qDot(const ESVector q1, const ESVector q2);
void qNorm(ESVector* q);
void qInvert(ESVector* q);
void qRotV(ESVector* v, const ESVector q);

//*************************************
// SHADER
//*************************************

void makeAllShaders();

void makeFullbright();
void makeLambert();
void makeLambert1();
void makeLambert2();
void makeLambert3();
void makePhong();
void makePhong1();
void makePhong2();
void makePhong3();

void shadeFullbrightT(GLint* position, GLint* projection, GLint* modelview, GLint* texcoord, GLint* sampler);                             // texture + no shading
void shadeFullbright(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* opacity);                                 // solid color + no shading

void shadeLambert(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* color, GLint* opacity);                   // solid color + no normals
void shadeLambert1(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity);   // solid color + normals
void shadeLambert2(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* color, GLint* opacity);                  // colors + no normals
void shadeLambert3(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity);   // colors + normals

void shadePhong(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* color, GLint* opacity);                   // solid color + no normals
void shadePhong1(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity);   // solid color + normals
void shadePhong2(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* color, GLint* opacity);                  // colors + no normals
void shadePhong3(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity);   // colors + normals

//*************************************
// MATRIX CODE
//*************************************

void vRuv(ESVector* v)
{
    v->x = (randf() * 2.f) - 1.f;
    v->y = (randf() * 2.f) - 1.f;
    v->z = (randf() * 2.f) - 1.f;
}

void vRuvN(ESVector* v)
{
    v->x = randfn();
    v->y = randfn();
    v->z = randfn();
}

void vRuvBT(ESVector* v)
{
    // https://math.stackexchange.com/a/1586185
    // or should I have called this vRuvLR()
    // https://mathworld.wolfram.com/SpherePointPicking.html
    const float y = acos((randf() * 2.f) - 1.f) - d2PI;
    const float p = x2PI * randf();
    v->x = cos(y) * cos(p);
    v->y = cos(y) * sin(p);
    v->z = sin(y);
}

void vRuvTA(ESVector* v)
{
    // T.P.Davison@tees.ac.uk
    while(1)
    {
        v->x = (randf() * 2.f) - 1.f;
        v->y = (randf() * 2.f) - 1.f;
        v->z = (randf() * 2.f) - 1.f;
        const float len = vMagnitude(*v);
        if(len <= 1.0f){return;}
    }
}

void vRuvTD(ESVector* v)
{
    // T.P.Davison@tees.ac.uk
    v->x = sin((randf() * x2PI) - PI);
    v->y = cos((randf() * x2PI) - PI);
    v->z = (randf() * 2.f) - 1.f;
}

void vCross(ESVector* r, const ESVector v1, const ESVector v2)
{
    r->x = (v1.y * v2.z) - (v2.y * v1.z);
    r->y = -((v1.x * v2.z) - (v2.x * v1.z));
    r->z = (v1.x * v2.y) - (v2.x * v1.y);
}

GLfloat vDot(const ESVector v1, const ESVector v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

void vNorm(ESVector* v)
{
    const GLfloat len = 1.0f/sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x *= len;
    v->y *= len;
    v->z *= len;
}

GLfloat vDist(const ESVector v1, const ESVector v2)
{
    const float xm = (v1.x - v2.x);
    const float ym = (v1.y - v2.y);
    const float zm = (v1.z - v2.z);
    return sqrt(xm*xm + ym*ym + zm*zm);
}

GLfloat vModulus(const ESVector v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

GLfloat vMagnitude(const ESVector v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

void vInvert(ESVector* v)
{
    v->x = -v->x;
    v->y = -v->y;
    v->z = -v->z;
}

void vCopy(ESVector* r, const ESVector v)
{
    memcpy(r, &v, sizeof(ESVector));
}

void vRotX(ESVector* v, const GLfloat radians)
{
    v->y = v->y * cos(radians) + v->z * sin(radians);
    v->z = v->y * sin(radians) - v->z * cos(radians);
}

void vRotY(ESVector* v, const GLfloat radians)
{
    v->x = v->z * sin(radians) - v->x * cos(radians);
    v->z = v->z * cos(radians) + v->x * sin(radians);
}

void vRotZ(ESVector* v, const GLfloat radians)
{
    v->x = v->x * cos(radians) + v->y * sin(radians);
    v->y = v->x * sin(radians) - v->y * cos(radians);
}

void vAdd(ESVector* r, const ESVector v1, const ESVector v2)
{
    r->x = v1.x + v2.x;
    r->y = v1.y + v2.y;
    r->z = v1.z + v2.z;
}

void vSub(ESVector* r, const ESVector v1, const ESVector v2)
{
    r->x = v1.x - v2.x;
    r->y = v1.y - v2.y;
    r->z = v1.z - v2.z;
}

void vDiv(ESVector* r, const ESVector denominator, const ESVector numerator)
{
    r->x = denominator.x / numerator.x;
    r->y = denominator.y / numerator.y;
    r->z = denominator.z / numerator.z;
}

void vMul(ESVector* r, const ESVector v1, const ESVector v2)
{
    r->x = v1.x * v2.x;
    r->y = v1.y * v2.y;
    r->z = v1.z * v2.z;
}

void vAddScalar(ESVector* r, const ESVector v1, const GLfloat v2)
{
    r->x = v1.x + v2;
    r->y = v1.y + v2;
    r->z = v1.z + v2;
}

void vSubScalar(ESVector* r, const ESVector v1, const GLfloat v2)
{
    r->x = v1.x - v2;
    r->y = v1.y - v2;
    r->z = v1.z - v2;
}

void vDivScalar(ESVector* r, const ESVector v1, const GLfloat v2)
{
    r->x = v1.x / v2;
    r->y = v1.y / v2;
    r->z = v1.z / v2;
}

void vMulScalar(ESVector* r, const ESVector v1, const GLfloat v2)
{
    r->x = v1.x * v2;
    r->y = v1.y * v2;
    r->z = v1.z * v2;
}

///

void esMatrixLoadIdentity(ESMatrix *result)
{
    memset(result, 0x0, sizeof(ESMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void esMatrixCopy(ESMatrix* r, const ESMatrix* v)
{
    memcpy(r, v, sizeof(ESMatrix));
}

void esMatrixMultiply(ESMatrix *result, const ESMatrix *srcA, const ESMatrix *srcB)
{
    ESMatrix tmp;
    for(int i=0; i<4; i++)
    {
        tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
                        (srcA->m[i][1] * srcB->m[1][0]) +
                        (srcA->m[i][2] * srcB->m[2][0]) +
                        (srcA->m[i][3] * srcB->m[3][0]) ;

        tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) + 
                        (srcA->m[i][1] * srcB->m[1][1]) +
                        (srcA->m[i][2] * srcB->m[2][1]) +
                        (srcA->m[i][3] * srcB->m[3][1]) ;

        tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) + 
                        (srcA->m[i][1] * srcB->m[1][2]) +
                        (srcA->m[i][2] * srcB->m[2][2]) +
                        (srcA->m[i][3] * srcB->m[3][2]) ;

        tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) + 
                        (srcA->m[i][1] * srcB->m[1][3]) +
                        (srcA->m[i][2] * srcB->m[2][3]) +
                        (srcA->m[i][3] * srcB->m[3][3]) ;
    }
    memcpy(result, &tmp, sizeof(ESMatrix));
}

void esMatrixMultiplyPoint(ESVector *r, const ESMatrix *srcA, const GLfloat x, const GLfloat y, const GLfloat z)
{
    r->x =  (srcA->m[0][0] * x) +
            (srcA->m[0][1] * x) +
            (srcA->m[0][2] * x) +
            (srcA->m[0][3] * x) ;

    r->y =  (srcA->m[1][0] * y) +
            (srcA->m[1][1] * y) +
            (srcA->m[1][2] * y) +
            (srcA->m[1][3] * y) ;

    r->z =  (srcA->m[2][0] * z) +
            (srcA->m[2][1] * z) +
            (srcA->m[2][2] * z) +
            (srcA->m[2][3] * z) ;
}

void esMatrixMultiplyVec(ESVector *r, const ESMatrix *srcA, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
{
    r->x =  (srcA->m[0][0] * x) +
            (srcA->m[0][1] * x) +
            (srcA->m[0][2] * x) +
            (srcA->m[0][3] * x) ;

    r->y =  (srcA->m[1][0] * y) +
            (srcA->m[1][1] * y) +
            (srcA->m[1][2] * y) +
            (srcA->m[1][3] * y) ;

    r->z =  (srcA->m[2][0] * z) +
            (srcA->m[2][1] * z) +
            (srcA->m[2][2] * z) +
            (srcA->m[2][3] * z) ;

    r->w =  (srcA->m[3][0] * w) +
            (srcA->m[3][1] * w) +
            (srcA->m[3][2] * w) +
            (srcA->m[3][3] * w) ;
}

void esScale(ESMatrix *result, const GLfloat sx, const GLfloat sy, const GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

void esTranslate(ESMatrix *result, const GLfloat tx, const GLfloat ty, const GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void esRotate(ESMatrix *result, const GLfloat degree, GLfloat x, GLfloat y, GLfloat z)
{
    const float mag = 1.f/sqrtf(x * x + y * y + z * z);
    const float sinAngle = sinf(degree * RADIAN);
    const float cosAngle = cosf(degree * RADIAN);
    if(mag > 0.0f)
    {
        x *= mag;
        y *= mag;
        z *= mag;

        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;
        const float xy = x * y;
        const float yz = y * z;
        const float zx = z * x;
        const float xs = x * sinAngle;
        const float ys = y * sinAngle;
        const float zs = z * sinAngle;
        const float oneMinusCos = 1.0f - cosAngle;

        ESMatrix rotMat;
        rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.m[0][1] = (oneMinusCos * xy) - zs;
        rotMat.m[0][2] = (oneMinusCos * zx) + ys;
        rotMat.m[0][3] = 0.0F; 

        rotMat.m[1][0] = (oneMinusCos * xy) + zs;
        rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.m[1][2] = (oneMinusCos * yz) - xs;
        rotMat.m[1][3] = 0.0F;

        rotMat.m[2][0] = (oneMinusCos * zx) - ys;
        rotMat.m[2][1] = (oneMinusCos * yz) + xs;
        rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.m[2][3] = 0.0F; 

        rotMat.m[3][0] = 0.0F;
        rotMat.m[3][1] = 0.0F;
        rotMat.m[3][2] = 0.0F;
        rotMat.m[3][3] = 1.0F;

        esMatrixMultiply( result, &rotMat, result );
    }
}

void esFrustum(ESMatrix *result, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat nearZ, const GLfloat farZ)
{
    GLfloat     deltaX = right - left;
    GLfloat     deltaY = top - bottom;
    GLfloat     deltaZ = farZ - nearZ;
    ESMatrix    frust;

    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
         (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
         return;

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    esMatrixMultiply(result, &frust, result);
}

void esPerspective(ESMatrix *result, const GLfloat fovy, const GLfloat aspect, const GLfloat nearZ, const GLfloat farZ)
{
   GLfloat frustumW, frustumH;
   
   frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void esOrtho(ESMatrix *result, const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat nearZ, const GLfloat farZ)
{
    GLfloat     deltaX = right - left;
    GLfloat     deltaY = top - bottom;
    GLfloat     deltaZ = farZ - nearZ;
    ESMatrix    ortho;

    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;

    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    esMatrixMultiply(result, &ortho, result);
}

void esLookAt(ESMatrix *result, const GLfloat origin_x, const GLfloat origin_y, const GLfloat origin_z, const GLfloat dirnorm_x, const GLfloat dirnorm_y, const GLfloat dirnorm_z)
{
    ESVector up;
    up.x = 0.f, up.y = 1.f, up.z = 0.f;

    ESVector dirn;
    dirn.x = dirnorm_x;
    dirn.y = dirnorm_y;
    dirn.z = dirnorm_z;

    ESVector c;
    vCross(&c, up, dirn);
    vNorm(&c);

    ESVector rup;
    vCross(&rup, dirn, c);

    result->m[0][0] = c.x;
    result->m[0][1] = c.y;
    result->m[0][2] = c.z;

    result->m[1][0] = rup.x;
    result->m[1][1] = rup.y;
    result->m[1][2] = rup.z;

    result->m[2][0] = dirn.x;
    result->m[2][1] = dirn.y;
    result->m[2][2] = dirn.z;

    result->m[3][0] = origin_x;
    result->m[3][1] = origin_y;
    result->m[3][2] = origin_z;
}

void esCramerInvert(GLfloat *dst, const GLfloat *mat)
{
    GLfloat tmp[12]; /* temp array for pairs */
    GLfloat src[16]; /* array of transpose source matrix */
    GLfloat det; /* determinant */

    /* transpose matrix */
    for(int i = 0; i < 4; i++)
    {
        src[i] = mat[i*4];
        src[i + 4] = mat[i*4 + 1];
        src[i + 8] = mat[i*4 + 2];
        src[i + 12] = mat[i*4 + 3];
    }

    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];

    /* calculate first 8 elements (cofactors) */
    dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
    dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
    dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
    dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
    dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
    dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
    dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
    dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
    dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
    dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
    dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
    dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
    dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
    dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
    dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
    dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];

    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src[2]*src[7];
    tmp[1] = src[3]*src[6];
    tmp[2] = src[1]*src[7];
    tmp[3] = src[3]*src[5];
    tmp[4] = src[1]*src[6];
    tmp[5] = src[2]*src[5];
    tmp[6] = src[0]*src[7];
    tmp[7] = src[3]*src[4];
    tmp[8] = src[0]*src[6];
    tmp[9] = src[2]*src[4];
    tmp[10] = src[0]*src[5];
    tmp[11] = src[1]*src[4];

    /* calculate second 8 elements (cofactors) */
    dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
    dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
    dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
    dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
    dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
    dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
    dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
    dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
    dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
    dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
    dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
    dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
    dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
    dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
    dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
    dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

    /* calculate determinant */
    det = src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];

    /* calculate matrix inverse */
    det = 1.0f/det;
    for(int j = 0; j < 16; j++){dst[j] *= det;}
}

void esTranspose(ESMatrix *r, const ESMatrix* m)
{
    r->m[1][0] = m->m[0][1];
    r->m[2][0] = m->m[0][2];
    r->m[3][0] = m->m[0][3];

    r->m[0][1] = m->m[1][0];
    r->m[1][1] = m->m[1][1];
    r->m[2][1] = m->m[1][2];
    r->m[3][1] = m->m[1][3];

    r->m[0][2] = m->m[2][0];
    r->m[1][2] = m->m[2][1];
    r->m[2][2] = m->m[2][2];
    r->m[3][2] = m->m[2][3];

    r->m[0][3] = m->m[3][0];
    r->m[1][3] = m->m[3][1];
    r->m[2][3] = m->m[3][2];
    r->m[3][3] = m->m[3][3];
}

void esSetDirection(ESMatrix *result, const ESVector dir_norm, const ESVector up_norm)
{
    ESVector c;
    vCross(&c, up_norm, dir_norm);
    vNorm(&c);

    ESVector rup;
    vCross(&rup, dir_norm, c);

    result->m[0][0] = c.x;
    result->m[0][1] = c.y;
    result->m[0][2] = c.z;

    result->m[1][0] = rup.x;
    result->m[1][1] = rup.y;
    result->m[1][2] = rup.z;

    result->m[2][0] = dir_norm.x;
    result->m[2][1] = dir_norm.y;
    result->m[2][2] = dir_norm.z;
}

void esGetDirectionX(ESVector *result, const ESMatrix matrix)
{
    result->x = matrix.m[0][0];
    result->y = matrix.m[0][1];
    result->z = matrix.m[0][2];
}

void esGetDirectionY(ESVector *result, const ESMatrix matrix)
{
    result->x = matrix.m[1][0];
    result->y = matrix.m[1][1];
    result->z = matrix.m[1][2];
}

void esGetDirectionZ(ESVector *result, const ESMatrix matrix)
{
    result->x = matrix.m[2][0];
    result->y = matrix.m[2][1];
    result->z = matrix.m[2][2];
}

void esGetDirection(ESVector *result, const ESMatrix matrix)
{
    result->x = matrix.m[2][0];
    result->y = matrix.m[2][1];
    result->z = matrix.m[2][2];
}

void esGetViewDirection(ESVector *result, const ESMatrix matrix)
{
    result->x = -matrix.m[0][2];
    result->y = -matrix.m[1][2];
    result->z = -matrix.m[2][2];
}

///

int srandfq = 8008135;
static inline void srandf(const int seed)
{
    srandfq = seed;
}

float randf()
{
    // https://www.musicdsp.org/en/latest/Other/273-fast-float-random-numbers.html
    // moc.liamg@seir.kinimod
    srandfq *= 16807;
    return (float)(srandfq & 0x7FFFFFFF) * 4.6566129e-010f;
}

float randfn()
{
    float u = randf() * 2 - 1;
    float v = randf() * 2 - 1;
    float r = u * u + v * v;
    while(r == 0 || r > 1)
    {
        u = randf() * 2 - 1;
        v = randf() * 2 - 1;
        r = u * u + v * v;
    }
    return u * sqrt(-2 * log(r) / r);
}

int vec_ftoi(float f)
{
    if(f < 0)
        f -= 0.5f;
    else
        f += 0.5f;
    return (int)f;
}

GLfloat esRandFloat(const GLfloat min, const GLfloat max)
{
    static GLfloat rndmax = (GLfloat)RAND_MAX;
    return ( (((GLfloat)rand())+1e-7f / rndmax) * (max-min) ) + min;
}

void esBind(const GLenum target, GLuint* buffer, const void* data, const GLsizeiptr datalen, const GLenum usage)
{
    glGenBuffers(1, buffer);
    glBindBuffer(target, *buffer);
    glBufferData(target, datalen, data, usage);
}

void esBindModel(ESModel* model, const GLfloat* vertices, const GLsizei vertlen, const GLushort* indices, const GLsizei indlen)
{
    esBind(GL_ARRAY_BUFFER, &model->vid, vertices, vertlen * sizeof(GLfloat) * 3, GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &model->iid, indices, indlen * sizeof(GLushort), GL_STATIC_DRAW);
}

GLuint esLoadTexture(const GLuint w, const GLuint h, const unsigned char* data)
{
   GLuint textureId;

   // Use tightly packed data
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Generate a texture object
   glGenTextures(1, &textureId);

   // Bind the texture object
   glBindTexture(GL_TEXTURE_2D, textureId);

   // Load the texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

   // Set the filtering mode
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   return textureId;
}

///

// quaternion to matrix
void qMatrix(ESMatrix* r, const ESVector q)
{
    float m00, m01, m02, m10, m11, m12, m20, m21, m22;

    const float sqw = q.w*q.w;
    const float sqx = q.x*q.x;
    const float sqy = q.y*q.y;
    const float sqz = q.z*q.z;

    m00 = ( sqx - sqy - sqz + sqw);
    m11 = (-sqx + sqy - sqz + sqw);
    m22 = (-sqx - sqy + sqz + sqw);

    float tmp1 = q.x*q.y;
    float tmp2 = q.z*q.w;
    m10 = 2.0f * (tmp1 + tmp2);
    m01 = 2.0f * (tmp1 - tmp2);

    tmp1 = q.x*q.z;
    tmp2 = q.y*q.w;
    m20 = 2.0f * (tmp1 - tmp2);
    m02 = 2.0f * (tmp1 + tmp2);
    tmp1 = q.y*q.z;
    tmp2 = q.x*q.w;
    m21 = 2.0f * (tmp1 + tmp2);
    m12 = 2.0f * (tmp1 - tmp2);

    r->m[0][0] = m00;
    r->m[0][1] = m01;
    r->m[0][2] = m02;
    
    r->m[1][0] = m10;
    r->m[1][1] = m11;
    r->m[1][2] = m12;

    r->m[2][0] = m20;
    r->m[2][1] = m21;
    r->m[2][2] = m22;
}

// axis and angle to quaternion
void qAngle(ESVector* r, const GLfloat x, const GLfloat y, const GLfloat z, GLfloat radians)
{
    ESVector vn;
    vn.x = x;
    vn.y = y;
    vn.z = z;
    vNorm(&vn);
    const GLfloat ha = radians * 0.5f;
    const GLfloat sa = sin(ha);
    r->w = cos(ha);
    r->x = vn.x * sa;
    r->y = vn.y * sa;
    r->z = vn.z * sa;
}

// multiply quaternions
void qMul(ESVector* r, const ESVector lhs, const ESVector rhs)
{
    ESVector c;
    vCross(&c, lhs, rhs);

    r->w = rhs.w * vDot(lhs, rhs);
    r->x = lhs.w * rhs.x + rhs.w * lhs.x + c.x;
    r->y = lhs.w * rhs.y + rhs.w * lhs.y + c.y;
    r->z = lhs.w * rhs.z + rhs.w * lhs.z + c.z;
}

// quaternion dot product
GLfloat qDot(const ESVector q1, const ESVector q2)
{
    return q1.w * q2.w + vDot(q1, q2);
}

// quaternion normalize
void qNorm(ESVector* q)
{
    const GLfloat modulus = 1.0f / sqrt(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);

    q->w *= modulus;
    q->x *= modulus;
    q->y *= modulus;
    q->z *= modulus;
}

// quaternion invert
void qInvert(ESVector* q)
{
    q->x = -q->x;
    q->y = -q->y;
    q->z = -q->z;
}

// direction to normalised quaternion (lacking accuracy)
void qDirection(ESVector* rq, const ESVector vn)
{
    ESVector up;
    up.x = 0.f, up.y = 1.f, up.z = 0.f;

    ESVector c;
    vCross(&c, up, vn);
    vNorm(&c);

    ESVector rup;
    vCross(&rup, vn, c);

    const GLfloat w = sqrt(1.0f + c.x + rup.y + vn.z) * 0.5f;
    const GLfloat rw4 = 1.0f / (4.0f * w);

    rq->w = w;
    rq->x = (vn.y - rup.z) * rw4;
    rq->y = (c.z - vn.x) * rw4;
    rq->z = (rup.x - c.y) * rw4;
}

// matrix to quaternion
void qCovert(ESVector* r, const ESMatrix m)
{
    const GLfloat w = sqrt(1.0f + m.m[0][0] + m.m[1][1] + m.m[2][2]) * 0.5f;
    const GLfloat rw4 = 1.0f / (4.0f * w);
    r->w = w;
    r->x = (m.m[2][1] - m.m[1][2]) * rw4;
    r->y = (m.m[0][2] - m.m[2][0]) * rw4;
    r->z = (m.m[1][0] - m.m[0][1]) * rw4;
}

// rotate unit vector by unit quaternion
void qRotV(ESVector* vn, const ESVector qn) // not tested
{
    const GLfloat s = qn.w;
    const GLfloat a = 2.0f *  vDot(qn, *vn);
    const GLfloat s2du = s*s - vDot(qn, qn);
    const GLfloat x2s = 2.0f * s;
    ESVector c;
    vCross(&c, qn, *vn);

    vn->x = a * qn.x + s2du * vn->x + x2s * c.x;
    vn->y = a * qn.y + s2du * vn->y + x2s * c.y;
    vn->z = a * qn.z + s2du * vn->z + x2s * c.z;
}

//*************************************
// SHADER CODE
//*************************************

const GLchar* vt0 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "attribute vec4 position;\n"
    "attribute vec2 texcoord;\n"
    "varying vec2 vtc;\n"
    "void main()\n"
    "{\n"
        "vtc = texcoord;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

const GLchar* ft0 =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 vtc;\n"
    "uniform sampler2D tex;\n"
    "void main()\n"
    "{\n"
        "gl_FragColor = texture2D(tex, vtc);\n"
    "}\n";

//

const GLchar* v0 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 color;\n"
    "uniform float opacity;\n"
    "attribute vec4 position;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "void main()\n"
    "{\n"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

const GLchar* f0 =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "void main()\n"
    "{\n"
        "gl_FragColor = vec4(vertCol, vertOpa);\n"
    "}\n";

//

// solid color + no normals
const GLchar* v1 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 color;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vec3(vertPos4) / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(position.xyz, 0.0));\n"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

// solid color + normal array
const GLchar* v11 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform vec3 color;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vec3(vertPos4) / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(normal.xyz, 0.0));\n"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

// color array + normal array
const GLchar* v12 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "attribute vec3 color;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vec3(vertPos4) / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(normal.xyz, 0.0));\n"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

// color array + no normals
const GLchar* v13 =
    "#version 100\n"
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 color;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;\n"
        "vertPos = vec3(vertPos4) / vertPos4.w;\n"
        "vertNorm = vec3(modelview * vec4(position.xyz, 0.0));\n"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "gl_Position = projection * modelview * position;\n"
    "}\n";

const GLchar* f1 =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertNorm;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec3 ambientColor = vertCol * 0.1;\n"
        "vec3 diffuseColor = vertCol;\n"
        "vec3 normal = normalize(vertNorm);\n"
        "vec3 lightDir = normalize(vlightPos - vertPos);\n"
        "float lambertian = max(dot(lightDir,normal), 0.0);\n"
        "gl_FragColor = vec4(ambientColor + lambertian*diffuseColor, vertOpa);\n"
    "}\n";

const GLchar* v2 = 
    "#version 100\n" 
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 normalmat;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "uniform vec3 color;\n"
    "attribute vec4 position;\n"
    "varying vec3 normalInterp;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;"
        "vertPos = vec3(vertPos4) / vertPos4.w;"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "normalInterp = vec3(normalmat * vec4(position.xyz, 0.0));"
        "gl_Position = projection * modelview * position;"
    "}\n";

const GLchar* v21 = 
    "#version 100\n" 
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 normalmat;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "uniform vec3 color;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "varying vec3 normalInterp;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;"
        "vertPos = vec3(vertPos4) / vertPos4.w;"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "normalInterp = vec3(normalmat * vec4(normal.xyz, 0.0));"
        "gl_Position = projection * modelview * position;"
    "}\n";

const GLchar* v22 = 
    "#version 100\n" 
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 normalmat;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 color;\n"
    "varying vec3 normalInterp;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;"
        "vertPos = vec3(vertPos4) / vertPos4.w;"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "normalInterp = vec3(normalmat * vec4(position.xyz, 0.0));"
        "gl_Position = projection * modelview * position;"
    "}\n";

const GLchar* v23 = 
    "#version 100\n" 
    "uniform mat4 modelview;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 normalmat;\n"
    "uniform float opacity;\n"
    "uniform vec3 lightpos;\n"
    "attribute vec4 position;\n"
    "attribute vec3 normal;\n"
    "attribute vec3 color;\n"
    "varying vec3 normalInterp;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec4 vertPos4 = modelview * position;"
        "vertPos = vec3(vertPos4) / vertPos4.w;"
        "vertCol = color;\n"
        "vertOpa = opacity;\n"
        "vlightPos = lightpos;\n"
        "normalInterp = vec3(normalmat * vec4(normal.xyz, 0.0));"
        "gl_Position = projection * modelview * position;"
    "}\n";
   
const GLchar* f2 = 
    "#version 100\n" 
    "precision mediump float;\n"
    "varying vec3 normalInterp;\n"
    "varying vec3 vertPos;\n"
    "varying vec3 vertCol;\n"
    "varying float vertOpa;\n"
    "varying vec3 vlightPos;\n"
    "void main()\n"
    "{\n"
        "vec3 ambientColor = vertCol * 0.1;\n"
        "vec3 diffuseColor = vertCol;\n"
        "vec3 specColor = vec3(1.0, 1.0, 1.0);\n"
        "float specAmount = 4.0;\n"
        "vec3 normal = normalize(normalInterp);\n"
        "vec3 lightDir = normalize(vlightPos - vertPos);\n"
        "vec3 viewDir = normalize(-vertPos);\n"
#ifdef REGULAR_PHONG
        "vec3 reflectDir = reflect(-lightDir, normal);\n"
#else
        "vec3 halfDir = normalize(viewDir + lightDir);\n"
#endif
        "float lumosity = dot(lightDir, normal);\n" // [0] you can max this or
        "vec3 specular = diffuseColor;\n"
        "if(lumosity > 0.0)\n"
        "{\n"
#ifdef REGULAR_PHONG
            "float specAngle = max(dot(reflectDir, viewDir), 0.0);\n"
#else
            "float specAngle = max(dot(halfDir, normal), 0.0);\n"
#endif
            "specular += pow(specAngle, specAmount) * specColor;\n"
        "}\n"
        "gl_FragColor = vec4(ambientColor + max(specular * lumosity, 0.0), vertOpa);\n" // [0] .. you can max this
    "}\n";

//

GLuint shdFullbrightT;
GLint  shdFullbrightT_position;
GLint  shdFullbrightT_projection;
GLint  shdFullbrightT_modelview;
GLint  shdFullbrightT_texcoord;
GLint  shdFullbrightT_sampler;
GLuint shdFullbright;
GLint  shdFullbright_position;
GLint  shdFullbright_projection;
GLint  shdFullbright_modelview;
GLint  shdFullbright_color;
GLint  shdFullbright_opacity;
GLuint shdLambert;
GLint  shdLambert_position;
GLint  shdLambert_projection;
GLint  shdLambert_modelview;
GLint  shdLambert_lightpos;
GLint  shdLambert_color;
GLint  shdLambert_opacity;
GLuint shdLambert1;
GLint  shdLambert1_position;
GLint  shdLambert1_projection;
GLint  shdLambert1_modelview;
GLint  shdLambert1_lightpos;
GLint  shdLambert1_color;
GLint  shdLambert1_normal;
GLint  shdLambert1_opacity;
GLuint shdLambert2;
GLint  shdLambert2_position;
GLint  shdLambert2_projection;
GLint  shdLambert2_modelview;
GLint  shdLambert2_lightpos;
GLint  shdLambert2_color;
GLint  shdLambert2_normal;
GLint  shdLambert2_opacity;
GLuint shdLambert3;
GLint  shdLambert3_position;
GLint  shdLambert3_projection;
GLint  shdLambert3_modelview;
GLint  shdLambert3_lightpos;
GLint  shdLambert3_color;
GLint  shdLambert3_opacity;
GLuint shdPhong;
GLint  shdPhong_position;
GLint  shdPhong_projection;
GLint  shdPhong_modelview;
GLint  shdPhong_normalmat;
GLint  shdPhong_lightpos;
GLint  shdPhong_color;
GLint  shdPhong_opacity;
GLuint shdPhong1;
GLint  shdPhong1_position;
GLint  shdPhong1_projection;
GLint  shdPhong1_modelview;
GLint  shdPhong1_normalmat;
GLint  shdPhong1_lightpos;
GLint  shdPhong1_color;
GLint  shdPhong1_normal;
GLint  shdPhong1_opacity;
GLuint shdPhong2;
GLint  shdPhong2_position;
GLint  shdPhong2_projection;
GLint  shdPhong2_modelview;
GLint  shdPhong2_normalmat;
GLint  shdPhong2_lightpos;
GLint  shdPhong2_color;
GLint  shdPhong2_opacity;
GLuint shdPhong3;
GLint  shdPhong3_position;
GLint  shdPhong3_projection;
GLint  shdPhong3_modelview;
GLint  shdPhong3_normalmat;
GLint  shdPhong3_lightpos;
GLint  shdPhong3_color;
GLint  shdPhong3_normal;
GLint  shdPhong3_opacity;

//
void makeFullbrightT()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vt0, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &ft0, NULL);
    glCompileShader(fragmentShader);

    shdFullbrightT = glCreateProgram();
        glAttachShader(shdFullbrightT, vertexShader);
        glAttachShader(shdFullbrightT, fragmentShader);
    glLinkProgram(shdFullbrightT);

    shdFullbrightT_position   = glGetAttribLocation(shdFullbrightT,  "position");
    shdFullbrightT_texcoord   = glGetAttribLocation(shdFullbrightT,  "texcoord");

    shdFullbrightT_projection = glGetUniformLocation(shdFullbrightT, "projection");
    shdFullbrightT_modelview  = glGetUniformLocation(shdFullbrightT, "modelview");
    shdFullbrightT_sampler    = glGetUniformLocation(shdFullbrightT, "tex");
}


void makeFullbright()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v0, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f0, NULL);
    glCompileShader(fragmentShader);

    shdFullbright = glCreateProgram();
        glAttachShader(shdFullbright, vertexShader);
        glAttachShader(shdFullbright, fragmentShader);
    glLinkProgram(shdFullbright);

    shdFullbright_position = glGetAttribLocation(shdFullbright, "position");
    
    shdFullbright_projection = glGetUniformLocation(shdFullbright, "projection");
    shdFullbright_modelview = glGetUniformLocation(shdFullbright, "modelview");
    shdFullbright_color = glGetUniformLocation(shdFullbright, "color");
    shdFullbright_opacity = glGetUniformLocation(shdFullbright, "opacity");
}

void makeLambert()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v1, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambert = glCreateProgram();
        glAttachShader(shdLambert, vertexShader);
        glAttachShader(shdLambert, fragmentShader);
    glLinkProgram(shdLambert);

    shdLambert_position = glGetAttribLocation(shdLambert, "position");
    
    shdLambert_projection = glGetUniformLocation(shdLambert, "projection");
    shdLambert_modelview = glGetUniformLocation(shdLambert, "modelview");
    shdLambert_lightpos = glGetUniformLocation(shdLambert, "lightpos");
    shdLambert_color = glGetUniformLocation(shdLambert, "color");
    shdLambert_opacity = glGetUniformLocation(shdLambert, "opacity");
}

void makeLambert1()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v11, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambert1 = glCreateProgram();
        glAttachShader(shdLambert1, vertexShader);
        glAttachShader(shdLambert1, fragmentShader);
    glLinkProgram(shdLambert1);

    shdLambert1_position = glGetAttribLocation(shdLambert1, "position");
    shdLambert1_normal = glGetAttribLocation(shdLambert1, "normal");
    
    shdLambert1_projection = glGetUniformLocation(shdLambert1, "projection");
    shdLambert1_modelview = glGetUniformLocation(shdLambert1, "modelview");
    shdLambert1_lightpos = glGetUniformLocation(shdLambert1, "lightpos");
    shdLambert1_color = glGetUniformLocation(shdLambert1, "color");
    shdLambert1_opacity = glGetUniformLocation(shdLambert1, "opacity");
}

void makeLambert2()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v1, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambert3 = glCreateProgram();
        glAttachShader(shdLambert3, vertexShader);
        glAttachShader(shdLambert3, fragmentShader);
    glLinkProgram(shdLambert3);

    shdLambert3_position = glGetAttribLocation(shdLambert3, "position");
    shdLambert3_color = glGetAttribLocation(shdLambert3, "color");
    
    shdLambert3_projection = glGetUniformLocation(shdLambert3, "projection");
    shdLambert3_modelview = glGetUniformLocation(shdLambert3, "modelview");
    shdLambert3_lightpos = glGetUniformLocation(shdLambert3, "lightpos");
    shdLambert3_opacity = glGetUniformLocation(shdLambert3, "opacity");
}

void makeLambert3()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v12, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f1, NULL);
    glCompileShader(fragmentShader);

    shdLambert2 = glCreateProgram();
        glAttachShader(shdLambert2, vertexShader);
        glAttachShader(shdLambert2, fragmentShader);
    glLinkProgram(shdLambert2);

    shdLambert2_position = glGetAttribLocation(shdLambert2, "position");
    shdLambert2_normal = glGetAttribLocation(shdLambert2, "normal");
    shdLambert2_color = glGetAttribLocation(shdLambert2, "color");
    
    shdLambert2_projection = glGetUniformLocation(shdLambert2, "projection");
    shdLambert2_modelview = glGetUniformLocation(shdLambert2, "modelview");
    shdLambert2_lightpos = glGetUniformLocation(shdLambert2, "lightpos");
    shdLambert2_opacity = glGetUniformLocation(shdLambert2, "opacity");
}

void makePhong()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v2, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f2, NULL);
    glCompileShader(fragmentShader);

    shdPhong = glCreateProgram();
        glAttachShader(shdPhong, vertexShader);
        glAttachShader(shdPhong, fragmentShader);
    glLinkProgram(shdPhong);

    shdPhong_position = glGetAttribLocation(shdPhong, "position");
    
    shdPhong_projection = glGetUniformLocation(shdPhong, "projection");
    shdPhong_modelview = glGetUniformLocation(shdPhong, "modelview");
    shdPhong_normalmat = glGetUniformLocation(shdPhong, "normalmat");
    shdPhong_lightpos = glGetUniformLocation(shdPhong, "lightpos");
    shdPhong_opacity = glGetUniformLocation(shdPhong, "opacity");
    shdPhong_color = glGetUniformLocation(shdPhong, "color");
}

void makePhong1()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v21, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f2, NULL);
    glCompileShader(fragmentShader);

    shdPhong1 = glCreateProgram();
        glAttachShader(shdPhong1, vertexShader);
        glAttachShader(shdPhong1, fragmentShader);
    glLinkProgram(shdPhong1);

    shdPhong1_position = glGetAttribLocation(shdPhong1, "position");
    shdPhong1_normal = glGetAttribLocation(shdPhong1, "normal");
    
    shdPhong1_projection = glGetUniformLocation(shdPhong1, "projection");
    shdPhong1_modelview = glGetUniformLocation(shdPhong1, "modelview");
    shdPhong1_normalmat = glGetUniformLocation(shdPhong1, "normalmat");
    shdPhong1_lightpos = glGetUniformLocation(shdPhong1, "lightpos");
    shdPhong1_opacity = glGetUniformLocation(shdPhong1, "opacity");
    shdPhong1_color = glGetUniformLocation(shdPhong1, "color");
}

void makePhong2()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v22, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f2, NULL);
    glCompileShader(fragmentShader);

    shdPhong2 = glCreateProgram();
        glAttachShader(shdPhong2, vertexShader);
        glAttachShader(shdPhong2, fragmentShader);
    glLinkProgram(shdPhong2);

    shdPhong2_position = glGetAttribLocation(shdPhong2, "position");
    shdPhong2_color = glGetAttribLocation(shdPhong2, "color");
    
    shdPhong2_projection = glGetUniformLocation(shdPhong2, "projection");
    shdPhong2_modelview = glGetUniformLocation(shdPhong2, "modelview");
    shdPhong2_normalmat = glGetUniformLocation(shdPhong2, "normalmat");
    shdPhong2_lightpos = glGetUniformLocation(shdPhong2, "lightpos");
    shdPhong2_opacity = glGetUniformLocation(shdPhong2, "opacity");
}

void makePhong3()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v23, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f2, NULL);
    glCompileShader(fragmentShader);

    shdPhong3 = glCreateProgram();
        glAttachShader(shdPhong3, vertexShader);
        glAttachShader(shdPhong3, fragmentShader);
    glLinkProgram(shdPhong3);

    shdPhong3_position = glGetAttribLocation(shdPhong3, "position");
    shdPhong3_color = glGetAttribLocation(shdPhong3, "color");
    shdPhong3_normal = glGetAttribLocation(shdPhong3, "normal");
    
    shdPhong3_projection = glGetUniformLocation(shdPhong3, "projection");
    shdPhong3_modelview = glGetUniformLocation(shdPhong3, "modelview");
    shdPhong3_normalmat = glGetUniformLocation(shdPhong3, "normalmat");
    shdPhong3_lightpos = glGetUniformLocation(shdPhong3, "lightpos");
    shdPhong3_opacity = glGetUniformLocation(shdPhong3, "opacity");
}

void makeAllShaders()
{
    makeFullbrightT();
    makeFullbright();
    makeLambert();
    makeLambert1();
    makeLambert2();
    makeLambert3();
    makePhong();
    makePhong1();
    makePhong2();
    makePhong3();
}

void shadeFullbrightT(GLint* position, GLint* projection, GLint* modelview, GLint* texcoord, GLint* sampler)
{
    *position = shdFullbrightT_position;
    *projection = shdFullbrightT_projection;
    *modelview = shdFullbrightT_modelview;
    *texcoord = shdFullbrightT_texcoord;
    *sampler = shdFullbrightT_sampler;
    glUseProgram(shdFullbrightT);
}

void shadeFullbright(GLint* position, GLint* projection, GLint* modelview, GLint* color, GLint* opacity)
{
    *position = shdFullbright_position;
    *projection = shdFullbright_projection;
    *modelview = shdFullbright_modelview;
    *color = shdFullbright_color;
    *opacity = shdFullbright_opacity;
    glUseProgram(shdFullbright);
}

void shadeLambert(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* color, GLint* opacity)
{
    *position = shdLambert_position;
    *projection = shdLambert_projection;
    *modelview = shdLambert_modelview;
    *lightpos = shdLambert_lightpos;
    *color = shdLambert_color;
    *opacity = shdLambert_opacity;
    glUseProgram(shdLambert);
}

void shadeLambert1(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity)
{
    *position = shdLambert1_position;
    *projection = shdLambert1_projection;
    *modelview = shdLambert1_modelview;
    *lightpos = shdLambert1_lightpos;
    *color = shdLambert1_color;
    *normal = shdLambert1_normal;
    *opacity = shdLambert1_opacity;
    glUseProgram(shdLambert1);
}

// notice: swapped this from 2 to 3
void shadeLambert3(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity)
{
    *position = shdLambert2_position;
    *projection = shdLambert2_projection;
    *modelview = shdLambert2_modelview;
    *lightpos = shdLambert2_lightpos;
    *color = shdLambert2_color;
    *normal = shdLambert2_normal;
    *opacity = shdLambert2_opacity;
    glUseProgram(shdLambert2);
}

// notice: swapped this from 3 to 2
void shadeLambert2(GLint* position, GLint* projection, GLint* modelview, GLint* lightpos, GLint* color, GLint* opacity)
{
    *position = shdLambert3_position;
    *projection = shdLambert3_projection;
    *modelview = shdLambert3_modelview;
    *lightpos = shdLambert3_lightpos;
    *color = shdLambert3_color;
    *opacity = shdLambert3_opacity;
    glUseProgram(shdLambert3);
}

void shadePhong(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* color, GLint* opacity)
{
    *position = shdPhong_position;
    *projection = shdPhong_projection;
    *modelview = shdPhong_modelview;
    *normalmat = shdPhong_normalmat;
    *lightpos = shdPhong_lightpos;
    *color = shdPhong_color;
    *opacity = shdPhong_opacity;
    glUseProgram(shdPhong);
}

void shadePhong1(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity)
{
    *position = shdPhong1_position;
    *projection = shdPhong1_projection;
    *modelview = shdPhong1_modelview;
    *normalmat = shdPhong1_normalmat;
    *lightpos = shdPhong1_lightpos;
    *color = shdPhong1_color;
    *normal = shdPhong1_normal;
    *opacity = shdPhong1_opacity;
    glUseProgram(shdPhong1);
}

void shadePhong2(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* color, GLint* opacity)
{
    *position = shdPhong2_position;
    *projection = shdPhong2_projection;
    *modelview = shdPhong2_modelview;
    *normalmat = shdPhong2_normalmat;
    *lightpos = shdPhong2_lightpos;
    *color = shdPhong2_color;
    *opacity = shdPhong2_opacity;
    glUseProgram(shdPhong2);
}

void shadePhong3(GLint* position, GLint* projection, GLint* modelview, GLint* normalmat, GLint* lightpos, GLint* normal, GLint* color, GLint* opacity)
{
    *position = shdPhong3_position;
    *projection = shdPhong3_projection;
    *modelview = shdPhong3_modelview;
    *normalmat = shdPhong3_normalmat;
    *lightpos = shdPhong3_lightpos;
    *color = shdPhong3_color;
    *normal = shdPhong3_normal;
    *opacity = shdPhong3_opacity;
    glUseProgram(shdPhong3);
}

#endif
