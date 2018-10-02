
#ifndef ATL_RECT_H
#define ATL_RECT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <atlbase.h>
//#include <atlwin.h>

class AtlPoint : public tagPOINT {
public:
    AtlPoint() {
        ResetPoint();
    }

    AtlPoint(LONG x, LONG y) {
        SetPoint(x, y);
    }

    AtlPoint(const POINT & src) {
        SetPoint(src);
    }

    AtlPoint(const AtlPoint & src) {
        SetPoint(src);
    }

    ~AtlPoint() {}

    AtlPoint & operator = (const POINT & rhs) {
        SetPoint(rhs);
        return *this;
    }

    AtlPoint & operator = (const AtlPoint & rhs) {
        SetPoint(rhs);
        return *this;
    }

    void CopyPoint(const POINT & src) {
        SetPoint(src);
    }

    void CopyPoint(const AtlPoint & src) {
        SetPoint(src);
    }

    void ResetPoint() {
        this->x = 0;
        this->y = 0;
    }

    POINT & GetPoint() const {
        return *((POINT *)this);
    }

    void SetPoint(LONG _x, LONG _y) {
        this->x = _x;
        this->y = _y;
    }

    void SetPoint(const POINT & point) {
        this->x = point.x;
        this->y = point.y;
    }

    void SetPoint(const AtlPoint & point) {
        SetPoint(this->GetPoint());
    }
};

class AtlSize : public tagSIZE {
public:
    AtlSize() {
        ResetSize();
    }

    AtlSize(LONG width, LONG height) {
        SetSize(width, height);
    }

    AtlSize(const SIZE & src) {
        CopySize(src);
    }

    AtlSize(const AtlSize & src) {
        CopySize(src);
    }

    ~AtlSize() {}

    AtlSize & operator = (const SIZE & rhs) {
        SetSize(rhs);
        return *this;
    }

    AtlSize & operator = (const AtlSize & rhs) {
        SetSize(rhs);
        return *this;
    }

    void ResetSize() {
        this->cx = 0;
        this->cy = 0;
    }

    void CopySize(const SIZE & src) {
        SetSize(src);
    }

    void CopySize(const AtlSize & src) {
        SetSize(src);
    }

    SIZE & GetSize() const {
        return *((SIZE *)this);
    }

    void SetSize(LONG width, LONG height) {
        this->cx = width;
        this->cx = height;
    }

    void SetSize(const SIZE & size) {
        this->cx = size.cx;
        this->cy = size.cy;
    }

    void SetSize(const AtlSize & size) {
        SetSize(size.GetSize());
    }

    LONG Width() const { return this->cx; }
    LONG Height() const { return this->cy; }
};

class AtlRect : public tagRECT {
public:
    AtlRect() {
        ResetRect();
    }

    AtlRect(LONG left, LONG top, LONG right, LONG bottom) {
        SetRect(left, top, right, bottom);
    }

    AtlRect(const POINT & topLeft, const POINT & bottomRight) {
        SetRect(topLeft, bottomRight);
    }

    AtlRect(const AtlPoint & topLeft, const AtlPoint & bottomRight) {
        SetRect(topLeft, bottomRight);
    }

    AtlRect(const RECT & src) {
        CopyRect(src);
    }

    AtlRect(const AtlRect & src) {
        CopyRect(src);
    }

    ~AtlRect() {}

    AtlRect & operator = (const RECT & rhs) {
        CopyRect(rhs);
        return *this;
    }

    AtlRect & operator = (const AtlRect & rhs) {
        CopyRect(rhs);
        return *this;
    }

    operator RECT () const {
        return *this;
    }

    operator RECT * () const {
        return ((RECT *)this);
    }

    void ResetRect() {
        this->left = 0;
        this->right = 0;
        this->top = 0;
        this->bottom = 0;
    }

    void CopyRect(const RECT & src) {
        SetRect(src);
    }

    void CopyRect(const AtlRect & src) {
        CopyRect(src.GetRect());
    }

    RECT & GetRect() const {
        return *((RECT *)this);
    }

    AtlSize GetSize() const {
        return AtlSize(Width(), Height());
    }

    void SetRect(LONG left, LONG top, LONG right, LONG bottom) {
        this->left = left;
        this->top = top;
        this->right = right;
        this->bottom = bottom;
    }

    void SetRect(const POINT & topLeft, const POINT & bottomRight) {
        this->left = topLeft.x;
        this->top = topLeft.y;
        this->right = bottomRight.x;
        this->bottom = bottomRight.y;
    }

    void SetRect(const AtlPoint & topLeft, const AtlPoint & bottomRight) {
        SetRect(topLeft.GetPoint(), bottomRight.GetPoint());
    }

    void SetRect(const RECT & rect) {
        this->left = rect.left;
        this->right = rect.right;
        this->top = rect.top;
        this->bottom = rect.bottom;
    }

    void SetRect(const AtlRect & rect) {
        SetRect(this->GetRect());
    }

    LONG Width() const {
        return (this->right - this->left);
    }

    LONG Height() const {
        return (this->bottom - this->top);
    }

    AtlPoint TopLeft() const {
        return AtlPoint(this->top, this->left);
    }

    AtlPoint BottonRight() const {
        return AtlPoint(this->bottom, this->right);
    }
};

#endif // ATL_RECT_H
