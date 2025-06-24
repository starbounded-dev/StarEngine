using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;
using StarEngine;

namespace StarEngine
{
	public abstract class Component
	{
		public Entity Entity { get; set; }
	}

	public class TagComponent : Component
	{
		public string Tag
		{
			get
			{
				unsafe
				{
					return InternalCalls.TagComponent_GetTag(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TagComponent_SetTag(Entity.ID, value);
				}
			}
		}
	}

	public class TransformComponent : Component
	{
		public bool Enabled
		{
			get
			{
				unsafe
				{
					return InternalCalls.TransformComponent_GetIsEnabled(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TransformComponent_SetIsEnabled(Entity.ID, value);
				}
			}
		}

		public Transform Transform
		{
			get
			{
				unsafe
				{
					return InternalCalls.TransformComponent_GetTransform(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TransformComponent_SetTransform(Entity.ID, value);
				}
			}
		}

		public Vector3 Translation
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetTranslationX(Entity.ID);
					float resultY = InternalCalls.TransformComponent_GetTranslationY(Entity.ID);
					float resultZ = InternalCalls.TransformComponent_GetTranslationZ(Entity.ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = value;
					InternalCalls.TransformComponent_SetTranslation(Entity.ID, result.X, result.Y, result.Z);
				}
			}
		}

		public Vector3 Rotation
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetRotationX(Entity.ID);
					float resultY = InternalCalls.TransformComponent_GetRotationY(Entity.ID);
					float resultZ = InternalCalls.TransformComponent_GetRotationZ(Entity.ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = value;
					InternalCalls.TransformComponent_SetRotation(Entity.ID, result.X, result.Y, result.Z);
				}
			}
		}

