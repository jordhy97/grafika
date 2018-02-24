#ifndef POINT_H
#define POINT_H

class Point {
public:
	/* Constructor */
	Point() : x_(0), y_(0) {}
	Point(int x, int y): x_(x), y_(y) {}

	/* Getter */
	int GetX() const { return x_; }
	int GetY() const { return y_; }

	/* Setter */
	void SetX(int x) { x_ = x; }
	void SetY(int y) { y_ = y; }

private:
	int x_; /* absis */
	int y_; /* ordinat */
};

#endif
