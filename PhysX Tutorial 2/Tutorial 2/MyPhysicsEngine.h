#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};
	const int wedgeh = 8;
	const int wedgel = 12;

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	static PxVec3 wedge_verts[] = { PxVec3(-6.2,0,-wedgel), PxVec3(-6.2,0,wedgel), PxVec3(-6.2,wedgeh,wedgel), PxVec3(6.2,0, -wedgel), PxVec3(6.2,0,wedgel), PxVec3(6.2,wedgeh,wedgel) };
	static PxVec3 flipper_verts[] = { PxVec3(-.1, 0, -0.75), PxVec3(-.1, 0, 0.75), PxVec3(-.1, 0.2, 0.75), PxVec3(.1, 0 , -0.75), PxVec3(.1, 0, 0.75), PxVec3(.1, 0.2, 0.75) };
	static PxVec3 hex_verts[] = { PxVec3(0,0,0), PxVec3(-1,1,0), PxVec3(-1, (1+ sqrt(2)) ,0), PxVec3(0,(2 + sqrt(2)),0), PxVec3((sqrt(2)),(2 + sqrt(2)),0), PxVec3(sqrt(2)+ 1, 1 + sqrt(2),0), PxVec3((sqrt(2) + 1), 1 ,0), PxVec3(sqrt(2), 0, 0),
		PxVec3(0,0,1), PxVec3(-1,1,1), PxVec3(-1, (1 + sqrt(2)) ,1), PxVec3(0,(2 + sqrt(2)),1), PxVec3((sqrt(2)),(2 + sqrt(2)),1), PxVec3(sqrt(2) + 1,1 + sqrt(2),1), PxVec3(sqrt(2) + 1, 1 ,1), PxVec3(sqrt(2), 0, 1) };
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	class TriangleWedge : public ConvexMesh
	{
	public:
		TriangleWedge(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(wedge_verts), end(wedge_verts)), pose, density)
		{
		}
	};

	class FlipperWedge : public ConvexMesh
	{
	public:
		FlipperWedge(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(flipper_verts), end(flipper_verts)), pose, density)
		{
		}
	};

	class Hexagon : public ConvexMesh
	{
	public:
		Hexagon(PxTransform pose = PxTransform(PxIdentity), PxReal density = 1.f) :
			ConvexMesh(vector<PxVec3>(begin(hex_verts), end(hex_verts)), pose, density)
		{
		}
	};
	
	static PxVec3 angularTranslate(PxReal x, PxReal z)
	{
		//angle of board
		float tans = 8.0f / (24.0f);
		float angle = atan(tans);
		//the distance to move down using the height at 0
		PxReal y = 4.0f + (z * tan(angle));
		PxVec3 coords(x, y, z);
		return coords;
	}

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		//initialise compound object
		RectangleEnclosure* obj;
		Box* box;
		Hexagon* hex;
		TriangleWedge* slope;
		Sphere* ball;
		FlipperWedge* flipper;
		PxParticleSystem *ps;

	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);
			//create compound object
			obj = new RectangleEnclosure(PxTransform(PxVec3(.0f, 0.f,2.f), PxQuat(radConv(-18.5), PxVec3(1.f, 0.f,0.f))));
			obj->SetKinematic(true);
			//angle in degrees
			obj->GetShape(0)->setLocalPose(PxTransform(PxVec3(0.f, 5.2f, 11.9f), PxQuat(radConv(90), PxVec3(1.f, 0.f, 0.f))));
			obj->GetShape(1)->setLocalPose(PxTransform(PxVec3(0.f, 5.2f, -11.9f), PxQuat(radConv(90), PxVec3(1.f, 0.f, 0.f))));
			obj->GetShape(2)->setLocalPose(PxTransform(PxVec3(6.f, 5.2f, .0f)));
			obj->GetShape(3)->setLocalPose(PxTransform(PxVec3(-6.f, 5.2f, .0f)));

			obj->Color(color_palette[5]);
			//add box
			//box = new Box(PxTransform(PxVec3(.0f, 3.f, .0f)));
			//box->Color(color_palette[4]);
			//hex = new Hexagon(PxTransform(PxVec3(.0f, 3.f, .0f)));
			//hex->Color(color_palette[5]);
			// particles
			//ps = GetPhysics()->createParticleSystem(50, false);
			////this->px_scene->addActor(*ps);
			//PxU32 parts[50];
			//PxVec3 positions[50];
			//PxVec3 velocities[50];
			//PxParticleCreationData pcd;
			//pcd.numParticles = 50;	
			//for (int i = 0; i < 50; i++)
			//{
			//	float f = float(i);
			//	parts[i] = i;
			//	positions[i] = { f / 2 , 4, 0 + f / 10  };
			//	velocities[i] = { 0,0,0 };
			//}
			//
			//pcd.indexBuffer = PxStrideIterator<const PxU32>(parts);
			//pcd.positionBuffer = PxStrideIterator<const PxVec3>(positions);
			//pcd.velocityBuffer = PxStrideIterator<const PxVec3>(velocities);
			//ps->createParticles(pcd);
			//ps->releaseParticles();
			// end particles

			flipper = new FlipperWedge(PxTransform(angularTranslate(-2, -7), PxQuat(radConv(-18.5), PxVec3(1.f, 0.f, 0.f))));
			flipper->SetKinematic(true);
			flipper->GetShape(0)->setLocalPose(PxTransform(PxVec3(0,0,0), PxQuat(radConv(90), PxVec3(0.f, 0.f, 1.f))));
			slope = new TriangleWedge(PxTransform(PxVec3(.0f, .0f, .0f)));
			slope->SetKinematic(true);
			slope->Color(color_palette[4]);
			ball = new Sphere(PxTransform(PxVec3(0.f, 5.f, 0.f)));
			ball->Color(color_palette[2]);
			
			//Add(hex);
			//Add(box);  
			Add(obj);
			Add(slope);
			Add(ball);
			Add(flipper);
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			//rotate 45 degrees every update
			//PxTransform pose = ((PxRigidBody*)hex->Get())->getGlobalPose();
			//pose.q *= PxQuat((PxHalfPi / 2)/100, PxVec3(.0f, 1.f, .0f));
			//((PxRigidBody*)hex->Get())->setGlobalPose(pose);
		}
	};
}
