#include <opengl/OpenGL.hpp>
#include <memory>
#include <opengl/IndexedTriangleIO.hpp>
#include <opengl/TriangleGeometry.hpp>
#include <opengl/ShaderProgram.hpp>
#include <opengl/Camera.hpp>
#include <raytracer/Math.hpp>


int gWidth = 800;  ///< The width of the OpenGL window
int gHeight = 600; ///< The height of the OpenGL window
GLFWwindow* gGLFWWindow;

ogl::Vec3f gBackgroundColor(0.7f, 0.7f, 0.7f); ///< The OpenGL background color

std::shared_ptr<ogl::ShaderProgram> gShaderProgram;         ///< Phong shader handle
std::shared_ptr<ogl::Camera> gCamera;                       ///< Orbit camera
std::vector<std::shared_ptr<ogl::TriangleGeometry>> gTanks; ///< The tanks
std::vector< std::vector< ogl::Vec2f > > gTankPathsControlPolygon;    ///< Control polygon for the paths of the tanks
std::vector< std::vector< ogl::Vec3f > > gTankPaths;        ///< Paths of the tanks
std::shared_ptr<ogl::TriangleGeometry> gTankPathsDisplay;
std::shared_ptr<ogl::TriangleGeometry> gLightSphere[3];     ///< Three point lights
std::shared_ptr<ogl::TriangleGeometry> gGroundPlane;        ///< The ground plane
std::shared_ptr<ogl::TriangleGeometry> gGroundPlaneGrid;    ///< The ground plane grid
std::vector< ogl::Vec3f > gBallisticControlPolygon;  ///< The path of the ballistic thingy
std::vector< std::vector< ogl::Vec3f > > gBallisticTrajectory;  ///< The path of the ballistic thingy
std::shared_ptr<ogl::TriangleGeometry> gBallisticThingy;    ///< and its display
std::shared_ptr<ogl::TriangleGeometry> gBallisticTrajectoryDisplay;    ///< and its display

bool gbShoot = false;
bool gbAnimate = true;
float gLastFrameTime = 0.f;
std::vector< size_t > gTankPosition(0);
const int TankAnimationSkipFrame(1);
int TankAnimationFrame(0);

std::string gDataPath = ""; ///< The path pointing to the resources (OBJ, shader)

#include "task.h"

void GetPerpendicularPoints(const ogl::Vec3f& P, const ogl::Vec3f& Q, const float Thickness,
                            ogl::Vec3f& A, ogl::Vec3f& B, ogl::Vec3f& C, ogl::Vec3f& D, ogl::Vec3f& N)
{
    ogl::Vec3f Tangent(Q-P);
    ogl::Vec3f Ortho(-Tangent.y(), Tangent.x(), Tangent.z()); //Orthogonal in the xy-projection
    N = vl::cross(Tangent, Ortho);
    N.normalize();
    Ortho.normalize();
    Ortho *= Thickness / 2;
    A = P + Ortho;
    B = P - Ortho;
    C = Q + Ortho;
    D = Q - Ortho;
}


