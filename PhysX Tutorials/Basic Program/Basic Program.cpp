#include <iostream> //cout, cerr
#include <iomanip> //stream formatting
#include <windows.h> //delay function
#include <PxPhysicsAPI.h> //PhysX

using namespace std;
using namespace physx;

//PhysX objects
PxPhysics* physics;
PxFoundation* foundation;
debugger::comm::PvdConnection* vd_connection;

//simulation objects
PxScene* scene;
PxRigidDynamic* box;
PxRigidDynamic* box1;
PxRigidStatic* box2;
PxRigidStatic* plane;
PxVec3 startPos(0, 10, -10);
PxVec3 startPos1(5, 10, -10);
PxVec3 startPos2(10, 10, -10);
PxVec3 force(100, 0, 0);

///Initialise PhysX objects
bool PxInit()
{
	//default error and allocator callbacks
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	//Init PhysX
	//foundation
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

	if(!foundation)
		return false;

	//physics
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());

	if(!physics)
		return false;

	//connect to an external visual debugger (if exists)
	vd_connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100, 
		PxVisualDebuggerExt::getAllConnectionFlags());

	//create a default scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);

	if (!scene)
		return false;

	return true;
}

/// Release all allocated resources
void PxRelease()
{
	if (scene)
		scene->release();
	if (vd_connection)
		vd_connection->release();
	if (physics)
		physics->release();
	if (foundation)
		foundation->release();
}

///Initialise the scene
void InitScene()
{
	//default gravity
	scene->setGravity(PxVec3(0.f, -9.81f, 0.f));

	//materials
	PxMaterial* default_material = physics->createMaterial(0.f, 0.f, .5f);   //static friction, dynamic friction, restitution

	//create a static plane (XZ)
	plane = PxCreatePlane(*physics, PxPlane(PxVec3(0.f, 1.f, 0.f), 0.f), *default_material);
	scene->addActor(*plane);

	//create a dynamic actor and place it 10 m above the ground
	box = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 10.f, 10.f)));
	box->setGlobalPose(PxTransform(startPos));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	//update the mass of the box
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f); //density of 1kg/m^3
	scene->addActor(*box);

	box1 = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 10.f, 10.f)));
	box->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); //kinematic actors cant be affected by external forces
	box1->setGlobalPose(PxTransform(startPos1));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box1->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	//update the mass of the box
	PxRigidBodyExt::updateMassAndInertia(*box1, 1.f); //density of 1kg/m^3
	scene->addActor(*box1);

	box2 = physics->createRigidStatic(PxTransform(PxVec3(0.f, 10.f, 10.f)));
	box2->setGlobalPose(PxTransform(startPos2));
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box2->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	//update the mass of the box
	//PxRigidBodyExt::updateMassAndInertia(*box2, 1.f); //density of 1kg/m^3
	scene->addActor(*box2);



	//PxReal mass = box->getMass(); //get mass and output it
	//cout << "Mass=" << mass << endl;
	//box->addForce(force);
}

/// Perform a single simulation step
void Update(PxReal delta_time)
{
	scene->simulate(delta_time);
	scene->fetchResults(true);
}

/// The main function
int main()
{
	//initialise PhysX	
	if (!PxInit())
	{
		cerr << "Could not initialise PhysX." << endl;
		return 0;
	}

	//initialise the scene
	InitScene();
	//set the simulation step to 1/60th of a second
	PxReal delta_time = 1.f/60.f;

	//simulate until the 'Esc' is pressed
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		//'visualise' position and velocity of the box
		PxVec3 position = box->getGlobalPose().p;
		PxVec3 velocity = box->getLinearVelocity();
		
		cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << 
			", y=" << position.y << ", z=" << position.z << ",  ";
		cout << setiosflags(ios::fixed) << setprecision(2) << "vx=" << velocity.x << 
			", vy=" << velocity.y << ", vz=" << velocity.z << endl;
		//if (box->isSleeping())
		//{
		//	cout << "Object Sleeping" << endl;
		//}
	   //
		//perform a single simulation step
		Update(delta_time);
		
		//introduce 100ms delay for easier visual analysis of the results
		Sleep(100);
	}

	//Release all resources
	PxRelease();

	return 0;
}
