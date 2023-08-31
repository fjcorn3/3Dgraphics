//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

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <strstream>



typedef struct vector3D {
	float x;
	float y;
	float z;
} Vector3D;

Vector3D crossProd(Vector3D v1, Vector3D v2) 
{
	Vector3D prod = {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};

	return prod;
}

float vectorLength(Vector3D v) 
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3D normalize(Vector3D v)
{
	float length = vectorLength(v);

	Vector3D norm = {
		v.x / length,
		v.y / length,
		v.z / length
	};

	return norm;
}


typedef struct triangle {
	Vector3D vertices[3];
} Triangle;

std::vector<Triangle> mesh;

std::vector<Triangle> readFromObjFile(std::string path) {
	std::vector<Triangle> mesh;
	std::vector<Vector3D> verts;
	std::ifstream objFile(path);

	while (!objFile.eof())
	{
		char line[128];
		objFile.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			Vector3D v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			mesh.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}
	}

	return mesh;
}
Vector3D MultiplyMatrixVector(Vector3D i, float m[4][4]) {
	Vector3D o;
	
	o.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + m[3][0];
	o.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + m[3][1];
	o.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + m[3][2];
	float w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}

	return o;
}



bool isRunning;
int screenWidth = 800;
int screenHeight = 480;

std::chrono::time_point<std::chrono::steady_clock> startTime, currentTime;
Vector3D camera;

float matProj[4][4] = { 0 };

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color Blue = {.r = 0, .g = 0, .b = 0xFF, .a = 0xFF};

void DrawTriangle(Triangle triangle) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);

	SDL_RenderDrawLine(renderer, triangle.vertices[0].x, triangle.vertices[0].y,
								 triangle.vertices[1].x, triangle.vertices[1].y);
	SDL_RenderDrawLine(renderer, triangle.vertices[1].x, triangle.vertices[1].y,
								 triangle.vertices[2].x, triangle.vertices[2].y);
	SDL_RenderDrawLine(renderer, triangle.vertices[2].x, triangle.vertices[2].y,
								 triangle.vertices[0].x, triangle.vertices[0].y);
}

void renderer3D_Initialize(std::string filePath) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("3D Wireframe Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_ShowCursor(SDL_DISABLE);

	mesh = readFromObjFile(filePath);

	// Projection Matrix
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)screenHeight / (float)screenWidth;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	matProj[0][0] = fAspectRatio * fFovRad;
	matProj[1][1] = fFovRad;
	matProj[2][2] = fFar / (fFar - fNear);
	matProj[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj[2][3] = 1.0f;
	matProj[3][3] = 0.0f;

	isRunning = true;
    startTime = std::chrono::steady_clock::now();
}



