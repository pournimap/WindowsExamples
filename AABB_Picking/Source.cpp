#include <windows.h>
#include <windowsx.h>
#include <stdio.h> // for FILE I/O

#include <gl\glew.h>//use it before other GL.h

#include <gl/GL.h>

#include "c:/glm/glm.hpp"
#include "c:/glm/gtc/matrix_transform.hpp"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

enum
{
	MALATI_ATTRIBUTE_VERTEX = 0,
	MALATI_ATTRIBUTE_COLOR,
	MALATI_ATTRIBUTE_NORMAL,
	MALATI_ATTRIBUTE_TEXTURE
};

FILE* gpFile = NULL;
float gWidth, gHeight;
glm::mat4 gPerspectiveProjectionMatrix;


#include "ObjLoader/Matrix_Obj_Loader.h"
#include "Camera_2.h"
Camera scene_camera;
#include "MousePicker.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;
GLuint gLdUniform, gLaUniform, gLsUniform;
GLuint gLightPositionUniform;
GLuint gKdUniform, gKaUniform, gKsUniform;
GLuint gMaterialShininessUniform;

GLuint gLKeyPressedUniform;


//glm::mat4 gModelMatrix;

bool gbLight;

GLfloat gAngle = 0.0f;


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;

GLuint gVao_model;
GLuint gVbo_model_position, gVbo_model_normal, gVbo_model_texture;

GLuint gVao_Cube, gVbo_Cube_Position, gVbo_Cube_Normal;

GLuint gVao_Pyramid;
GLuint gVbo_Pyramid_Position;



bool showAABB = false;
bool showOOBB = false;
GLuint gIs_ObjectHitUniform;

float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;
float midX = WIN_WIDTH / 2.0f;
float midY = WIN_HEIGHT / 2.0f;



glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool FirstMouse = true;
float _yaw = -90.0f;
float _pitch = 0.0f;

BoundingBox gAABBox[TOTAL_OBJECTS_IN_SCENE];
BoundingBox gOOBBox[TOTAL_OBJECTS_IN_SCENE];
bool isHitObject[TOTAL_OBJECTS_IN_SCENE];

glm::vec3 objectPosition[TOTAL_OBJECTS_IN_SCENE];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void Update(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	//code
	// create log file
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}

	//initializing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	//Registering Class
	RegisterClassEx(&wndclass);

	//Create Window
	//Parallel to glutInitWindowSize(), glutInitWindowPosition() and glutCreateWindow() all three together
	hwnd = CreateWindow(szClassName,
		TEXT("OpenGL Programmable Pipeline Window"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//initialize
	initialize();

	//Message Loop
	while (bDone == false) //Parallel to glutMainLoop();
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// rendring function
			display();
			Update();
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true) //Continuation to glutLeaveMainLoop();
					bDone = true;
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);
	void mouseClick(float xPos, float yPos);
	void MouseMovement(double xpos, double ypos);

	static bool bIsAKeyPressed = false;
	static bool bIsLKeyPressed = false;

	double xPos, yPos;

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) //if 0, the window is active
			gbActiveWindow = true;
		else //if non-zero, the window is not active
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE: //Parallel to glutReshapeFunc();
		resize(LOWORD(lParam), HIWORD(lParam)); //Parallel to glutReshapeFunc(resize);
		break;
	case WM_MOUSEMOVE:
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);
		MouseMovement((double)xPos, (double)yPos);
		break;

	case WM_KEYDOWN: //Parallel to glutKeyboardFunc();
		if (GetAsyncKeyState(VK_DOWN))
		{
			scene_camera.ProcessKeyboard(BACKWARD, 10.0f);
		}
		if (GetAsyncKeyState(VK_UP))
		{
			scene_camera.ProcessKeyboard(FORWARD, 10.0f);
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			scene_camera.ProcessKeyboard(RIGHT, 10.0f);
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			scene_camera.ProcessKeyboard(LEFT, 10.0f);
		}
		switch (wParam)
		{
		case VK_ESCAPE: //case 27
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true; //Parallel to glutLeaveMainLoop();
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		case 0x4C:
			if (bIsLKeyPressed == false)
			{
				gbLight = true;
				bIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				bIsLKeyPressed = false;
			}
			break;
			//1
		case 0x31:
			showAABB = !showAABB;
			break;
			//2
		case 0x32:
			showOOBB = !showOOBB;
			break;
			//3
		/*case 0x33:
			showBoundingSphere = !showBoundingSphere;
			break;*/
		default:
			break;
		}
		break;
	
	case WM_LBUTTONDOWN:  //Parallel to glutMouseFunc();
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);

		mouseClick((float)xPos, (float)yPos);
		break;
	case WM_CLOSE: //Parallel to glutCloseFunc();
		uninitialize(); //Parallel to glutCloseFunc(uninitialize);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void mouseClick(float xPos, float yPos)
{
	Ray ray;
	ray = computeRay((float)xPos, (float)yPos);

	for (int i = 0; i < TOTAL_OBJECTS_IN_SCENE; i++)
	{
		isHitObject[i] = HitBoundingBox(ray, gAABBox[i]);

		if (isHitObject[i])
		{
			fprintf(gpFile, "Object Hit : %d\n", i);
			fflush(gpFile);
		}
		else
		{
			fprintf(gpFile, "%d Object Not Hit\n", i);
			fflush(gpFile);
		}
	}
}


