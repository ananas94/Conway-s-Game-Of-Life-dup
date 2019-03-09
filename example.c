
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "life.h"

//here was dirty hack
//this header file have 127k+ lines
//and it was bitmap
//#include "Textures.h"
#include "Textures.h"
static unsigned int backgroundTexture;
static unsigned int lifeTexture;
static unsigned int lonelinessTexture;
static unsigned int overcrowdingTexture;
static short field[fieldW][fieldH];
static short numb[fieldW][fieldH];
static short itWorks = 0;
static int stepTime = 1000;
static int mouseButton;
static int drawDead = 1;

void reshape(int w, int h)
{
  /*  here's some bag... sometimes w!=windowW or h!=windowH
      but glutReshpeWindow don't change window size
      somewere i saw, that nobody can fix */
  if ((w != windowW) || (h != windowH))
  {
    glutReshapeWindow(windowW, windowH);
  }
  glViewport(0, 0, windowW, windowH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, windowW, windowH, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouseMotion(int x, int y)
{
  if (mouseButton == GLUT_LEFT_BUTTON)
    if ((x / cellSize >= 0 && x / cellSize < fieldW) && (y / cellSize >= 0 && y / cellSize < fieldH))
      field[x / cellSize][y / cellSize] = ALIVE;
  if (mouseButton == GLUT_RIGHT_BUTTON)
    if ((x / cellSize >= 0 && x / cellSize < fieldW) && (y / cellSize >= 0 && y / cellSize < fieldH))
      field[x / cellSize][y / cellSize] = 0;
}
void mouseClick(int button, int state, int x, int y)
{
  mouseButton = button;
  mouseMotion(x, y);
}
TEXTURE *readImg(char *fileName, int alpha)
{
  int rgbBytes, d, bytesPerRow, i, ob;
  char *Img, *otherDate;
  TEXTURE *t = malloc(sizeof(TEXTURE));
  FILE *f;
  HeadGr head;
  f = fopen(fileName, "rb");
  if (f == NULL)
    return NULL;
  else
  {
    fread(&head, 54, 1, f);
    rgbBytes = head.biWidth * (3 + alpha);
    d = rgbBytes % 4;
    bytesPerRow = d ? rgbBytes + (4 - d) : rgbBytes;
    Img = (char *)malloc(bytesPerRow * head.biHeight);
    otherDate = (char *)malloc(head.bfOffBits - 54);
    fread(otherDate, (head.bfOffBits - 54), 1, f);
    fread(Img, bytesPerRow * head.biHeight, 1, f);
    fclose(f);
    for (i = 0; i < bytesPerRow * head.biHeight; i += 3)
    {
      ob = *(Img + i);
      *(Img + i) = *(Img + i + 2);
      *(Img + 2 + i) = ob;
    }
    free(otherDate);
    (*t).width = head.biWidth;
    (*t).height = head.biHeight;
    (*t).bytes_per_pixel = 3 + alpha;
    (*t).pixel_data = Img;
    return t;
  }
}
int initTexture(TEXTURE tex)
{
  int textureNum;
  glGenTextures(1, &textureNum);
  glBindTexture(GL_TEXTURE_2D, textureNum);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               tex.width,
               tex.height,
               0,
               (tex.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA),
               GL_UNSIGNED_BYTE, tex.pixel_data);
  return textureNum;
}
void display()
{
  int i, j;
  glClear(GL_COLOR_BUFFER_BIT);
  //draw background
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, backgroundTexture);
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 1.0);
  glTexCoord2d(0, 0);
  glVertex2i(0, 0);
  glTexCoord2d(0, 1);
  glVertex2i(windowW, 0);
  glTexCoord2d(1, 1);
  glVertex2i(windowW, windowH);
  glTexCoord2d(1, 0);
  glVertex2i(0, windowH);
  glEnd();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //draw field
  for (i = 0; i < fieldW; i++)
    for (j = 0; j < fieldH; j++)
      if (field[i][j])
      {
        glColor4f(1.0, 1.0, 1.0, 1.0);
        if (field[i][j] == ALIVE)
          glBindTexture(GL_TEXTURE_2D, lifeTexture);
        else if (field[i][j] == 0 || !drawDead)
          continue;
        if (field[i][j] == DEAD_BY_LONELINESS && drawDead)
          glBindTexture(GL_TEXTURE_2D, lonelinessTexture);
        if (field[i][j] == DEAD_BY_OVERCROWDING && drawDead)
          glBindTexture(GL_TEXTURE_2D, overcrowdingTexture);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex2i(i * cellSize, j * cellSize);
        glTexCoord2d(0, 1);
        glVertex2i((i + 1) * cellSize, j * cellSize);
        glTexCoord2d(1, 1);
        glVertex2i((i + 1) * cellSize, (j + 1) * cellSize);
        glTexCoord2d(1, 0);
        glVertex2i(i * cellSize, (j + 1) * cellSize);
        glEnd();
      }
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  //draw lines
  for (i = 0; i < fieldH; i++)
  {
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2i(0, i * cellSize);
    glVertex2i(windowW, i * cellSize);
    glEnd();
  }
  for (i = 0; i < fieldW; i++)
  {
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2i(i * cellSize, 0);
    glVertex2i(i * cellSize, windowH);
    glEnd();
  }
  glutSwapBuffers();
}

