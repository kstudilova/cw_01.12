#include <iostream>
namespace top {

    struct p_t {
        int x, y;
    };

    struct IDraw {
        virtual ~IDraw() = default;
        virtual p_t begin() const = 0;
        virtual p_t next(p_t) const = 0;
    };

    struct Dot : IDraw {
        p_t begin() const override;
        p_t next(p_t) const override;
        p_t o;
        explicit Dot(p_t dd);
        Dot(int x, int y);
    };

    struct frame_t {
        p_t aa; //left_bot
        p_t bb; //right_top
    };

    struct VSeg : IDraw {
        p_t begin() const override;
        p_t next(p_t) const override;
        p_t beg;
        int length;
        VSeg(p_t, int);
    };

    struct HSeg : IDraw {
        p_t begin() const override;
        p_t next(p_t) const override;
        p_t beg;
        int length;
        HSeg(p_t, int);
    };

    struct DSeg : IDraw {
        p_t begin() const override;
        p_t next(p_t) const override;
        p_t beg;
        int length;
        DSeg(p_t, int);
    };

    struct Square : IDraw {
        p_t begin() const override;
        p_t next(p_t a) const override;
        p_t top_left;
        int size;
        Square(p_t tl, int s);
    };

    struct Rectangle : IDraw {
        p_t begin() const override;
        p_t next(p_t a) const override;
        p_t top_left;
        int width;
        int height;
        Rectangle(p_t tl, int w, int h);
    };

    bool operator==(p_t a, p_t b){
        return a.x == b.x && a.y == b.y;
    }

    bool operator!=(p_t a, p_t b){
        return !(a == b);
    }

    size_t rows(frame_t fr);
    size_t cols(frame_t fr);

    void make_f(IDraw ** b, size_t k);

    size_t get_points(const IDraw& d, p_t** pts, size_t& s);

    frame_t build_frame(const p_t  * ps, size_t s);

    char * build_canvas(frame_t f, char fill);

    void print_canvas(std::ostream& os, const char* cnv, frame_t fr);

    void paint_canvas(char* cnv, frame_t fr, p_t p, char fill);

    void extend (p_t** pts, size_t s, p_t p);
}

void top::make_f(IDraw** b, size_t k) {
    b[0] = new Dot(-10, 8);
    b[1] = new Dot(10, -8);
    b[2] = new HSeg({1,0}, 5);
    b[3] = new VSeg({-1, -5}, 3);
    b[4] = new DSeg({2, 2}, 4);
    b[5] = new Square({-8, 5}, 4);
    b[6] = new Rectangle({4, -7}, 4, 6);
}

void top::extend(p_t** pts, size_t s, p_t p) {
    p_t* res = new p_t[s + 1];
    for (size_t i = 0; i < s; ++i) {
        res[i] = (*pts)[i];
    }
    res[s] = p;
    delete [] *pts;
    *pts = res;
}

size_t top::get_points(const IDraw& d, p_t** pts, size_t& s) {
    p_t p = d.begin();
    extend(pts, s, p);
    size_t delta = 1;
    p_t next_p = d.next(p);
    while (next_p != d.begin()) {
        extend(pts, s + delta, next_p);
        ++delta;
        p = next_p;
        next_p = d.next(p);
    }
    s += delta;
    return delta;
}

top::frame_t top::build_frame(const p_t* pts, size_t s) {
    if (!s) {
        throw std::logic_error("bad size");
    }
    int minx = pts[0].x , maxx = minx;
    int miny = pts[0].y , maxy = miny;
    for (size_t i = 0; i < s; ++i) {
        minx = std::min(minx, pts[i].x);
        maxx = std::max(maxx, pts[i].x);
        miny = std::min(miny, pts[i].y);
        maxy = std::max(maxy, pts[i].y);
    }
    p_t aa{minx, miny};
    p_t bb{maxx, maxy};
    return {aa, bb};
}

size_t top::rows(frame_t fr) {
    return (fr.bb.y - fr.aa.y + 1);
}

size_t top::cols(frame_t fr) {
    return (fr.bb.x - fr.aa.x + 1);
}

char * top::build_canvas(frame_t fr, char fill) {
    char* cnv = new char[rows(fr) * cols(fr)];
    for (size_t i = 0; i < rows(fr) * cols(fr); ++i) {
        cnv[i] = fill;
    }
    return cnv;
}

void top::paint_canvas(char* cnv, frame_t fr, p_t p, char fill) {
    int dx = p.x - fr.aa.x;
    int dy = fr.bb.y - p.y;
    cnv[dy * cols(fr) + dx] = fill;
}

void top::print_canvas(std::ostream& os, const char* cnv, frame_t fr) {
    for (size_t i = 0; i < rows(fr); ++i) {
        for (size_t j = 0; j < cols(fr); ++j) {
            os << cnv[i * cols(fr) + j];
        }
        os << "\n";
    }
}

top::Dot::Dot(int x, int y) :
    IDraw(),
    o{x, y}
{}

top::Dot::Dot(p_t dd) :
    IDraw(),
    o{dd}
{}

