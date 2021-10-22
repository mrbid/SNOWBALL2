/*
    James William Fletcher (james@voxdsp.com)
        June 2020 - October 2021

    *** COVERTED FROM SDL2 TO GLFW ***
        I like how GFLW is just letting me call OpenGL ES functions in GL.
        16x MSAA now mandatory
        I had to manually adjust timings...

    Two of the application 16x16 icons are from http://www.forrestwalter.com/icons/
    A Helpful Colour Converter: https://www.easyrgb.com

    I released the original game back in November 2020 as a webgl game:
    http://snowball.mobi

    An article that covers this engine:
    https://medium.com/swlh/a-simple-3d-renderer-for-the-web-in-c-15397bb2352f

    Not the cleanest code ever, I was excited when making this so only
    performance and the end product was in mind.

    Why is there no sound? Well, sound is a hard thing to get right, and I
    had some ideas such as using SAM: https://discordier.github.io/sam/
    or playing the next beat in a very simple tune on each snowball collection
    so that the faster the flow the more in time the beat played.
    or just using simple 8bit style sound effects synthesized in real-time.

    But ultimately I just like it with no sound.

    This can be launched in any window size from command line:
    ./snowball 1920 1080

    Or for a full commandline:
    ./snowball <width> <height> <mouse sensitivity> <random seed>

    Specify -1.0 to invert the mouse.

    Controls:
    Mouse 1 (Left)  = Toggle Control
    Mouse 2 (Right) = Change View
    Mouse X1        = Decrease mouse speed
    Mouse X2        = Increase mouse speed
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define uint unsigned int

#include "gl.h"
#define GLFW_INCLUDE_NONE
#include "glfw3.h"

#include "res.h"
#include "esAux.h"

#include "assets/icogrid.h"
#include "assets/icosmooth2.h"
#include "assets/tree.h"
#include "assets/sadply.h"
#include "assets/cat.h"
#include "assets/heart.h"
#include "assets/intro.h"
#include "assets/tele.h"


//*************************************
// globals
//*************************************
GLFWwindow* window;
uint winw = 1024; //1920
uint winh = 768;  //1080
GLfloat t = 0;
GLfloat aspect;
GLfloat sens = 0.3f;
GLfloat sens_mul = 0.2f;

double x=0, y=0, sx=0, sy=0;
uint md = 0; // mouse input

GLint projection_id;
GLint modelview_id;
GLint position_id;
GLint lightpos_id;
GLint solidcolor_id;
GLint color_id;
GLint opacity_id;
GLint normal_id;
GLint type_id;

GLuint bindstate = 0;
GLuint bindstate2 = 0; //Just for rTree() color array change
//It's unlikely there's going to be many color state changes
//so I did not split the arrays into iced and regular passes.
// -  this is also true with the rPole (the obelisk) color
//    state changes here just happen at the end of the level
//    on just three rPole calls.
// Hindsight says, I could have reduced state changes more
// but not significantly so, aka this is negligible.

ESMatrix projection;
ESMatrix view;
ESMatrix model;
ESMatrix modelview;

ESVector lightpos = {0.f, 0.f, 0.f};
GLfloat xrot = 0.f;
GLfloat yrot = 0.f;
GLfloat zoom = -20.0f;

ESModel mdlGrid;
ESModel mdlSphere;
ESModel mdlTree;
    GLuint mdlTree_iced;
ESModel mdlCat;
    GLuint mdlCat_aura;
ESModel mdlSad;
    GLfloat sfd = -1.01f;
    GLfloat sfa = 0.0f;
    GLfloat sfs = 0.0f;
ESModel mdlIntro;
    GLfloat inrot = 0;
ESModel mdlHeart;
    GLfloat hrt = 0;
ESModel mdlTele;

#define max_tree 768
uint tp[max_tree];
char tpi[max_tree] = {0};

#define max_snow 768
uint sp[max_snow];

#define max_gold 128
uint gp[max_gold];

GLfloat sround = 0;
uint level = 0;
uint sport = 0;
uint rounds = 33;
uint seed = 1337;
double score = 0.f;
GLfloat drag = 0.0023f;
GLfloat maxspeed = 0.f;
GLfloat start = 0;
uint psw = 0;
ESVector pp;
GLfloat pscale = 1.0f;
ESMatrix pm;
GLfloat ps = 0.12f;
GLfloat pr = 1.0f, pg = 1.0f, pb = 1.0f;
GLfloat msca = 6.0f;


//*************************************
// utility functions
//*************************************
uint qRand(const uint min, const uint max)
{
    static float rndmax = (float)RAND_MAX;
    return ((((float)rand())+1e-7f) / rndmax) * ((max+1)-min) + min;
}

uint qRandSeed(const uint seed, const uint min, const uint max)
{
    srand(seed);
    return qRand(min, max);
}

float qRandFloat(const float min, const float max)
{
    static const float rmax = (float)RAND_MAX;
    return (((((float)rand())+1e-7f) / rmax) * (max-min)) + min;
}

float qRandFloatSeed(const uint seed, const float min, const float max)
{
    srand(seed);
    return qRandFloat(min, max);
}

float distance(const ESVector a, const uint16_t b)
{
    const float xm = (a.x - icogrid_vertices[b]);
    const float ym = (a.y - icogrid_vertices[b+1]);
    const float zm = (a.z - icogrid_vertices[b+2]);
    return sqrt(xm*xm + ym*ym + zm*zm);
}

void resetGame(char sf)
{
    for(int i = 0; i < max_tree; i++)
    {
        uint ni = qRand(0, icogrid_numvert-3)*3;
        while(ni == 330 || ni == 132)
            ni = qRand(0, icogrid_numvert-3)*3;

        uint ex = 0;
        for(int j = 0; j < max_tree; j++)
        {
            if(tp[j] == ni)
            {
                ex = 1;
                break;
            }
        }
        if(ex == 0)
            tp[i] = ni;

        tpi[i] = 0;
    }

    for(int i = 0; i < max_snow; i++)
    {
        const uint ni = qRand(0, icogrid_numvert-3)*3;
        uint ex = 0;
        for(int j = 0; j < max_snow; j++)
        {
            if(sp[j] == ni)
            {
                ex = 1;
                break;
            }
        }
        if(ex == 0)
            sp[i] = ni;
    }

    for(int i = 0; i < max_gold; i++)
    {
        const uint ni = qRand(0, icogrid_numvert-3)*3;
        uint ex = 0;
        for(int j = 0; j < max_gold; j++)
        {
            if(gp[j] == ni)
            {
                ex = 1;
                break;
            }
        }
        if(ex == 0)
            gp[i] = ni;
    }

    pscale = 1.0f;

    esMatrixLoadIdentity(&view);
    esMatrixLoadIdentity(&model);
    esMatrixLoadIdentity(&modelview);
    esMatrixLoadIdentity(&pm);

    xrot = 0;
    yrot = 0;

    const uint pstart = 330;
    pp.x = icogrid_vertices[pstart];
    pp.y = icogrid_vertices[pstart+1];
    pp.z = icogrid_vertices[pstart+2];

    if(sf == 1 || sf == 3)
    {
        glfwSetWindowTitle(window, "Snowball.mobi");
        sfd = -1.01f;
        sfa = 0.0f;
        if(sf == 1)
            sfs = 1.0f;
        pr = 1.0f, pg = 1.0f, pb = 1.0f;
        level = 0;
        ps = 0.12f;
        psw = 0;
        msca = 6.0f;
        score = 0;
        start = t;
    }
    else if(sf == 0)
    {
        inrot = t+3.3f;
    }
    else if(sf == 2)
    {
        if(psw)
        {
            pr = qRandFloat(0.3f, 1.f);
            pg = qRandFloat(0.3f, 1.f);
            pb = qRandFloat(0.3f, 1.f);
        }
        else
        {
            pb -= 0.22f;
            if(pb < 0.0f)
            {
                pr = 0.f;
                pg = 0.34117f;
                pb = 0.49412f;
                psw = 1;
            }
        }

        hrt = t+1.6f;
        msca += 1.0f;
        level++;
        
        char title[256];
        sprintf(title, "Level %d - Points %.2f - Time %.2f mins - Score %.2f", level, score, ((double)(t-start))/60.0, (score / sqrt(t-start))*100);
        glfwSetWindowTitle(window, title);
    }

    sround = t + 6.f; //Immune from tree's in the first 6 seconds of a round starting
}



//*************************************
// render functions
//*************************************
void rGrid()
{
    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &view.m[0][0]);
    glUniform3f(color_id, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);
    glUniform1f(opacity_id, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mdlGrid.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlGrid.iid);

    glDrawElements(GL_TRIANGLES, icogrid_numind, GL_UNSIGNED_SHORT, 0);
}

void rTree(const uint pos, const GLfloat scale, const char iced)
{
    esMatrixLoadIdentity(&model);

    ESVector v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    esLookAt(&model, icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2], v.x, v.y, v.z);
    esScale(&model, scale, scale, scale);

    esMatrixMultiply(&modelview, &model, &view);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);
    glUniform1f(opacity_id, 1.0f);

    if(bindstate != 2)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlTree.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, mdlTree.nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlTree.iid);

        glBindBuffer(GL_ARRAY_BUFFER, mdlTree.cid);
        glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(color_id);

        bindstate = 2;
    }

    //eh, I should have split the rendering arrays for this but it's not a big deal
    if(iced == 0)
    {
        if(bindstate2 != 1)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mdlTree.cid);
            glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(color_id);
            bindstate2 = 1;
        }
    }
    else
    {
        if(bindstate2 != 2)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mdlTree_iced);
            glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(color_id);
            bindstate2 = 2;
        }
    }

    glDrawElements(GL_TRIANGLES, tree_numind, GL_UNSIGNED_SHORT, 0);
}

void rSphere(const uint pos, const GLfloat scale, const GLfloat r, const GLfloat g, const GLfloat b)
{
    esMatrixLoadIdentity(&model);

    ESVector v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    esLookAt(&model, icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2], v.x, v.y, v.z);
    if(scale != 1.0)
        esScale(&model, scale, scale, scale);

    esMatrixMultiply(&modelview, &model, &view);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);
    glUniform3f(color_id, r, g, b);
    glUniform1f(opacity_id, 1.0f);

    if(bindstate != 3)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlSphere.iid);
        bindstate = 3;
    }

    glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
}

void rPole(const uint pos, const GLfloat scale, const GLfloat yaw, const char aura)
{
    esMatrixLoadIdentity(&model);

    ESVector v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    esLookAt(&model, icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2], v.x, v.y, v.z);
    esRotate(&model, yaw, 0.f, 0.f, 1.f);
    if(scale != 1.0f)
        esScale(&model, scale, scale, scale);

    esMatrixMultiply(&modelview, &model, &view);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);
    glUniform1f(opacity_id, 1.0);

    if(bindstate != 4)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlCat.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, mdlCat.nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlCat.iid);
        bindstate = 4;
    }

    if(aura == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlCat.cid);
        glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(color_id);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlCat_aura);
        glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(color_id);
    }

    glDrawElements(GL_TRIANGLES, cat_numind, GL_UNSIGNED_SHORT, 0);
}

void rTele(const uint pos, const GLfloat scale, const GLfloat yaw)
{
    esMatrixLoadIdentity(&model);

    ESVector v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    esLookAt(&model, icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2], v.x, v.y, v.z);
    esRotate(&model, yaw, 0.f, 0.f, 1.f);
    if(scale != 1.0f)
        esScale(&model, scale, scale, scale);

    esMatrixMultiply(&modelview, &model, &view);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);
    glUniform1f(opacity_id, 1.0f);

    if(bindstate != 5)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlTele.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, mdlTele.nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlTele.iid);
        bindstate = 5;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mdlTele.cid);
    glVertexAttribPointer(color_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(color_id);

    glDrawElements(GL_TRIANGLES, tele_numind, GL_UNSIGNED_SHORT, 0);
}

void rPlayer(const GLfloat dt, ESVector ndir, const ESVector nup, const GLfloat scale)
{
    static ESVector lpp;
    static ESMatrix lpm;
    static GLfloat lrot = 0;
    static GLfloat rot = 0;

    // rotation
    rot += (-1000.f * dt)*vDist(lpp, pp);

    // speed drag
    ps -= drag*dt;
    if(ps < 0.12f){ps = 0.12f;}

    // render
    esMatrixLoadIdentity(&pm);

    // dont render if in epsilon motion increments (basically not moving)
    if(vDist(lpp, pp) > 0.001f)
    {
        esTranslate(&pm, pp.x, pp.y, pp.z);
        esSetDirection(&pm, ndir, nup);
    }
    else
    {
        pp.x = lpm.m[3][0];
        pp.y = lpm.m[3][1];
        pp.z = lpm.m[3][2];
        rot = lrot;
        esMatrixCopy(&pm, &lpm);
    }

    esMatrixCopy(&lpm, &pm);
    vCopy(&lpp, pp);
    lrot = rot;

    esMatrixCopy(&model, &pm);
    if(scale != 1.0f)
        esScale(&model, scale, scale, scale);
    esRotate(&model, rot, 1.f, 0.f, 0.f);

    esMatrixMultiply(&modelview, &model, &view);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(lightpos_id, lightpos.x, lightpos.y, lightpos.z);

    // this only re-binds if necessary
    if(bindstate != 3)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.nid);
        glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlSphere.iid);
        bindstate = 3;
    }

    // transparent (start round) or not
    if(t < sround)
    {
        if(sport == 0)
        {
            glUniform3f(color_id, 0.50588f, 0.84705f, 1.0f);
            glUniform1f(opacity_id, 0.5f);
        }
        else
        {
            glUniform3f(color_id, 0.25f, 0.25f, 0.25f);
            glUniform1f(opacity_id, 1.0f);
        }
        glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
        glDisable(GL_BLEND);
    }
    else
    {
        if(sport == 0)
            glUniform3f(color_id, pr, pg, pb);
        else
            glUniform3f(color_id, 0.f, 0.f, 0.f);
        glUniform1f(opacity_id, 1.0f);
        glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
    }

}

void rSad(const GLfloat dt)
{
    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.f, 0.f, sfd);
    esRotate(&modelview, sfa, 1.f, 0.f, 0.f);
    esScale(&modelview, sfs, sfs, sfs);
    sfd -= 0.16f * dt;
    sfa += 9.0f * dt;
    if(sfs > 0.0f)
        sfs -= 0.02f * dt;
    else
        sfs = 0;
    if(sfd <= -20.0f)
    {
        sfd = -1.01f;
        sfa = 0.0f;
        sfs = 1.0f;
    }

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(color_id, 0.7f, 0.1f, 0.0f);
    glUniform1f(opacity_id, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mdlSad.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlSad.iid);

    glDisable(GL_CULL_FACE);
        glDrawElements(GL_TRIANGLES, sad_numind, GL_UNSIGNED_SHORT, 0);
    glEnable(GL_CULL_FACE);
}

void rIntro(const GLfloat opacity)
{
    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.f, 0.f, -1.01f);

    //0.21
    GLfloat ns = 0.14f * aspect * 0.5f;
    if(ns < 0.14f)
        ns = 0.14f;
    esScale(&modelview, ns, ns, ns);

    glEnable(GL_BLEND);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(color_id, 0.f, 0.f, 0.f);
    glUniform1f(opacity_id, opacity*0.5f);

    glBindBuffer(GL_ARRAY_BUFFER, mdlIntro.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlIntro.iid);
    glDrawElements(GL_TRIANGLES, intro_numind, GL_UNSIGNED_SHORT, 0);

    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.f, 0.f, -1.01f);

    //0.20
    ns = 0.133f * aspect * 0.5f;
    if(ns < 0.133f)
        ns = 0.133f;
    esScale(&modelview, ns, ns, ns);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(color_id, 0.53f, 0.80f, 0.92f);
    glUniform1f(opacity_id, opacity);

    glBindBuffer(GL_ARRAY_BUFFER, mdlIntro.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlIntro.iid);

    glDrawElements(GL_TRIANGLES, intro_numind, GL_UNSIGNED_SHORT, 0);

    glDisable(GL_BLEND);
}

void rHeart(const GLfloat opacity)
{
    esMatrixLoadIdentity(&modelview);
    esTranslate(&modelview, 0.f, 0.f, -1.01f);
    esScale(&modelview, 4.2f, 4.2f, 4.2f);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(color_id, 0.67059f, 0.12157f, 0.12157f);
    glUniform1f(opacity_id, opacity);

    glBindBuffer(GL_ARRAY_BUFFER, mdlHeart.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlHeart.iid);

    glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLES, heart_numind, GL_UNSIGNED_SHORT, 0);
    glDisable(GL_BLEND);
}

void setBaseSensitivity()
{
    if(zoom == -16.0f)
        sens = 0.1f*sens_mul;
    else if(zoom == -20.0)
        sens = 0.2f*sens_mul;
    else if(zoom == -26.0f)
        sens = 0.3f*sens_mul;
    else if(zoom == -27.0f)
        sens = 0.3f*sens_mul;
}

//*************************************
// update & render
//*************************************
void main_loop()
{
//*************************************
// time delta for interpolation
//*************************************
    static GLfloat lt = 0;
    GLfloat deltaTime = (t-lt)*30.0f;
    //printf("%f\n", deltaTime);
    if(deltaTime > 1.0f)
        deltaTime = 1.0f;
    lt = t;

//*************************************
// orbit / mouse control scaling
//*************************************

    if(md == 1)
        glfwGetCursorPos(window, &x, &y);

    double xd = (sx-x);
    double yd = (sy-y);
    //printf("%f %f | %f %f | %f %f\n", x, y, sx, sy, xd, yd);

    if(glfwJoystickPresent(GLFW_JOYSTICK_1) == 1)
    {
        int count;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
        if(count >= 2)
        {
            xd = -axes[0]*50.f*sens;
            yd = -axes[1]*50.f*sens;
            //printf("%f %f\n", axes[0], axes[1]);
        }

        static float bt = 0;
        if(t > bt)
        {
            const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
            if(count >= 3)
            {
                if(buttons[0] == GLFW_PRESS)
                {
                    sens_mul -= 0.03f;
                    setBaseSensitivity();
                    printf("Sens: %.3f\n", sens_mul);
                    bt = t + 0.3f;
                }
                else if(buttons[1] == GLFW_PRESS)
                {
                    sens_mul += 0.03f;
                    setBaseSensitivity();
                    printf("Sens: %.3f\n", sens_mul);
                    bt = t + 0.3f;
                }
                else if(buttons[2] == GLFW_PRESS)
                {
                    if(zoom == -20.0f)
                    {
                        zoom = -16.0f;
                        setBaseSensitivity();
                        sport = 0;
                    }
                    else if(zoom == -16.0f)
                    {
                        zoom = -26.0f;
                        setBaseSensitivity();
                        sport = 0;
                    }
                    else if(zoom == -26.0f)
                    {
                        zoom = -27.0f;
                        setBaseSensitivity();
                        sport = 1;
                    }
                    else if(zoom == -27.0)
                    {
                        zoom = -20.0f;
                        setBaseSensitivity();
                        sport = 0;
                    }
                    bt = t + 0.3f;
                }
            }
        }
    }

    xrot += xd*sens;
    yrot += yd*sens;

    // prevent pole inversion
    if(yrot > 180.0f)
        yrot = 180.0f;
    if(yrot < -0.0f)
        yrot = -0.0f;
    
    if(md == 1)
        glfwSetCursorPos(window, sx, sy);

//*************************************
// camera control
//*************************************

    esMatrixLoadIdentity(&view);

    esTranslate(&view, 0.f, 0.f, zoom);

    esRotate(&view, -yrot, 1.f, 0.f, 0.f);
    esRotate(&view, xrot, 0.f, 0.f, 1.f);

//*************************************
// camera look vector
//*************************************

    ESVector ray_eye;
    ray_eye.x = view.m[0][2];
    ray_eye.y = view.m[1][2];
    ray_eye.z = view.m[2][2]; // already normalised

    ESVector dp;
    vCopy(&dp, ray_eye);
    vMulScalar(&dp, dp, ps*deltaTime); //cast out
    vAdd(&pp, pp, dp);

    ESVector pn;
    vCopy(&pn, pp);
    vNorm(&pn);
    ESVector vn;
    vMulScalar(&vn, pn, 12.55f + ((pscale*0.5f)*0.1f));
    vCopy(&pp, vn);

//*************************************
// begin render
//*************************************

    // Clear the image to the background colour and clear the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//*************************************
// render grid
//*************************************

glEnable(GL_DEPTH_TEST);
// --
shadeLambert(&position_id, &projection_id, &modelview_id, &lightpos_id, &color_id, &opacity_id);
    rGrid();
// --
shadeLambert3(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);

    // poles
    if(pscale >= msca)
    {
        if(distance(pp, 0) <= pscale*0.14f || distance(pp, 132) <= pscale*0.14f)
            resetGame(2);

        rTele(0.f, 1.0f, 0.f);
        rTele(132.f, 1.0f, 0.f);
    }
    else
    {
        char aura = 0;
        if(distance(pp, 0) <= 4.0f || distance(pp, 132) <= 4.0f)
            aura = 1;
        
        rPole(0.f, 2.6f, 0.f, aura);
        rPole(132.f, 2.6f, 0.f, aura);
    }

    // tree's
    for(int i = 0; i < max_tree; i++)
    {
        if(tp[i] == 0)
            continue;

        GLfloat ns = 1;
        if(level >= 1)
        {
            ns = (float)qRandSeed(tp[i]+level, 1, 2);
            if(ns == 2.f)
                ns = 1.6f;
        }

        if(t > sround)
        {
            if(pscale >= ns*4.0f && distance(pp, tp[i]) <= pscale*0.3f)
                ns *= 0.5f;
            if(distance(pp, tp[i]) <= pscale*0.1)
            {
                ps -= 0.12f; // hitting a tree is always bad for speed
                if(pscale >= ns*4.0f)
                {
                    pscale += 0.01f;
                    tp[i] = 0;
                    continue;
                }
                else
                {
                    if(tpi[i] != 1)
                        pscale -= 0.3f;
                    tpi[i] = 1;
                }
            }
        }

        rTree(tp[i], ns, tpi[i]);
    }
// --
shadeLambert1(&position_id, &projection_id, &modelview_id, &lightpos_id, &normal_id, &color_id, &opacity_id);
    for(int i = 0; i < max_snow; i++)
    {
        if(sp[i] == 0)
            continue;

        if(distance(pp, sp[i]) <= pscale*0.1f)
        {
            const float ss = qRandFloatSeed(sp[i]+level, 0.3f, 1.0f);
            sp[i] = 0;
            ps += 0.06f*ss;
            pscale += ss*0.03f;
            score += 0.06f*ss;
            continue;
        }

        if(sport == 0)
            rSphere(sp[i], qRandFloatSeed(sp[i]+level, 0.5f, 1.0f), 1.0f, 1.0f, 1.0f);
        else
            rSphere(sp[i], qRandFloatSeed(sp[i]+level, 0.5f, 1.0f), 0.f, 0.f, 1.0f);
    }

    for(int i = 0; i < max_gold; i++)
    {
        if(gp[i] == 0)
            continue;

        if(distance(pp, gp[i]) <= pscale*0.1f)
        {
            const float ss = qRandFloatSeed(gp[i]+level, 0.3f, 1.0f);
            gp[i] = 0;
            ps += 0.12f*ss;
            pscale += ss*0.12f;
            score += 0.12f*ss;
            continue;
        }

        if(sport == 0)
            rSphere(gp[i], qRandFloatSeed(gp[i]+level, 0.4f, 0.65f), 1.0f, 1.0f, 0.0f);
        else
            rSphere(gp[i], qRandFloatSeed(gp[i]+level, 0.4f, 0.65f), 1.0f, 0.f, 0.f);
    }
// --
if(pscale < 1.0f)
    resetGame(1); //dead
if(pscale > msca)
    pscale = msca;

if(maxspeed != 0 && ps > maxspeed) //speed limiter
    ps = maxspeed;

    rPlayer(deltaTime, ray_eye, pn, pscale);
// --
glDisable(GL_DEPTH_TEST);

if(sfs != 0)
{
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    rSad(deltaTime);
}
if(t < inrot)
{
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    rIntro((inrot-t)*0.3f);
}
if(t < hrt)
{
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    rHeart((hrt-t)*0.3);
}

//*************************************
// swap buffers / display render
//*************************************

    glfwSwapBuffers(window);
}

//*************************************
// Helper Funcs
//*************************************
void esBindModel(ESModel* model, const GLfloat* vertices, const GLsizei vertlen, const GLushort* indices, const GLsizei indlen)
{
    glGenBuffers(1, &model->vid);
    glBindBuffer(GL_ARRAY_BUFFER, model->vid);
    glBufferData(GL_ARRAY_BUFFER, vertlen * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &model->iid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indlen * sizeof(GLushort), indices, GL_STATIC_DRAW);
}

//*************************************
// Input Handelling
//*************************************
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS){return;}

    switch(key)
    {
        case GLFW_KEY_SPACE:
        {
            char title[256];
            sprintf(title, "Level %d - Points %.2f - Time %.2f mins - Score %.2f", level, score, ((double)(t-start))/60.0, (score / sqrt(t-start))*100);
            glfwSetWindowTitle(window, title);
        }
        break;

        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        break;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            if(md == 0)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                x = winw/2, y = winh/2;
                glfwSetCursorPos(window, x, y);
                sx = x, sy = y;
                md = 1;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                sx = x, sy = y;
                md = 0;
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS)
    {
        sens_mul -= 0.1f;
        setBaseSensitivity();
        printf("Sens: %.3f\n", sens_mul);
    }
    
    if(button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS)
    {
        sens_mul += 0.1f;
        setBaseSensitivity();
        printf("Sens: %.3f\n", sens_mul);
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        if(zoom == -20.0f)
        {
            zoom = -16.0f;
            setBaseSensitivity();
            sport = 0;
        }
        else if(zoom == -16.0f)
        {
            zoom = -26.0f;
            setBaseSensitivity();
            sport = 0;
        }
        else if(zoom == -26.0f)
        {
            zoom = -27.0f;
            setBaseSensitivity();
            sport = 1;
        }
        else if(zoom == -27.0)
        {
            zoom = -20.0f;
            setBaseSensitivity();
            sport = 0;
        }
    }
}

// void cursor_position_callback(GLFWwindow* window, double ix, double iy){
//     if(md == 1){x = ix, y = iy;}
// }

void window_size_callback(GLFWwindow* window, int width, int height)
{
    winw = width;
    winh = height;

    glViewport(0, 0, winw, winh);
    aspect = (GLfloat)winw / (GLfloat)winh;

    esMatrixLoadIdentity(&projection);
    esPerspective(&projection, 60.0f, aspect, 1.0f, 60.0f); 
}

//*************************************
// Process Entry Point
//*************************************
int main(int argc, char** argv)
{
    // you can specify your own window size on execution
    uint noborder = 0;
    if(argc >= 3)
    {
        winw = atoi(argv[1]);
        winh = atoi(argv[2]);
    }
    // and input sensitivity (invert the mouse with -1.0)
    if(argc >= 4)
    {
        sens_mul = atof(argv[3]);
    }
    // and random seed
    if(argc >= 5)
    {
        seed = atof(argv[4]);
    }
    // and the really cool part (noborder!)
    if(argc == 6)
    {
        noborder = atoi(argv[5]);
    }

    // help
    printf("Snowball by James William Fletcher (james@voxdsp.com)\n");
    printf("A novel speed run game akin to a platform loop/buzz wire game.\n\n");
    printf("CONTROLS:\n");
    printf("Mouse 1 (Left)  = Toggle Control\n");
    printf("Mouse 2 (Right) = Change View\n");
    printf("Mouse X1        = Decrease mouse speed\n");
    printf("Mouse X2        = Increase mouse speed\n\n");
    printf("COMMAND LINE:\n");
    printf("./snowball <width> <height> <mouse sensitivity> <random seed> <noborder>\n\n");
    printf("Specify a mouse sensitivity of -x such as -1.0 to invert the mouse.\n\n");
    printf("A web version of the game: http://snowball.mobi\n\n");
    printf("HOW TO:\n");
    printf("You have to roll around collecting snow and avoiding trees until your snowball is large enough to continue to the next level.\n\n");
    printf("Once you have accumulated a large enough snowball, the poles (obelisks) will turn into teleportation pads to the next round; the aim is to complete each level or a sequence of levels with the highest score in the shortest time.\n\n");
    printf("The larger the snowballs you collect higher the score. Gold snow is double the reward of white. Picking up snow increases your speed temporarily; the idea is to enter a flow of collecting snow so that your speed stays up.\n\n");
    printf("Try not to hit the trees as they will slow you down and reduce the mass of your snowball, unless your snowball is large enough to consume them, then they slow you down but also add to the mass of your snowball. Trees will shrink slightly as you approach them to show that they can be consumed.\n\n");
    printf("Your score is updated in the program title bar at the end of each level.\n\n");

    if(!glfwInit()){exit(EXIT_FAILURE);}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    if(noborder == 1)
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
    else
        glfwWindowHint(GLFW_SAMPLES, 16);
    window = glfwCreateWindow(winw, winh, "Snowball.mobi", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, ((desktop->width-sx)/2)-(winw/2), ((desktop->height-sy)/2)-(winh/2));
    setBaseSensitivity(); // important
    if(noborder == 1)
    {
        glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
        zoom = -28.0f;
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync

    // set random icon
    unsigned char* ipd = (unsigned char*)&icon_image2.pixel_data;
    const uint r = qRandSeed(time(0), 0, 2);
    if(r == 0)
        ipd = (unsigned char*)&icon_image.pixel_data;
    else if(r == 1)
        ipd = (unsigned char*)&icon_image1.pixel_data;
    else
        ipd = (unsigned char*)&icon_image2.pixel_data;
    GLFWimage icon = {16, 16, ipd};
    glfwSetWindowIcon(window, 1, &icon);

    // seed random
    srand(seed);


//*************************************
// projection
//*************************************

    aspect = (GLfloat)winw / (GLfloat)winh;

    if(aspect < 1.0f)
        zoom = -26.0f;

    esMatrixLoadIdentity(&projection);
    esPerspective(&projection, 60.0f, aspect, 1.0f, 60.0f);


//*************************************
// bind vertex and index buffers
//*************************************

    // ***** BIND ICOGRID *****
    esBindModel(&mdlGrid, icogrid_vertices, icogrid_numvert, icogrid_indices, icogrid_numind);

    // ***** BIND SADPLY *****
    esBindModel(&mdlSad, sad_vertices, sad_numvert, sad_indices, sad_numind);

    // ***** BIND HEART *****
    esBindModel(&mdlHeart, heart_vertices, heart_numvert, heart_indices, heart_numind);

    // ***** BIND INTRO *****
    esBindModel(&mdlIntro, intro_vertices, intro_numvert, intro_indices, intro_numind);

    // ***** BIND TREE *****
    glGenBuffers(1, &mdlTree.vid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTree.vid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tree_vertices), tree_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTree.cid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTree.cid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tree_colors), tree_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTree_iced);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTree_iced);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tree_iced_colors), tree_iced_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTree.nid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTree.nid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tree_normals), tree_normals, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTree.iid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlTree.iid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tree_indices), tree_indices, GL_STATIC_DRAW);

    // ***** BIND POLE / OBELISK *****
    glGenBuffers(1, &mdlTele.vid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTele.vid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tele_vertices), tele_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTele.nid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTele.nid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tele_normals), tele_normals, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTele.cid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlTele.cid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tele_colors), tele_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlTele.iid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlTele.iid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tele_indices), tele_indices, GL_STATIC_DRAW);

    // ***** BIND TELE *****
    glGenBuffers(1, &mdlCat.vid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlCat.vid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cat_vertices), cat_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlCat.nid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlCat.nid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cat_normals), cat_normals, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlCat_aura);
    glBindBuffer(GL_ARRAY_BUFFER, mdlCat_aura);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cat_aura_colors), cat_aura_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlCat.cid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlCat.cid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cat_colors), cat_colors, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlCat.iid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlCat.iid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cat_indices), cat_indices, GL_STATIC_DRAW);

    // ***** BIND SPHERE *****
    glGenBuffers(1, &mdlSphere.vid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.vid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(icosmooth2_vertices), icosmooth2_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlSphere.nid);
    glBindBuffer(GL_ARRAY_BUFFER, mdlSphere.nid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(icosmooth2_normals), icosmooth2_normals, GL_STATIC_DRAW);

    glGenBuffers(1, &mdlSphere.iid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlSphere.iid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(icosmooth2_indices), icosmooth2_indices, GL_STATIC_DRAW);

//*************************************
// compile & link shader program
//*************************************

    //makeAllShaders();
    makeLambert();
    makeLambert1();
    makeLambert3();
    makeFullbright();

//*************************************
// configure render options
//*************************************

    //glDisable(GL_DEPTH_TEST);
    //glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);

//*************************************
// execute update / render loop
//*************************************

    // reset game
    t = glfwGetTime();
    resetGame(0);

    // event loop
    uint lt = glfwGetTime() + 16, fc = 0;
    while(!glfwWindowShouldClose(window))
    {
        t = glfwGetTime();
        //printf("%f\n", t);

        glfwPollEvents();
        main_loop();
        
        fc++;
        if(t > lt)
        {
            printf("FPS: %u\n", fc/16);
            fc = 0;
            lt = t + 16;
        }
    }

    // done
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