void InitLineGeometry(const std::vector< std::vector< ogl::Vec3f> >& Lines, std::shared_ptr<ogl::TriangleGeometry> Geo, const float Thickness = 0.1f, const bool bClosed = false)
{
    const int NumLines = (int)Lines.size();
    if (NumLines < 1) return;

    std::vector<ogl::Vec3f> Vertices;
    std::vector<unsigned int> Triangles;
    std::vector<ogl::Vec3f> Normals;
    for(int i(0);i<NumLines;i++)
    {
        //Shorthand
        const std::vector<ogl::Vec3f>& ThisLine = Lines[i];
        if (ThisLine.size() < 2) continue;

        //How much we had before
        const int NumPointsBefore = (int)Vertices.size();

        //Reserve space
        Vertices.reserve(Vertices.size() + 2 * ThisLine.size());
        Normals.reserve(Vertices.size());
        Triangles.reserve(Triangles.size() + 6 * (ThisLine.size() - 1));

        //Closed line?
        const int EndIter = (int)ThisLine.size() - (bClosed ? 0 : 1);

        //Add each segment
        ogl::Vec3f A,B,C,D,N;
        for(int p(0);p<EndIter;p++)
        {
            GetPerpendicularPoints(ThisLine[p], ThisLine[(p+1) % ThisLine.size()], Thickness, A, B, C, D, N);
            if (p==0)
            {
                Vertices.push_back(A);
                Vertices.push_back(B);
                Normals.push_back(N);
                Normals.push_back(N);
            }
            Vertices.push_back(C);
            Vertices.push_back(D);
            Normals.push_back(N);
            Normals.push_back(N);

            //Make triangles out of the last four vertices.
            const int iA = (int)Vertices.size() - 4;
            const int iB = (int)Vertices.size() - 3;
            const int iC = (int)Vertices.size() - 2;
            const int iD = (int)Vertices.size() - 1;
            // --
            Triangles.push_back(iA);
            Triangles.push_back(iB);
            Triangles.push_back(iD);
            // --
            Triangles.push_back(iA);
            Triangles.push_back(iD);
            Triangles.push_back(iC);
        }
    }

    Geo->init(Vertices, Normals, Triangles);
}

// Display the tanks
void displayTanks()
{
    TankAnimationFrame++;
    const bool bUpdatePosition = gbAnimate && !(TankAnimationFrame % TankAnimationSkipFrame);

	for (size_t i = 0; i<gTanks.size(); ++i)
	{
		// Set the updated light positions
		std::shared_ptr<ogl::TriangleGeometry> tank = gTanks[i];
		tank->setLightPosition0(gLightSphere[0]->getPosition());
		tank->setLightPosition1(gLightSphere[1]->getPosition());
		tank->setLightPosition2(gLightSphere[2]->getPosition());

		// Upload the new data to the GPU
		tank->updateUniforms();

		// Bind model uniform buffer
		tank->bind(gShaderProgram->handle(), 1, "ub_Geometry");

        //Update position
        if (bUpdatePosition)
        {
            const std::vector< ogl::Vec3f >& ThisPath = gTankPaths[i];
            size_t& ThisPosition = gTankPosition[i];
            ThisPosition++;
            ThisPosition %= ThisPath.size();
		    tank->modelMatrix().setIdentity();
		    tank->modelMatrix().scale(10, 10, 10);
		    tank->modelMatrix().translate(ThisPath[ThisPosition]);
        }

		// Bind triangle geometry
		glBindVertexArray(tank->handle());

		// Draw the indexed triangle set
		glDrawElements(GL_TRIANGLES, tank->numIndices(), GL_UNSIGNED_INT, (void*)0);
	}

	// Set the updated light positions
	gTankPathsDisplay->setLightPosition0(gLightSphere[0]->getPosition());
	gTankPathsDisplay->setLightPosition1(gLightSphere[1]->getPosition());
	gTankPathsDisplay->setLightPosition2(gLightSphere[2]->getPosition());

	// Upload the new data to the GPU
	gTankPathsDisplay->updateUniforms();

	// Bind model uniform buffer
	gTankPathsDisplay->bind(gShaderProgram->handle(), 1, "ub_Geometry");

	// Bind triangle geometry
	glBindVertexArray(gTankPathsDisplay->handle());

	// Draw the indexed triangle set
	glDrawElements(GL_TRIANGLES, gTankPathsDisplay->numIndices(), GL_UNSIGNED_INT, (void*)0);
}

