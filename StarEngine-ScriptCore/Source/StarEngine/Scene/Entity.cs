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

		protected Entity() { ID = 0; }

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

		public T? CreateComponent<T>() where T : Component, new()
		{
			if (HasComponent<T>())
				return GetComponent<T>();

			unsafe { InternalCalls.Entity_CreateComponent(ID, typeof(T)); }
			var component = new T { Entity = this };
			m_ComponentCache.Add(typeof(T), component);
			return component;
		}

		public bool HasComponent<T>() where T : Component
		{
			unsafe { return InternalCalls.Entity_HasComponent(ID, typeof(T)); }
		}

		public bool HasComponent(Type type)
		{
			unsafe { return InternalCalls.Entity_HasComponent(ID, type); }
		}

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

		public bool RemoveComponent<T>() where T : Component
		{
			Type componentType = typeof(T);
			bool removed;

			unsafe { removed = InternalCalls.Entity_RemoveComponent(ID, componentType); }

			if (removed && m_ComponentCache.ContainsKey(componentType))
				m_ComponentCache.Remove(componentType);

			return removed;
		}

		public void DestroyEntity()
		{
			unsafe
			{
				InternalCalls.Entity_DestroyEntity(ID);
			}
		}

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
