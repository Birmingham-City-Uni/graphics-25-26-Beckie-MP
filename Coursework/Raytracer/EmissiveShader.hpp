#pragma once
#include "Shader.hpp"

/// <summary>
/// Shader for diffuse, Lambertian surfaces of a single colour.
/// </summary>
class EmissiveShader : public Shader
{
private:
	Eigen::Vector3f albedo_;
public:
	EmissiveShader(const Eigen::Vector3f& albedo)
		:albedo_(albedo)
	{}

	virtual Eigen::Vector3f getColor(const HitInfo& hitInfo, 
		const Renderable* scene, 
		const std::vector<std::unique_ptr<Light>>& lights,
		const Eigen::Vector3f& ambientLight,
		int currBounceCount,
		const int maxBounces) const
	{

		return albedo_;
	}
};

