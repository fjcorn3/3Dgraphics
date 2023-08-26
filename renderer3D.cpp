#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <stdio.h>
#include <vector>

typedef struct vector3D {
	float x;
	float y;
	float z;
} Vector3D;


typedef struct triangle {
	Vector3D vertices[3];
} Triangle;

std::vector<Triangle> mesh;


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


float matProj[4][4] = { 0 };

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color Blue = {.r = 0, .g = 0, .b = 0xFF, .a = 0xFF};

void DrawTriangle(Triangle triangle) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_RenderDrawLine(renderer, triangle.vertices[0].x, triangle.vertices[0].y,
								 triangle.vertices[1].x, triangle.vertices[1].y);
	SDL_RenderDrawLine(renderer, triangle.vertices[1].x, triangle.vertices[1].y,
								 triangle.vertices[2].x, triangle.vertices[2].y);
	SDL_RenderDrawLine(renderer, triangle.vertices[2].x, triangle.vertices[2].y,
								 triangle.vertices[0].x, triangle.vertices[0].y);
}

void renderer3D_Initialize(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("3D Wireframe Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_ShowCursor(SDL_DISABLE);

	
	isRunning = true;
    mesh = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};

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

	float matRotZ[4][4] = { 0 };
	float matRotX[4][4] = { 0 };

	
	std::chrono::duration<float> elapsedTime = currentTime - startTime;
	float fTheta = 1.2f * elapsedTime.count();

	// Rotation Z
	matRotZ[0][0] = cosf(fTheta);
	matRotZ[0][1] = sinf(fTheta);
	matRotZ[1][0] = -sinf(fTheta);
	matRotZ[1][1] = cosf(fTheta);
	matRotZ[2][2] = 1;
	matRotZ[3][3] = 1;

	// Rotation X
	matRotX[0][0] = 1;
	matRotX[1][1] = cosf(fTheta * 0.5f);
	matRotX[1][2] = sinf(fTheta * 0.5f);
	matRotX[2][1] = -sinf(fTheta * 0.5f);
	matRotX[2][2] = cosf(fTheta * 0.5f);
	matRotX[3][3] = 1;

	Triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

	for(auto &tri : mesh) {
		// Rotate in Z-Axis
		triRotatedZ.vertices[0] = MultiplyMatrixVector(tri.vertices[0], matRotZ);
		triRotatedZ.vertices[1] = MultiplyMatrixVector(tri.vertices[1], matRotZ);
		triRotatedZ.vertices[2] = MultiplyMatrixVector(tri.vertices[2], matRotZ);
				
		// Rotate in X-Axis
		triRotatedZX.vertices[0] = MultiplyMatrixVector(triRotatedZ.vertices[0], matRotX);
		triRotatedZX.vertices[1] = MultiplyMatrixVector(triRotatedZ.vertices[1], matRotX);
		triRotatedZX.vertices[2] = MultiplyMatrixVector(triRotatedZ.vertices[2], matRotX);

		// Offset into Screen
		triTranslated = triRotatedZX;
		triTranslated.vertices[0].z = triRotatedZX.vertices[0].z + 3.0f;
		triTranslated.vertices[1].z = triRotatedZX.vertices[1].z + 3.0f;
		triTranslated.vertices[2].z = triRotatedZX.vertices[2].z + 3.0f;

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
	renderer3D_Initialize();

	while(isRunning) {
		renderer3D_Update();
		renderer3D_Render();
	}

	renderer3D_Close();
	exit(EXIT_SUCCESS);
}

int main() {
	renderer3D_Run();
}





