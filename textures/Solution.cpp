#include "Common.h"
#include <functional>
#include <iostream>

using namespace std;

class Textured : public IShape
{
public:
    void SetPosition(Point p) override
    {
        _position = p;
    }

    Point GetPosition() const override
    {
        return _position;
    }

    void SetSize(Size s) override
    {
        _size = s;
    }

    Size GetSize() const override
    {
        return _size;
    }

    void SetTexture(shared_ptr<ITexture> texture) override
    {
        _texture = texture;
    }

    ITexture *GetTexture() const override
    {
        return _texture.get();
    }

    unique_ptr<IShape> Clone() const override
    {
        auto copy = make_unique<Textured>();
        copy->_position = _position;
        copy->_size = _size;
        copy->_texture = _texture;
        return copy;
    }

    virtual void Draw(Image &img) const override
    {
        drawFilter(img);
    }

protected:
    Point _position = {0, 0};
    Size _size = {0, 0};
    shared_ptr<ITexture> _texture;

    Point getEnd() const
    {
        return {_position.x + _size.width, _position.y + _size.height};
    }

    Point getTextureEnd() const
    {
        if (_texture)
        {
            return {_position.x + _texture->GetSize().width,
                    _position.y + _texture->GetSize().height};
        }

        return {0, 0};
    }

    void drawFilter(Image &img, function<bool(Point)> pred = [](Point p) { return true; }) const
    {
        Point end = getEnd();
        for (int y = _position.y; y < img.size() && y < end.y; ++y)
        {
            for (int x = _position.x; x < img[y].size() && x < end.x; ++x)
            {
                // cout << "cycle iteration with x = " << x << endl;
                char pixel = '.';

                if (_texture &&
                        x < getTextureEnd().x &&
                        y < getTextureEnd().y)
                {
                    try
                    {
                        pixel = _texture->GetImage().at(y - _position.y).at(x - _position.x);
                    }
                    catch (...)
                    {
                        cout << "Exception caught with x = " << x << " and y = " << y << endl;
                    }
                }

                if (pred({x, y}))
                {
                    img[y][x] = pixel;
                }
            }
        }
    }
};

using Rectangle = Textured;

class Ellipse : public Textured
{
public:
    Ellipse() : _drawingPredicate([this](Point p){
        Point normalized = {p.x - this->_position.x, p.y - this->_position.y};
        return IsPointInEllipse(normalized, this->_size); })
    {}

    void Draw(Image &img) const override
    {
        drawFilter(img, _drawingPredicate);
    }

private:
    const function<bool(Point)> _drawingPredicate;
};

unique_ptr<IShape> MakeShape(ShapeType shape_type)
{
    switch (shape_type)
    {
    case ShapeType::Ellipse:
        return make_unique<Ellipse>();
        break;
    default:
        return make_unique<Rectangle>();
    }
}