// Display the ground plane
void displayGroundPlane()
{
	// Set the updated light positions
	gGroundPlane->setLightPosition0(gLightSphere[0]->getPosition());
	gGroundPlane->setLightPosition1(gLightSphere[1]->getPosition());
	gGroundPlane->setLightPosition2(gLightSphere[2]->getPosition());

	// Upload the new data to the GPU
	gGroundPlane->updateUniforms();

	// Bind model uniform buffer
	gGroundPlane->bind(gShaderProgram->handle(), 1, "ub_Geometry");

	// Bind triangle geometry
	glBindVertexArray(gGroundPlane->handle());

	// Draw the indexed triangle set
	glDrawElements(GL_TRIANGLES, gGroundPlane->numIndices(), GL_UNSIGNED_INT, (void*)0);


	// Set the updated light positions
	gGroundPlaneGrid->setLightPosition0(gLightSphere[0]->getPosition());
	gGroundPlaneGrid->setLightPosition1(gLightSphere[1]->getPosition());
	gGroundPlaneGrid->setLightPosition2(gLightSphere[2]->getPosition());

	// Upload the new data to the GPU
	gGroundPlaneGrid->updateUniforms();

	// Bind model uniform buffer
	gGroundPlaneGrid->bind(gShaderProgram->handle(), 1, "ub_Geometry");

	// Bind triangle geometry
	glBindVertexArray(gGroundPlaneGrid->handle());

	// Draw the indexed triangle set
	glDrawElements(GL_TRIANGLES, gGroundPlaneGrid->numIndices(), GL_UNSIGNED_INT, (void*)0);
}

// Display the light spheres indicating the light positions
void displayLightSpheres()
{
	// Update the light positions by rotating 3.6 degrees per second about z-axis (up vector)
	// One full rotation thus takes 100 seconds
    if (gbAnimate)
    {
	    for (int i = 0; i<3; ++i)
        {
		    gLightSphere[i]->modelMatrix().rotate(gLastFrameTime*3.6f, ogl::Vec3f(0, 0, 1));
        }
    }

	// Set the updated light positions
	for (int i = 0; i<3; ++i)
	{
		gLightSphere[i]->setLightPosition0(gLightSphere[0]->getPosition());
		gLightSphere[i]->setLightPosition1(gLightSphere[1]->getPosition());
		gLightSphere[i]->setLightPosition2(gLightSphere[2]->getPosition());

		// Upload the new data to the GPU
		gLightSphere[i]->updateUniforms();

		// Bind model uniform buffer
		gLightSphere[i]->bind(gShaderProgram->handle(), 1, "ub_Geometry");

		// Bind triangle geometry
		glBindVertexArray(gLightSphere[i]->handle());

		// Draw the indexed triangle set
		glDrawElements(GL_TRIANGLES, gLightSphere[i]->numIndices(), GL_UNSIGNED_INT, (void*)0);
	}
}


int gShootFirst = 0;

void initBallisticTrajectory(const bool bFirstTime)
{
    ogl::IndexedTriangleIO io;
    #ifdef _DEBUG
    // This smaller model loads much faster in debug mode
    io.loadFromOBJ(gDataPath + "assets/lightSphere.obj");
    #else
    // This large model loads fast only in release mode
    io.loadFromOBJ(gDataPath + "assets/lightSphere.obj");
    #endif
    if (io.vertexNormals().empty())
    {
        std::cerr << "OBJ model needs vertex normals!" << std::endl;
        return;
    }

    std::cerr << "Loaded " << io.vertexPositions().size() << " vertices, " <<
        io.triangleIndices().size() / 3 << " triangles.\n\n";

    gBallisticThingy = std::make_shared<ogl::TriangleGeometry>();
    gBallisticThingy->init(io.vertexPositions(), io.vertexNormals(), io.triangleIndices());
    gBallisticThingy->setMaterial(30, (gShootFirst % 2) ? ogl::Vec3f(0.f, 0.25f, 1.0f) : ogl::Vec3f(1.f, 0.25f, 0.f) );

    //Get current positions of the tanks
	const ogl::Vec3f& ShootingTankPos = gTankPaths[gShootFirst % 2][gTankPosition[gShootFirst % 2]];
	const ogl::Vec3f& TargetTankPos = gTankPaths[(gShootFirst+1) % 2][gTankPosition[(gShootFirst+1) % 2]];

    //Get the control polygon for the ballistic thingy
    gBallisticControlPolygon.clear();
    DefineBallisticParabolaControlPolygon(ShootingTankPos, TargetTankPos, gBallisticControlPolygon);

    //Compute the actual ballistic trajectory
    gBallisticTrajectory.resize(1);
    gBallisticTrajectory[0].clear();
    Bezier(gBallisticControlPolygon, 600, gBallisticTrajectory[0]);

    //Get a display
    gBallisticTrajectoryDisplay = std::make_shared<ogl::TriangleGeometry>();
    InitLineGeometry(gBallisticTrajectory, gBallisticTrajectoryDisplay, 0.3f);
    gBallisticTrajectoryDisplay->setMaterial(30, (gShootFirst % 2) ? ogl::Vec3f(0.f, 0.25f, 1.0f) : ogl::Vec3f(1.f, 0.25f, 0.f) );

    //Next time, the other one shoots
    gShootFirst++;
}

