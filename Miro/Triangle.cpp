#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r,float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle
	//std::cout << "v0 = " << v0.x << "," << v0.y << "," << v0.z << "\n";
	//std::cout << "v1 = " << v1.x << "," << v1.y << "," << v1.z << "\n";
	//std::cout << "v2 = " << v2.x << "," << v2.y << "," << v2.z << "\n";

	TriangleMesh::TupleI3 tn3 = m_mesh->nIndices()[m_index];
	const Vector3 & n0 = m_mesh->normals()[tn3.x]; //vertex a of triangle
	const Vector3 & n1 = m_mesh->normals()[tn3.y]; //vertex b of triangle
	const Vector3 & n2 = m_mesh->normals()[tn3.z]; //vertex c of triangle
	//std::cout << "n0 = " << n0.x << "," << n0.y << "," << n0.z << "\n";
	//std::cout << "n1 = " << n1.x << "," << n1.y << "," << n1.z << "\n";
	//std::cout << "n2 = " << n2.x << "," << n2.y << "," << n2.z << "\n";
	

	Vector3 n = cross(v1 - v0, v2 - v0);
	//n.normalize();
	//std::cout << "normal = " << n.x << "," << n.y << "," << n.z << "\n";
	//std::cin.get();
	float t = -1;
	float alpha = 0;
	float beta = 0;
	float gemma = 0;

	// check if we are dividing by 0
	if (dot(n, r.d) != 0){
		// calculate t for p = o + td
		// also calculate beta,gemma for barycentric coordinates
		t = dot(n, v0 - r.o) / dot(n, r.d);
		beta = dot(cross(r.o - v0, v2 - v0),r.d) / dot(n,r.d);
		gemma = dot(cross(v1 - v0, r.o - v0), r.d) / dot(n, r.d);
		
		//std::cout << "nxd = " << dot(n, r.d) << std::endl;
		//std::cout << "r.o = " << r.o.x << "," << r.o.y << "," << r.o.z << std::endl;
		//std::cout << "nxd = " << cross(r.o - v0, v2 - v0) << std::endl;
		//std::cout << "beta = " << beta << std::endl;
		// check whether the point is inside the triangle
		if (t < tMin || t > tMax){
			//std::cout << "t out of range\n";
			//std::cin.get();
			return false;
		}
		if (beta < 0 || beta > 1){
			//std::cout << "beta out of range\n";
			//std::cin.get();
			return false;
		}
		if (gemma < 0 || gemma > 1){
			//std::cout << "beta out of range\n";
			//std::cin.get();
			return false;
		}
		if (beta + gemma > 1){
			//std::cout << "beta out of range\n";
			//std::cin.get();
			return false;
		}

		// calculate alpha and interpolate normal on barycentric coordinates
		alpha = 1 - beta - gemma;
		n = alpha*n0 + beta*n1 + gemma*n2;
		n.normalize();

		result.t = t;
		result.P = r.o + result.t*r.d;
		result.N = n;
		result.material = this->m_material;
		return true;
	}
	else{
		return false;
	}
}
