///////////////////////////////////////////////////////////////////////////////
//
//  Name:      renderDepthMex.cc
//  Purpose:   Renders an inverse depth map given a triangle mesh (0-indexed)
//             and 3x4 camera matrix P = K*[R t].
//             This can also be used to generate a silhouette from mesh.
//  Author:    Daeyun Shin <dshin11@illinois.edu>
//  Created:   04.05.2015
//  Modified:  10.17.2015
//  Version:   0.2
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
///////////////////////////////////////////////////////////////////////////////

/*
arg0: vertices  n by 3
arg1: faces     n by 3
arg2: imSize    1 by 2
arg3: P         3 by 4

Outputs and image of inverse depth values d(y,x) such that inv(K)*[x y 1]/d(y,x)
restores the camera coordinates.

Example:
fv = isosurface(X, Y, Z, V, 0.5);
im = renderDepthMex(fv.vertices, fv.faces-1, [720 1280], K*[R t]);

*/

#include "mex.h"
#include "mexutil.h"
#include <vector>
#include <cstdlib>

struct Vertex {
  Vertex() = default;
  Vertex(double x, double y, double d) : x(x), y(y), d(d) {}
  double x, y, d;
};

struct Triangle {
  Triangle() = default;
  Triangle(const Vertex& a, const Vertex& b, const Vertex& c)
      : a(a), b(b), c(c) {}
  Vertex a, b, c;
};

class Image {
 public:
  Image(double w, double h) : w_(w), h_(h), data_(w * h, 0) {}

  double get(int x, int y) const { return data_[y + x * h_]; }

  void set_depth(int x, int y, double d) {
    if (d < 1e-5) {
      return;
    }

    double idepth = 1 / d;
    if (data_[y + x * h_] < idepth) {
      data_[y + x * h_] = idepth;
    }
  }

  int w() { return w_; }
  int h() { return h_; }

 private:
  int w_, h_;
  std::vector<double> data_;
};

int Round(double a) { return ((int)(a + 0.5)); }

// Given x_im = x_i/d_i in p_i = (x_i, y_i, d_i) and two points that form a line
// that contains p_i, find d_i;
double FindDepthFrom2dX(double x2d, const Vertex& start, const Vertex& end) {
  double x0 = start.x * start.d, x1 = end.x * end.d, d0 = start.d, d1 = end.d;
  return (x0 * d1 - x1 * d0) / (x2d * d1 - x2d * d0 - x1 + x0);
}

double FindDepthFrom2dY(double y2d, const Vertex& start, const Vertex& end) {
  double y0 = start.y * start.d, y1 = end.y * end.d, d0 = start.d, d1 = end.d;
  return (y0 * d1 - y1 * d0) / (y2d * d1 - y2d * d0 - y1 + y0);
}

void DrawVertLine(double fx, double fy1, double fy2, double fd1, double fd2,
                  Image* im) {
  if (fy2 - fy1 < 0.5) {
    return;
  }

  int x = Round(fx), y1 = Round(fy1), y2 = Round(fy2);

  if (x < 0 || y2 < 0 || x >= im->w() || y1 >= im->h()) {
    return;
  }

  Vertex s(fx, fy1, fd1), e(fx, fy2, fd2);

  for (int y = std::max(y1, 0); y <= std::min(y2, im->h() - 1); ++y) {
    double w = FindDepthFrom2dY(y, s, e);
    im->set_depth(x, y, w);
  }
}

void FillLeftFlatTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3,
                          Image* im) {
  double slope1 = (v1.y - v2.y) / (v1.x - v2.x);
  double slope2 = (v1.y - v3.y) / (v1.x - v3.x);
  double y1, y2, d1, d2;

  for (double x = Round(v1.x); x >= v2.x; x--) {
    y1 = v1.y - slope1 * (v1.x - x);
    y2 = v1.y - slope2 * (v1.x - x);
    d1 = FindDepthFrom2dX(x, v1, v2);
    d2 = FindDepthFrom2dX(x, v1, v3);
    DrawVertLine(x, y1, y2, d1, d2, im);
  }
}

void FillRightFlatTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3,
                           Image* im) {
  double slope1 = (v3.y - v1.y) / (v3.x - v1.x);
  double slope2 = (v3.y - v2.y) / (v3.x - v2.x);
  double y1, y2, d1, d2;

  for (double x = Round(v3.x); x < v1.x; x++) {
    y1 = v3.y - slope1 * (v3.x - x);
    y2 = v3.y - slope2 * (v3.x - x);
    d1 = FindDepthFrom2dX(x, v3, v1);
    d2 = FindDepthFrom2dX(x, v3, v2);
    DrawVertLine(x, y1, y2, d1, d2, im);
  }
}

