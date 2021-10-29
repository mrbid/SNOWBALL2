/*
    James William Fletcher (james@voxdsp.com)
        June 2020 - October 2021

    It's all to the GLES 2.0 spec for portability.

    Two of the application 16x16 icons are from http://www.forrestwalter.com/icons/
    A Helpful Colour Converter: https://www.easyrgb.com/en/convert.php

    I released the original game back in November 2020 as a webgl game:
    http://snowball.mobi

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

#define NOSSE
#define SEIR_RAND
#include "esAux2.h"

#include "res.h"
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
double t = 0;
GLfloat aspect;
GLfloat sens = 0.3f;
GLfloat sens_mul = 0.2f;
double ww, wh, ww2, wh2; 
double uw, uh, uw2, uh2; // normalised pixel dpi

// mouse input
double x=0, y=0, sx=0, sy=0;
uint md = 0;

// ui
uint show_ui = 0;
uint menu_focus = 0;
uint seed_bits[27] = {0};
uint uhi = 0;
GLuint tex_menu;
GLuint tex_mouse;
double msx = -999;
double js1 = -999;
double js2 = -999;

// joystick input
uint doublestick = 1;
GLfloat ss1 = 1.f;
GLfloat ss2 = 1.5f;

GLint projection_id;
GLint modelview_id;
GLint position_id;
GLint lightpos_id;
GLint solidcolor_id;
GLint color_id;
GLint opacity_id;
GLint normal_id;
GLint type_id;
GLint texcoord_id;
GLint sampler_id;

GLuint bindstate = 0;
GLuint bindstate2 = 0; //Just for rTree() color array change
//It's unlikely there's going to be many color state changes
//so I did not split the arrays into iced and regular passes.
// -  this is also true with the rPole (the obelisk) color
//    state changes here just happen at the end of the level
//    on just three rPole calls.
// Hindsight says, I could have reduced state changes more
// but not significantly so, aka this is negligible.

mat projection;
mat view;
mat model;
mat modelview;

vec lightpos = {0.f, 0.f, 0.f};
GLfloat xrot = 0.f;
GLfloat yrot = 0.f;
GLfloat zoom = -20.0f;

ESModel mdlPlane;
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
    double inrot = 0;
ESModel mdlHeart;
    double hrt = 0;
ESModel mdlTele;

#define max_tree 768
uint tp[max_tree];
char tpi[max_tree] = {0};

#define max_snow 768
uint sp[max_snow];

#define max_gold 128
uint gp[max_gold];

double sround = 0;
uint level = 0;
uint sport = 0;
uint rounds = 33;
uint seed = 1337;
double points = 0.f;
GLfloat drag = 0.0023f;
GLfloat maxspeed = 0.f;
#define minspeed 0.22f
GLfloat start = 0;
uint psw = 0;
vec pp;
GLfloat pscale = 1.0f;
mat pm;
GLfloat ps = minspeed;
GLfloat pr = 1.0f, pg = 1.0f, pb = 1.0f;
GLfloat msca = 6.0f;


//*************************************
// utility functions
//*************************************
void timestamp(char* ts)
{
    const time_t tt = time(0);
    strftime(ts, 16, "%H:%M:%S", localtime(&tt));
}

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

float distance(const vec a, const uint16_t b)
{
    const float xm = (a.x - icogrid_vertices[b]);
    const float ym = (a.y - icogrid_vertices[b+1]);
    const float zm = (a.z - icogrid_vertices[b+2]);
    return sqrt(xm*xm + ym*ym + zm*zm);
}

void resetGame(char sf)
{
    if(sf == 2)
        level++;
    srand(seed+level);
    char strts[16];
    timestamp(&strts[0]);
    printf("[%s] Level Seed: %d\n", strts, seed+level);

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

    if(sf != 4)
    {
        pscale = 1.0f;

        mIdent(&view);
        mIdent(&model);
        mIdent(&modelview);
        mIdent(&pm);

        xrot = 0;
        yrot = 0;

        const uint pstart = 330;
        pp.x = icogrid_vertices[pstart];
        pp.y = icogrid_vertices[pstart+1];
        pp.z = icogrid_vertices[pstart+2];
    }

    if(sf == 1 || sf == 3)
    {
        glfwSetWindowTitle(window, "Snowball.mobi");
        sfd = -1.01f;
        sfa = 0.0f;
        if(sf == 1)
            sfs = 1.0f;
        pr = 1.0f, pg = 1.0f, pb = 1.0f;
        level = 0;
        ps = minspeed;
        psw = 0;
        msca = 6.0f;
        points = 0;
        start = t;
    }
    else if(sf == 0)
    {
        inrot = t+3.3;
    }
    else if(sf == 2)
    {
        if(psw)
        {
            pr = qRandFloatSeed(seed+level, 0.3f, 1.f);
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

        hrt = t+1.6;
        msca += 0.33f;
        
        char title[256];
        sprintf(title, "Level %d - Points %.2f - Time %.2f mins - Score %.2f", level, points, ((double)(t-start))/60.0, (points / sqrt(t-start))*100);
        glfwSetWindowTitle(window, title);
        char strts[16];
        timestamp(&strts[0]);
        printf("[%s] Level %d - Points %f - Time %f mins - Score %f\n", strts, level, points, ((double)(t-start))/60.0, (points / sqrt(t-start))*100);
    }

    sround = t + 6.0; //Immune from tree's in the first 6 seconds of a round starting
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
    mIdent(&model);

    vec v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    mLookAt(&model, (vec){icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2]}, (vec){v.x, v.y, v.z});
    mScale(&model, scale, scale, scale);

    mMul(&modelview, &model, &view);

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
    mIdent(&model);

    vec v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    mLookAt(&model, (vec){icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2]}, (vec){v.x, v.y, v.z});
    if(scale != 1.0)
        mScale(&model, scale, scale, scale);

    mMul(&modelview, &model, &view);

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
    mIdent(&model);

    vec v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    mLookAt(&model, (vec){icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2]}, (vec){v.x, v.y, v.z});
    //mRotate(&model, yaw * DEG2RAD, 0.f, 0.f, 1.f);
    mRotX(&model, yaw * DEG2RAD);
    if(scale != 1.0f)
        mScale(&model, scale, scale, scale);

    mMul(&modelview, &model, &view);

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
    mIdent(&model);

    vec v;
    v.x = icogrid_vertices[pos];
    v.y = icogrid_vertices[pos+1];
    v.z = icogrid_vertices[pos+2];
    vNorm(&v);
    mLookAt(&model, (vec){icogrid_vertices[pos], icogrid_vertices[pos+1], icogrid_vertices[pos+2]}, (vec){v.x, v.y, v.z});
    //mRotate(&model, yaw * DEG2RAD, 0.f, 0.f, 1.f);
    mRotX(&model, yaw * DEG2RAD);
    if(scale != 1.0f)
        mScale(&model, scale, scale, scale);

    mMul(&modelview, &model, &view);

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

void rPlayer(const GLfloat dt, vec ndir, const vec nup, const GLfloat scale)
{
    static vec lpp;
    static mat lpm;
    static GLfloat lrot = 0;
    static GLfloat rot = 0;

    // rotation
    float sscale = scale*100.f;
    if(sscale > 900.f){sscale = 900.f;}
    const float rd = (-(1300.f-sscale)*vDist(lpp, pp)) * dt;
    rot += rd;
    //printf("%f %f\n", rd, dt);

    // speed drag
    ps -= drag*dt;
    if(ps < minspeed){ps = minspeed;}

    // render
    mIdent(&pm);

    // dont render if in epsilon motion increments (basically not moving)
    if(vDist(lpp, pp) > 0.001f)
    {
        mTranslate(&pm, pp.x, pp.y, pp.z);
        mSetViewDir(&pm, ndir, nup);
    }
    else
    {
        pp.x = lpm.m[3][0];
        pp.y = lpm.m[3][1];
        pp.z = lpm.m[3][2];
        rot = lrot;
        mCopy(&pm, &lpm);
    }

    mCopy(&lpm, &pm);
    vCopy(&lpp, pp);
    lrot = rot;

    mCopy(&model, &pm);
    if(scale != 1.0f)
        mScale(&model, scale, scale, scale);
    //mRotate(&model, rot * DEG2RAD, 1.f, 0.f, 0.f);
    mRotY(&model, rot * DEG2RAD);

    mMul(&modelview, &model, &view);

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
            glUniform3f(color_id, 0.50588f, 0.84705f, 1.0f);
        else
            glUniform3f(color_id, 0.54118f, 0.16863f, 0.88627f);
        glUniform1f(opacity_id, 0.5f);
        glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
        glDisable(GL_BLEND);
        return;
    }

    // indicate round complete
    if(pscale >= msca)
    {
        const float peelspeed = 0.03f;
        static float r=0.349f, g=0.773f, b=0.471f, inv=1.f;
        r += peelspeed * dt * inv;
        g += peelspeed * dt * inv;
        b += peelspeed * dt * inv;
        if(g > 1.f)
            inv = -1.f;
        else if(g < 0.773f)
            inv = 1.f;
        glUniform3f(color_id, r, g, b);
        glUniform1f(opacity_id, 1.0f);
        glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
        return;
    }

    // normal colouring
    if(sport == 0)
        glUniform3f(color_id, pr, pg, pb);
    else
        glUniform3f(color_id, 0.753874f, 0.329309f, 0.896980f);
    glUniform1f(opacity_id, 1.0f);
    glDrawElements(GL_TRIANGLES, icosmooth2_numind, GL_UNSIGNED_SHORT, 0);
}

void rSad(const GLfloat dt)
{
    mIdent(&modelview);
    mTranslate(&modelview, 0.f, 0.f, sfd);
    //mRotate(&modelview, sfa * DEG2RAD, 1.f, 0.f, 0.f);
    mRotY(&modelview, sfa * DEG2RAD);
    mScale(&modelview, sfs, sfs, sfs);
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
    mIdent(&modelview);
    mTranslate(&modelview, 0.f, 0.f, -1.01f);

    //0.21
    GLfloat ns = 0.14f * aspect * 0.5f;
    if(ns < 0.14f)
        ns = 0.14f;
    mScale(&modelview, ns, ns, ns);

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

    mIdent(&modelview);
    mTranslate(&modelview, 0.f, 0.f, -1.01f);

    //0.20
    ns = 0.133f * aspect * 0.5f;
    if(ns < 0.133f)
        ns = 0.133f;
    mScale(&modelview, ns, ns, ns);

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
    mIdent(&modelview);
    mTranslate(&modelview, 0.f, 0.f, -1.01f);
    mScale(&modelview, 4.2f, 4.2f, 4.2f);

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

void rMenuHighlight(const GLfloat x, const GLfloat y)
{
    mIdent(&modelview);
    mTranslate(&modelview, x, y, -1.730f);
    mScale(&modelview, uw*28.f, uh*28.f, 0);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*) &projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*) &modelview.m[0][0]);
    glUniform3f(color_id, 0.39608f, 0.70196f, 0.07059f);
    glUniform1f(opacity_id, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mdlPlane.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlPlane.iid);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void rMenuMouse(const GLfloat x, const GLfloat y)
{
    shadeFullbrightT(&position_id, &projection_id, &modelview_id, &texcoord_id, &sampler_id);

    mIdent(&modelview);
    mTranslate(&modelview, x, y, -1.730f);
    mScale(&modelview, uw*36.f, uh*27.f, 0);

    glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*)&projection.m[0][0]);
    glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*)&modelview.m[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, mdlPlane.tid);
    glVertexAttribPointer(texcoord_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texcoord_id);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_mouse);
    glUniform1i(sampler_id, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mdlPlane.vid);
    glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlPlane.iid);

    glEnable(GL_BLEND);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
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
    static double lt = 0;
    double deltaTime = (t-lt)*15.0;
    //printf("%f\n", deltaTime);
    // if(deltaTime > 1.0)
    //     deltaTime = 1.0;
    lt = t;

//*************************************
// orbit / mouse control scaling
//*************************************
    if(show_ui == 0)
    {
        if(md == 1)
            glfwGetCursorPos(window, &x, &y);

        double xd = (sx-x);
        double yd = (sy-y);
        //printf("%f %f | %f %f | %f %f\n", x, y, sx, sy, xd, yd);

        if(glfwJoystickPresent(GLFW_JOYSTICK_1) == 1)
        {
            int count;
            const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
            if(count >= 2) // && fabs(axes[0]) > 0.00001f && fabs(axes[1]) > 0.00001f
            {
                if(sens < 0.f)
                {
                    xd = axes[0]*50.f*ss1*sens;
                    yd = axes[1]*50.f*ss1*sens;
                }
                else
                {
                    xd = -axes[0]*50.f*ss1*sens;
                    yd = -axes[1]*50.f*ss1*sens;
                }
                //printf("%f %f\n", axes[0], axes[1]);
            }
            if(doublestick == 1)
            {
                if(sens < 0.f)
                {
                    xd += axes[3]*50.f*ss2*sens;
                    yd += axes[4]*50.f*ss2*sens;
                }
                else
                {
                    xd += -axes[3]*50.f*ss2*sens;
                    yd += -axes[4]*50.f*ss2*sens;
                }
                //printf("%f %f\n", axes[3], axes[4]);
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
                        char strts[16];
                        timestamp(&strts[0]);
                        printf("[%s] Sens: %.3f\n", strts, sens_mul);
                        bt = t + 0.3f;
                    }
                    else if(buttons[1] == GLFW_PRESS)
                    {
                        sens_mul += 0.03f;
                        setBaseSensitivity();
                        char strts[16];
                        timestamp(&strts[0]);
                        printf("[%s] Sens: %.3f\n", strts, sens_mul);
                        bt = t + 0.3f;
                    }
                    else if(buttons[2] == GLFW_PRESS || buttons[9] == GLFW_PRESS || buttons[10] == GLFW_PRESS)
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
    }

//*************************************
// camera control
//*************************************

    mIdent(&view);
    mTranslate(&view, 0.f, 0.f, zoom);
    mRotate(&view, -yrot * DEG2RAD, 1.f, 0.f, 0.f);
    mRotate(&view, xrot * DEG2RAD, 0.f, 0.f, 1.f);

//*************************************
// camera look vector
//*************************************

    vec ray_eye;
    ray_eye.x = view.m[0][2];
    ray_eye.y = view.m[1][2];
    ray_eye.z = view.m[2][2]; // already normalised

    vec dp;
    vCopy(&dp, ray_eye);
    vMulS(&dp, dp, ps*deltaTime); //cast out
    vAdd(&pp, pp, dp);

    vec pn;
    vCopy(&pn, pp);
    vNorm(&pn);
    vec vn;
    vMulS(&vn, pn, 12.55f + ((pscale*0.5f)*0.1f));
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
            ns = (float)qRandSeed(seed+tp[i]+level, 1, 2);
            if(ns == 2.f)
                ns = 1.6f;
        }

        if(t > sround)
        {
            if(pscale >= ns*4.0f && distance(pp, tp[i]) <= pscale*0.3f)
                ns *= 0.5f;
            if(distance(pp, tp[i]) <= pscale*0.1)
            {
                static double lt = 0;
                if(t > lt)
                {
                    if(ns == 1.6f)
                    {
                        ps -= ps * 0.43f;
                        char strts[16];
                        timestamp(&strts[0]);
                        if(ps < minspeed){ps = minspeed;}
                        printf("[%s] Big Tree Collision: %.3f\n", strts, ps);
                    }
                    else
                    {
                        ps -= ps * 0.2f;
                        char strts[16];
                        timestamp(&strts[0]);
                        if(ps < minspeed){ps = minspeed;}
                        printf("[%s] Small Tree Collision: %.3f\n", strts, ps);
                    }

                    lt = t + 0.3;
                }

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
            const float ss = qRandFloatSeed(seed+sp[i]+level, 0.3f, 1.0f);
            sp[i] = 0;
            ps += 0.06f*ss;
            pscale += ss*0.03f;
            points += 0.06f*ss;

            char strts[16];
            timestamp(&strts[0]);
            printf("[%s] Snow: %.3f, %.3f, %.3f\n", strts, points, pscale, ps);

            continue;
        }

        if(sport == 0)
            rSphere(sp[i], qRandFloatSeed(seed+sp[i]+level, 0.5f, 1.0f), 1.0f, 1.0f, 1.0f);
        else
            rSphere(sp[i], qRandFloatSeed(seed+sp[i]+level, 0.5f, 1.0f), 0.f, 1.f, 1.f);
    }

    for(int i = 0; i < max_gold; i++)
    {
        if(gp[i] == 0)
            continue;

        if(distance(pp, gp[i]) <= pscale*0.1f)
        {
            const float ss = qRandFloatSeed(seed+gp[i]+level, 0.3f, 1.0f);
            gp[i] = 0;
            ps += 0.12f*ss;
            pscale += ss*0.12f;
            points += 0.12f*ss;

            char strts[16];
            timestamp(&strts[0]);
            printf("[%s] Gold: %.3f, %.3f, %.3f\n", strts, points, pscale, ps);

            continue;
        }

        if(sport == 0)
            rSphere(gp[i], qRandFloatSeed(seed+gp[i]+level, 0.4f, 0.65f), 1.0f, 1.0f, 0.0f);
        else
            rSphere(gp[i], qRandFloatSeed(seed+gp[i]+level, 0.4f, 0.65f), 1.0f, 0.f, 0.f);
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
else if(t < inrot)
{
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    rIntro((inrot-t)*0.3);
}
else if(t < hrt)
{
    shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);
    rHeart((hrt-t)*0.3);
}

//*************************************
// Draw UI
//*************************************

    if(show_ui == 1)
    {
        glfwGetCursorPos(window, &x, &y);

        // DRAW MENU BG

        shadeFullbrightT(&position_id, &projection_id, &modelview_id, &texcoord_id, &sampler_id);

        mIdent(&modelview);
        mTranslate(&modelview, 0.f, 0.f, -1.730f);
        mScale(&modelview, uw*370.f, uh*197.f, 0);

        glUniformMatrix4fv(projection_id, 1, GL_FALSE, (GLfloat*)&projection.m[0][0]);
        glUniformMatrix4fv(modelview_id, 1, GL_FALSE, (GLfloat*)&modelview.m[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, mdlPlane.tid);
        glVertexAttribPointer(texcoord_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(texcoord_id);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_menu);
        glUniform1i(sampler_id, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mdlPlane.vid);
        glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mdlPlane.iid);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        // DRAW SEED SELECTOR
        
        shadeFullbright(&position_id, &projection_id, &modelview_id, &color_id, &opacity_id);

        double rx = uw2*(x-ww2);
        double ry = uh2*-(y-wh2);
        rx += uw2*14;
        ry += uw2*14;
        //rMenuHighlight(rx, ry);
        menu_focus = 0;

        double xo = -172;
        double yo = -92;
        for(uint i = 0; i < 3; i++)
        {
            double iyo = yo + (62 * i);
            if(i == 0)
                iyo -= 1.5;
            else if(i == 1)
                iyo -= 1.5;
            for(uint j = 0; j < 9; j++)
            {
                double ixo = xo + (62 * j);
                if(j > 5)
                    ixo += 8;
                else if(j > 2)
                    ixo += 4;

                //rMenuHighlight(uw*ixo, uw*-iyo);
                if(rx > uw*ixo && rx < uw*(ixo+59) && ry > uh*-iyo && ry < uh*(-iyo+59))
                {
                    rMenuHighlight(uw*ixo, uw*-iyo);
                    uhi = (i*9)+j;
                    menu_focus = 1;
                }
                if(seed_bits[(i*9)+j] == 1)
                    rMenuHighlight(uw*ixo, uw*-iyo);
            }
        }

        // DRAW SENSITIVITY SLIDERS

        shadeFullbrightT(&position_id, &projection_id, &modelview_id, &texcoord_id, &sampler_id);
        //rMenuMouse(uw2*-162, uw2*80);
        //printf("%.3f %.3f\n", rx, ry);

        // master sensitivity
        if(ry > uh2*77 && ry < uh2*107 && rx > uw2*-162 && rx < uw2*194)
        {
            uint jammed = 0;
            if(rx < (uw2*-162)+(uw2*14))
            {
                const double nx = uw2*-162;
                rMenuMouse(nx, uh2*80);
                msx = nx;
                jammed = 1;
            }
            else if(rx > (uw2*162)+(uw2*14))
            {
                const double nx = uw2*162;
                rMenuMouse(nx, uh2*80);
                msx = nx;
                jammed = 1;
            }
            if(jammed == 0)
            {
                const double nx = rx-(uw2*15);
                rMenuMouse(nx, uh2*80);
                msx = nx;
            }
            //printf("%.3f\n", msx);

            const GLfloat mb = uw2*162.f;
            GLfloat ns = (GLfloat)( (msx+mb)/(mb*2.f) );
            if(ns <= 0.077f)
                sens_mul = -(0.3f - (ns*7.5f));
            else
                sens_mul = ns;
            //printf("%.3f %.3f %.3f\n", msx, sens_mul, mb);

            setBaseSensitivity();

            static double tc = 0;
            static GLfloat tls = 0;
            if(t > tc && tls != sens_mul)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] Sens: %.3f\n", strts, sens_mul);
                tls = sens_mul;
                tc = t+0.1;
            }
        }
        else
        {
            rMenuMouse(msx, uh2*80);
        }

        // joy1 sensitivity
        if(ry > uh2*-51 && ry < uh2*-21 && rx > uw2*-50 && rx < uw2*194)
        {
            uint jammed = 0;
            if(rx < (uw2*-49.8)+(uw2*14))
            {
                const double nx = uw2*-49.8;
                rMenuMouse(nx, uh2*-48);
                js1 = nx;
                jammed = 1;
            }
            else if(rx > (uw2*162)+(uw2*14))
            {
                const double nx = uw2*162;
                rMenuMouse(nx, uh2*-48);
                js1 = nx;
                jammed = 1;
            }
            if(jammed == 0)
            {
                const double nx = rx-(uw2*15);
                rMenuMouse(nx, uh2*-48);
                js1 = nx;
            }

            const GLfloat lmb = uw2*50.f;
            const GLfloat rmb = uw2*162.f;
            GLfloat ns = (GLfloat)( (js1+lmb)/(lmb+rmb) );
            ss1 = ns*2.f;
            //printf("%.3f %.3f %.3f %.3f\n", js1, ss1, lmb, rmb);

            static double tc = 0;
            static GLfloat tls = 0;
            if(t > tc && tls != ss1)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] JS1 Sens: %.3f\n", strts, ss1);
                tls = ss1;
                tc = t+0.1;
            }
        }
        else
        {
            rMenuMouse(js1, uh2*-48);
        }

        // joy2 sensitivity
        if(ry > uh2*-81 && ry < uh2*-53 && rx > uw2*-50 && rx < uw2*194)
        {
            uint jammed = 0;
            if(rx < (uw2*-49.8)+(uw2*14))
            {
                const double nx = uw2*-49.8;
                rMenuMouse(nx, uh2*-79);
                js2 = nx;
                jammed = 1;
            }
            else if(rx > (uw2*162)+(uw2*14))
            {
                const double nx = uw2*162;
                rMenuMouse(nx, uh2*-79);
                js2 = nx;
                jammed = 1;
            }
            if(jammed == 0)
            {
                const double nx = rx-(uw2*15);
                rMenuMouse(nx, uh2*-79);
                js2 = nx;
            }

            const GLfloat lmb = uw2*50.f;
            const GLfloat rmb = uw2*162.f;
            GLfloat ns = (GLfloat)( (js2+lmb)/(lmb+rmb) );
            ss2 = ns*2.f;
            //printf("%.3f %.3f %.3f %.3f\n", js1, ss1, lmb, rmb);

            static double tc = 0;
            static GLfloat tls = 0;
            if(t > tc && tls != ss2)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] JS2 Sens: %.3f\n", strts, ss2);
                tls = ss2;
                tc = t+0.1;
            }
        }
        else
        {
            rMenuMouse(js2, uh2*-79);
        }
    }

//*************************************
// swap buffers / display render
//*************************************

    glfwSwapBuffers(window);
}

//*************************************
// Input Handelling
//*************************************
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS){return;}

    switch(key)
    {
        // case GLFW_KEY_SPACE:
        // {
        //     // char title[256];
        //     // sprintf(title, "Level %d - Points %.2f - Time %.2f mins - Score %.2f", level, score, ((double)(t-start))/60.0, (score / sqrt(t-start))*100);
        //     // glfwSetWindowTitle(window, title);
        //     // pscale += 1.f;
        //     // msca = pscale;

        //     // pscale = msca;
        //     // sround = 0;

        //     // static uint nb = 1;
        //     // setBorderMode(nb);
        //     // nb = 1 - nb;

        //     // resetGame(2);
        //     // sround = 0;
        //     // printf("%f, %f, %f\n", pr, pg, pb);

        //     resetGame(2);
        //     pscale = 2.0f;
        //     ps = 1.f;
        //     sround = 0;
        // }
        // break;

        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        break;

        default:
        {
            // toggle ui
            show_ui = 1 - show_ui;
            if(show_ui == 1)
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] Menu Opened.\n", strts);

                // set master sense pos
                const GLfloat mb = uw2*162.f;
                msx = (sens_mul * (mb*2.f)) - mb;
                if(msx > mb)
                    msx = mb;
                else if(msx <= -mb)
                    msx = -mb;

                // set joy 1&2 sense pos
                const GLfloat lmb = uw2*50.f;
                const GLfloat rmb = uw2*162.f;
                js1 = ((ss1*0.5f) * (lmb+rmb)) - lmb;
                if(js1 > rmb)
                    js1 = rmb;
                else if(js1 <= -lmb)
                    js1 = -lmb;
                js2 = ((ss2*0.5f) * (lmb+rmb)) - lmb;
                if(js2 > rmb)
                    js2 = rmb;
                else if(js2 <= -lmb)
                    js2 = -lmb;
            }
            else
            {
                char strts[16];
                timestamp(&strts[0]);
                printf("[%s] Menu Closed.\n", strts);
            }

            // restore game mouse focus state
            static uint sv = 0;
            if(show_ui == 1)
            {
                sv = md;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                sx = x, sy = y;
                md = 0;
            }
            else
            {
                x = winw/2, y = winh/2;
                sx = x, sy = y;
                if(sv == 1)
                {
                    glfwSetCursorPos(window, x, y);
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                    sx = x, sy = y;
                    md = 1;
                }
            }
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
            if(show_ui == 0)
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
            else
            {
                if(menu_focus == 1)
                {
                    seed_bits[uhi] = 1 - seed_bits[uhi];
                    seed = 1337;
                    for(int i = 0; i < 27; i++)
                        seed ^= seed_bits[i] << i;
                    char strts[16];
                    timestamp(&strts[0]);
                    printf("[%s] Seed: %u\n", strts, seed);
                    srand(seed);
                    resetGame(4);
                }
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS && show_ui == 0)
    {
        sens_mul -= 0.1f;
        setBaseSensitivity();
        char strts[16];
        timestamp(&strts[0]);
        printf("[%s] Sens: %.3f\n", strts, sens_mul);
    }
    
    if(button == GLFW_MOUSE_BUTTON_5 && action == GLFW_PRESS && show_ui == 0)
    {
        sens_mul += 0.1f;
        setBaseSensitivity();
        char strts[16];
        timestamp(&strts[0]);
        printf("[%s] Sens: %.3f\n", strts, sens_mul);
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && show_ui == 0)
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
    ww = winw;
    wh = winh;
    ww2 = ww/2;
    wh2 = wh/2;
    uw = (double)aspect / ww;
    uh = 1 / wh;
    uw2 = (double)aspect / ww2;
    uh2 = 1 / wh2;

    mIdent(&projection);
    mPerspective(&projection, 60.0f, aspect, 1.0f, 60.0f); 
}

//*************************************
// Process Entry Point
//*************************************
int main(int argc, char** argv)
{
    // should I be using getopt? I prefer this.
    uint noborder = 0, msaa = 16;
    if(argc >= 2){seed = atof(argv[1]);}
    if(argc >= 3){msaa = atoi(argv[2]);}
    if(argc >= 4){noborder = atoi(argv[3]);}
    if(argc >= 5){winw = atoi(argv[4]);}
    if(argc >= 6){winh = atoi(argv[5]);}
    if(argc >= 7){sens_mul = atof(argv[6]);}
    if(argc >= 8){doublestick = atoi(argv[7]);}
    if(argc >= 9){ss1 = atof(argv[8]);}
    if(argc >= 10){ss2 = atof(argv[9]);}

    // help
    printf("Snowball by James William Fletcher (james@voxdsp.com)\n");
    printf("A novel speed run game akin to a platform loop/buzz wire game.\n\n");
    printf("CONTROLS:\n");
    printf("Mouse 1 (Left)  = Toggle Control\n");
    printf("Mouse 2 (Right) = Change View\n");
    printf("Mouse X1        = Decrease mouse speed\n");
    printf("Mouse X2        = Increase mouse speed\n\n");
    printf("COMMAND LINE:\n");
    printf("./snowball <random seed> <msaa 0-16> <noborder 0-1> <width> <height> <master input/mouse sensitivity> <double joy stick> <joy 1 sensitivity> <joy 2 sensitivity>\n\n");
    printf("Specify a mouse sensitivity of -x such as -1.0 to invert the mouse.\n\n");
    printf("A web version of the game: http://snowball.mobi\n\n");
    printf("HOW TO:\n");
    printf("You have to roll around collecting snow and avoiding trees until your snowball is large enough to continue to the next level.\n\n");
    printf("Once you have accumulated a large enough snowball, the poles (obelisks) will turn into teleportation pads to the next round; the aim is to complete each level or a sequence of levels with the highest score in the shortest time.\n\n");
    printf("The larger the snowballs you collect higher the score. Gold snow is double the reward of white. Picking up snow increases your speed temporarily; the idea is to enter a flow of collecting snow so that your speed stays up.\n\n");
    printf("Try not to hit the trees as they will slow you down and reduce the mass of your snowball, unless your snowball is large enough to consume them, then they slow you down but also add to the mass of your snowball. Trees will shrink slightly as you approach them to show that they can be consumed.\n\n");
    printf("Your score is updated in the program title bar at the end of each level.\n\n");

    // settings
    printf("Argv Settings:\n");
    printf("SEED: %u\n", seed);
    printf("MSAA: %u\n", msaa);
    printf("NoBorder: %u\n", noborder);
    if(noborder == 1){winh = winw;}
    printf("Resolution: %ux%u\n", winw, winh);
    printf("Master Sensitivity: %.3f\n", sens_mul);
    printf("DoubleStick: %u\n", doublestick);
    printf("SS1: %.3f\n", ss1);
    printf("SS2: %.3f\n\n", ss2);

    // init glfw
    if(!glfwInit()){exit(EXIT_FAILURE);}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    if(noborder == 1)
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
    else
        glfwWindowHint(GLFW_SAMPLES, msaa);
    window = glfwCreateWindow(winw, winh, "Snowball.mobi", NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    const GLFWvidmode* desktop = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, ((desktop->width-sx)/2)-(winw/2), ((desktop->height-sy)/2)-(winh/2)); // center window on desktop
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
    if(r == 0)      {ipd = (unsigned char*)&icon_image.pixel_data;}
    else if(r == 1) {ipd = (unsigned char*)&icon_image1.pixel_data;}
    else            {ipd = (unsigned char*)&icon_image2.pixel_data;}
    glfwSetWindowIcon(window, 1, &(GLFWimage){16, 16, ipd});

    // seed random
    srand(seed);


//*************************************
// projection
//*************************************

    window_size_callback(window, winw, winh);
    if(aspect < 1.0f)
        zoom = -26.0f;

//*************************************
// bind vertex and index buffers
//*************************************

    // ***** BIND MENU PLANE *****
    GLfloat  plane_vert[] = {1.000000,-1.000000,0.000000,-1.000000,1.000000,0.000000,-1.000000,-1.000000,0.000000,1.000000,1.000000,0.000000};
    GLushort plane_indi[] = {0,1,2,0,3,1};
    esBindModel(&mdlPlane, plane_vert, 9, plane_indi, 6);
    GLfloat plane_texc[] = {1.f,1.f, 0.f,0.f, 0.f,1.f, 1.f,0.f};
    esBind(GL_ARRAY_BUFFER, &mdlPlane.tid, plane_texc, sizeof(plane_texc), GL_STATIC_DRAW);
    tex_menu = esLoadTexture(menu_image.width, menu_image.height, &menu_image.pixel_data[0]);
    tex_mouse = esLoadTextureA(mouse_image.width, mouse_image.height, &mouse_image.pixel_data[0]);

    // ***** BIND ICOGRID *****
    esBindModel(&mdlGrid, icogrid_vertices, icogrid_numvert, icogrid_indices, icogrid_numind);

    // ***** BIND SADPLY *****
    esBindModel(&mdlSad, sad_vertices, sad_numvert, sad_indices, sad_numind);

    // ***** BIND HEART *****
    esBindModel(&mdlHeart, heart_vertices, heart_numvert, heart_indices, heart_numind);

    // ***** BIND INTRO *****
    esBindModel(&mdlIntro, intro_vertices, intro_numvert, intro_indices, intro_numind);

    // ***** BIND TREE *****
    esBind(GL_ARRAY_BUFFER, &mdlTree.vid, tree_vertices, sizeof(tree_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree.cid, tree_colors, sizeof(tree_colors), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree_iced, tree_iced_colors, sizeof(tree_iced_colors), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree.nid, tree_normals, sizeof(tree_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTree.iid, tree_indices, sizeof(tree_indices), GL_STATIC_DRAW);

    // ***** BIND POLE / OBELISK *****
    esBind(GL_ARRAY_BUFFER, &mdlTele.vid, tele_vertices, sizeof(tele_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTele.nid, tele_normals, sizeof(tele_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTele.cid, tele_colors, sizeof(tele_colors), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlTele.iid, tele_indices, sizeof(tele_indices), GL_STATIC_DRAW);

    // ***** BIND TELE *****
    esBind(GL_ARRAY_BUFFER, &mdlCat.vid, cat_vertices, sizeof(cat_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlCat.nid, cat_normals, sizeof(cat_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlCat_aura, cat_aura_colors, sizeof(cat_aura_colors), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlCat.cid, cat_colors, sizeof(cat_colors), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlCat.iid, cat_indices, sizeof(cat_indices), GL_STATIC_DRAW);

    // ***** BIND SPHERE *****
    esBind(GL_ARRAY_BUFFER, &mdlSphere.vid, icosmooth2_vertices, sizeof(icosmooth2_vertices), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlSphere.nid, icosmooth2_normals, sizeof(icosmooth2_normals), GL_STATIC_DRAW);
    esBind(GL_ARRAY_BUFFER, &mdlSphere.iid, icosmooth2_indices, sizeof(icosmooth2_indices), GL_STATIC_DRAW);

//*************************************
// compile & link shader program
//*************************************

    //makeAllShaders();
    makeLambert();
    makeLambert1();
    makeLambert3();
    makeFullbright();
    makeFullbrightT();

//*************************************
// configure render options
//*************************************

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
    double lt = glfwGetTime() + 16, fc = 0;
    while(!glfwWindowShouldClose(window))
    {
        t = glfwGetTime();

        glfwPollEvents();
        main_loop();
        
        fc++;
        if(t > lt)
        {
            static double lp = 0;
            char strts[16];
            timestamp(&strts[0]);
            printf("[%s] FPS: %f\n", strts, fc/16);
            printf("[%s] PPS: %f\n", strts, (points-lp)/16);
            fc = 0;
            lp = points;
            lt = t + 16;
        }
    }

    // final score
    char strts[16];
    timestamp(&strts[0]);
    printf("[%s] Level %d - Points %f - Time %f mins - Score %f\n", strts, level, points, ((double)(t-start))/60.0, (points / sqrt(t-start))*100);

    // done
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}
