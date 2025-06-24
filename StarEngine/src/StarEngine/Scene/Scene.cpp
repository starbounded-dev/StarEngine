#include "sepch.h"
#include "StarEngine/Scene/Scene.h"

#include "StarEngine/Asset/AssetManager.h"

#include "StarEngine/Audio/AudioEngine.h"
#include "StarEngine/Audio/AudioSource.h"
#include "StarEngine/Audio/AudioListener.h"

#include "StarEngine/Scene/Components.h"
#include "StarEngine/Scene/Entity.h"
#include "StarEngine/Scripting/ScriptEngine.h"
#include "StarEngine/Renderer/Renderer2D.h"
#include "StarEngine/Physics/ContactListener2D.h"

#include <glm/glm.hpp>

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace StarEngine {

	bool Scene::s_SetPaused = false;
	glm::vec2 Scene::s_Gravity = { 0.0f, -9.81f };

	static ContactListener2D s_Box2DContactListener;

	/**
	 * @brief Constructs a new Scene instance with default settings.
	 */
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	/**
	 * @brief Initializes the scene for runtime execution.
	 *
	 * Sets the scene as running, starts 2D physics simulation, activates audio listeners and sources (including playlist handling), and initializes all script instances, invoking their "OnCreate" method.
	 */
	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		ContactListener2D::m_IsPlaying = true;

		{
			auto filter = m_Registry.view<TransformComponent, AudioListenerComponent>();
			filter.each([&](TransformComponent& transform, AudioListenerComponent& ac)
				{
					ac.Listener = CreateRef<AudioListener>();
					if (ac.Active)
					{
						const glm::mat4 inverted = glm::inverse(transform.GetTransform());
						const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2].x, inverted[2].y, inverted[2].z));
						ac.Listener->SetConfig(ac.Config);
						ac.Listener->SetPosition(glm::vec4(transform.Translation, 1.0f));
						ac.Listener->SetDirection(glm::vec3{ -forward.x, -forward.y, -forward.z });
					}
				});
		}

		{
			auto view = m_Registry.view<TransformComponent, AudioSourceComponent>();
			view.each([&](TransformComponent& transform, AudioSourceComponent& ac)
				{
					if (AssetManager::IsAssetHandleValid(ac.Audio))
					{
						if (ac.Audio && !ac.AudioSourceData.UsePlaylist)
						{
							Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(ac.Audio);
							const glm::mat4 inverted = glm::inverse(transform.GetTransform());
							const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2].x, inverted[2].y, inverted[2].z));

							if (audioSource != nullptr)
							{
								audioSource->SetConfig(ac.Config);
								audioSource->SetPosition(glm::vec4(transform.Translation, 1.0f));
								audioSource->SetDirection(forward);
								if (ac.Config.PlayOnAwake)
									audioSource->Play();
							}
						}
						else if (ac.Audio && ac.AudioSourceData.UsePlaylist)
						{
							if (ac.AudioSourceData.CurrentIndex >= ac.AudioSourceData.Playlist.size())
								ac.AudioSourceData.CurrentIndex = 0;

							if (ac.AudioSourceData.CurrentIndex < ac.AudioSourceData.Playlist.size())
							{
								Ref<AudioSource> playingSourceIndex = AssetManager::GetAsset<AudioSource>(ac.AudioSourceData.Playlist[ac.AudioSourceData.CurrentIndex]);
								const glm::mat4 inverted = glm ::inverse(transform.GetTransform());
								const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2].x, inverted[2].y, inverted[2].z));

								if (playingSourceIndex != nullptr)
								{
									playingSourceIndex->SetConfig(ac.Config);
									playingSourceIndex->SetPosition(glm::vec4(transform.Translation, 1.0f));
									playingSourceIndex->SetDirection(forward);
									if (ac.Config.PlayOnAwake)
										playingSourceIndex->Play();

									ac.AudioSourceData.PlayingCurrentIndex = true;
									ac.AudioSourceData.CurrentIndex++;
								}
							}
						}
					}
				});
		}

		// Scripting
		{
			auto& scriptEngine = ScriptEngine::GetMutable();
			scriptEngine.SetCurrentScene(Ref<Scene>(this, [](Scene*) {}));

			auto view = m_Registry.view<ScriptComponent>();
			view.each([&](auto entity, ScriptComponent& sc) 
				{
					sc.Instance = scriptEngine.Instantiate(uint64_t(entity), m_ScriptStorage, uint64_t(entity));
				});

			auto filter = m_Registry.view<IDComponent, ScriptComponent>();
			filter.each([&](entt::entity scriptEntity, IDComponent& id, ScriptComponent& sc)
				{
					sc.Instance.Invoke("OnCreate");
				});
		}
	}

	/**
	 * @brief Stops the runtime, halting physics, audio playback, and script execution.
	 *
	 * This method disables the running state, stops the physics simulation, halts all playing audio sources and playlists, and invokes the "OnDestroy" method on all script instances before destroying them and shutting down their associated script storage.
	 */
	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		ContactListener2D::m_IsPlaying = false;

		OnPhysics2DStop();

		{
			auto view = m_Registry.view<AudioSourceComponent>();
			view.each([&](entt::entity entity, AudioSourceComponent& asc)
				{
					auto& ac = asc;
					if (AssetManager::IsAssetHandleValid(ac.Audio))
					{
						if (ac.Audio && !ac.AudioSourceData.UsePlaylist)
						{
							Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(ac.Audio);

							if (audioSource != nullptr && audioSource->IsPlaying())
								audioSource->Stop();
						}
						else if (ac.Audio && ac.AudioSourceData.UsePlaylist)
						{
							ac.AudioSourceData.CurrentIndex = ac.AudioSourceData.StartIndex;
							ac.AudioSourceData.PlayingCurrentIndex = false;

							for (auto audio : ac.AudioSourceData.Playlist)
							{
								Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(audio);

								if (audioSource != nullptr && audioSource->IsPlaying())
									audioSource->Stop();
							}
						}
					}
				});
		}

		{
			auto& scriptEngine = ScriptEngine::GetMutable();

			auto view = m_Registry.view<IDComponent, ScriptComponent>();
			view.each([&](entt::entity scriptEntity, IDComponent& id, ScriptComponent& sc)
				{
					sc.Instance.Invoke("OnDestroy");
					scriptEngine.DestroyInstance(id.ID, m_ScriptStorage);
				});

			view.each([&](entt::entity scriptEntity, IDComponent& id, ScriptComponent& sc)
				{
					Entity e = { scriptEntity, this };

					sc.HasInitializedScript = false;

					if (m_ScriptStorage.EntityStorage.find(e.GetUUID()) == m_ScriptStorage.EntityStorage.end())
						return;

					m_ScriptStorage.ShutdownEntityStorage(sc.ScriptHandle, e.GetUUID());
				});
		}
	}

	/**
	 * @brief Initializes the simulation by starting the 2D physics system.
	 */
	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	/**
	 * @brief Updates the scene during runtime, including scripts, physics, audio, and rendering.
	 *
	 * Advances the simulation if not paused or stepping frames remain. Invokes script update methods, steps the physics simulation, synchronizes entity transforms with physics bodies, updates audio listeners and sources (including playlist logic), and renders all visible entities using the primary camera. If paused, updates audio listeners and pauses all playing audio sources and playlists.
	 *
	 * @param ts The timestep for the current update.
	 */
	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			{
				SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::ScriptComponent Scope", 0xFF7200);

				// Update Scripts
				auto filter = m_Registry.view<IDComponent, ScriptComponent>();
				filter.each([&](IDComponent& id, ScriptComponent& sc)
					{
						sc.Instance.Invoke<float>("OnUpdate", ts);
					});
			}

			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<RigidBody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;

					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}

			{
				SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioListenerComponent Scope", 0xFF7200);

				auto view = m_Registry.view<AudioListenerComponent>();
				view.each([&](entt::entity entity, AudioListenerComponent& alc)
					{
						Entity e = { entity, this };
						auto& ac = e.GetComponent<AudioListenerComponent>();
						auto& transform = e.GetComponent<TransformComponent>();

						if (ac.Active)
						{
							const glm::mat4 inverted = glm::inverse(transform.GetTransform());
							const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2].x, inverted[2].y, inverted[2].z));
							ac.Listener->SetPosition(glm::vec4(transform.Translation, 1.0f));
							ac.Listener->SetDirection(glm::vec3{ -forward.x, -forward.y, -forward.z });
							//break;
						}
					});
			}

			{
				SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent Scope", 0xFF7200);

				auto view = m_Registry.view<TransformComponent, AudioSourceComponent>();
				view.each([&](entt::entity entity, TransformComponent& transform, AudioSourceComponent& asc)
					{
						//Entity e = { entity, this };
						//auto& transform = e.GetComponent<TransformComponent>();

						//const glm::mat4 inverted = glm::inverse(transform.GetTransform());
						//const glm::vec3 forward = glm::vector_normalize3(inverted.Value.z_axis);

						if (asc.Audio && !asc.AudioSourceData.UsePlaylist)
						{
							Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(asc.Audio);
							if (!audioSource->IsPlaying() && asc.Paused)
							{
								audioSource->SetConfig(asc.Config);
								audioSource->Play();
								asc.Paused = false;
							}

							if (audioSource != nullptr)
							{
								audioSource->SetConfig(asc.Config);
								audioSource->SetPosition(glm::vec4(transform.Translation, 1.0f));
								//audioSource->SetDirection(forward);
							}
						}
						else if (asc.Audio && asc.AudioSourceData.UsePlaylist)
						{
							SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 2 Scope", 0xEE3AFF);

							Ref<AudioSource> audioSourceIndex = AssetManager::GetAsset<AudioSource>(asc.AudioSourceData.Playlist[asc.AudioSourceData.OldIndex]);

							//if (ac.AudioSourceData.OldIndex <= ac.AudioSourceData.Playlist.size() - 1)
							if (asc.AudioSourceData.CurrentIndex < asc.AudioSourceData.Playlist.size() && audioSourceIndex != nullptr && asc.Config.PlayOnAwake && !audioSourceIndex->IsPlaying() && !asc.Paused)
							{
								SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 3 Scope", 0xFF8E68);

								audioSourceIndex = AssetManager::GetAsset<AudioSource>(asc.AudioSourceData.Playlist[asc.AudioSourceData.CurrentIndex]);

								if (!audioSourceIndex->IsLooping())
								{
									SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 4 Scope", 0xFF2F68);

									audioSourceIndex->SetConfig(asc.Config);
									audioSourceIndex->Play();
									asc.AudioSourceData.PlayingCurrentIndex = true;
									asc.Paused = false;

									//const rtmcpp::Mat4 inverted = rtmcpp::Inverse(transform.GetTransform());
									//const rtmcpp::Vec3 forward = rtm::vector_normalize3(inverted.Value.z_axis);

									audioSourceIndex->SetConfig(asc.Config);
									audioSourceIndex->SetPosition(glm::vec4(transform.Translation, 1.0f));
									//audioSourceIndex->SetDirection(forward);

									if (asc.AudioSourceData.RepeatAfterSpecificTrackPlays && asc.AudioSourceData.CurrentIndex == asc.AudioSourceData.StartIndex)
									{
										SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 5 Scope", 0xA191FF);

										audioSourceIndex->SetLooping(true);
									}

									if (asc.AudioSourceData.OldIndex != asc.AudioSourceData.CurrentIndex)
									{
										SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 6 Scope", 0x8CCBFF);

										asc.AudioSourceData.OldIndex = asc.AudioSourceData.CurrentIndex;
									}

									asc.AudioSourceData.CurrentIndex++;
								}
							}
							else if (asc.AudioSourceData.CurrentIndex < asc.AudioSourceData.Playlist.size() && audioSourceIndex != nullptr && asc.Config.PlayOnAwake && asc.Paused)
							{
								audioSourceIndex->SetConfig(asc.Config);
								audioSourceIndex->Play();
								asc.AudioSourceData.PlayingCurrentIndex = true;
								asc.Paused = false;
							}

							if (asc.AudioSourceData.RepeatPlaylist && !asc.AudioSourceData.RepeatAfterSpecificTrackPlays && asc.AudioSourceData.CurrentIndex >= asc.AudioSourceData.Playlist.size())
							{
								if (audioSourceIndex != nullptr && !audioSourceIndex->IsPlaying())
									asc.AudioSourceData.CurrentIndex = 0;
							}

							if (asc.AudioSourceData.RepeatAfterSpecificTrackPlays && !asc.AudioSourceData.RepeatPlaylist && asc.AudioSourceData.CurrentIndex > asc.AudioSourceData.StartIndex)
							{
								if (audioSourceIndex != nullptr && !audioSourceIndex->IsPlaying())
									asc.AudioSourceData.CurrentIndex = asc.AudioSourceData.StartIndex;
							}
						}
					});
			}
		}
		else if (m_IsPaused)
		{
			SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioListenerComponent 2 Scope", 0xFF7200);

			auto view = m_Registry.view<AudioListenerComponent>();
			view.each([&](entt::entity acEntity, AudioListenerComponent& alc)
				{
					Entity e = { acEntity, this };
					auto& ac = e.GetComponent<AudioListenerComponent>();
					auto& transform = e.GetComponent<TransformComponent>();

					if (ac.Active)
					{
						const glm::mat4 inverted = glm::inverse(transform.GetTransform());
						const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2].x, inverted[2].y, inverted[2].z));
						ac.Listener->SetPosition(glm::vec4(transform.Translation, 1.0f));
						ac.Listener->SetDirection(glm::vec3{ -forward.x, -forward.y, -forward.z });
					}
				});


			{
				SE_PROFILE_SCOPE_COLOR("Scene::OnUpdateRuntime::AudioSourceComponent 2 Scope", 0xFF7200);

				auto view = m_Registry.view<AudioSourceComponent>();
				view.each([&](entt::entity entity, AudioSourceComponent& asc)
					{

						Entity e = { entity , this};
						auto& transform = e.GetComponent<TransformComponent>();

						if (asc.Audio)
						{
							if (!asc.AudioSourceData.UsePlaylist)
							{
								Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(asc.Audio);
								if (audioSource->IsPlaying())
								{
									audioSource->SetConfig(asc.Config);
									audioSource->Pause();
									asc.Paused = true;
								}
							}
							else if (asc.AudioSourceData.UsePlaylist)
							{
								if (asc.AudioSourceData.OldIndex == 0)
								{
									Ref<AudioSource> audioSourceIndex = AssetManager::GetAsset<AudioSource>(asc.Audio);

									if (audioSourceIndex->IsPlaying())
									{
										audioSourceIndex->SetConfig(asc.Config);
										audioSourceIndex->Pause();
										//ac.AudioSourceData.PlayingCurrentIndex = false;
										asc.Paused = true;
									}
								}
								else if (asc.AudioSourceData.OldIndex > 0)
								{
									Ref<AudioSource> audioSourceIndex = AssetManager::GetAsset<AudioSource>(asc.AudioSourceData.Playlist[asc.AudioSourceData.OldIndex]);

									if (asc.AudioSourceData.OldIndex < asc.AudioSourceData.Playlist.size())
									{
										if (audioSourceIndex->IsPlaying())
										{
											audioSourceIndex->SetConfig(asc.Config);
											audioSourceIndex->Pause();
											//ac.AudioSourceData.PlayingCurrentIndex = false;
											asc.Paused = true;
										}
									}
								}
							}
						}
					});
			}
		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			// Draw sprites
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			}

			// Draw circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}

			// Draw text
			{
				auto view = m_Registry.view<TransformComponent, TextComponent>();
				for (auto entity : view)
				{
					auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

					Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
				}
			}

			Renderer2D::EndScene();
		}

	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<RigidBody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}
		}

		// Render
		RenderScene(camera);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		// Render 2D
		RenderScene(camera);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}


	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	/**
	 * @brief Creates a duplicate of the specified entity, copying all of its components.
	 *
	 * The new entity will have the same name and component data as the original, except for its unique identifier.
	 *
	 * @param entity The entity to duplicate.
	 * @return Entity The newly created duplicate entity.
	 */
	Entity Scene::DuplicateEntity(Entity entity)
	{
		// Copy name because we're going to modify component data structure
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	/**
	 * @brief Finds and returns the first entity with a matching tag.
	 *
	 * @param tag The tag string to search for.
	 * @return Entity The first entity with the specified tag, or an empty entity if not found.
	 */
	Entity Scene::FindEntityByTag(const std::string& tag)
	{
		Entity e{};
		auto filter = m_Registry.view<TagComponent>();
		filter.each([&](entt::entity entity, TagComponent& tc)
			{
				const auto& candidate = tc.Tag;

				if (candidate == tag)
					e = Entity{ entity, this };

			});

		return e;
	}

	/**
	 * @brief Invokes the scene transition callback with the specified asset handle.
	 *
	 * If no transition callback is set, logs a warning indicating that the scene cannot be transitioned.
	 *
	 * @param handle The asset handle representing the target scene for transition.
	 */
	void Scene::OnSceneTransition(AssetHandle handle)
	{
		if (m_OnSceneTransitionCallback)
			m_OnSceneTransitionCallback(handle);

		// Debug
		if (!m_OnSceneTransitionCallback)
		{
			SE_CORE_WARN("Cannot transition scene - no callback set!");
		}
	}

	/**
	 * @brief Returns the current 2D gravity vector used for physics simulation.
	 *
	 * @return glm::vec2 The gravity vector applied to the physics world.
	 */
	glm::vec2 Scene::GetPhysics2DGravity()
	{
		return s_Gravity;
	}

	/**
	 * @brief Sets the 2D physics gravity vector for the scene.
	 *
	 * Updates the static gravity value and applies it to the Box2D physics world. If the physics world does not exist, it is created with the specified gravity.
	 *
	 * @param gravity The new gravity vector to use for 2D physics simulation.
	 */
	void Scene::SetPhysics2DGravity(const glm::vec2& gravity)
	{
		s_Gravity = gravity;

		if (m_PhysicsWorld)
			m_PhysicsWorld->SetGravity(b2Vec2(s_Gravity.x, s_Gravity.y));
		else if (m_PhysicsWorld == nullptr)
			m_PhysicsWorld = new b2World({ s_Gravity.x, s_Gravity.y });
	}

	/**
	 * @brief Finds the first entity with the specified name.
	 *
	 * Searches all entities for a TagComponent whose tag matches the provided name and returns the corresponding entity. If no entity is found, returns an empty entity.
	 *
	 * @param name The name to search for.
	 * @return Entity with the matching name, or an empty entity if not found.
	 */

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	/**
	 * @brief Retrieves an entity by its numeric ID.
	 *
	 * @param id The unique numeric identifier of the entity.
	 * @return Entity corresponding to the given ID, or an empty entity if not found.
	 */
	Entity Scene::GetEntityByID(uint64_t id)
	{
		// TODO: Maybe should be assert
		if (this != nullptr && m_EntityMap.size() > 0)
		{
			if (m_EntityMap.find(id) != m_EntityMap.end())
				return { m_EntityMap.at(id), this };

		}

		return {};
	}

	/**
	 * @brief Attempts to retrieve an entity by its numeric ID.
	 *
	 * @param id The unique numeric ID of the entity.
	 * @return The entity with the specified ID if found; otherwise, a null or invalid entity.
	 */
	Entity Scene::TryGetEntityWithID(uint64_t id) const
	{
		if (const auto iter = m_EntityMap.find(id); iter != m_EntityMap.end())
			return { iter->second, const_cast<Scene*>(this) };
	}

	/**
	 * @brief Retrieves an entity by its UUID.
	 *
	 * @param uuid The universally unique identifier of the entity.
	 * @return The entity associated with the given UUID, or an empty entity if not found.
	 */
	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return { m_EntityMap.at(uuid), this };

		return {};
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Utils::RigidBody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::RenderScene(EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}
		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);
				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}

	template<>
	/**
	 * @brief Handles logic when a TagComponent is added to an entity.
	 *
	 * This specialization is intentionally left empty as no additional processing is required when a TagComponent is added.
	 */
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	} 

	template<>
	/**
	 * @brief Handles logic when a RigidBody2DComponent is added to an entity.
	 *
	 * This specialization is a placeholder for any setup required when a RigidBody2DComponent is attached to an entity. Currently, it performs no actions.
	 */
	void Scene::OnComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity entity, TextComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<AudioData>(Entity entity, AudioData& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
	{
		if (component.Audio && !component.AudioSourceData.UsePlaylist)
		{
			Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(component.Audio);
			if (audioSource != nullptr)
				audioSource->SetConfig(component.Config);
		}
		else if (component.Audio && component.AudioSourceData.UsePlaylist)
		{
			for (auto audio : component.AudioSourceData.Playlist)
			{
				Ref<AudioSource> audioSource = AssetManager::GetAsset<AudioSource>(audio);
				if (audioSource != nullptr)
					audioSource->SetConfig(component.Config);
			}
		}
	}

	template<>
	void Scene::OnComponentAdded<AudioListenerComponent>(Entity entity, AudioListenerComponent& component)
	{
		if (component.Listener)
			component.Listener->SetConfig(component.Config);
	}
}
