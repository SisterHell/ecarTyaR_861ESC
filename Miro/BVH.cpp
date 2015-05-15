#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include <functional>

int BVH::box_num = 0;

void
BVH::build(Objects * objs)
{
    // construct the bounding volume hierarchy
    m_objects = objs;
	root->obj_arr = 0;
	subdivide(root,objs);
	//std::cout << "box number = " << box_num << std::endl;
}
	
void BVH::subdivide(BSP_node* node, Objects * objs)
{
	Objects * left_objs = new Objects();
	Objects * right_objs = new Objects();
	int axis = 0;  // 012 xyz
	int pos = -1;
	float m_cost = 100000000;
	float left_area = 0;
	float right_area = 0;
	float ci = 0.8;
	float ct = 0.2;
	// try to divide along x-axis 
	std::sort(objs->begin(), objs->end(),xcomp);
	for (int i = 0; i < objs->size()-1; i++)
	{
		left_area = 0;
		right_area = 0;
		for (int j = 0; j <= i; j++)
		{
			left_area += objs->at(j)->s_area;
		}
		for (int j = i + 1; j < objs->size(); j++)
		{
			right_area += objs->at(j)->s_area;
		}
		float p_area = left_area + right_area;
		float cost = ct + (left_area*i*ci / p_area) + (right_area*(objs->size() - i)*ci / p_area);
		if (cost < m_cost){
			m_cost = cost;
			axis = 0;
			pos = i;
		}
	}
	// try to divide along y-axis 
	std::sort(objs->begin(), objs->end(),ycomp);
	for (int i = 0; i < objs->size() - 1; i++)
	{
		left_area = 0;
		right_area = 0;
		for (int j = 0; j <= i; j++)
		{
			left_area += objs->at(j)->s_area;
		}
		for (int j = i + 1; j < objs->size(); j++)
		{
			right_area += objs->at(j)->s_area;
		}
		float p_area = left_area + right_area;
		float cost = ct + (left_area*i*ci / p_area) + (right_area*(objs->size() - i)*ci / p_area);
		if (cost < m_cost){
			m_cost = cost;
			axis = 1;
			pos = i;
		}
	}
	// try to divide along z-axis 
	std::sort(objs->begin(), objs->end(),zcomp);
	for (int i = 0; i < objs->size() - 1; i++)
	{
		left_area = 0;
		right_area = 0;
		for (int j = 0; j <= i; j++)
		{
			left_area += objs->at(j)->s_area;
		}
		for (int j = i + 1; j < objs->size(); j++)
		{
			right_area += objs->at(j)->s_area;
		}
		float p_area = left_area + right_area;
		float cost = ct + (left_area*i*ci / p_area) + (right_area*(objs->size() - i)*ci / p_area);
		if (cost < m_cost){
			m_cost = cost;
			axis = 2;
			pos = i;
		}
	}
	if (axis == 0)
	{
		std::sort(objs->begin(), objs->end(), xcomp);
	}
	else if (axis == 1)
	{
		std::sort(objs->begin(), objs->end(), ycomp);
	}
	for (int i = 0; i <= pos; i++){
		left_objs->push_back(objs->at(i));
	}
	for (int i = pos+1; i < objs->size(); i++){
		right_objs->push_back(objs->at(i));
	}

	// setup left and right nodes and their boudning box
	node->l_node = new BSP_node();
	node->r_node = new BSP_node();
	node->l_node->box.min.set(objs->at(0)->min);
	node->l_node->box.max.set(objs->at(pos)->max);
	node->r_node->box.min.set(objs->at(pos+1)->min);
	node->r_node->box.max.set(objs->at(objs->size()-1)->max);
	box_num += 2;
	if (pos > 8){
		subdivide(node->l_node, left_objs);
	}
	else{
		node->l_node->obj_arr = left_objs;
	}
	if (objs->size() - pos > 8){
		subdivide(node->r_node, right_objs);
	}
	else{
		node->r_node->obj_arr = right_objs;
	}
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
	/////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////////
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

bool BVH::xcomp(Object* a, Object* b)
{
	return (a->center.x < b->center.x);
}

bool BVH::ycomp(Object* a, Object* b)
{
	return (a->center.y < b->center.y);
}

bool BVH::zcomp(Object* a, Object* b)
{
	return (a->center.z < b->center.z);
}