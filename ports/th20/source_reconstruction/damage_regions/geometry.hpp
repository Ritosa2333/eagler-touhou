#pragma once
// Collision policies preserve the original strict/inclusive edges and sampling.
namespace th20::source::geometry {
bool circle_point(float x,float y,float center_x,float center_y,float radius); //456fe0
bool rectangle_point(float x,float y,float center_x,float center_y,float width,float height); //457300
bool ellipse_point(float x,float y,float center_x,float center_y,float rx,float ry,float angle); //457040
bool segment_segment(float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy); //456920
bool rectangle_circle(float x,float y,float width,float height,float angle,float cx,float cy,float radius); //457610
bool rectangle_rectangle(float x,float y,float width,float height,float angle,float ox,float oy,float ow,float oh,float other_angle); //4580c0
bool ellipse_circle(float cx,float cy,float radius,float ex,float ey,float rx,float ry,float angle); //4562e0
bool rectangle_ellipse(float x,float y,float width,float height,float angle,float ex,float ey,float rx,float ry,float ellipse_angle); //4578b0
bool polygon_point(float x,float y,float cx,float cy,float radius,float angle,int sides); //4570f0
bool star_point(float x,float y,float cx,float cy,float outer,float inner,float angle,int sides); //457380
bool polygon_segment(float ax,float ay,float bx,float by,float cx,float cy,float radius,float angle,int sides); //456b40
bool star_segment(float ax,float ay,float bx,float by,float cx,float cy,float outer,float inner,float angle,int sides); //456d50
bool polygon_circle(float x,float y,float radius,float cx,float cy,float polygon_radius,float angle,int sides); //456690
bool star_circle(float x,float y,float radius,float cx,float cy,float outer,float inner,float angle,int sides); //4567d0
bool rectangle_polygon(float x,float y,float width,float height,float angle,float cx,float cy,float radius,float polygon_angle,int sides); //457c10
bool rectangle_star(float x,float y,float width,float height,float angle,float cx,float cy,float outer,float inner,float polygon_angle,int sides); //458670
}