void MouseMovement(double xpos, double ypos)
{
	if (FirstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		FirstMouse = false;
	}

	GLfloat xoffset = (float)xpos - lastX;
	GLfloat yoffset = lastY - (float)ypos;

	lastX = (float)xpos;
	lastY = (float)ypos;


	scene_camera.ProcessMouseMovement(xoffset, yoffset);
}

void initialize(void)
{
	//function prototypes
	void uninitialize(void);
	void resize(int, int);
	void ToggleFullscreen(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
	//Parallel to glutInitDisplayMode()
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	//choose a pixel format which best matches with that of 'pfd'
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//set the pixel format chosen above
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//make the rendering context created above as current n the current hdc
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	fprintf(gpFile, "%s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "Shading Lang Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	scene_camera = Camera(cameraPos, cameraUp, _yaw, _pitch);
	//VERTEX SHADER
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	const GLchar* vertextShaderSourceCode =
		"#version 400" \
		"\n" \
		"in vec3 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;"\
		"uniform int u_LKeyPressed;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyPressed == 1)" \
		"{" \
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vec4(vPosition, 1.0);" \
		"transformed_normals=mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \
		"viewer_vector = -eye_coordinates.xyz;" \
		"}"\
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(vPosition, 1.0);" \
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertextShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char* szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//FRAGMENT SHADER

	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* fragmentShaderSourceCode =
		"#version 400" \
		"\n" \
		"in vec3 transformed_normals;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"out vec4 FragColor;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_LKeyPressed;" \
		"uniform int is_ObjectHit;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;" \
		"if(u_LKeyPressed==1)" \
		"{" \
		"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
		"vec3 normalized_light_direction=normalize(light_direction);" \
		"vec3 normalized_viewer_vector=normalize(viewer_vector);" \
		"vec3 ambient = u_La * u_Ka;" \
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
		"phong_ads_color=ambient + diffuse + specular;" \
		/*"phong_ads_color = vec3(1.0, 1.0, 1.0);" \*/
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"if(is_ObjectHit == 1)" \
		"{" \
		"FragColor = vec4(phong_ads_color, 1.0) * vec4(1.0, 0.0, 0.0, 1.0);" \
		"}" \
		"else" \
		"{" \
		"FragColor = vec4(phong_ads_color, 1.0);" \
		"}" \
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(gFragmentShaderObject);

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//Shader Program

	gShaderProgramObject = glCreateProgram();

	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, MALATI_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, MALATI_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObject);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_LKeyPressed");

	gLdUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	gLaUniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	gLsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");

	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");

	gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
	gMaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	gIs_ObjectHitUniform = glGetUniformLocation(gShaderProgramObject, "is_ObjectHit");

	LoadAllModels();

	const GLfloat pyramidVertices[] =
	{
		//front face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		//right face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		//back face
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		//Left face
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f
	};

	const GLfloat pyramidColors[] =
	{
		//front
		1.0f, 0.0f, 0.0f,//r
		0.0f, 1.0f, 0.0f,//g
		0.0f, 0.0f, 1.0f,//b
		//right
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		//back
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		//left
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &gVao_Pyramid);
	glBindVertexArray(gVao_Pyramid);

	//position
	glGenBuffers(1, &gVbo_Pyramid_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Pyramid_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	objectPosition[0] = glm::vec3(15.0f, -10.0f, -30.0f);
	objectPosition[1] = glm::vec3(-10.0f, -10.0f, -40.0f);
	objectPosition[2] = glm::vec3(20.0f, -10.0f, -50.0f);
	objectPosition[3] = glm::vec3(-20.0f, -10.0f, -50.0f);
	objectPosition[4] = glm::vec3(0.0f, -10.0f, -30.0f);
	objectPosition[5] = glm::vec3(-30.0f, -10.0f, -60.0f);
	objectPosition[6] = glm::vec3(0.0f, -10.0f, -20.0f);
	objectPosition[7] = glm::vec3(0.0f, -10.0f, -50.0f);
	objectPosition[8] = glm::vec3(-15.0f, -10.0f, -30.0f);
	objectPosition[9] = glm::vec3(10.0f, -10.0f, -40.0f);
	

	/*objectPosition[0] = glm::vec3(10.0f, -1.0f, -10.0f);
	//objectPosition[1] = glm::vec3(-10.0f, -1.0f, -10.0f);
	objectPosition[2] = glm::vec3(5.0f, -1.0f, -10.0f);
	objectPosition[3] = glm::vec3(0.0f, -1.0f, -10.0f);
	objectPosition[4] = glm::vec3(5.0f, -1.0f, -10.0f);*/
	const GLfloat cubeVertices[] =
	{
		//top
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		//bottom
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		//front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		//back
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		//right
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		//left
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f

	};



	const GLfloat cubeNormals[] =
	{
		//top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		//bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		//front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		//right
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		//left
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

	};

	glGenVertexArrays(1, &gVao_Cube);
	glBindVertexArray(gVao_Cube);

	//position
	glGenBuffers(1, &gVbo_Cube_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Cube_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVbo_Cube_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Cube_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//for (int i = 0; i < TOTAL_OBJECTS_IN_SCENE; i++)
	//{
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[0]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[1]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[2]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[3]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[4]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[5]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[6]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[7]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[8]);
	calculateBoundingVolumes(gModel_Nanosuit.model_vertices, &gOOBBox[9]);
	/*calculateBoundingVolumes(cubeVertices, sizeof(cubeVertices), &gOOBBox[0]);
	//calculateBoundingVolumes(cubeVertices, sizeof(cubeVertices), &gOOBBox[1]);
	calculateBoundingVolumes(cubeVertices, sizeof(cubeVertices), &gOOBBox[2]);
	calculateBoundingVolumes(cubeVertices, sizeof(cubeVertices), &gOOBBox[3]);
	calculateBoundingVolumes(cubeVertices, sizeof(cubeVertices), &gOOBBox[4]);*/
	//}

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// depth test to do
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // blue

	gPerspectiveProjectionMatrix = glm::mat4(1.0f);

	gWidth = WIN_WIDTH;
	gHeight = WIN_HEIGHT;

	gbLight = false;
	// resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	ToggleFullscreen();
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	if (gbLight == true)
	{
		glUniform1i(gLKeyPressedUniform, 1);

		glUniform3fv(gLdUniform, 1, lightDiffuse);//white
		glUniform3fv(gLaUniform, 1, lightAmbient);
		glUniform3fv(gLsUniform, 1, lightSpecular);
		glUniform4fv(gLightPositionUniform, 1, lightPosition);

		glUniform3fv(gKdUniform, 1, material_diffuse);
		glUniform3fv(gKaUniform, 1, material_ambient);
		glUniform3fv(gKsUniform, 1, material_specular);
		glUniform1f(gMaterialShininessUniform, material_shininess);

		float lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

	}
	else
	{
		glUniform1i(gLKeyPressedUniform, 0);
	}

	for (int i = 0; i < TOTAL_OBJECTS_IN_SCENE; i++)
	{

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 viewMatrix = glm::mat4(1.0f);


		modelMatrix = glm::translate(modelMatrix, objectPosition[i]);

		viewMatrix = scene_camera.GetViewMatrix();
		
		glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, &viewMatrix[0][0]);

		glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, &gPerspectiveProjectionMatrix[0][0]);

		if (isHitObject[i])
		{
			glUniform1i(gIs_ObjectHitUniform, 1);
		}
		else
		{
			glUniform1i(gIs_ObjectHitUniform, 0);
		}

		/*if (i == 0 || i == 1)
		{*/

			glBindVertexArray(gModel_Nanosuit.Vao);
			for (int i = 0; i < gModel_Nanosuit.model_mesh_data.size(); i++)
			{
				if (gbLight == true)
				{
					glUniform3fv(gKaUniform, 1, gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].Ka);
					glUniform3fv(gKdUniform, 1, gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].Kd);
					glUniform3fv(gKsUniform, 1, gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].Ks);
					glUniform1f(gMaterialShininessUniform, material_shininess);

					if (gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].ismap_Kd == true)
					{
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].gTexture_diffuse);

					}

				}
				glDrawArrays(GL_TRIANGLES, gModel_Nanosuit.model_mesh_data[i].vertex_Index, gModel_Nanosuit.model_mesh_data[i].vertex_Count);
			}
			glBindVertexArray(0);
		/*}

		if (i > 1)
		{
			glBindVertexArray(gVao_Cube);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
			glBindVertexArray(0);
		}*/
		updateBoundingVolumes(modelMatrix, gOOBBox[i], &gAABBox[i]);


		if (showOOBB)
		{
			renderBoundingBox(gOOBBox[i]);
		}
		if (showAABB)
		{
			modelMatrix = glm::mat4(1.0f);
			glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, &modelMatrix[0][0]);
			renderBoundingBox(gAABBox[i]);
		}
	}
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void Update(void)
{
	gAngle += 0.5;
	if (gAngle >= 360.0f)
		gAngle = 0.0f;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gWidth = width;
	gHeight = height;
	/*
	if (width <= height)
	{
	gOrthographicProjectionMatrix = ortho(-100.0f, 100.0f, (-100.0f *(height / width)), (100.0f * (height / width)), -100.0f, 100.0f);
	}
	else
	{
	gOrthographicProjectionMatrix = ortho((-100.0f *(width / height)), (100.0f * (width / height)), -100.0f, 100.0f, -100.0f, 100.0f);
	}*/
	gPerspectiveProjectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height,
		0.1f, 100.0f); //perspective(3rd change)
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}

	uninitializeAllModelData();

	glDetachShader(gShaderProgramObject, gVertexShaderObject);
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(ghrc);
	ghrc = NULL;

	//Delete the device context
	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(TRUE);
	}
	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}