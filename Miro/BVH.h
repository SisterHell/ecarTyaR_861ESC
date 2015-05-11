#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"

class BVH
{
public:
	struct bbox{
		Vector3 min;
		Vector3 max;
	};

	struct BSP_node{
		bbox box;
		BSP_node *l_node, *r_node;
		Objects * obj_arr;
		int axis;
		float p_pos;
	};

	void subdivide(BSP_node* node);
	void setRoot(Vector3, Vector3);
    void build(Objects * objs);

    bool intersect(HitInfo& result, const Ray& ray,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

	BSP_node* root;
protected:
    Objects * m_objects;
};

#endif // CSE168_BVH_H_INCLUDED