int gTrajectoryAnimationFrame = -1;
bool gbTrajectoryInitialized = false;

void displayBallisticTrajectory()
{
    const std::vector< ogl::Vec3f >& Flugbahn = gBallisticTrajectory[0];

    if (gbShoot && gTrajectoryAnimationFrame < (int)Flugbahn.size()-1)
    {
        gTrajectoryAnimationFrame++;
    }
    else
    {
        gbShoot = false;
        gbTrajectoryInitialized = false;
        gTrajectoryAnimationFrame = -1;
    }

	gBallisticTrajectoryDisplay->setLightPosition0(gLightSphere[0]->getPosition());
	gBallisticTrajectoryDisplay->setLightPosition1(gLightSphere[1]->getPosition());
	gBallisticTrajectoryDisplay->setLightPosition2(gLightSphere[2]->getPosition());
	gBallisticThingy->setLightPosition0(gLightSphere[0]->getPosition());
	gBallisticThingy->setLightPosition1(gLightSphere[1]->getPosition());
	gBallisticThingy->setLightPosition2(gLightSphere[2]->getPosition());

    if (gbShoot)
    {
        gBallisticTrajectoryDisplay->modelMatrix().setIdentity();
        gBallisticThingy->modelMatrix().setIdentity();
        gBallisticThingy->modelMatrix().scale(3, 3, 3);
        gBallisticThingy->modelMatrix().translate(Flugbahn[gTrajectoryAnimationFrame]);
    }
    else
    {
        gBallisticTrajectoryDisplay->modelMatrix().setIdentity();
        gBallisticTrajectoryDisplay->modelMatrix().scale(0, 0, 0);
        gBallisticThingy->modelMatrix().setIdentity();
        gBallisticThingy->modelMatrix().scale(0, 0, 0);
    }

	gBallisticTrajectoryDisplay->updateUniforms();
	gBallisticTrajectoryDisplay->bind(gShaderProgram->handle(), 1, "ub_Geometry");
	glBindVertexArray(gBallisticTrajectoryDisplay->handle());
	glDrawElements(GL_TRIANGLES, gBallisticTrajectoryDisplay->numIndices(), GL_UNSIGNED_INT, (void*)0);

	gBallisticThingy->updateUniforms();
	gBallisticThingy->bind(gShaderProgram->handle(), 1, "ub_Geometry");
	glBindVertexArray(gBallisticThingy->handle());
	glDrawElements(GL_TRIANGLES, gBallisticThingy->numIndices(), GL_UNSIGNED_INT, (void*)0);
}


void displayCB()
{
    if (gbShoot && !gbTrajectoryInitialized)
    {
        initBallisticTrajectory(false);
        gbTrajectoryInitialized = true;
    }

    gbAnimate = !gbShoot;

	// Set polygon mode to allow solid front and back-facing triangles
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Use the Phong shader
	glUseProgram(gShaderProgram->handle());

	// Upload possibly changed camera setup (e.g. camera eye) to GPU
	gCamera->updateUniforms();

	// Bind camera uniform buffer
	gCamera->bind(gShaderProgram->handle(), 0, "ub_Camera");

	// Render the scene content
	displayLightSpheres();
	displayGroundPlane();
	displayTanks();
    displayBallisticTrajectory();

	// Unbind the Phong shader
	glUseProgram(0);
}


