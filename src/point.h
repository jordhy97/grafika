#ifndef POINT_H
#define POINT_H

 #define PI 3.14159265 

class Point {
public:
	/* Constructor */
	Point();
	Point(int x, int y);

	/* Returns p1 translated by p2 */
	static Point Translate(const Point& p1, const Point& p2);

	/* Translate this point by p */
	Point& Translate(const Point& p);

	/* Return p scaled by scale factor with the specified pivot */
	static Point Scale(const Point& p, const Point& pivot, double scale_factor);

	/* Scale this point by scale factor with the specified pivot */
	Point& Scale(const Point& pivot, double scale_factor);

	/* Return p rotated by theta degree with the specified pivot */
	static Point Rotate(const Point& p, const Point& pivot, double theta);

	/* Rotate this point by thetha degree with the specified pivot */
	Point& Rotate(const Point& pivot, double theta);

	/* Getter */
	int GetX() const;
	int GetY() const;

	/* Setter */
	void SetX(int x);
	void SetY(int y);

private:
	int x_; /* absis */
	int y_; /* ordinat */
};

#endif
