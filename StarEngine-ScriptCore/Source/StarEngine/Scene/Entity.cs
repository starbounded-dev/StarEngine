using StarEngine;
using System;
using System.Collections.Generic;

namespace StarEngine
{
	[EditorAssignable]
	public class Entity
	{
		//private Action<float> m_Collision2DBeginCallbacks;
		//private Action<float> m_Collision2DEndCallbacks;

		private Dictionary<Type, Component> m_ComponentCache = new Dictionary<Type, Component>();

		/// <summary>
/// Initializes a new instance of the Entity class with a default ID of 0.
/// </summary>
protected Entity() { ID = 0; }

		/// <summary>
		/// Initializes a new instance of the Entity class with the specified unique identifier.
		/// </summary>
		/// <param name="id">The unique identifier for the entity.</param>
		internal Entity(ulong id)
		{
			ID = id;
		}

		public readonly ulong ID;

		public Vector3 Translation
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetTranslationX(ID);
					float resultY = InternalCalls.TransformComponent_GetTranslationY(ID);
					float resultZ = InternalCalls.TransformComponent_GetTranslationZ(ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = (Vector3)value;
					InternalCalls.TransformComponent_SetTranslation(ID, result.X, result.Y, result.Z);
				}
			}
		}

		public Vector3 Rotation
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetRotationX(ID);
					float resultY = InternalCalls.TransformComponent_GetRotationY(ID);
					float resultZ = InternalCalls.TransformComponent_GetRotationZ(ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = (Vector3)value;
					InternalCalls.TransformComponent_SetRotation(ID, result.X, result.Y, result.Z);
				}
			}
		}

		public Vector3 Scale
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetScaleX(ID);
					float resultY = InternalCalls.TransformComponent_GetScaleY(ID);
					float resultZ = InternalCalls.TransformComponent_GetScaleZ(ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = (Vector3)value;
					InternalCalls.TransformComponent_SetScale(ID, result.X, result.Y, result.Z);
				}
			}
		}

		/// <summary>
		/// Creates and attaches a component of type <typeparamref name="T"/> to the entity if it does not already exist.
		/// </summary>
		/// <typeparam name="T">The type of component to create and attach.</typeparam>
		/// <returns>The created component instance, or the existing one if already present.</returns>
		public T? CreateComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			unsafe { InternalCalls.Entity_CreateComponent(ID, typeof(T)); }
			var component = new T { Entity = this };
			m_ComponentCache.Add(typeof(T), component);
			return component;
		}

		/// <summary>
		/// Determines whether the entity has a component of the specified type.
		/// </summary>
		/// <typeparam name="T">The type of component to check for.</typeparam>
		/// <returns>True if the entity has the component; otherwise, false.</returns>
		public bool HasComponent<T>() where T : Component
		{
			unsafe { return InternalCalls.Entity_HasComponent(ID, typeof(T)); }
		}

		/// <summary>
		/// Determines whether the entity has a component of the specified type attached.
		/// </summary>
		/// <param name="type">The type of the component to check for.</param>
		/// <returns>True if the component exists on the entity; otherwise, false.</returns>
		public bool HasComponent(Type type)
		{
			unsafe { return InternalCalls.Entity_HasComponent(ID, type); }
		}

		/// <summary>
		/// Retrieves the component of type <typeparamref name="T"/> attached to this entity, or returns null if the component does not exist.
		/// </summary>
		/// <typeparam name="T">The type of component to retrieve.</typeparam>
		/// <returns>The component of type <typeparamref name="T"/> if present; otherwise, null.</returns>
		public T? GetComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);

			if (!HasComponent<T>())
			{
				m_ComponentCache.Remove(componentType);
				return null;
			}

			if (!m_ComponentCache.ContainsKey(componentType))
			{
				var component = new T { Entity = this };
				m_ComponentCache.Add(componentType, component);
				return component;
			}

			return m_ComponentCache[componentType] as T;
		}

		/// <summary>
		/// Removes the component of type <typeparamref name="T"/> from the entity if it exists.
		/// </summary>
		/// <typeparam name="T">The type of component to remove.</typeparam>
		/// <returns>True if the component was removed; otherwise, false.</returns>
		public bool RemoveComponent<T>() where T : Component
		{
			Type componentType = typeof(T);
			bool removed;

			unsafe { removed = InternalCalls.Entity_RemoveComponent(ID, componentType); }

			if (removed && m_ComponentCache.ContainsKey(componentType))
				m_ComponentCache.Remove(componentType);

			return removed;
		}

		/// <summary>
		/// Destroys this entity, removing it from the engine.
		/// </summary>
		public void DestroyEntity()
		{
			unsafe
			{
				InternalCalls.Entity_DestroyEntity(ID);
			}
		}

		/// <summary>
		/// Finds and returns the first entity with the specified tag, or null if no such entity exists.
		/// </summary>
		/// <param name="tag">The tag to search for.</param>
		/// <returns>The entity with the given tag, or null if not found.</returns>
		public Entity FindEntityByTag(string tag)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Entity_FindEntityByTag(tag);
				if (entityID == 0)
					return null;
				return new Entity(entityID);
			}
		}

		/// <summary>
		/// Finds and returns the entity with the specified name, or null if no such entity exists.
		/// </summary>
		/// <param name="name">The name of the entity to search for.</param>
		/// <returns>The entity with the given name, or null if not found.</returns>
		public Entity FindEntityByName(string name)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Entity_FindEntityByName(name);
				if (entityID == 0)
					return null;

				return new Entity(entityID);
			}
		}

		//public T As<T>() where T : Entity, new()
		//{
		//    unsafe
		//    {
		//        object instance = InternalCalls.GetScriptInstance(ID);
		//        return instance as T;
		//    }
		//}

	}

}
