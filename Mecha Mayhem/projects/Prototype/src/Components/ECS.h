#pragma once
#include <entt/entt.hpp>
#include "PhysBody.h"

class ECS abstract
{
public:
	/////	Managing the class	/////

	//Let the class edit the registry
	static void AttachRegistry(entt::registry* reg);
	static void AttachWorld(btDiscreteDynamicsWorld* world);

	//Make this class not attached to anything
	static void DettachRegistry();

	//in case there are functions not covered here. use with caution
	static entt::registry* GetRegistry();

	/////	Entity management	/////

	//Create an Entity and return the index, defaults with transform component
	static entt::entity CreateEntity(bool hasTransform = true);

	//Delete an entity
	static void DestroyEntity(entt::entity entity);

	//check if the entity is in the registry
	static bool Exists(entt::entity entity);

	//Attach a class (use the overload plz)
	template<typename T>
	static T& AttachComponent(entt::entity entity);

	//Attach a class with existing data (use this)
	template<typename T>
	static T& AttachComponent(entt::entity entity, T object);

	//Remove a class from an entity
	template<typename T>
	static void RemoveComponent(entt::entity entity);

	//Get the class of an entity
	template<typename T>
	static T& GetComponent(entt::entity entity);

	//check if entity has component, returns nullptr if failed
	template<typename T>
	static bool HasComponent(entt::entity entity);

	static entt::registry* m_registry;
private:
	static btDiscreteDynamicsWorld* m_world;
};

template<typename T>
inline T& ECS::AttachComponent(entt::entity entity)
{
	m_registry->assign<T>(entity);

	return m_registry->get<T>(entity);
}

template<typename T>
inline T& ECS::AttachComponent(entt::entity entity, T object)
{
	m_registry->assign<T>(entity);

	return (m_registry->get<T>(entity) = object);
}

template<typename T>
inline void ECS::RemoveComponent(entt::entity entity)
{
	m_registry->remove<T>(entity);
}

template<typename T>
inline T& ECS::GetComponent(entt::entity entity)
{
	return m_registry->get<T>(entity);
}

template<typename T>
inline bool ECS::HasComponent(entt::entity entity)
{
	return m_registry->has<T>(entity);
}