float Terrain(const float x, const float y)
{
    return sin(x/2) * cos(y/3) + exp( -( pow(0.1f*x-0, 2)*5 + pow(0.1f*y+0, 2)*20 ) )*2;
}

void CallChaikin(const std::vector< ogl::Vec2f >& ControlPolygon, const int MinNumDesiredPoints,
                std::vector< ogl::Vec3f >& Curve)
{
    std::vector< ogl::Vec2f > Curve2D;
    Chaikin(ControlPolygon, MinNumDesiredPoints, Curve2D);

    //Add terrain
    Curve.resize(Curve2D.size());
    for(int i(0);i<(int)Curve.size();i++)
    {
        Curve[i][0] = Curve2D[i][0];
        Curve[i][1] = Curve2D[i][1];
        Curve[i][2] = 1 + Terrain(Curve2D[i][0], Curve2D[i][1]);
    }
}

bool initTanks()
{
	ogl::IndexedTriangleIO io;
#ifdef _DEBUG
	// This smaller model loads much faster in debug mode
	io.loadFromOBJ(gDataPath + "assets/lightSphere.obj");
#else
	// This large model loads fast only in release mode
	io.loadFromOBJ(gDataPath + "assets/lightSphere.obj");
#endif
	if (io.vertexNormals().empty())
	{
		std::cerr << "OBJ model needs vertex normals!" << std::endl;
		return false;
	}

	std::cerr << "Loaded " << io.vertexPositions().size() << " vertices, " <<
		io.triangleIndices().size() / 3 << " triangles.\n\n";

	// Create the tanks
	gTanks.resize(2);
    gTankPaths.resize(2);
    gTankPathsControlPolygon.resize(2);
    gTankPosition.resize(2);
    gTankPosition[0] = gTankPosition[1] = 0;

	for (size_t i = 0; i<gTanks.size(); ++i)
	{
		gTanks[i] = std::make_shared<ogl::TriangleGeometry>();
		// The first tank stores the original geometry (indexed triangle set)
		if (i == 0)
			gTanks[0]->init(io.vertexPositions(), io.vertexNormals(), io.triangleIndices());
		// The other tanks are instances of the original geometry (no duplicate geometry stored)
		else
			gTanks[i]->initInstance(gTanks[0]);
	}

	// Set a redish material with shininess=10.f
	gTanks[0]->setMaterial(10.f, ogl::Vec3f(1.0f, 0.5f, 0.2f));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f(-9, 1));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f( 0, 5));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f( 9, 1));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f( 9, 9));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f( 0, 5));
    gTankPathsControlPolygon[0].push_back(ogl::Vec2f(-9, 9));
    CallChaikin(gTankPathsControlPolygon[0], 1000, gTankPaths[0]);

	// Set a bluish material with shininess=10.f
	gTanks[1]->setMaterial(10.f, ogl::Vec3f(0.2f, 0.5f, 1.0f));
    gTankPathsControlPolygon[1].push_back(ogl::Vec2f(  9, -9));
    gTankPathsControlPolygon[1].push_back(ogl::Vec2f( -9, -1));
    gTankPathsControlPolygon[1].push_back(ogl::Vec2f( -9, -9));
    gTankPathsControlPolygon[1].push_back(ogl::Vec2f(  9, -1));
    CallChaikin(gTankPathsControlPolygon[1], 1000, gTankPaths[1]);

    //Create the display for the paths
    gTankPathsDisplay = std::make_shared<ogl::TriangleGeometry>();
    InitLineGeometry(gTankPaths, gTankPathsDisplay,  0.2f, true);
    gTankPathsDisplay->setMaterial(30, ogl::Vec3f(0.5f, 0.5f, 0.5f));

	return true;
}