void step(int value)
{
  int i, j, k, g;
  if (itWorks)
  {
    //clear numb array
    for (i = 0; i < fieldW; i++)
      for (j = 0; j < fieldH; j++)
        numb[i][j] = 0;
    //counting number of alive cell's neighbors
    for (i = 0; i < fieldW; i++)
      for (j = 0; j < fieldH; j++)
      {
        if (field[i][j] == ALIVE)
          for (k = -1; k < 2; k++)
            for (g = -1; g < 2; g++)
              if (k != 0 || g != 0)
                if ((i + k) >= 0 && (j + g) >= 0 && (i + k) < fieldW && (j + g) < fieldH)
                  numb[i + k][j + g]++;
      }
    //making findings from numb array
    for (i = 0; i < fieldW; i++)
      for (j = 0; j < fieldH; j++)
      {
        if (field[i][j] == ALIVE && numb[i][j] < 2)
          field[i][j] = DEAD_BY_LONELINESS;
        if (field[i][j] == ALIVE && numb[i][j] > 3)
          field[i][j] = DEAD_BY_OVERCROWDING;
        if (field[i][j] != ALIVE && numb[i][j] == 3)
          field[i][j] = ALIVE;
      }
  }
  glutTimerFunc(stepTime, step, 1);
}
void reDisplay(int value)
{
  display();
  glutTimerFunc(30, reDisplay, 2);
}
void keyboard(unsigned char key, int x, int y)
{
  if (key = ' ')
    itWorks ^= 1;
}
void funcKeys(int key, int x, int y)
{
  if (key == GLUT_KEY_DOWN)
    if (stepTime < 5000)
      stepTime += 20;
  if (key == GLUT_KEY_UP)
    if (stepTime > 200)
      stepTime -= 20;
  if (key == GLUT_KEY_F1)
  {
    //here may be help
    //or youre ads ^_^
  }
  if (key == GLUT_KEY_F2)
    drawDead ^= 1;
  if (key == GLUT_KEY_F9)
  {
    int i, j;
    for (i = 0; i < fieldW; i++)
      for (j = 0; j < fieldH; j++)
        field[i][j] = 0;
  }
}
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(windowW, windowH);
  glutCreateWindow("Lab5");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(mouseClick);
  glutMotionFunc(mouseMotion);
  glutTimerFunc(1000, step, 1);
  glutTimerFunc(100, reDisplay, 2);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(funcKeys);
  //init textures
  TEXTURE *t = readImg("backgroundTexture.bmp", 0);
  backgroundTexture = initTexture(*t);
  printf("%x\n", glGetError());
  free((*t).pixel_data);
  free(t);
  lifeTexture = initTexture(life);
  lonelinessTexture = initTexture(loneliness);
  overcrowdingTexture = initTexture(overcrowding);
  glutMainLoop();
  return 0;
}