		public Vector3 Scale
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TransformComponent_GetScaleX(Entity.ID);
					float resultY = InternalCalls.TransformComponent_GetScaleY(Entity.ID);
					float resultZ = InternalCalls.TransformComponent_GetScaleZ(Entity.ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = value;
					InternalCalls.TransformComponent_SetScale(Entity.ID, result.X, result.Y, result.Z);
				}
			}
		}
	}

	public class CameraComponent : Component
	{
		public bool Primary
		{
			get
			{
				unsafe
				{
					bool result = InternalCalls.CameraComponent_GetIsPrimary(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					bool result = value;
					InternalCalls.CameraComponent_SetPrimary(Entity.ID, result);
				}
			}
		}

		public bool FixedAspectRatio
		{
			get
			{
				unsafe
				{
					bool result = InternalCalls.CameraComponent_GetFixedAspectRatio(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					bool result = value;
					InternalCalls.CameraComponent_SetFixedAspectRatio(Entity.ID, result);
				}
			}
		}
	}

	public class ScriptComponent : Component
	{
		// Not used from C#
	}

	public class SpriteRendererComponent : Component
	{
		public Vector3 SpriteOffset
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.SpriteRendererComponent_GetOffsetX(Entity.ID);
					float resultY = InternalCalls.SpriteRendererComponent_GetOffsetY(Entity.ID);
					float resultZ = InternalCalls.SpriteRendererComponent_GetOffsetZ(Entity.ID);

					return new Vector3(resultX, resultY, resultZ);
				}
			}
			set
			{
				unsafe
				{
					Vector3 result = value;
					InternalCalls.SpriteRendererComponent_SetOffset(Entity.ID, result.X, result.Y, result.Z);
				}
			}
		}

		public Vector4 Color
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.SpriteRendererComponent_GetColorX(Entity.ID);
					float resultY = InternalCalls.SpriteRendererComponent_GetColorY(Entity.ID);
					float resultZ = InternalCalls.SpriteRendererComponent_GetColorZ(Entity.ID);
					float resultW = InternalCalls.SpriteRendererComponent_GetColorW(Entity.ID);

					return new Vector4(resultX, resultY, resultZ, resultW);
				}
			}
			set
			{
				unsafe
				{
					Vector4 result = value;
					InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, result.X, result.Y, result.Z, result.W);
				}
			}
		}

		public Vector2 UV
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.SpriteRendererComponent_GetUVX(Entity.ID);
					float resultY = InternalCalls.SpriteRendererComponent_GetUVY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.SpriteRendererComponent_SetUV(Entity.ID, result.X, result.Y);
				}
			}
		}

		public bool UseParallaxScrolling
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetUseParallax(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetUseParallax(Entity.ID, value);
				}
			}
		}

		public Vector2 ParallaxSpeed
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.SpriteRendererComponent_GetParallaxSpeedX(Entity.ID);
					float resultY = InternalCalls.SpriteRendererComponent_GetParallaxSpeedY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.SpriteRendererComponent_SetParallaxSpeed(Entity.ID, result.X, result.Y);
				}
			}
		}

		public float ParallaxDivision
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetParallaxDivision(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetParallaxDivision(Entity.ID, value);
				}
			}
		}

		public bool UseAtlasAnimation
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetUseTextureAtlasAnimation(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetUseTextureAtlasAnimation(Entity.ID, value);
				}
			}
		}

		public float AnimationSpeed
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetAnimationSpeed(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetAnimationSpeed(Entity.ID, value);
				}
			}
		}

		public int NumberOfTiles
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetNumTiles(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetNumTiles(Entity.ID, value);
				}
			}
		}

		public int StartIndexX
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetStartIndexX(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetStartIndexX(Entity.ID, value);
				}
			}
		}

		public int StartIndexY
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetStartIndexY(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetStartIndexY(Entity.ID, value);
				}
			}
		}

		public int CurrentIndexX
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetCurrentIndexX(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetCurrentIndexX(Entity.ID, value);
				}
			}
		}

		public int CurrentIndexY
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetCurrentIndexY(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetCurrentIndexY(Entity.ID, value);
				}
			}
		}

		public int Column
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetColumn(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetColumn(Entity.ID, value);
				}
			}
		}

		public int Row
		{
			get
			{
				unsafe
				{
					return InternalCalls.SpriteRendererComponent_GetRow(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.SpriteRendererComponent_SetRow(Entity.ID, value);
				}
			}
		}

		public float Saturation
		{
			get
			{
				unsafe
				{
					float result = InternalCalls.SpriteRendererComponent_GetSaturation(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					float result = value;
					InternalCalls.SpriteRendererComponent_SetSaturation(Entity.ID, result);
				}
			}
		}

		public AssetHandle TextureHandle
		{
			get
			{
				unsafe
				{
					AssetHandle result = InternalCalls.SpriteRendererComponent_GetTextureAssetHandle(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					AssetHandle result = value;
					InternalCalls.SpriteRendererComponent_SetTextureAssetHandle(Entity.ID, result);
				}
			}
		}

		public ulong TextureID
		{
			get
			{
				unsafe
				{
					ulong result = InternalCalls.SpriteRendererComponent_GetTextureAssetID(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					ulong result = value;
					InternalCalls.SpriteRendererComponent_SetTextureAssetID(Entity.ID, result);
				}
			}
		}
	}

	public class CircleRendererComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.CircleRendererComponent_GetColorX(Entity.ID);
					float resultY = InternalCalls.CircleRendererComponent_GetColorY(Entity.ID);
					float resultZ = InternalCalls.CircleRendererComponent_GetColorZ(Entity.ID);
					float resultW = InternalCalls.CircleRendererComponent_GetColorW(Entity.ID);

					return new Vector4(resultX, resultY, resultZ, resultW);
				}
			}
			set
			{
				unsafe
				{
					Vector4 result = value;
					InternalCalls.CircleRendererComponent_SetColor(Entity.ID, result.X, result.Y, result.Z, result.W);
				}
			}
		}

		public Vector2 UV
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.CircleRendererComponent_GetUVX(Entity.ID);
					float resultY = InternalCalls.CircleRendererComponent_GetUVY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.CircleRendererComponent_SetUV(Entity.ID, result.X, result.Y);
				}
			}
		}

		public bool UseParallaxScrolling
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetUseParallax(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetUseParallax(Entity.ID, value);
				}
			}
		}

		public Vector2 ParallaxSpeed
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.CircleRendererComponent_GetParallaxSpeedX(Entity.ID);
					float resultY = InternalCalls.CircleRendererComponent_GetParallaxSpeedY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.CircleRendererComponent_SetParallaxSpeed(Entity.ID, result.X, result.Y);
				}
			}
		}

		public float ParallaxDivision
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetParallaxDivision(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetParallaxDivision(Entity.ID, value);
				}
			}
		}

		public bool UseAtlasAnimation
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetUseTextureAtlasAnimation(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetUseTextureAtlasAnimation(Entity.ID, value);
				}
			}
		}

		public float AnimationSpeed
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetAnimationSpeed(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetAnimationSpeed(Entity.ID, value);
				}
			}
		}

		public int NumberOfTiles
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetNumTiles(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetNumTiles(Entity.ID, value);
				}
			}
		}

		public int StartIndexX
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetStartIndexX(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetStartIndexX(Entity.ID, value);
				}
			}
		}

		public int StartIndexY
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetStartIndexY(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetStartIndexY(Entity.ID, value);
				}
			}
		}

		public int CurrentIndexX
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetCurrentIndexX(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetCurrentIndexX(Entity.ID, value);
				}
			}
		}

		public int CurrentIndexY
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetCurrentIndexY(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetCurrentIndexY(Entity.ID, value);
				}
			}
		}

		public int Column
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetColumn(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetColumn(Entity.ID, value);
				}
			}
		}

		public int Row
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleRendererComponent_GetRow(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleRendererComponent_SetRow(Entity.ID, value);
				}
			}
		}
	}

	public class LineRendererComponent : Component
	{
		public float LineThickness
		{
			get
			{
				unsafe
				{
					float result = InternalCalls.LineRendererComponent_GetLineThickness(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					float result = value;
					InternalCalls.LineRendererComponent_SetLineThickness(Entity.ID, result);
				}
			}
		}
	}

	public class TextComponent : Component
	{

		public string Text
		{
			get
			{
				unsafe
				{
					return InternalCalls.TextComponent_GetText(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TextComponent_SetText(Entity.ID, value);
				}
			}
		}
		public Vector4 Color
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.TextComponent_GetColorX(Entity.ID);
					float resultY = InternalCalls.TextComponent_GetColorY(Entity.ID);
					float resultZ = InternalCalls.TextComponent_GetColorZ(Entity.ID);
					float resultW = InternalCalls.TextComponent_GetColorW(Entity.ID);

					return new Vector4(resultX, resultY, resultZ, resultW);
				}
			}
			set
			{
				unsafe
				{
					Vector4 result = value;
					InternalCalls.TextComponent_SetColor(Entity.ID, result.X, result.Y, result.Z, result.W);
				}
			}
		}

		public float Kerning
		{
			get
			{
				unsafe
				{
					return InternalCalls.TextComponent_GetKerning(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TextComponent_SetKerning(Entity.ID, value);
				}
			}
		}

		public float LineSpacing
		{
			get
			{
				unsafe
				{
					return InternalCalls.TextComponent_GetLineSpacing(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.TextComponent_SetLineSpacing(Entity.ID, value);
				}
			}
		}

	}

	public class RigidBody2DComponent : Component
	{
		public enum BodyType { Static = 0, Dynamic, Kinematic }

		/// <summary>
		/// Applies a linear impulse to the rigid body at the specified world position.
		/// </summary>
		/// <param name="impulse">The impulse vector to apply.</param>
		/// <param name="worldPosition">The world position where the impulse is applied.</param>
		/// <param name="wake">Whether to wake the rigid body if it is sleeping.</param>
		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
		{
			unsafe
			{
				InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(Entity.ID, impulse.X, impulse.Y, worldPosition.X, worldPosition.Y, wake);
			}
		}

		/// <summary>
		/// Applies a linear impulse to the center of the 2D rigid body.
		/// </summary>
		/// <param name="impulse">The impulse vector to apply.</param>
		/// <param name="wake">Whether to wake the body if it is sleeping.</param>
		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
		{
			unsafe
			{
				InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, impulse.X, impulse.Y, wake);
			}
		}

		public Vector2 LinearVelocity
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.RigidBody2DComponent_GetLinearVelocityX(Entity.ID);
					float resultY = InternalCalls.RigidBody2DComponent_GetLinearVelocityY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.RigidBody2DComponent_SetLinearVelocity(Entity.ID, result.X, result.Y);
				}
			}
		}

		public BodyType Type
		{
			get
			{
				unsafe
				{
					return InternalCalls.RigidBody2DComponent_GetType(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.RigidBody2DComponent_SetType(Entity.ID, value);
				}
			}
		}

		public Vector2 Gravity
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.RigidBody2DComponent_GetGravityX(Entity.ID);
					float resultY = InternalCalls.RigidBody2DComponent_GetGravityY(Entity.ID);
					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.RigidBody2DComponent_SetGravity(Entity.ID, result.X, result.Y);
				}
			}
		}

		public bool Enabled
		{
			get
			{
				unsafe
				{
					return InternalCalls.RigidBody2DComponent_GetEnabled(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.RigidBody2DComponent_SetEnabled(Entity.ID, value);
				}
			}
		}
	}

	public class BoxCollider2DComponent : Component
	{
		public Vector2 Ray
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.BoxCollider2DComponent_GetCollisionRayX(Entity.ID);
					float resultY = InternalCalls.BoxCollider2DComponent_GetCollisionRayY(Entity.ID);
					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.BoxCollider2DComponent_SetCollisionRay(Entity.ID, result.X, result.Y);
				}
			}
		}

		public Vector2 Offset
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.BoxCollider2DComponent_GetOffsetX(Entity.ID);
					float resultY = InternalCalls.BoxCollider2DComponent_GetOffsetY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.BoxCollider2DComponent_SetOffset(Entity.ID, result.X, result.Y);
				}
			}
		}

		public Vector2 Size
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.BoxCollider2DComponent_GetSizeX(Entity.ID);
					float resultY = InternalCalls.BoxCollider2DComponent_GetSizeY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.BoxCollider2DComponent_SetSize(Entity.ID, result.X, result.Y);
				}
			}
		}

		public float Density
		{
			get
			{
				unsafe
				{
					return InternalCalls.BoxCollider2DComponent_GetDensity(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.BoxCollider2DComponent_SetDensity(Entity.ID, value);
				}
			}
		}

		public float Friction
		{
			get
			{
				unsafe
				{
					return InternalCalls.BoxCollider2DComponent_GetFriction(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.BoxCollider2DComponent_SetFriction(Entity.ID, value);
				}
			}
		}

		public float Restitution
		{
			get
			{
				unsafe
				{
					return InternalCalls.BoxCollider2DComponent_GetRestitution(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.BoxCollider2DComponent_SetRestitution(Entity.ID, value);
				}
			}
		}

		public float RestitutionThreshold
		{
			get
			{
				unsafe
				{
					return InternalCalls.BoxCollider2DComponent_GetRestitutionThreshold(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.BoxCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
				}
			}
		}

		public bool Awake
		{
			get
			{
				unsafe
				{
					return InternalCalls.BoxCollider2DComponent_GetAwake(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.BoxCollider2DComponent_SetAwake(Entity.ID, value);
				}
			}
		}
	}

	public class CircleCollider2DComponent : Component
	{
		public Vector2 Ray
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.CircleCollider2DComponent_GetCollisionRayX(Entity.ID);
					float resultY = InternalCalls.CircleCollider2DComponent_GetCollisionRayY(Entity.ID);
					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.CircleCollider2DComponent_SetCollisionRay(Entity.ID, result.X, result.Y);
				}
			}
		}

		public Vector2 Offset
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.CircleCollider2DComponent_GetOffsetX(Entity.ID);
					float resultY = InternalCalls.CircleCollider2DComponent_GetOffsetY(Entity.ID);

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = value;
					InternalCalls.CircleCollider2DComponent_SetOffset(Entity.ID, result.X, result.Y);
				}
			}
		}

		public float Size
		{
			get
			{
				unsafe
				{
					float result = InternalCalls.CircleCollider2DComponent_GetRadius(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetRadius(Entity.ID, value);
				}
			}
		}

		public float Density
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleCollider2DComponent_GetDensity(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetDensity(Entity.ID, value);
				}
			}
		}

		public float Friction
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleCollider2DComponent_GetFriction(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetFriction(Entity.ID, value);
				}
			}
		}

		public float Restitution
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleCollider2DComponent_GetRestitution(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetRestitution(Entity.ID, value);
				}
			}
		}

		public float RestitutionThreshold
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleCollider2DComponent_GetRestitutionThreshold(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetRestitutionThreshold(Entity.ID, value);
				}
			}
		}

		public bool Awake
		{
			get
			{
				unsafe
				{
					return InternalCalls.CircleCollider2DComponent_GetAwake(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.CircleCollider2DComponent_SetAwake(Entity.ID, value);
				}
			}
		}
	}

	public class AudioListenerComponent : Component
	{
		public bool Active
		{
			get
			{
				unsafe
				{
					bool result = InternalCalls.AudioListenerComponent_GetActive(Entity.ID);
					return result;
				}
			}
			set
			{
				unsafe
				{
					bool result = value;
					InternalCalls.AudioListenerComponent_SetActive(Entity.ID, result);
				}
			}
		}
	}

	public class AudioSourceComponent : Component
	{
		public enum AttenuationModelType
		{
			None = 0,
			Inverse,
			Linear,
			Exponential
		};

		public AssetHandle Handle
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetAssetHandle(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetAssetHandle(Entity.ID, value);
				}
			}
		}

		/// <summary>
		/// Sets the audio asset handle for this audio source.
		/// </summary>
		public void SetAssetHandle(AssetHandle handle)
		{
			unsafe
			{
				InternalCalls.AudioSourceComponent_SetAssetHandle(Entity.ID, handle);
			}
		}

		public float Volume
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetVolume(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetVolume(Entity.ID, value);
				}
			}

		}

		public float Pitch
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetPitch(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetPitch(Entity.ID, value);
				}
			}
		}

		public bool PlayOnAwake
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetPlayOnAwake(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetPlayOnAwake(Entity.ID, value);
				}
			}
		}

		public bool Looping
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetLooping(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetLooping(Entity.ID, value);
				}
			}
		}

		public bool Spatialization
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetSpatialization(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetSpatialization(Entity.ID, value);
				}
			}
		}

		public AttenuationModelType AttenuationModel
		{
			get
			{
				unsafe
				{
					AttenuationModelType type = (AttenuationModelType)InternalCalls.AudioSourceComponent_GetAttenuationModel(Entity.ID);
					return type;
				}
			}
			set
			{
				unsafe
				{
					int v = (int)value;
					InternalCalls.AudioSourceComponent_SetAttenuationModel(Entity.ID, v);
				}
			}
		}

		public float RollOff
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetRollOff(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetRollOff(Entity.ID, value);
				}
			}
		}

		public float MinGain
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetMinGain(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetMinGain(Entity.ID, value);
				}
			}
		}

		public float MaxGain
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetMaxGain(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetMaxGain(Entity.ID, value);
				}
			}
		}

		public float MinDistance
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetMinDistance(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetMinDistance(Entity.ID, value);
				}
			}
		}

		public float MaxDistance
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetMaxDistance(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetMaxDistance(Entity.ID, value);
				}
			}
		}

		public float ConeInnerAngle
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetConeInnerAngle(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetConeInnerAngle(Entity.ID, value);
				}
			}
		}

		public float ConeOuterAngle
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetConeOuterAngle(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetConeOuterAngle(Entity.ID, value);
				}
			}

		}

		public float ConeOuterGain
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetConeOuterGain(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetConeOuterGain(Entity.ID, value);
				}
			}
		}

		public float DopplerFactor
		{
			get
			{
				unsafe
				{
					return InternalCalls.AudioSourceComponent_GetDopplerFactor(Entity.ID);
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.AudioSourceComponent_SetDopplerFactor(Entity.ID, value);
				}
			}
		}

		/// <summary>
		/// Determines whether the audio source is currently playing.
		/// </summary>
		/// <returns>True if the audio is playing; otherwise, false.</returns>
		public bool IsPlaying()
		{
			unsafe
			{
				return InternalCalls.AudioSourceComponent_IsPlaying(Entity.ID);
			}
		}

		/// <summary>
		/// Starts playback of the audio source attached to the entity.
		/// </summary>
		public void Play()
		{
			unsafe
			{
				InternalCalls.AudioSourceComponent_Play(Entity.ID);
			}
		}

		/// <summary>
		/// Pauses audio playback for the associated audio source component.
		/// </summary>
		public void Pause()
		{
			unsafe
			{
				InternalCalls.AudioSourceComponent_Pause(Entity.ID);
			}
		}

		/// <summary>
		/// Resumes playback of the audio source if it is currently paused.
		/// </summary>
		public void UnPause()
		{
			unsafe
			{
				InternalCalls.AudioSourceComponent_UnPause(Entity.ID);
			}
		}

		/// <summary>
		/// Stops audio playback for the associated audio source component.
		/// </summary>
		public void Stop()
		{
			unsafe
			{
				InternalCalls.AudioSourceComponent_Stop(Entity.ID);
			}
		}
	}
}
