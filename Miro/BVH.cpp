#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include <functional>
#include <stdio.h>
#include <time.h>

int BVH::box_num = 0;
int BVH::tri_num = 0;
int BVH::leaf_num = 0;
int BVH::ray_box_intersection = 0;
int BVH::ray_tri_intersection = 0;

void
BVH::build(Objects * objs)
{
	clock_t start_time, end_time;

	start_time = clock();
    // construct the bounding volume hierarchy
    m_objects = objs;
	root->obj_arr = 0;
	subdivide(root,objs);
	tri_num = 0;

	std::cout << "box number = " << box_num << std::endl;
	std::cout << "tri number = " << tri_num << std::endl;
	std::cout << "leaf number = " << leaf_num << std::endl;

	end_time = clock();
	std::cout << "build time : " << ((double)(end_time - start_time)) / CLOCKS_PER_SEC << "\n";
}
	
void BVH::subdivide(BSP_node* node, Objects * objs)
{
	clock_t start_time, end_time;

	Objects * left_objs = new Objects();
	Objects * right_objs = new Objects();
	node->obj_arr = NULL;
	int axis = 0;  // 012 xyz
	int pos = -1;
	float m_cost = 100000000;
	double left_area = 0;
	double right_area = 0;
	double total_area = 0;
	double size = objs->size();
	float ci = 0.8;
	float ct = 0.2;
	
	double mean[3] = { 0.0, 0.0, 0.0 };
	double variance[3] = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < size; i++){
		Vector3 p = objs->at(i)->center;
		mean[0] += p.x;
		mean[1] += p.y;
		mean[2] += p.z;
	}
	mean[0] /= size;
	mean[1] /= size;
	mean[2] /= size;
	for (int i = 0; i < size; i++){
		total_area += objs->at(i)->s_area;
		Vector3 p = objs->at(i)->center;
		double temp = p.x - mean[0];
		variance[0] += temp*temp;
		temp = p.y - mean[1];
		variance[1] += temp*temp;
		temp = p.z - mean[2];
		variance[2] += temp*temp;
	}
	variance[0] = variance[0] / size;
	variance[1] = variance[1] / size;
	variance[2] = variance[2] / size;

	if (variance[0] > variance[1] && variance[0] > variance[2]){
		axis = 0;
	}
	else if (variance[1] > variance[0] && variance[1] > variance[2]){
		axis = 1;
	}
	else{
		axis = 2;
	}
	
	//start_time = clock();
	if (axis == 0){  //x 
		std::sort(objs->begin(), objs->end(), xcomp);
		left_area = 0;
		right_area = 0;
		for (int i = 0; i < objs->size() - 1; i++)
		{
			left_area += objs->at(i)->s_area;
			right_area = total_area - left_area;
			//float p_area = left_area + right_area;
			float cost = ct + (left_area*i*ci /total_area) + (right_area*(size - i)*ci / total_area);
			if (cost < m_cost){
				m_cost = cost;
				axis = 0;
				pos = i;
			}
		}
	}
	else if (axis == 1){  //y 
		std::sort(objs->begin(), objs->end(), ycomp);
		left_area = 0;
		right_area = 0;
		for (int i = 0; i < objs->size() - 1; i++)
		{
			left_area += objs->at(i)->s_area;
			right_area = total_area - left_area;
			//float p_area = left_area + right_area;
			float cost = ct + (left_area*i*ci / total_area) + (right_area*(size - i)*ci / total_area);
			if (cost < m_cost){
				m_cost = cost;
				axis = 1;
				pos = i;
			}
		}
	}
	else{  //z
		std::sort(objs->begin(), objs->end(), zcomp);
		left_area = 0;
		right_area = 0;
		for (int i = 0; i < objs->size() - 1; i++)
		{
			left_area += objs->at(i)->s_area;
			right_area = total_area - left_area;
			//float p_area = left_area + right_area;
			float cost = ct + (left_area*i*ci / total_area) + (right_area*(size - i)*ci / total_area);
			if (cost < m_cost){
				m_cost = cost;
				axis = 2;
				pos = i;
			}
		}
	}
	
	//end_time = clock();
	//std::cout << "cut test time : " << ((double)(end_time - start_time)) / CLOCKS_PER_SEC << "\n";
	
	/*
	// try to divide along x-axis 
	//start_time = clock();
	std::sort(objs->begin(), objs->end(),xcomp);
	//end_time = clock();
	//std::cout << "sort Time : " << ((double)(end_time - start_time)) / CLOCKS_PER_SEC << "\n";
	//start_time = clock();
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
	//end_time = clock();
	//std::cout << "cut test time : " << ((double)(end_time - start_time)) / CLOCKS_PER_SEC << "\n";

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
	*/
	// setup left and right nodes and their boudning box
	node->l_node = new BSP_node();
	node->r_node = new BSP_node();
	box_num += 2;
	Vector3 min, max;
	min.set(INFINITY, INFINITY, INFINITY);
	max.set(-INFINITY, -INFINITY, -INFINITY);

	for (int i = 0; i <= pos; i++){
		if (objs->at(i)->min.x < min.x){
			min.x = objs->at(i)->min.x;
		}
		if (objs->at(i)->min.y < min.y){
			min.y = objs->at(i)->min.y;
		}
		if (objs->at(i)->min.z < min.z){
			min.z = objs->at(i)->min.z;
		}
		if (objs->at(i)->max.x > max.x){
			max.x = objs->at(i)->max.x;
		}
		if (objs->at(i)->max.y > max.y){
			max.y = objs->at(i)->max.y;
		}
		if (objs->at(i)->max.z > max.z){
			max.z = objs->at(i)->max.z;
		}
		left_objs->push_back(objs->at(i));
	}
	node->l_node->box.min.set(min);
	node->l_node->box.max.set(max);
	min.set(INFINITY, INFINITY, INFINITY);
	max.set(-INFINITY, -INFINITY, -INFINITY);
	for (int i = pos+1; i < objs->size(); i++){
		if (objs->at(i)->min.x < min.x){
			min.x = objs->at(i)->min.x;
		}
		if (objs->at(i)->min.y < min.y){
			min.y = objs->at(i)->min.y;
		}
		if (objs->at(i)->min.z < min.z){
			min.z = objs->at(i)->min.z;
		}
		if (objs->at(i)->max.x > max.x){
			max.x = objs->at(i)->max.x;
		}
		if (objs->at(i)->max.y > max.y){
			max.y = objs->at(i)->max.y;
		}
		if (objs->at(i)->max.z > max.z){
			max.z = objs->at(i)->max.z;
		}
		right_objs->push_back(objs->at(i));
	}
	node->r_node->box.min.set(min);
	node->r_node->box.max.set(max);
	
	if (pos > 8){
		subdivide(node->l_node, left_objs);
	}
	else{
		node->l_node->obj_arr = left_objs;
		leaf_num++;
	}
	if (objs->size() - pos > 8){
		subdivide(node->r_node, right_objs);
	}
	else{
		node->r_node->obj_arr = right_objs;
		leaf_num++;
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
	hit = traverse(root, minHit, ray);
	/////////////////////////////////////////////////////////////////////////
    /*for (size_t i = 0; i < m_objects->size(); ++i)
    {
        if ((*m_objects)[i]->intersect(tempMinHit, ray, tMin, tMax))
        {
            hit = true;
            if (tempMinHit.t < minHit.t)
                minHit = tempMinHit;
        }
    }*/
	/////////////////////////////////////////////////////////////////////////
	if (hit){
		ray_tri_intersection++;
	}
    return hit;
}

