#include "Triangulation.h"

#include <igl/opengl/glfw/Viewer.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
//#include <CGAL/draw_triangulation_2.h>
//CGAL_USE_BASIC_VIEWER


namespace Triangulation {

	std::vector<glm::uint32> triangulate(std::vector<std::vector<glm::vec2>> polygon)
	{
		typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
		typedef CGAL::Triangulation_2<K>         Triangulation;
		typedef Triangulation::Point             Point;



		std::vector<Point> points = { Point(0,0), Point(1,0), Point(0,1) };
		Triangulation t;

		t.insert(points.begin(), points.end());


		//Vertex_circulator vc = t.incident_vertices(t.infinite_vertex()),
			//done(vc);

		for (auto& vh : t.finite_vertex_handles()) {
			Point p = vh->point();

		}

		igl::opengl::glfw::Viewer viewer;
		//viewer.data().set_mesh(V, F);
		//viewer.launch();

		return {};
	}


}
