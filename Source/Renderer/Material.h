//	DX11Renderer - VDemo | DirectX11 Renderer
//	Copyright(C) 2016  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com

#include "Utilities/Color.h"
#include "Application/HandleTypedefs.h"
#include "RenderingEnums.h"

class Renderer;

struct BlinnPhong_Material;
struct BRDF_Material;
struct Material
{
#if 0
	union
	{
		Color	albedo;
		Color	color;
	};
#else
	LinearColor		diffuse;
#endif
	float		alpha;
	vec3		specular;

	vec2		tiling;	// default=(1,1)
	TextureID	diffuseMap;
	TextureID	normalMap;


	static BlinnPhong_Material RandomBlinnPhongMaterial();
	static BRDF_Material RandomBRDFMaterial();

	Material();
	~Material();
	void SetMaterialConstants(Renderer* renderer, EShaders shader, bool bIsDeferredRendering) const;
	virtual void SetMaterialSpecificConstants(Renderer* renderer, EShaders shader, bool bIsDeferredRendering) const = 0;
	virtual void Clear() = 0;
};

struct BRDF_Material : public Material
{	// cook-torrence brdf
	float		metalness;
	float		roughness;
	//--------------------
	BRDF_Material() : Material(), metalness(0.1f), roughness(0.6f) {}
	void SetMaterialSpecificConstants(Renderer* renderer, EShaders shader, bool bIsDeferredRendering) const override;
	void Clear() override;
};

struct BlinnPhong_Material : public Material
{
	float		shininess;
	//--------------------
	static const BlinnPhong_Material jade, ruby, bronze, gold;
	BlinnPhong_Material(const vec3& diffuse, const vec3& specular, float shininess);
	BlinnPhong_Material() : Material(), shininess(90) {}
	void SetMaterialSpecificConstants(Renderer* renderer, EShaders shader, bool bIsDeferredRendering) const override;
	void Clear() override;
};