bool BVH::box_intersect(const Ray& ray, BSP_node* node) const{
	float tmin = -INFINITY;
	float tmax = INFINITY;
	bbox* box = &node->box;

	float tx1 = (box->min.x - ray.o.x) / ray.d.x;
	float tx2 = (box->max.x - ray.o.x) / ray.d.x;
	tmin = fmax(tmin, fmin(tx1, tx2));
	tmax = fmin(tmax, fmax(tx1, tx2));

	float ty1 = (box->min.y - ray.o.y) / ray.d.y;
	float ty2 = (box->max.y - ray.o.y) / ray.d.y;
	tmin = fmax(tmin, fmin(ty1, ty2));
	tmax = fmin(tmax, fmax(ty1, ty2));

	float tz1 = (box->min.z - ray.o.z) / ray.d.z;
	float tz2 = (box->max.z - ray.o.z) / ray.d.z;
	tmin = fmax(tmin, fmin(tz1, tz2));
	tmax = fmin(tmax, fmax(tz1, tz2));

	return tmax >= tmin;
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

bool BVH::traverse(BSP_node* node, HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	bool hit = false;
	HitInfo tempMinHit;

	if (box_intersect(ray, node)){
		ray_box_intersection++;
		//std::cout << "box hit!\n";
		// check if node is leaf
		if (node->obj_arr != NULL){
			//std::cout << "we have " << node->obj_arr->size() << " triangles!\n";
			for (int i = 0; i < node->obj_arr->size(); i++){
				Object* tri = node->obj_arr->at(i);
				//std::cout << "triangle = " << tri->center.x << "," << tri->center.y << "," << tri->center.z << std::endl;
				//std::cin.get();
				if (node->obj_arr->at(i)->intersect(tempMinHit, ray, tMin, tMax))
				{
					//std::cout << "triangle hit!" << std::endl;
					//std::cout << "HIT TRI = " << tri->center.x << "," << tri->center.y << "," << tri->center.z << std::endl;
					hit = true;
					if (tempMinHit.t < minHit.t)
						minHit = tempMinHit;
				}
			}
		}
		else{
			if (traverse(node->l_node, minHit, ray, tMin, tMax)){
				hit = true;
			}
			if (traverse(node->r_node, minHit, ray, tMin, tMax)){
				hit = true;
			}
		}
	}
	return hit;
}