void Chaikin(const std::vector< ogl::Vec2f >& ControlPolygon, const size_t MinNumDesiredPoints,
             std::vector< ogl::Vec2f >& Curve)
{
    //TODO: Extend and edit the code below for task 5.1
    Curve.clear();

    const int NumPointsPerPolygonLeg = 1 + (int)MinNumDesiredPoints / ((int)ControlPolygon.size());
    int noOfIterations = log(NumPointsPerPolygonLeg) / log(2);

    Curve.reserve(NumPointsPerPolygonLeg * ControlPolygon.size());

    std::vector< ogl::Vec2f > points;

    std::copy(ControlPolygon.begin(), ControlPolygon.end(), std::back_inserter(points));

    for(int i(0); i < noOfIterations; i++) {
        for(int j(0); j < (int)points.size(); j++) {

          const ogl::Vec2f& LeftPoint = points[j];
          const ogl::Vec2f& RightPoint = points[(j+1) % points.size()];
          
          const float t = 0.75;
          Curve.push_back(t * LeftPoint + (1-t) * RightPoint);
          Curve.push_back((1-t) * LeftPoint + t * RightPoint);

        }
        points.clear();
        std::copy(Curve.begin(), Curve.end(), std::back_inserter(points));
        Curve.clear();
    }
    std::copy(points.begin(), points.end(), std::back_inserter(Curve));

}

void DefineBallisticParabolaControlPolygon(const ogl::Vec3f& ShootingTankPos, const ogl::Vec3f& TargetTankPos,
                                           std::vector< ogl::Vec3f >& ControlPolygon)
{
    //TODO: Define the control polygon so that the the projectile will move from the
    //shooting tank to the target following a parabola
    ControlPolygon.push_back(ShootingTankPos);
    ControlPolygon.push_back(ogl::Vec3f(0,0,5));
    ControlPolygon.push_back(TargetTankPos);
}


void Bezier(const std::vector< ogl::Vec3f >& ControlPolygon, const int NumPoints,
            std::vector< ogl::Vec3f >& BezierCurve)
{
    //We want exactly NumPoints in the final curve
    BezierCurve.resize(NumPoints);

    //TODO: Compute points on the Bezier curve defined by the control points given
    //For task 5.2 a quadratic bezier curve suffices
    //For the bonus task 5.3, make this work for an arbitrary number of control points
    //Here, just a simple thing, not a Bezier Curve

    std::vector< ogl::Vec3f > tempPolygon;
    std::copy(ControlPolygon.begin(), ControlPolygon.end(), std::back_inserter(tempPolygon));

    for(int i(0);i<NumPoints;i++)
    {
        const float t = float(i) / float(NumPoints-1);
        
        BezierCurve[i] = float(pow(1-t, 2)) * tempPolygon[0] + 2 * t * (1-t) * tempPolygon[1] + float(pow(t, 2)) * tempPolygon[2];
        //BezierCurve[i] = (1-t) * ControlPolygon.front() + t * ControlPolygon.back();

        BezierCurve[i][2] = 5 - pow(4*(t-0.5f), 2); //Give it some arbitrary height
    }
}
