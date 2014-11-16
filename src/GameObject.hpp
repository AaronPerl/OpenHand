#ifndef __GAMEOBJECT_HPP__
#define __GAMEOBJECT_HPP__

#include "libmodel/model.h"
#include <glm/glm.hpp>

class GameObject {
public:
	GameObject(model newModel) : myModel(newModel)
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < myModel.objectc; i++)
		{
			expandVertices(myModel.objects[i]);
		}
	}
	glm::vec3 getPosition() const
	{
		return position;
	}
	void setPosition(glm::vec3 newPosition)
	{
		position = newPosition;
	}
	const model getModel() { return myModel; }

private:
	model myModel;
	glm::vec3 position;

};

#endif