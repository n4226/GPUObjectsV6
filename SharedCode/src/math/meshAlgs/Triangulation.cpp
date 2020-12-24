#include "pch.h"

#include "Triangulation.h"

//#include <CGAL/draw_triangulation_2.h>
//CGAL_USE_BASIC_VIEWER

//#include "igl/triangle/triangulate.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>
#include <iostream>


namespace meshAlgs {

	struct FaceInfo2
	{
		FaceInfo2() {}
		int nesting_level;
		bool in_domain() {
			return nesting_level % 2 == 1;
		}
	};


	typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
	typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
	typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K>    Fbb;
	typedef CGAL::Constrained_triangulation_face_base_2<K, Fbb>        Fb;
	typedef CGAL::Triangulation_data_structure_2<Vb, Fb>               TDS;
	typedef CGAL::Exact_predicates_tag                                Itag;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
	typedef CDT::Point                                                Point;
	typedef CGAL::Polygon_2<K>                                        Polygon_2;
	typedef CDT::Face_handle                                          Face_handle;


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


	
	static void 
		mark_domains(CDT& ct,
			Face_handle start,
			int index,
			std::list<CDT::Edge>& border)
	{
		if (start->info().nesting_level != -1) {
			return;
		}
		std::list<Face_handle> queue;
		queue.push_back(start);
		while (!queue.empty()) {
			Face_handle fh = queue.front();
			queue.pop_front();
			if (fh->info().nesting_level == -1) {
				fh->info().nesting_level = index;
				for (int i = 0; i < 3; i++) {
					CDT::Edge e(fh, i);
					Face_handle n = fh->neighbor(i);
					if (n->info().nesting_level == -1) {
						if (ct.is_constrained(e)) border.push_back(e);
						else queue.push_back(n);
					}
				}
			}
		}
	}
	//explore set of facets connected with non constrained edges,
	//and attribute to each such set a nesting level.
	//We start from facets incident to the infinite vertex, with a nesting
	//level of 0. Then we recursively consider the non-explored facets incident
	//to constrained edges bounding the former set and increase the nesting level by 1.
	//Facets in the domain are those with an odd nesting level.
	static void
		mark_domains(CDT& cdt)
	{
		for (CDT::Face_handle f : cdt.all_face_handles()) {
			f->info().nesting_level = -1;
		}
		std::list<CDT::Edge> border;
		mark_domains(cdt, cdt.infinite_face(), 0, border);
		while (!border.empty()) {
			CDT::Edge e = border.front();
			border.pop_front();
			Face_handle n = e.first->neighbor(e.second);
			if (n->info().nesting_level == -1) {
				mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			}
		}
	}

	// poly gons should not have the start and end point be the same eg start and end should be different will close the path automatically
	BinaryMeshSeirilizer::Mesh* triangulate(std::vector<std::vector<glm::dvec2>> polygon)
	{
		// see : https://doc.cgal.org/latest/Triangulation_2/index.html#title30
			// - File Triangulation_2 / polygon_triangulation.cpp

		auto mesh = new BinaryMeshSeirilizer::Mesh{};


		std::vector<Polygon_2> cg_polygons = {};





		//std::vector<std::pair<Point,uint32_t>> points = {};
		//std::vector<Point> points = {};
		cg_polygons.resize(polygon.size());

		for (size_t i = 0; i < polygon.size(); i++)
		{
			for (size_t p = 0; p < polygon[i].size(); p++)
			{
				auto point = polygon[i][p];
				cg_polygons[i].push_back(Point(point.x, point.y));
			}//);
		}


		//Insert the polygons into a constrained triangulation
		CDT cdt;
		for (size_t i = 0; i < polygon.size(); i++)
			cdt.insert_constraint(cg_polygons[i].vertices_begin(), cg_polygons[i].vertices_end(), true);


		//Mark facets that are inside the domain bounded by the polygon
		mark_domains(cdt);

		std::set<Point> newVerts = {};

		for (Face_handle f : cdt.finite_face_handles())
		{
			if (f->info().in_domain()) {
				newVerts.insert(f->vertex(0)->point());
				newVerts.insert(f->vertex(1)->point());
				newVerts.insert(f->vertex(2)->point());
			}
		}

		for (auto& point : newVerts) {
			mesh->verts.emplace_back(point.x(), point.y(),0);
		}


		mesh->indicies.push_back({});

		for (Face_handle f : cdt.finite_face_handles())
		{
			if (f->info().in_domain()) {
				auto vPos1 = newVerts.find(f->vertex(0)->point());
				if (vPos1 == newVerts.end()) continue;
				auto index1 = std::distance(newVerts.begin(), vPos1);

				auto vPos2 = newVerts.find(f->vertex(1)->point());
				if (vPos2 == newVerts.end()) continue;
				auto index2 = std::distance(newVerts.begin(), vPos2);

				auto vPos3 = newVerts.find(f->vertex(2)->point());
				if (vPos3 == newVerts.end()) continue;
				auto index3 = std::distance(newVerts.begin(), vPos3);

				mesh->indicies[mesh->indicies.size() - 1].push_back(index1);
				mesh->indicies[mesh->indicies.size() - 1].push_back(index2);
				mesh->indicies[mesh->indicies.size() - 1].push_back(index3);
			}
		}

		return mesh;

		//Triangulation t;	

		//t.insert(points.begin(), points.end());


		/*Vertex_circulator vc = t.incident_vertices(t.infinite_vertex()),
			done(vc);*/

		/*Vertex_circulator vc = t.incident_vertices(t.infinite_vertex()),
			done(vc);
		if (vc != 0) {
			do {
				std::cout << vc->point() << std::endl;
			} while (++vc != done);
		}*/




		//for (auto& vh : t.finite_face_handles()) {
		//	auto p = {vh->vertex(0)->info(), vh->vertex(1)->info(),vh->vertex(2)->info() };
		//	
		//	mesh->indicies[0].push_back(vh->vertex(0)->info());
		//	mesh->indicies[0].push_back(vh->vertex(1)->info());
		//	mesh->indicies[0].push_back(vh->vertex(2)->info());

		//	//t.triangle(vh)[0].
		//	
		//	/*for (auto pz : p)
		//		std::cout << pz << std::endl;
		//	std::cout << std::endl;*/
		//}

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

		//return mesh;
	}

	


}
