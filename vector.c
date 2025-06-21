//Calcs distance between two vectors
double vectorDistance(Vector v1, Vector v2) {
    double dx = v2.x - v1.x;
    double dy = v2.y - v1.y;
    return sqrt(dx * dx + dy * dy);
}