// Initialize the ground plane geometry
// Load the OBJ files, apply materials and transformations
bool initGroundPlane()
{
	// Create the ground plane
	gGroundPlane = std::make_shared<ogl::TriangleGeometry>();
    const int NumX(100);
    const int NumY(100);
    const ogl::Vec3f MinPoint(-10, -10, 0);
    const ogl::Vec3f MaxPoint(10, 10, 0);
    std::vector<ogl::Vec3f> Vertices(NumX * NumY);
    std::vector<ogl::Vec3f> Normals(Vertices.size());
    std::vector<unsigned int> Triangles((NumX-1) * (NumY-1) * 6);

    const ogl::Vec3f GridSpacing = (MaxPoint - MinPoint) / ogl::Vec3f(NumX-1, NumY-1, 1);
    for (int j(0); j < NumY; j++)
    {
        for (int i(0); i < NumX; i++)
        {
            const int LinearIdx = j*NumX + i;

            //2D Position in the uniform grid
            ogl::Vec3f& ThisP = Vertices[LinearIdx];
            ThisP = MinPoint + ogl::Vec3f((float)i, (float)j, 0) * GridSpacing;

            //Height
            ThisP[2] = Terrain(ThisP.x(), ThisP.y());

            //Normal
            // - discrete
            const float ZDerivativeX = Terrain(ThisP.x() + 0.1f, ThisP.y()) - Terrain(ThisP.x() - 0.1f, ThisP.y());
            const float ZDerivativeY = Terrain(ThisP.x(), ThisP.y() + 0.1f) - Terrain(ThisP.x(), ThisP.y() - 0.1f);
            Normals[LinearIdx] = vl::cross(ogl::Vec3f(0.1f, 0, ZDerivativeX/2), ogl::Vec3f(0, 0.1f, ZDerivativeY/2));
            // - analytical... I may be off with the derivatives.
            //const float ZDerivativeX = -0.2f*exp(ThisP.x()) + (cos(sin(ThisP.y())))/6;
            //const float ZDerivativeY = cos(sin(ThisP.x()))/6 - 0.8f*exp(ThisP.y());
            //Normals[LinearIdx] = vl::cross(ogl::Vec3f(1, 0, ZDerivativeX), ogl::Vec3f(0, 1, ZDerivativeY));

            Normals[LinearIdx].normalize();

            //Add the triangle
            if (i < NumX - 1 && j < NumY - 1)
            {
                const int TriangleLinearIdx = j*(NumX-1) + i;
                Triangles[6 * TriangleLinearIdx + 0] = LinearIdx;
                Triangles[6 * TriangleLinearIdx + 1] = LinearIdx + 1;
                Triangles[6 * TriangleLinearIdx + 2] = LinearIdx + 1 + NumY;
                Triangles[6 * TriangleLinearIdx + 3] = LinearIdx;
                Triangles[6 * TriangleLinearIdx + 4] = LinearIdx + 1 + NumY;
                Triangles[6 * TriangleLinearIdx + 5] = LinearIdx + NumY;
            }
        }
    }

	gGroundPlane->init(Vertices, Normals, Triangles);
	gGroundPlane->setMaterial(10.f, ogl::Vec3f(1.0f, 0.7f, 0.1f));

    
    //And now for the grid
    std::vector< std::vector< ogl::Vec3f> > GridLines;
    const int EveryX = 10;
    const int EveryY = 10;
    GridLines.resize(NumX / EveryX + NumY / EveryY);
    int idxLine(0);
    // - horizontal
    for (int j=0;j<NumY;j+=10,idxLine++)
    {
        std::vector< ogl::Vec3f>& ThisLine = GridLines[idxLine];
        ThisLine.resize(NumX);
        for(int i=0;i<NumX;i++)
        {
            ThisLine[i] = Vertices[j*NumX + i];
            ThisLine[i][2] += 0.01f;
        }
    }
    // - vertical
    for(int i=0;i<NumX;i+=EveryX,idxLine++)
    {
        std::vector< ogl::Vec3f>& ThisLine = GridLines[idxLine];
        ThisLine.resize(NumY);
        for(int j=0;j<NumY;j++)
        {
            ThisLine[j] = Vertices[j*NumX + i];
            ThisLine[j][2] += 0.01f;
        }
    }

    gGroundPlaneGrid = std::make_shared<ogl::TriangleGeometry>();
    InitLineGeometry(GridLines, gGroundPlaneGrid, 0.05f);
    gGroundPlaneGrid->setMaterial(10, ogl::Vec3f(0.5f, 0.35f, 0.05f));

    return true;
}


