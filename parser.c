#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

void parse_file ( char * filename ) {

  FILE *f;
  if ( strcmp(filename, "stdin") == 0 ){
    f = stdin;
  }
  else{
    f = fopen(filename, "r");
  }
  screen s;
  clear_screen(s);
  color c;
  c.red = 0;
  c.green = 0;
  c.blue = 0;
  char line[256], axis;
  double xval[4], yval[4], zval[4], theta, r, r1;
  int step = 100, step_3d = 10;
  struct stack * cstack = new_stack();
  struct matrix *tmp;

  while ( fgets(line, 255, f) ) {
    line[strlen(line)-1]='\0';
    if ( !strcmp(line, "line") || !strcmp(line, "bezier") || !strcmp(line, "hermite") || !strcmp(line, "circle") || !strcmp(line, "box") || !strcmp(line, "sphere") || !strcmp(line, "torus")) {
      tmp = new_matrix(4, 4);
      if ( !strcmp(line, "box") || !strcmp(line, "sphere") || !strcmp(line, "torus") ) {
        if ( !strcmp(line, "box") ) {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf %lf %lf", xval, yval, zval, xval+1, yval+1, zval+1);
          add_box(tmp, xval[0], yval[0], zval[0], xval[1], yval[1], zval[1]);
        } else if ( !strcmp(line, "sphere") ) {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf", xval, yval, zval, &r);
          add_sphere(tmp, xval[0], yval[0], zval[0], r, step_3d);
        } else {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf %lf", xval, yval, zval, &r, &r1);
          add_torus(tmp, xval[0], yval[0], zval[0], r, r1, step_3d);
        }
        matrix_mult(peek(cstack), tmp);
        draw_polygons(tmp, s, c);
      }
      else {
        if ( !strcmp(line, "line") ) {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf %lf %lf", xval, yval, zval, xval+1, yval+1, zval+1);
          add_edge(tmp, xval[0], yval[0], zval[0], xval[1], yval[1], zval[1]);
        } else if ( !strcmp(line, "hermite") ) {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", xval, yval, xval+1, yval+1, xval+2, yval+2, xval+3, yval+3);
          add_curve( tmp, xval[0], yval[0], xval[1], yval[1], xval[2], yval[2], xval[3], yval[3], step, HERMITE);
        } else if ( !strcmp(line, "bezier") ) {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf", xval, yval, xval+1, yval+1, xval+2, yval+2, xval+3, yval+3);
          add_curve( tmp, xval[0], yval[0], xval[1], yval[1], xval[2], yval[2], xval[3], yval[3], step, BEZIER);
        } else {
          fgets(line, sizeof(line), f);
          sscanf(line, "%lf %lf %lf %lf", xval, yval, zval, &r);
          add_circle( tmp, xval[0], yval[0], zval[0], r, step);
        }
        matrix_mult(peek(cstack), tmp);
        draw_lines(tmp, s, c);
      }
      free_matrix(tmp);
    } else if ( !strcmp(line, "move") || !strcmp(line, "rotate") || !strcmp(line, "scale") ) {
      if ( !strcmp(line, "move") ) {
        fgets(line, sizeof(line), f);
        sscanf(line, "%lf %lf %lf", xval, yval, zval);
        tmp = make_translate( xval[0], yval[0], zval[0]);
      } else if ( !strcmp(line, "rotate") ) {
        fgets(line, sizeof(line), f);
        sscanf(line, "%c %lf", &axis, &theta);
        theta = theta * (M_PI / 180);
        if ( axis == 'x' ) tmp = make_rotX( theta );
        else if ( axis == 'y' ) tmp = make_rotY( theta );
        else tmp = make_rotZ( theta );
      } else {
        fgets(line, sizeof(line), f);
        sscanf(line, "%lf %lf %lf", xval, yval, zval);
        tmp = make_scale( xval[0], yval[0], zval[0]);
      }
      matrix_mult(peek(cstack), tmp);
      copy_matrix(tmp, peek(cstack));
      free_matrix(tmp);
    } else if ( !strcmp(line, "push") ) {
      push(cstack);
    } else if ( !strcmp(line, "pop") ) {
      pop(cstack);
    } else if ( !strcmp(line, "display") ) {
      display( s );
    } else if ( !strcmp(line, "save") ) {
      fgets(line, 255, f);
      save_extension(s, line);
    } else if ( !strcmp(line, "clear") ) {
      clear_screen(s);
      free_stack(cstack);
      cstack = new_stack();
    }
  }
  free_stack(cstack);
  fclose(f);
}
