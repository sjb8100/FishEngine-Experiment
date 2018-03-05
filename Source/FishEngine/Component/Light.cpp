#include <FishEngine/Component/Light.hpp>
//#include <FishEngine/Gizmos.hpp>
#include <FishEngine/Transform.hpp>
//#include <FishEngine/RenderTarget.hpp>
//#include <FishEngine/QualitySettings.hpp>

namespace FishEngine
{
//	std::list<std::weak_ptr<Light>> Light::m_lights;

//	void Light::OnDrawGizmos()
//	{
//		Gizmos::DrawIcon(transform()->position(), "Light");
//	}
//
//	void Light::OnDrawGizmosSelected()
//	{
//		Gizmos::setColor(Color::yellow);
//		Vector3 center = transform()->position();
//		Vector3 dir = transform()->forward();
//		Gizmos::DrawLight(center, dir);
//	}
//
//	void Light::ResizeShadowMaps()
//	{
//		auto shadow_map_size = QualitySettings::CalculateShadowMapSize();
//		for (auto & l : m_lights)
//		{
//			auto light = l.lock();
//			if (light != nullptr)
//			{
//				light->m_shadowMap->Resize(shadow_map_size, shadow_map_size);
//			}
//		}
//	}

//	Light::Light()
//	{
//		auto shadow_map_size = QualitySettings::CalculateShadowMapSize();
//		//constexpr uint32_t shadow_map_size = 2048;
//
//		m_renderTarget = std::make_shared<RenderTarget>();
//		//m_renderTarget->Set(m_shadowMap, m_depthBuffer);
//
//		m_shadowMap = LayeredDepthBuffer::Create(shadow_map_size, shadow_map_size, 4, false);
//		//m_tempColorBuffer = LayeredColorBuffer::Create(shadow_map_size, shadow_map_size, 4, TextureFormat::R32);
//		m_shadowMap->setFilterMode(FilterMode::Bilinear);
//		m_shadowMap->setWrapMode(TextureWrapMode::Clamp);
//		m_renderTarget->SetDepthBufferOnly(m_shadowMap);
//	}

//	LightPtr Light::Create()
//	{
//		auto l = MakeShared<Light>();
//		m_lights.push_back(l);
//		return l;
//	}
}