// Initialize the light sphere geometry
// Load the OBJ files, apply materials and transformations
bool initLightSpheres()
{
	ogl::IndexedTriangleIO io;
	io.loadFromOBJ(gDataPath + "assets/lightSphere.obj");

	if (io.vertexNormals().empty())
	{
		std::cerr << "OBJ model needs vertex normals!" << std::endl;
		return false;
	}

	std::cerr << "Loaded " << io.vertexPositions().size() << " vertices, " <<
		io.triangleIndices().size() / 3 << " triangles.\n\n";

	for (int i = 0; i<3; ++i)
	{
		gLightSphere[i] = std::make_shared<ogl::TriangleGeometry>();
		// The first sphere stores the original geometry (indexed triangle set)
		if (i == 0)
			gLightSphere[i]->init(io.vertexPositions(), io.vertexNormals(), io.triangleIndices());
		// The other spheres are instances of the original geometry (no duplicate geometry stored)
		else
			gLightSphere[i]->initInstance(gLightSphere[0]);

		// Move away from the origin
		gLightSphere[i]->modelMatrix().translate(6, 6, 5 + i/2.f);
	}

	// Position three spheres on a circle equidistantly
	//gLightSphere[0]->modelMatrix().rotate(0,ogl::Vec3f(0,0,1));
	gLightSphere[1]->modelMatrix().rotate(120, ogl::Vec3f(0, 0, 1));
	gLightSphere[2]->modelMatrix().rotate(240, ogl::Vec3f(0, 0, 1));

	return true;
}

// Initialize the OpenGL, camera, shader and calls
// geometry-related init-functions.
bool init()
{
	// Activate depth test to discard fragment that are hidden
	glEnable(GL_DEPTH_TEST);

	// Activate anti-aliasing
	glEnable(GL_MULTISAMPLE);

	// Create camera
	gCamera = std::make_shared<ogl::Camera>(gWidth, gHeight);
	gCamera->setPosition(ogl::Vec3f(16, -9, 6));
	gCamera->setTarget(ogl::Vec3f(3, 3, 2));

	// Create Phong shader
	gShaderProgram = std::make_shared<ogl::ShaderProgram>();
	if (!gShaderProgram->init(
		gDataPath + "assets/SolidWirePhong.vs",
		gDataPath + "assets/SolidWirePhong.gs",
		gDataPath + "assets/SolidWirePhong.fs"))
		return false;

	//Call geometry-related init-functions
	if (!initLightSpheres())
		return false;

	if (!initGroundPlane())
		return false;

	if (!initTanks())
		return false;

    initBallisticTrajectory(true);

	return true;
}

// This callback is called upon resizing the OpenGL window,
// e.g. when maximizing the window
void resizeCB(GLFWwindow* window, int width, int height)
{
	gCamera->resize(width, height);
}

// This callback is called when moving the mouse while a button is pressed
void motionCB(GLFWwindow* window, double x, double y)
{
	gCamera->mouseMoved(int(x), int(y));
}

// This callback is called when a mouse button is pressed or released
void mouseCB(GLFWwindow* window, int button, int state, int mods)
{
	gCamera->mouseButtonPressed(button, state);
}

// This callback is called when the scroll wheel is used
void wheelCB(GLFWwindow* window, double xOffset, double yOffset)
{
	gCamera->mouseWheelScrolled(int(yOffset));
}

