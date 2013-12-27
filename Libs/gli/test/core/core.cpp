///////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL Image Copyright (c) 2008 - 2011 G-Truc Creation (www.g-truc.net)
///////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2011-10-07
// Updated : 2011-10-07
// Licence : This source is under MIT licence
// File    : test/core/core.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gli/gli.hpp>
#include <ctime>
#include <cstdio>

int test_create_texture_storage()
{
	int Error(0);

	gli::texture2D Texture(
		gli::texture2D::size_type(glm::log2(256.f)), 
		gli::RGBA8U, 
		gli::texture2D::dimensions_type(256));
	gli::texture2D::size_type Levels = Texture.levels();

	assert(!Texture.empty());

	void const * Pointer = Texture[0].data();

	glm::u8vec4 TexelA = Texture[0].data<glm::u8vec4>()[0];
	glm::u8vec4 TexelB = Texture[0].data<glm::u8vec4>()[1];
	glm::u8vec4 TexelC = Texture[0].data<glm::u8vec4>()[2];
	glm::u8vec4 TexelD = Texture[0].data<glm::u8vec4>()[3];

	return Error;
}

int test_reset_memset_zero()
{
	std::vector<int> Data;
	Data.resize(1 << 24);

	std::clock_t LastTime = std::clock();

	for(std::size_t j = 0; j < (1 << 4); ++j)
		memset(&Data[0], 0, Data.size());

	std::clock_t Time = std::clock();

	printf("test_reset_memset_zero: %d\n", Time - LastTime);

	return 0;
}

int test_reset_memset_ff()
{
	std::vector<int> Data;
	Data.resize(1 << 24);

	std::clock_t LastTime = std::clock();

	for(std::size_t j = 0; j < (1 << 4); ++j)
		memset(&Data[0], 0x12345678, Data.size());

	std::clock_t Time = std::clock();

	printf("test_reset_memset_zero: %d\n", Time - LastTime);

	return 0;
}

int test_reset_loop_zero()
{
	std::vector<int> Data;
	Data.resize(1 << 24);

	std::clock_t LastTime = std::clock();

	for(std::size_t j = 0; j < (1 << 4); ++j)
	for(std::size_t i = 0; i < Data.size(); ++i)
		Data[i] = 0;

	std::clock_t Time = std::clock();

	printf("test_reset_loop_zero: %d\n", Time - LastTime);

	return 0;
}

int test_reset_loop_ff()
{
	std::vector<int> Data;
	Data.resize(1 << 24);

	std::clock_t LastTime = std::clock();

	for(std::size_t j = 0; j < (1 << 4); ++j)
	for(std::size_t i = 0; i < Data.size(); ++i)
		Data[i] = 0xFFFFFFFF;

	std::clock_t Time = std::clock();

	printf("test_reset_loop_ff: %d\n", Time - LastTime);

	return 0;
}

int main()
{
	int Error(0);

	int A = glm::lowerMultiple(3, 4);
	int B = glm::lowerMultiple(6, 4);
	int C = glm::lowerMultiple(8, 4);
	int D = glm::lowerMultiple(9, 4);

	Error += test_reset_memset_zero();
	Error += test_reset_memset_ff();
	Error += test_reset_loop_zero();
	Error += test_reset_loop_ff();
	Error += test_create_texture_storage();
	
	return Error;
}
