#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "gfx.h"
#include "bmp.h"

int width = 128;
int height = 128;

struct pixel
{
  int red;
  int green;
  int blue;
};

struct pixel orig[128][128]; // original image
struct pixel eb[128][128];

struct operation
{
  int op; // 1-copy/paste, 2-blur, 3-grayscale, 4-brighten
  int x;  // destination for copy/paste
  int y;
  //struct pixel stack_eb[128][128];
};

struct operation stack[100]; // max. 100 operations
struct operation *sptr;      // stack pointer
struct operation *max_sptr;  // maximum stack pointer for redo

void edit(char *filename);
void filter(int op, int x, int y);
void copy_paste(int x, int y);
void blur();
void grayscale();
void brighten(int brightness);
void undo();
void redo();
void view();
void save(char *filename);

int main(int argc,char **argv)
{

  // initialize stack pointers
  sptr = stack;
  max_sptr = stack;
  char input[2];
  int op, x, y;
  char *filename=argv[1];
  edit(filename);

  while (1)
  {
    printf("Enter command: ");
    scanf("%s", input);

    switch (input[0])
    {
    case 'F':
      scanf("%d %d %d", &op, &x, &y);
      filter(op, x, y);
      break;
    case 'U':
      undo();
      break;
    case 'R':
      redo();
      break;
    case 'V':
      view();
      break;
    case 'S':
      save(filename);
      break;
    case 'X':
      return 0;
      break;
    }
  }
  return 0;
}

void edit(char *filename)
{
  bmp_image *image = malloc(sizeof(bmp_image));
  image = bmp_read(filename);
  if (image == NULL)
  {
    printf("Error: Failed to open the file.\n");
    return;
  }

  width = image->dib.bmiHeader.biWidth;
  height = image->dib.bmiHeader.biHeight;

  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      orig[i][j].red = bmp_getpixelcolor(image, i, height-1-j, BMP_COLOR_RED);
      orig[i][j].green = bmp_getpixelcolor(image, i,height-1-j, BMP_COLOR_GREEN);
      orig[i][j].blue = bmp_getpixelcolor(image, i,height-1-j, BMP_COLOR_BLUE);
      eb[i][j] = orig[i][j];
    }
  }

  gfx_open(width, height, "Iedit");

  // draw image on screen
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      gfx_color(eb[i][j].red, eb[i][j].green, eb[i][j].blue);
      gfx_point(i, j);
    }
  }
  gfx_flush();
}
void filter(int op, int x, int y)
{
  if (sptr == stack + 101)
  {
    return;
  }

  // push operation on stack
  sptr->op = op;
  sptr->x = x;
  sptr->y = y;
  sptr++;
  max_sptr = sptr;

  switch (op)
  {
  case 1:
    copy_paste(x, y);
    break;
  case 2:
    blur();
    break;
  case 3:
    grayscale();
    break;
  case 4:
    brighten(x);
    break;
  }
}

void copy_paste(int x, int y)
{
  // copy top left part of image from (0,0) to (20,20)
  // paste starting from (x,y)
  for (int i = 0; i < x; i++)
  {
    for (int j = 0; j < y; j++)
    {
      eb[x + i][y + j] = eb[i][j];
    }
  }
}

void blur()
{

  for (int i = 1; i < width ; i++)
  {
    for (int j = 1; j < height ; j++)
    {
      int red = (eb[i][j].red + eb[i + 1][j].red + eb[i - 1][j].red + eb[i][j + 1].red + eb[i][j - 1].red) / 5;
      int green = (eb[i][j].green + eb[i + 1][j].green + eb[i - 1][j].green + eb[i][j + 1].green + eb[i][j - 1].green) / 5;
      int blue = (eb[i][j].blue + eb[i + 1][j].blue + eb[i - 1][j].blue + eb[i][j + 1].blue + eb[i][j - 1].blue) / 5;
      eb[i][j].red = red;
      eb[i][j].green = green;
      eb[i][j].blue = blue;
    }
  }
}

void grayscale()
{
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      int gray = (eb[i][j].red + eb[i][j].green + eb[i][j].blue) / 3;
      eb[i][j].red = gray;
      eb[i][j].green = gray;
      eb[i][j].blue = gray;
    }
  }
}

void brighten(int brightness)
{
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      eb[i][j].red = eb[i][j].red + brightness;
      if (eb[i][j].red > 255)
        eb[i][j].red = 255;
      if (eb[i][j].red < 0)
        eb[i][j].red = 0;
      eb[i][j].green = eb[i][j].green + brightness;
      if (eb[i][j].green > 255)
        eb[i][j].green = 255;
      if (eb[i][j].green < 0)
        eb[i][j].green = 0;
      eb[i][j].blue = eb[i][j].blue + brightness;
      if (eb[i][j].blue > 255)
        eb[i][j].blue = 255;
      if (eb[i][j].blue < 0)
        eb[i][j].blue = 0;
    }
  }
}

void undo()
{
  if (sptr > stack)
  {
    sptr--;
    
  }
  else
  {
    printf("No more operations to undo.\n");
  }
}

void redo()
{
  if (sptr < max_sptr)
  {
    
    sptr++;
  }
  else
  {
    printf("No more operations to redo.\n");
  }
}

void view()
{
  int i, j;
  // copy orig to eb
  for (i = 0; i < width; i++)
  {
    for (j = 0; j < height; j++)
    {
      eb[i][j] = orig[i][j];
    }
  }

  // apply filters on eb
  struct operation *ptr;
  for (ptr = stack; ptr < sptr; ptr++)
  {
    switch (ptr->op)
    {
    case 1:
      copy_paste(ptr->x, ptr->y);
      break;
    case 2:
      blur();
      break;
    case 3:
      grayscale();
      break;
    case 4:
      brighten(ptr->x);
      break;
    }
  }

  // draw image on window
  for (i = 0; i < width; i++)
  {
    for (j = 0; j < height; j++)
    {
      gfx_color(eb[i][j].red, eb[i][j].green, eb[i][j].blue);
      gfx_point(i, j);
    }
  }

  gfx_flush();
}

void save(char *filename)
{

  bmp_image *image = malloc(sizeof(bmp_image));
  image = bmp_read(filename);
  if (image == NULL)
  {
    printf("Error: Failed to open the file.\n");
    return;
  }
  int i, j;
  // apply filters on img
  struct operation *ptr;
  for (ptr = stack; ptr < sptr; ptr++)
  {
    switch (ptr->op)
    {
    case 1:
      copy_paste(ptr->x, ptr->y);
      break;
    case 2:
      blur();
      break;
    case 3:
      grayscale();
      break;
    case 4:
      brighten(ptr->x);
      break;
    }
  }
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < height; j++)
    {
      bmp_setpixelcolor(image, i,height-1-j, BMP_COLOR_RED,eb[i][j].red);
      bmp_setpixelcolor(image, i,height-1-j, BMP_COLOR_GREEN,eb[i][j].green);
      bmp_setpixelcolor(image, i,height-1-j, BMP_COLOR_BLUE,eb[i][j].blue);
    }
  }
  // write img to file
  if (bmp_save(image, filename) == 0)
  {
    printf("Error saving file.\n");
    return;
  }

  free(image);
}
