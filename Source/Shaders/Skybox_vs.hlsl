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

// source: http://richardssoftware.net/Home/Post/25

struct VSIn
{
	float3 position : POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT0;
	float2 texCoord : TEXCOORD0;
};

struct PSIn
{
	float4 HPos : SV_POSITION;
	float3 LPos : POSITION;
};

cbuffer matrices {
	matrix worldViewProj;
	float fovH;
};

#include "Panini.hlsl"

PSIn VSMain(VSIn In)
{
	const float4 projectedPos = mul(worldViewProj, In.position).xyww;
	
	PSIn Out;
	Out.LPos = In.position;	 // sample direction
	Out.HPos = projectedPos; // z=w makes depth=1 -> drawn at far plane
	return Out;
}