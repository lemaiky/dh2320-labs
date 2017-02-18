void Chaikin(const std::vector< ogl::Vec2f >& ControlPolygon, const size_t MinNumDesiredPoints,
             std::vector< ogl::Vec2f >& Curve)
{
    //TODO: Extend and edit the code below for task 5.1
    Curve.clear();

    const int NumPointsPerPolygonLeg = 1 + (int)MinNumDesiredPoints / ((int)ControlPolygon.size());
    Curve.reserve(NumPointsPerPolygonLeg * ControlPolygon.size());
    for(int i(0);i<(int)ControlPolygon.size();i++)
    {
        const ogl::Vec2f& LeftPoint = ControlPolygon[i];
        const ogl::Vec2f& RightPoint = ControlPolygon[(i+1) % ControlPolygon.size()];

        //Linearly interpolate between left and right point in the t-interval [0,1)
        for(int j(0);j<NumPointsPerPolygonLeg;j++)
        {
            const float t = float(j) / float(NumPointsPerPolygonLeg); //Gives values from 0 to almost 1
            Curve.push_back((1-t) * LeftPoint + t * RightPoint);
        }
    }
}


void DefineBallisticParabolaControlPolygon(const ogl::Vec3f& ShootingTankPos, const ogl::Vec3f& TargetTankPos,
                                           std::vector< ogl::Vec3f >& ControlPolygon)
{
    //TODO: Define the control polygon so that the the projectile will move from the
    //shooting tank to the target following a parabola
    ControlPolygon.push_back(ogl::Vec3f(10,10,0));
    ControlPolygon.push_back(ogl::Vec3f(0,0,5));
    ControlPolygon.push_back(ShootingTankPos);
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
    for(int i(0);i<NumPoints;i++)
    {
        const float t = float(i) / float(NumPoints-1);
        BezierCurve[i] = (1-t) * ControlPolygon.front() + t * ControlPolygon.back();
        BezierCurve[i][2] = 5 - pow(4*(t-0.5f), 2); //Give it some arbitrary height
    }
}
