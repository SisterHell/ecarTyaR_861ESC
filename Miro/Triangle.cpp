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

void Triangle::preCalc()
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle
	float xmin = fmin(fmin(v0.x, v1.x), v2.x);
	float ymin = fmin(fmin(v0.y, v1.y), v2.y);
	float zmin = fmin(fmin(v0.z, v1.z), v2.z);
	float xmax = fmax(fmax(v0.x, v1.x), v2.x);
	float ymax = fmax(fmax(v0.y, v1.y), v2.y);
	float zmax = fmax(fmax(v0.z, v1.z), v2.z);

	//set min , max ,and surface area
	min.set(xmin, ymin, zmin);
	max.set(xmax, ymax, zmax);
	s_area = 0.5*cross((v1 - v0), (v2 - v0)).length(); // surface area of this triangle
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

	TriangleMesh::TupleI3 tn3 = m_mesh->nIndices()[m_index];
	const Vector3 & n0 = m_mesh->normals()[tn3.x]; //normal a of triangle
	const Vector3 & n1 = m_mesh->normals()[tn3.y]; //normal b of triangle
	const Vector3 & n2 = m_mesh->normals()[tn3.z]; //normal c of triangle	

	Vector3 n = cross(v1 - v0, v2 - v0);
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
		
		//std::cout << "t = " << t << std::endl;
		if (t < tMin || t > tMax || t < epsilon){
			return false;
		}
		if (beta < -epsilon || beta > 1){
			return false;
		}
		if (gemma < -epsilon || gemma > 1){
			return false;
		}
		if (beta + gemma > 1){
			return false;
		}

		// calculate alpha and interpolate normal on barycentric coordinates
		alpha = 1 - beta - gemma;
		n = alpha*n0 + beta*n1 + gemma*n2;
		n.normalize();

		// update hit result
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
