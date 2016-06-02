#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <vector>
#include <cmath>

#include "pixyhandle.hpp"

void RowReduce(double A[2][3]);
void InnerProduct(double A[3][3], double x[3], double b[3]);
void CalculateAngles(double x_fov, double y_fov, double max_xpos, double max_ypos, int*[2] pos_array, double*[2] angle_array);
void CalculateLineVectors(double** angle_array, double*[3] lines);
void CreateLineMatrix(double*[3] line_matrix, double*[3] lines, double cam1_x, double cam1_y, double cam1_z, double cam2_x, double cam2_y, double cam2_z);
void CreateSolveMatrix(double*[3] solve_matrix, double* inner_prod1, double* inner_prod2);
void FindPoints(double cam1_x, double cam1_y, double cam1_z, double cam2_x, double cam2_y, double cam2_z, double*[3] lines, double* point1, double* point2, double* midpoint, double t, double s);