void renderer3D_Update(void) {
	SDL_Event e;

	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) 
		{
			case SDL_KEYDOWN:
				isRunning = false;
				break;
		}
	}

	currentTime = std::chrono::steady_clock::now();
}
void renderer3D_Render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer);

    // Triangle tri;

    // tri.vertices[0].x = screenWidth/3;
    // tri.vertices[1].x = screenWidth/2;
    // tri.vertices[2].x = screenWidth/3*2;
    // tri.vertices[0].y = screenHeight/3*2;
    // tri.vertices[1].y = screenHeight/3;
    // tri.vertices[2].y = screenHeight/3*2;

    // DrawTriangle(tri);

	// float matRotZ[4][4] = { 0 };
	float matRotX[4][4] = { 0 };
	// float matRotY[4][4] = { 0 };
	
	std::chrono::duration<float> elapsedTime = currentTime - startTime;
	float fTheta = 1.2f * elapsedTime.count();

	// Rotation Z
	// matRotZ[0][0] = cosf(fTheta);
	// matRotZ[0][1] = sinf(fTheta);
	// matRotZ[1][0] = -sinf(fTheta);
	// matRotZ[1][1] = cosf(fTheta);
	// matRotZ[2][2] = 1;
	// matRotZ[3][3] = 1;

	// Rotation X
	matRotX[0][0] = 1;
	matRotX[1][1] = cosf(fTheta * 0.5f);
	matRotX[1][2] = sinf(fTheta * 0.5f);
	matRotX[2][1] = -sinf(fTheta * 0.5f);
	matRotX[2][2] = cosf(fTheta * 0.5f);
	matRotX[3][3] = 1;


	// Rotation Y
	// matRotY[0][0] = cosf(fTheta);
	// matRotY[1][1] = 1;
	// matRotY[2][2] = cosf(fTheta);
	// matRotY[2][0] = sinf(fTheta);
	// matRotY[0][2] = -sinf(fTheta);
	// matRotY[3][3] = 1;

	Triangle triProjected, triTranslated, triRotated;

	for(auto &tri : mesh) {
		// Rotate in Z-Axis

		//triangleRotateZ(Triangle triangle, float theta)
		triRotated.vertices[0] = MultiplyMatrixVector(tri.vertices[0], matRotX);
		triRotated.vertices[1] = MultiplyMatrixVector(tri.vertices[1], matRotX);
		triRotated.vertices[2] = MultiplyMatrixVector(tri.vertices[2], matRotX);
				
		// Rotate in X-Axis
		// triRotated.vertices[0] = MultiplyMatrixVector(triRotated.vertices[0], matRotX);
		// triRotated.vertices[1] = MultiplyMatrixVector(triRotated.vertices[1], matRotX);
		// triRotated.vertices[2] = MultiplyMatrixVector(triRotated.vertices[2], matRotX);

		// Offset into Screen
		triTranslated = triRotated;
		triTranslated.vertices[0].z = triRotated.vertices[0].z + 2.0f;
		triTranslated.vertices[1].z = triRotated.vertices[1].z + 2.0f;
		triTranslated.vertices[2].z = triRotated.vertices[2].z + 2.0f;


        Vector3D normal, line1, line2;
		line1.x = triTranslated.vertices[1].x - triTranslated.vertices[0].x;
		line1.y = triTranslated.vertices[1].y - triTranslated.vertices[0].y;
		line1.z = triTranslated.vertices[1].z - triTranslated.vertices[0].z;

		line2.x = triTranslated.vertices[2].x - triTranslated.vertices[0].x;
		line2.y = triTranslated.vertices[2].y - triTranslated.vertices[0].y;
		line2.z = triTranslated.vertices[2].z - triTranslated.vertices[0].z;

		normal = crossProd(line1, line2);
        normal = normalize(normal);
        

        if (normal.x * (triTranslated.vertices[0].x - camera.x) +
			normal.y * (triTranslated.vertices[0].y - camera.y) +
			normal.z * (triTranslated.vertices[0].z - camera.z) >= 0.0f)
		{
			continue;
		}
		

		// Project from 3D to 2D
		triProjected.vertices[0] = MultiplyMatrixVector(triTranslated.vertices[0], matProj);
		triProjected.vertices[1] = MultiplyMatrixVector(triTranslated.vertices[1], matProj);
		triProjected.vertices[2] = MultiplyMatrixVector(triTranslated.vertices[2], matProj);

		// Scale into view
		triProjected.vertices[0].x += 1.0f; triProjected.vertices[0].y += 1.0f;
		triProjected.vertices[1].x += 1.0f; triProjected.vertices[1].y += 1.0f;
		triProjected.vertices[2].x += 1.0f; triProjected.vertices[2].y += 1.0f;
		triProjected.vertices[0].x *= 0.5f * (float)screenWidth;
		triProjected.vertices[0].y *= 0.5f * (float)screenHeight;
		triProjected.vertices[1].x *= 0.5f * (float)screenWidth;
		triProjected.vertices[1].y *= 0.5f * (float)screenHeight;
		triProjected.vertices[2].x *= 0.5f * (float)screenWidth;
		triProjected.vertices[2].y *= 0.5f * (float)screenHeight;

        // Draw Triangle
		DrawTriangle(triProjected);							
	}

	SDL_RenderPresent(renderer);
}

void renderer3D_Close(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void renderer3D_Run(void) {
	while(isRunning) {
		renderer3D_Update();
		renderer3D_Render();
	}
}

int main(int argc, char* argv[]) {
	if(argc != 2)
		exit(EXIT_FAILURE);

	renderer3D_Initialize(argv[1]);
	renderer3D_Run();
	renderer3D_Close();

	exit(EXIT_SUCCESS);
}





