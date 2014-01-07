
#include <GL/glut.h>    
#include <stdio.h>
#include "life.h"
static char *backgroundTexture;
static char *lifeTexture;
static char *lonelinessTexture;
static char *overcrowdingTexture;
static short field[35][32];
void reshape(int w, int h)
{
  //  printf("%d %d \n",w, h );  here's some bag... sometimes w!=699 or h!=699
  // but glutReshpeWindow don't change window size
    if ((w!=699) || (h!=699)){
          glutReshapeWindow(699,699);
        }
                    glViewport(0, 0, 700, 700);
           
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 700, 0, 700);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void mouseClick( int button, int state, int x, int y )
{
    printf("%d %d \n",x/20,y/20); 
    if (button==GLUT_LEFT_BUTTON)
    {
      field[x/20][y/20]=ALIVE;
    }
}
void mouseMotion(int x, int y)
{
    field[x/20][y/20]=ALIVE;
    printf("%d %d \n",x/20,y/20); 
}
char* readTexture(char* fileName, int alpha)
{
    int rgbBytes,d,bytesPerRow;
    char *backgroundTexture, *otherDate;
    FILE *f;
    HeadGr head;
    f=fopen(fileName,"rb");
    if (f==NULL)
        return NULL;
    else
    {
        fread(&head,54,1,f);
        rgbBytes=head.biWidth*(3+alpha);
        d=rgbBytes%4;
        bytesPerRow=d ? rgbBytes+(4-d)  :rgbBytes;
        backgroundTexture=(char*)malloc(bytesPerRow*head.biHeight);
        otherDate=(char*) malloc(head.bfOffBits-54);
        fread(otherDate,(head.bfOffBits-54),1,f);
        fread(backgroundTexture,bytesPerRow*head.biHeight,1,f);          
        fclose(f); 
        free(otherDate);
        return backgroundTexture;
    }
}
void display()
{
    int i,j;
    glClear(GL_COLOR_BUFFER_BIT);
    //background
    if(backgroundTexture==NULL)
    {
        glBegin(GL_QUADS);
            glColor3f(0.0, 1.0, 1.0);
              glVertex2i(0, 0);
              glVertex2i(699,0);
              glVertex2i(699,699);
              glVertex2i(0,699);
        glEnd();
    }
    else
    {
        glDrawPixels(699, 699, GL_BGR, GL_UNSIGNED_BYTE, backgroundTexture);
    }
    //lines
    for (i=0;i<70;i++)
      {
        glBegin(GL_LINES);
          glColor3f(0.0, 0.0, 0.0);
          glVertex2i(0,i*20);
          glVertex2i(699,i*20);
          glVertex2i(i*20,0);
          glVertex2i(i*20,699);

        glEnd();
      }
      for (i=0;i<35;i++)
        for (j=0;j<32;j++)
          if (field[i][j]==ALIVE)
          {
            glBegin(GL_QUADS);
              glColor3f(1.0, 1.0, 1.0);
              glVertex2i(i*20,700- j*20);
              glVertex2i((i+1)*20,700-j*20);
              glVertex2i((i+1)*20,700-(j+1)*20);
              glVertex2i(i*20,700-(j+1)*20);
            glEnd();

          }


    glutSwapBuffers();
}
void step(int value){
  static int i=0;
 //here must be rules
  display();
  glutTimerFunc(1000,step,1);
}
int main (int argc, char * argv[])
{
        backgroundTexture=readTexture("backgroundTexture.bmp",0);
        overcrowdingTexture=readTexture("overcrowding.bmp",1);
        lifeTexture=readTexture("life.bmp",1);
        lonelinessTexture=readTexture("loneliness.bmp",1);

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
        glutInitWindowSize(699, 699);
        glutCreateWindow("Lab5");       
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutMouseFunc(mouseClick);
        glutMotionFunc(mouseMotion);
        glutTimerFunc(1000,step,1);
        glutMainLoop();
        free(backgroundTexture);
        return 0;
}
