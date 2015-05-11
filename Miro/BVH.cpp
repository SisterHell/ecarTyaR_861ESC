#include "BVH.h"
#include "Ray.h"
#include "Console.h"

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
	root->obj_arr = objs;
	subdivide(root);
}
	
void BVH::subdivide(BSP_node* node)
{


}

void BVH::setRoot(Vector3 min, Vector3 max)
{
	root = new BSP_node();
	root->box.max = max;
	root->box.min = min;
	std::cout << "root max = " << root->box.max.x << "," << root->box.max.y << "," << root->box.max.z << std::endl;
	std::cout << "root min = " << root->box.min.x << "," << root->box.min.y << "," << root->box.min.z << std::endl;
}

bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;
    
    for (size_t i = 0; i < m_objects->size(); ++i)
    {
        if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }
    
    return hit;
}
