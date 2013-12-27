///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL Image Copyright (c) 2008 - 2011 G-Truc Creation (www.g-truc.net)
///////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2013-02-07
// Updated : 2013-02-07
// Licence : This source is under MIT licence
// File    : test/core/core.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gli/gli.hpp>

template <typename texture, typename genType>
int test_texture(std::size_t Size, gli::format const & Format, std::vector<genType> const & Colors)
{
	int Error(0);

	texture TextureA(
		Format, 
		typename texture::dimensions_type(Size));

	for(typename texture::size_type Level = 0; Level < TextureA.levels(); ++Level)
		TextureA[Level].clear(Colors[Level]);

	texture TextureB = TextureA;
	Error += TextureB == TextureA ? 0 : 1;

	texture TextureC = gli::copy(TextureA);
	Error += TextureC == TextureA ? 0 : 1;

	texture TextureD = gli::copy(TextureB);
	Error += TextureD == TextureB ? 0 : 1;

	texture TextureE = gli::copy(TextureC, 1, 2);
	texture TextureF(TextureC, 1, 2);
	Error += TextureE == TextureF ? 0 : 1;

	texture TextureG = gli::copy(TextureD, 1, 2);
	texture TextureH(TextureD, 1, 2);
	Error += TextureG == TextureH ? 0 : 1;

	TextureG.clear(Colors[Colors.size() - 1]);
	TextureH.clear(Colors[Colors.size() - 1]);
	Error += TextureG == TextureH ? 0 : 1;

	TextureG.clear();
	TextureH.clear();
	Error += TextureG == TextureH ? 0 : 1;

	return Error;
}

int main()
{
	int Error(0);

	std::vector<glm::u64> ColorDXT1;
	ColorDXT1.push_back(255);
	ColorDXT1.push_back(127);
	ColorDXT1.push_back(64);
	ColorDXT1.push_back(32);
	ColorDXT1.push_back(16);
	ColorDXT1.push_back(8);
	ColorDXT1.push_back(4);

	std::vector<glm::u8> ColorR8_UNORM;
	ColorR8_UNORM.push_back(255);
	ColorR8_UNORM.push_back(127);
	ColorR8_UNORM.push_back(64);
	ColorR8_UNORM.push_back(32);
	ColorR8_UNORM.push_back(16);
	ColorR8_UNORM.push_back(8);
	ColorR8_UNORM.push_back(4);

	std::vector<glm::u8vec3> ColorRGB8_UNORM;
	ColorRGB8_UNORM.push_back(glm::u8vec3(255,   0,   0));
	ColorRGB8_UNORM.push_back(glm::u8vec3(255, 127,   0));
	ColorRGB8_UNORM.push_back(glm::u8vec3(255, 255,   0));
	ColorRGB8_UNORM.push_back(glm::u8vec3(  0, 255,   0));
	ColorRGB8_UNORM.push_back(glm::u8vec3(  0, 255, 255));
	ColorRGB8_UNORM.push_back(glm::u8vec3(  0,   0, 255));
	ColorRGB8_UNORM.push_back(glm::u8vec3(255,   0, 255));

	std::vector<glm::u8vec4> ColorRGBA8_UNORM;
	ColorRGBA8_UNORM.push_back(glm::u8vec4(255,   0,   0, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(255, 127,   0, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(255, 255,   0, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(  0, 255,   0, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(  0, 255, 255, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(  0,   0, 255, 255));
	ColorRGBA8_UNORM.push_back(glm::u8vec4(255,   0, 255, 255));

	std::vector<glm::vec4> ColorRGBA32F;
	ColorRGBA32F.push_back(glm::vec4(1.0,   0,   0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(1.0, 0.5,   0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(1.0, 1.0,   0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(  0, 1.0,   0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(  0, 1.0, 1.0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(  0,   0, 1.0, 1.0));
	ColorRGBA32F.push_back(glm::vec4(1.0,   0, 1.0, 1.0));

	std::vector<std::size_t> Sizes;
	Sizes.push_back(32);
	Sizes.push_back(16);
	Sizes.push_back(17);
	Sizes.push_back(15);
	Sizes.push_back(12);
	Sizes.push_back(5);

	for(std::size_t i = 0; i < Sizes.size(); ++i)
	{
		Error += test_texture<gli::texture1D>(Sizes[i], gli::RGB_DXT1, ColorDXT1);
		Error += test_texture<gli::texture2D>(Sizes[i], gli::RGB_DXT1, ColorDXT1);
		Error += test_texture<gli::texture3D>(Sizes[i], gli::RGB_DXT1, ColorDXT1);
		Error += test_texture<gli::texture1D>(Sizes[i], gli::R8_UNORM, ColorR8_UNORM);
		Error += test_texture<gli::texture2D>(Sizes[i], gli::R8_UNORM, ColorR8_UNORM);
		Error += test_texture<gli::texture3D>(Sizes[i], gli::R8_UNORM, ColorR8_UNORM);
		Error += test_texture<gli::texture1D>(Sizes[i], gli::RGB8_UNORM, ColorRGB8_UNORM);
		Error += test_texture<gli::texture2D>(Sizes[i], gli::RGB8_UNORM, ColorRGB8_UNORM);
		Error += test_texture<gli::texture3D>(Sizes[i], gli::RGB8_UNORM, ColorRGB8_UNORM);
		Error += test_texture<gli::texture1D>(Sizes[i], gli::RGBA8_UNORM, ColorRGBA8_UNORM);
		Error += test_texture<gli::texture2D>(Sizes[i], gli::RGBA8_UNORM, ColorRGBA8_UNORM);
		Error += test_texture<gli::texture3D>(Sizes[i], gli::RGBA8_UNORM, ColorRGBA8_UNORM);
		Error += test_texture<gli::texture1D>(Sizes[i], gli::RGBA32F, ColorRGBA32F);
		Error += test_texture<gli::texture2D>(Sizes[i], gli::RGBA32F, ColorRGBA32F);
		Error += test_texture<gli::texture3D>(Sizes[i], gli::RGBA32F, ColorRGBA32F);
	}
		
	return Error;
}
