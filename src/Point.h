class Point
{
public:
    float x;
    float y;
    float z;

    Point(const float x, const float y, const float z) 
    {
        this->x = x;
        this->y = y;
        this->z = z;
    };

    static Point findMiddle(const Point* first, const Point* other)
    {
        return Point((first->x + other->x)/2, (first->y + other->y)/2, (first->z + other->z)/2);
    }
};