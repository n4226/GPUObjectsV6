#pragma once

#include "../../pch.h"

class CameraSystem : public System
{
public:
	CameraSystem();
	void update() override;

private:

	void movePlayerAlongCamPath();


	const double speed = 500; // in meters / second

	size_t currentSegment = 0;
	double currentSegmentTime = 0;
	double totalSegmentTime = 0;

	std::vector<glm::dvec3> cameraPoints = {

		//glm::dvec3(40.610319941413, -74.039182662964, 100);

		// reversed order
		//glm::dvec3(40.54709068849014 , -73.98451039585544,500),
		//glm::dvec3(40.60603999938215 , -74.02412689599031,500),
		//glm::dvec3(40.68617096089564 , -74.03331417746679,500),
		//glm::dvec3(40.73199901859223 , -74.01813813429706,500),
		//glm::dvec3(40.76734669031339 , -74.01368629698507,500),
		//glm::dvec3(40.79415639586417 , -73.98463708020655,500),
		//glm::dvec3(40.78492880489573 , -73.96389654744658,500),
		//glm::dvec3(40.76711927501298 , -73.97452691880592,500),
		//glm::dvec3(40.73273944708617 , -73.98185517766292,500),
		//glm::dvec3(40.71398903798578 , -73.99861573311571,500),
		//glm::dvec3(40.63009052816579 , -74.05752673164039,500),
		//glm::dvec3(40.59898784065786 , -74.04207906535621,500),
		//glm::dvec3(40.5273511630884  , -74.01884552005585,500),
		//glm::dvec3(40.51994075985466 , -73.96928018695641,500)


		glm::dvec3(40.51994075985466 , -73.96928018695641,90), //500),
		glm::dvec3(40.54709068849014 , -73.98451039585544,90), //500),
		glm::dvec3(40.60603999938215 , -74.02412689599031,90), //500),
		glm::dvec3(40.68617096089564 , -74.03331417746679,90), //500),
		glm::dvec3(40.73199901859223 , -74.01813813429706,90), //500),
		glm::dvec3(40.76734669031339 , -74.01368629698507,90), //500),
		glm::dvec3(40.79415639586417 , -73.98463708020655,90), //500),
		glm::dvec3(40.78492880489573 , -73.96389654744658,90), //500),
		glm::dvec3(40.76711927501298 , -73.97452691880592,90), //500),
		glm::dvec3(40.73273944708617 , -73.98185517766292,90), //500),
		glm::dvec3(40.71398903798578 , -73.99861573311571,90), //500),
		glm::dvec3(40.63009052816579 , -74.05752673164039,90), //500),
		glm::dvec3(40.59898784065786 , -74.04207906535621,90), //500),
		glm::dvec3(40.5273511630884  , -74.01884552005585,90), //500),

		//- this is the first point again 
		//glm::dvec3(40.51994075985466 , -73.96928018695641,100)

	};

	//std::vector<glm::dvec3> cameraPoints = {

	//	glm::dvec3(40.58247271163678, -74.03006951604831, 400),
	//	glm::dvec3(40.61167467092515, -74.02970854633186,400),
	//	glm::dvec3(40.6298066902089 , -74.03581033962581, 400),
	//	glm::dvec3(40.63660028914893, -74.07056592763701,400),
	//	glm::dvec3(40.61839198197045, -74.07515546810707,400 ),
	//	//glm::dvec3(-74.03006951604831, 40.58247271163678,400 )


	//};

	
	Path<glm::dvec3> cameraPath;

};