void DrawTriangle(const Vertex& a, const Vertex& b, const Vertex& c,
                  Image* im) {
  Vertex v1(a), v2(b), v3(c);

  if (v1.x < v2.x) std::swap(v1, v2);
  if (v1.x < v3.x) std::swap(v1, v3);
  if (v2.x < v3.x) std::swap(v2, v3);

  if (v2.x == v3.x) {
    if (v2.y > v3.y) {
      FillLeftFlatTriangle(v1, v3, v2, im);
    } else {
      FillLeftFlatTriangle(v1, v2, v3, im);
    }
  } else if (v1.x == v2.x) {
    if (v1.y > v2.y) {
      FillRightFlatTriangle(v2, v1, v3, im);
    } else {
      FillRightFlatTriangle(v1, v2, v3, im);
    }
  } else {
    Vertex v4;
    v4.x = v2.x;
    double slope = (v3.y - v1.y) / (v1.x - v3.x);
    v4.y = v3.y - (v4.x - v3.x) * slope;
    v4.d = FindDepthFrom2dX(v4.x, v1, v3);

    if (v2.y > v4.y) {
      FillLeftFlatTriangle(v1, v4, v2, im);
      FillRightFlatTriangle(v4, v2, v3, im);
    } else {
      FillLeftFlatTriangle(v1, v2, v4, im);
      FillRightFlatTriangle(v2, v4, v3, im);
    }
  }
}

void RenderDepthImage(const std::vector<Triangle>& triangles, Image* im) {
  for (const auto& triangle : triangles) {
    DrawTriangle(triangle.a, triangle.b, triangle.c, im);
  }
}

void mexFunction(int nargout, mxArray* out[], int nargin, const mxArray* in[]) {
  using namespace mexutil;

  N_IN(4);
  N_OUT(1);

  for (int i = 0; i < 4; ++i) {
    M_ASSERT(mxGetNumberOfDimensions(in[i]) == 2);
  }

  const mwSize* dims0 = mxGetDimensions(in[0]);
  M_ASSERT(dims0[0] >= 1);
  M_ASSERT(dims0[1] == 3);

  const mwSize* dims1 = mxGetDimensions(in[1]);
  M_ASSERT(dims1[0] >= 1);
  M_ASSERT(dims1[1] == 3);

  const mwSize* dims2 = mxGetDimensions(in[2]);
  M_ASSERT(dims2[0] == 1);
  M_ASSERT(dims2[1] == 2);

  const mwSize* dims3 = mxGetDimensions(in[3]);
  M_ASSERT(dims3[0] == 3);
  M_ASSERT(dims3[1] == 4);

  int num_vertices = dims0[0];
  int num_faces = dims1[0];

  double* fv_v = mxGetPr(in[0]);
  double* fv_f = mxGetPr(in[1]);
  double* imsize = mxGetPr(in[2]);
  double* P = mxGetPr(in[3]);

  std::vector<Vertex> vertices;
  vertices.reserve(num_vertices);
  for (int i = 0; i < num_vertices; ++i) {
    Vertex v;
    double xyz[3];
    for (int j = 0; j < 3; ++j) {
      xyz[j] = P[j + 0 * 3] * fv_v[i + 0 * num_vertices] +
               P[j + 1 * 3] * fv_v[i + 1 * num_vertices] +
               P[j + 2 * 3] * fv_v[i + 2 * num_vertices] + P[j + 3 * 3];
    }
    vertices.push_back({xyz[0] / xyz[2], xyz[1] / xyz[2], xyz[2]});
  }

  std::vector<Triangle> triangles;
  vertices.reserve(num_faces);
  for (int i = 0; i < num_faces; ++i) {
    triangles.push_back({vertices[fv_f[i + 0 * num_faces]],
                         vertices[fv_f[i + 1 * num_faces]],
                         vertices[fv_f[i + 2 * num_faces]]});
  }

  Image im(imsize[1], imsize[0]);

  RenderDepthImage(triangles, &im);

  mwSize out_dims[2] = {(mwSize)imsize[0], (mwSize)imsize[1]};
  out[0] = mxCreateNumericArray(2, out_dims, mxDOUBLE_CLASS, mxREAL);
  double* out_im = mxGetPr(out[0]);

  for (int x = 0; x < im.w(); ++x) {
    for (int y = 0; y < im.h(); ++y) {
      out_im[y + x * im.h()] = im.get(x, y);
    }
  }
}
