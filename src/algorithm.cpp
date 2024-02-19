#include<vector>
#include<iostream>
#include <set>
#include <cmath>
#include <algorithm>
#include "..\include\algorithm.hpp"
static float EPS = 1e-9;

Point::Point(float x1, float y1)
{
    this->x = x1;
    this->y = y1;
}
Point::Point() {
    x = 0;
    y = 0;
}
float Point::get_x()
{
    return x;
}
float Point::get_y()
{
    return y;
}
typedef Point Vector;
static Point P = Point(0,0);
int sign(float a)
{
    if (a > 0) return 1;
    if (a == 0) return 0;
    return -1;
}

void Point::setX(float x)
{
    this->x = x;
}
void Point::setY(float y)
{
    this->y = y;
}


float angle(Point a)
{
    return (a.get_y() - P.get_y()) / sqrt(pow(a.get_x() - P.get_x(), 2) + pow(a.get_y() - P.get_y(), 2));
}
bool f(Point a, Point b)
{
    if (angle(a) < angle(b)) return true;
    return false;
}
std::vector<Point> convex_hull(std::vector<Point>& points) //выпуклая оболочка
{
    int n = points.size();
    Point minr = Point(1e10, 0);
    std::set<std::pair<float, float>> b;
    for (int i = 0; i < n; i++)
    {
        b.insert({ points[i].get_x(), points[i].get_y()});
        if (points[i].get_x() < minr.get_x()) {
            minr = points[i];
        }
    }
    P = minr;
    n = b.size();
    std::vector<Point>a(n);
    int count = 1;
    for (auto& it : b)
    {
        if (it.first != minr.get_x() || it.second != minr.get_y())
        {
            a[count] = Point(it.first, it.second);
            count++;
        }
    }
    a[0] = minr;
    auto it = a.begin();
    it++;
    sort(it, a.end(), f);
    a.push_back(minr);
    n = a.size();
    std::vector<Point> st;
    st.push_back(a[0]);
    st.push_back(a[1]);
    std::vector<Point>obol;
    obol.push_back(minr);
    for (int i = 2; i < n; i++)
    {
        while (st.size() >= 2 && ((st[st.size() - 1].get_x() - st[st.size() - 2].get_x()) * (a[i].get_y() - st[st.size() - 1].get_y()) - (st[st.size() - 1].get_y() - st[st.size() - 2].get_y()) * (a[i].get_x() - st[st.size() - 1].get_x()) < 0))
        {
            st.pop_back();
        }
        st.push_back(a[i]);
    }
    return st;
}
bool per_otr(std::pair<Point, Point> otr1, std::pair<Point, Point> otr2, std::vector<Point>& res) //peresechenie otrezkov
{
    Vector vec1 = Point( otr1.second.get_x() - otr1.first.get_x(), otr1.second.get_y() - otr1.first.get_y() );
    Vector vec2 = Point( otr2.second.get_x() - otr2.first.get_x(), otr2.second.get_y() - otr2.first.get_y() );
    float x1 = otr1.first.get_x(), y1 = otr1.first.get_y(), x2 = otr1.second.get_x(), y2 = otr1.second.get_y();
    float x3 = otr2.first.get_x(), y3 = otr2.first.get_y(), x4 = otr2.second.get_x(), y4 = otr2.second.get_y();
    if (vec1.get_x() != 0)
    {
        if (vec1.get_y() != 0)
        {
            if (vec2.get_x() / vec1.get_x() > vec2.get_y() / vec1.get_y() - EPS && vec2.get_x() / vec1.get_x() < vec2.get_y() / vec1.get_y() + EPS)
            {
                return false;
            }
        }
        else
        {
            if (fabs(vec2.get_y()) < EPS)
            {
                return false;
            }
        }
    }
    else
    {
        if (fabs(vec2.get_x()) < EPS)
        {
            return false;
        }
    }
    float det = (x1 - x2) * (y4 - y3) - (x4 - x3) * (y1 - y2);
    if (det == 0)
    {
        return false;
    }
    float t1 = ((y4 - y3) * (x4 - x2) + (x3 - x4) * (y4 - y2)) / det;
    float t2 = ((y2 - y1) * (x4 - x2) + (x1 - x2) * (y4 - y2)) / det;
    if (t1 > 0-EPS && t1 < 1+EPS && t2 > 0-EPS && t2 < 1+EPS)
    {
        res.push_back(Point(x1 * t1 + x2 * (1 - t1), y1 * t1 + y2 * (1 - t1)));
        return true;
    }
    return false;
}
bool is_inside(std::vector<Point>& fig, Point p)
{
    Vector prev = Point( fig[0].get_x() - p.get_x(), fig[0].get_y() - p.get_y() );
    float det_prev = 0;
    fig.push_back(fig[0]);
    for (int i = 1; i < fig.size(); i++)
    {
        Vector tek = Point( fig[i].get_x() - p.get_x(), fig[i].get_y() - p.get_y() );
        float det_tek = tek.get_x() * prev.get_y() - tek.get_y() * prev.get_x();
        if (det_prev != 0 && det_tek != 0)
        {
            if (sign(det_prev) != sign(det_tek))
            {
                fig.pop_back();
                return false;
            }
        }
        det_prev = det_tek;
        prev = tek;
    }
    fig.pop_back();
    return true;
}
std::vector<Point> The_area_of_intersection(std::vector<Point> fig1, std::vector<Point> fig2)
{
    std::vector<Point> points;
    for (auto& it : fig2)
    {
        if (is_inside(fig1, it))
        {
            points.push_back(it); 
        }
    }
    for (auto& it : fig1)
    {
        if (is_inside(fig2, it)) 
        { 
            points.push_back(it);
        }
    }
    auto fig1_copy = fig1;
    auto fig2_copy = fig2;
    fig1_copy.push_back(fig1[0]);
    fig2_copy.push_back(fig2[0]);
    for (int i = 1; i < fig1.size() + 1; i++)
    {
        for (int j = 1; j < fig2.size() + 1; j++)
        {
            per_otr({ fig1_copy[i - 1],fig1_copy[i] }, { fig2_copy[j - 1], fig2_copy[j] }, points);
        }
    }
    points = convex_hull(points);
    points.pop_back();
    return points;
}