// This callback is called when a key is pressed
 //*  @param[in] window The window that received the event.
 //*  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 //*  @param[in] scancode The system-specific scancode of the key.
 //*  @param[in] action @ref GLFW_PRESS, @ref GLFW_RELEASE or @ref GLFW_REPEAT.
 //*  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
 //*  held down.
void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == ' ' && action == GLFW_PRESS) gbShoot = true;
}

void glfwErrorCallback(int errorCode, const char* description)
{
	std::cerr << "GLFW Error Code " << errorCode << ": " << description << std::endl;
}


// Main entry point
int main(int argc, char** argv)
{
	std::string pathToThisFile = __FILE__;

	if (gDataPath.empty())
		gDataPath = rt::Math::getParentDirectoryFromFilePath(pathToThisFile);
	std::cerr << "Using data path: " << gDataPath << std::endl;

	std::cerr << "Use your mouse to rotate,pan and zoom the camera" << std::endl;
	std::cerr << "left mouse button + drag -> rotate" << std::endl;
	std::cerr << "middle mouse button + drag -> pan" << std::endl;
	std::cerr << "scroll wheel up / down -> zoom in / out" << std::endl;

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n" << std::endl;
		return -1;
	}

	glfwSetErrorCallback(glfwErrorCallback);

	// Create the OpenGL window
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Those stop GLFW from initializing successfully?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open OpenGL fullscreen window
	gGLFWWindow = glfwCreateWindow(gWidth, gHeight, "GLFW OpenGL Window", nullptr, nullptr);

	//// Open OpenGL fullscreen window
	//if( !glfwOpenWindow( gWidth, gHeight, 0,0,0,0, 16,0, GLFW_WINDOW ) )
	//{
	//  fprintf( stderr, "Failed to open GLFW window\n" );
	//  glfwTerminate();
	//  exit( EXIT_FAILURE );
	//}

	if (!gGLFWWindow)
	{
		std::cerr << "Failed to open GLFW window\n" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Disable VSync (we want to get as high FPS as possible!)

	glfwMakeContextCurrent(gGLFWWindow);
	glfwSwapInterval(1);

	// Setting this is necessary for core profile (tested with MSVC 2013 x64, Windows 7)
	glewExperimental = GL_TRUE;
	// GLEW wraps all OpenGL functions and extensions
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		std::cerr << (char*)glewGetErrorString(err) << std::endl;
		glfwTerminate();
		return -1;
	}
	glGetError(); //GLEW might cause an 'invalid enum' error, safely ignore it?

    // Print OpenGL context information to console
	ogl::printContextInformation();

	// Perform our initialization (OpenGL states, shader, camera, geometry)
	if (!init())
		return -1;

	// Set the appropriate callback functions
	// Window resize callback function
	glfwSetFramebufferSizeCallback(gGLFWWindow, resizeCB);
	glfwSetMouseButtonCallback(gGLFWWindow, mouseCB);
	glfwSetScrollCallback(gGLFWWindow, wheelCB);
	glfwSetCursorPosCallback(gGLFWWindow, motionCB);
    glfwSetKeyCallback(gGLFWWindow, keyCB);

	while (!glfwWindowShouldClose(gGLFWWindow))
	{
		// Get the current time (in milliseconds since program start)
		double t0 = glfwGetTime();

		// Clear frame and depth buffers
		glClearColor(gBackgroundColor[0], gBackgroundColor[1], gBackgroundColor[2], 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw...
		displayCB();

		// Hopefully there hasn't been any mistake?
		ogl::printOpenGLError();

		// Swap the rendering target with the frame buffer shown on the monitor
		glfwSwapBuffers(gGLFWWindow);

		glfwPollEvents();

		// Compute rendering time in seconds by taking difference to current time
		double t1 = glfwGetTime();
		gLastFrameTime = float(t1 - t0);
	}

	// Terminate OpenGL
	glfwTerminate();
	return 0;
}
