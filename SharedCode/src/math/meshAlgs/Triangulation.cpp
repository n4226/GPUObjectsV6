#include "pch.h"

#include "Triangulation.h"

//#include <CGAL/draw_triangulation_2.h>
//CGAL_USE_BASIC_VIEWER

//#include "igl/triangle/triangulate.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

namespace meshAlgs {

	void makeLibiglMesh(const BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, Eigen::MatrixXd& verts, Eigen::MatrixXi& indicies)
	{
		verts.resize(mesh.verts.size(), 3);
		for (size_t i = 0; i < mesh.verts.size(); i++)
		{
			verts(i, 0) = mesh.verts[i].x;
			verts(i, 1) = mesh.verts[i].y;
			verts(i, 2) = mesh.verts[i].z;
		}
		indicies.resize(mesh.indicies[subMesh].size() / 3, 3);
		//indicies(0, 0) = 0;
		//indicies(0, 1) = 1;
		//indicies(0, 2) = 2;

		//size_t i = 0;

		for (size_t t = 0; t < mesh.indicies[subMesh].size() / 3; t++)
		{
			indicies(t, 0) = mesh.indicies[subMesh][t * 3 + 0];
			indicies(t, 1) = mesh.indicies[subMesh][t * 3 + 1];
			indicies(t, 2) = mesh.indicies[subMesh][t * 3 + 2];
		}
	}

	void makeMeshFromLibigl(BinaryMeshSeirilizer::Mesh& mesh, size_t subMesh, const Eigen::MatrixXd& verts, const Eigen::MatrixXi& indicies)
	{
		auto baseVert = mesh.verts.size();
		for (size_t i = 0; i < verts.rows(); i++) {
			mesh.verts.emplace_back(verts(i, 0), verts(i, 1), verts(i, 2));
		}

		if (mesh.indicies.size() == subMesh)
			mesh.indicies.emplace_back();

		for (size_t t = 0; t < indicies.rows(); t++)
		{
			mesh.indicies[subMesh][t * 3 + 0] = indicies(t, 0) + baseVert;
			mesh.indicies[subMesh][t * 3 + 1] = indicies(t, 1) + baseVert;
			mesh.indicies[subMesh][t * 3 + 2] = indicies(t, 2) + baseVert;
		}
	}

	BinaryMeshSeirilizer::Mesh* triangulate(std::vector<std::vector<glm::vec2>> polygon)
	{
		typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
		typedef CGAL::Triangulation_vertex_base_with_info_2<uint32_t, K> Vb;
		typedef CGAL::Triangulation_data_structure_2<Vb>                   Tds;
		typedef CGAL::Triangulation_2<K,Tds>         Triangulation;
		typedef Triangulation::Point             Point; 
		typedef Triangulation::Vertex_circulator Vertex_circulator;


		auto mesh = new BinaryMeshSeirilizer::Mesh{};


		std::vector<std::pair<Point,uint32_t>> points = {};
		points.resize(polygon[0].size());
		for (size_t i = 0; i < polygon[0].size(); i++)
		{
		//std::transform(polygon[0].begin(), polygon[0].end(),points.begin(), [](glm::vec2 point) {
			auto point = polygon[0][i];
			points[i] = { Point(point.x, point.y), uint32_t(i)};
			mesh->verts.emplace_back(point.x,point.y,0);
		}//);

		Triangulation t;	

		t.insert(points.begin(), points.end());


		/*Vertex_circulator vc = t.incident_vertices(t.infinite_vertex()),
			done(vc);*/

		/*Vertex_circulator vc = t.incident_vertices(t.infinite_vertex()),
			done(vc);
		if (vc != 0) {
			do {
				std::cout << vc->point() << std::endl;
			} while (++vc != done);
		}*/


		mesh->indicies.push_back({});


		for (auto& vh : t.finite_face_handles()) {
			auto p = {vh->vertex(0)->info(), vh->vertex(1)->info(),vh->vertex(2)->info() };
			
			mesh->indicies[0].push_back(vh->vertex(0)->info());
			mesh->indicies[0].push_back(vh->vertex(1)->info());
			mesh->indicies[0].push_back(vh->vertex(2)->info());

			//t.triangle(vh)[0].
			
			/*for (auto pz : p)
				std::cout << pz << std::endl;
			std::cout << std::endl;*/
		}

		//for (auto& vh : t.finite_face_handles()) {
		//	auto p = { vh->vertex(0),vh->vertex(1), vh->vertex(2) };
		//	for (auto pz : p)
		//		std::cout << *pz << std::endl;
		//	std::cout << std::endl;
		//}

		//for (auto& vh : t.finite_vertex_handles()) {
		//	/*auto p = { vh->index(vh->vertex(0)), vh->index(vh->vertex(1)), vh->index(vh->vertex(2)) };
		//	for (auto pz : p)*/
		//		std::cout << *vh;
		//	std::cout << std::endl;
		//}

		//igl::opengl::glfw::Viewer viewer;
		//viewer.data().set_mesh(V, F);
		//viewer.launch();


		// Input polygon

		//// verticies
		//Eigen::MatrixXd V;
		//// edges 
		//Eigen::MatrixXi E;
		//// pointes inside of holes specified in edges
		//Eigen::MatrixXd H;


		//auto vertCount = 0;

		//for (size_t p = 0; p < polygon.size(); p++)
		//{
		//	vertCount += polygon[p].size();
		//}

		//V.resize(vertCount, 2);
		//E.resize(vertCount, 2);
		//H.resize(glm::max(polygon.size() - 1,size_t(0)), 2);

		//auto row = 0;
		//for (size_t p = 0; p < polygon.size(); p++)
		//{
		//	for (size_t point = 0; point < polygon[p].size(); point++)
		//	{

		//		V(row, 0) = polygon[p][point].x;
		//		V(row, 1) = polygon[p][point].y;

		//		row++;
		//	}
		//}

		//row = 0;
		//for (size_t p = 0; p < polygon.size(); p++)
		//{
		//	for (size_t point = 0; point < polygon[p].size(); point++)
		//	{
		//		if (point == 0)
		//			E(row, 0) = polygon.size() - 1;
		//		else
		//			E(row, 0) = point - 1;
		//		E(row, 1) = point;

		//	}
		//}

		////TODO fix this -- the average of a poly will not always be inside it

		//for (size_t p = 1; p < polygon.size(); p++)
		//{
		//	glm::vec2 center = glm::vec2(0);
		//	for (size_t point = 0; point < polygon[p].size(); point++)
		//	{
		//		center += polygon[p][point];
		//	}
		//	center /= polygon[p].size();

		//	H(p - 1, 0) = center.x;
		//	H(p - 1, 1) = center.y;
		//}

		//// Triangulated mesh
		//Eigen::MatrixXd V2;
		//Eigen::MatrixXi F2;

		//igl::triangle::triangulate(V, E, H, "q0", V2, F2);

		//auto m = new BinaryMeshSeirilizer::Mesh();

		//makeMeshFromLibigl(*m, 0, V2, F2);

		return mesh;
	}

	


}