top::p_t top::Dot::begin() const {
    return o;
}

top::p_t top::Dot::next(p_t) const {
    return begin();
}

top::HSeg::HSeg(p_t a, int b) :
    beg(a), length(b)
    {}

top::p_t top::HSeg::begin() const {
    return beg;
}

top::p_t top::HSeg::next(p_t a) const {
    if (a.x - beg.x + 1 > length - 1) {
        return begin();
    }
    return {a.x + 1, a.y};
}

top::VSeg::VSeg(p_t a, int b) :
    beg(a), length(b)
    {}

top::p_t top::VSeg::begin() const {
    return beg;
}

top::p_t top::VSeg::next(p_t a) const {
    if (a.y - beg.y + 1 > length - 1) {
        return begin();
    }
    return {a.x, a.y + 1};
}

top::DSeg::DSeg(p_t a, int b) :
    beg(a), length(b)
    {}

top::p_t top::DSeg::begin() const {
    return beg;
}

top::p_t top::DSeg::next(p_t a) const {
    if (a.x - beg.x + 1 > length - 1) {
        return begin();
    }
    return {a.x + 1, a.y + 1};
}

top::Square::Square(p_t tl, int s) :
    top_left(tl), size(s)
    {}

top::p_t top::Square::begin() const {
    return top_left;
}

top::p_t top::Square::next(p_t a) const {
    if (a.x == top_left.x && a.y == top_left.y) { //левый верхний угол
        return {top_left.x + 1, top_left.y};
    }
    else if (a.y == top_left.y && a.x < top_left.x + size - 1) { //на верхней стороне
        return {a.x + 1, a.y};
    }
    else if (a.x == top_left.x + size - 1 && a.y == top_left.y) { //правый верний угол
        return {a.x, a.y - 1};
    }
    else if (a.x == top_left.x + size - 1 && a.y > top_left.y - size + 1) { //правая сторона
        return {a.x, a.y - 1};
    }
    else if (a.x == top_left.x + size - 1 && a.y == top_left.y - size + 1) { // правый нижний угол
        return {a.x - 1, a.y};
    }
    else if (a.x > top_left.x && a.y == top_left.y - size + 1) { // нижняя сторона
        return {a.x - 1, a.y};
    }
    else if (a.x == top_left.x && a.y == top_left.y - size + 1) { // левый нижний угол
        return {a.x, a.y + 1};
    }
    else if (a.x == top_left.x && a.y < top_left.y) { // левая сторона
        if (a.y < top_left.y - 1) {
            return {a.x, a.y + 1};
        } else {
            return begin ();
        }
    }
    return begin();
}

top::Rectangle::Rectangle(p_t tl, int w, int h) :
    top_left(tl), width(w), height(h)
    {}

top::p_t top::Rectangle::begin() const {
    return top_left;
}

top::p_t top::Rectangle::next(p_t a) const {
    if (a.x == top_left.x && a.y == top_left.y) {
        return {top_left.x + 1, top_left.y};
    }
    else if (a.y == top_left.y && a.x >= top_left.x + 1 && a.x < top_left.x + width - 1) {
        return {a.x + 1, a.y};
    }
    else if (a.x == top_left.x + width - 1 && a.y == top_left.y) {
        return {a.x, a.y - 1};
    }
    else if (a.x == top_left.x + width - 1 && a.y <= top_left.y - 1 && a.y > top_left.y - height + 1) {
        return {a.x, a.y - 1};
    }
    else if (a.x == top_left.x + width - 1 && a.y == top_left.y - height + 1) {
        return {a.x - 1, a.y};
    }
    else if (a.y == top_left.y - height + 1 && a.x <= top_left.x + width - 2 && a.x > top_left.x) {
        return {a.x - 1, a.y};
    }
    else if (a.x == top_left.x && a.y == top_left.y - height + 1) {
        return {a.x, a.y + 1};
    }
    else if (a.x == top_left.x && a.y >= top_left.y - height + 2 && a.y < top_left.y) {
        return {a.x, a.y + 1};
    }
    else if (a.x == top_left.x && a.y == top_left.y - 1) {
        return begin();
    }
    return begin();
}

int main(){
    using namespace top;
    IDraw * f[7] = {};
    p_t * p = nullptr;
    size_t s = 0;
    char * cnv = nullptr;
    int err = 0;
    try {
        make_f(f, 7);
        for (size_t i = 0; i < 7; ++i){
            get_points(*(f[i]), &p, s);
        }
        frame_t fr = build_frame(p, s);
        cnv = build_canvas(fr, '.');
        for (size_t i = 0; i < s; ++i) {
            paint_canvas(cnv, fr, p[i], '0');
        }
        print_canvas(std::cout, cnv, fr);
    } catch(...){
        err = 1;
    }
    for (int i = 0; i < 7; ++i) {
        delete f[i];
    }
    delete[] p;
    delete[] cnv;
    return err;
}

// до поcлезавтра вечер 10.12 горизонтальный отрезок, вертикальный отр, отр под 45 градусов, дз до сб: квадрат, прямоугольник