#define _CRT_SECURE_NO_WARNINGS
#include "Controller.h"
#include "Window.h"

const float moveSpeed = 6;
const float placeSpeed = 0.05;

static Control* control = Control::getInstance();
static WindowBuild* window = WindowBuild::getInstance();
static RandomBlock* world = RandomBlock::getInstance();

Control* Control::getInstance()
{
	static Control control;
	return &control;
}

Control::Control()
{
}

void Control::keyPress()
{
	windowKey();//窗体功能
	takeCubeKey();//数字键切换手持物体
	cameraMoveKey();
	cameraJumpKey();
}

void Control::getHeight(int altitude)
{
	eyePos.y = altitude + EYE_HEIGHT;
	cameraPos.y = eyePos.y;
	height = eyePos.y;
}

void Control::fall()
{
	if (ps == Status::UP)
	{
		eyePos.y += (beginJumpAltitude - eyePos.y) * 0.025 + control->elapseTime;
		cameraPos.y = eyePos.y;
		height = eyePos.y;

		if (eyePos.y >= beginJumpAltitude)
		{
			height = beginJumpAltitude;
			eyePos.y = height;
			cameraPos.y = eyePos.y;
			ps = Status::DOWN;
		}
	}
	else if (ps == Status::DOWN)
	{
		eyePos.y -= (beginJumpAltitude - eyePos.y) * 0.025 + control->elapseTime;
		cameraPos.y = eyePos.y;
		height = eyePos.y;

		if (eyePos.y <= world->getHeight(ceil(eyePos.x - 0.5), ceil(eyePos.z - 0.5), ceil(eyePos.y - 1.5)) + EYE_HEIGHT)
		{
			height = world->getHeight(ceil(eyePos.x - 0.5), ceil(eyePos.z - 0.5), ceil(eyePos.y - 1.5)) + EYE_HEIGHT;
			eyePos.y = height;
			cameraPos.y = eyePos.y;
			ps = Status::GROUND;
		}
	}
	else if (isCollision(eyePos.x, eyePos.y - 1, eyePos.z))
	{
		ps = Status::DOWN;
		beginJumpAltitude = eyePos.y;
	}
}

void mousePress(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
			case GLFW_MOUSE_BUTTON_RIGHT:
				control->destroyBlock();
				break;
			case GLFW_MOUSE_BUTTON_LEFT:
				control->placeBlock();
				break;
			default:
				return;
		}
	}
	return;
}

void Control::placeBlock()
{
	glm::vec3 place;

	for (int i = 1; i < 150; i++)
	{
		place = cameraPos + placeSpeed * i * cameraFront;
		if (!world->judgeCube(round(place.x), round(place.y), round(place.z), CubeType::NONE))
		{
			place = cameraPos + placeSpeed * (i - 1) * cameraFront;
			if (world->judgeCube(round(place.x), round(place.y), round(place.z), CubeType::NONE) &&
				!(round(place.x) == ceil(eyePos.x - 0.5) &&
					round(place.z) == ceil(eyePos.z - 0.5) &&
					round(place.y) == ceil(eyePos.y - 1.5)))
			{
				world->writeCube(round(place.x), round(place.y), round(place.z), nowPlaceCube);
				break;
			}
		}
	}
}

void Control::destroyBlock()
{
	glm::vec3 place;
	for (int i = 1; i < 150; i++)
	{
		place = cameraPos + placeSpeed * i * cameraFront;

		if (!world->judgeCube(round(place.x), round(place.y), round(place.z), CubeType::NONE))
		{
			world->writeCube(round(place.x), round(place.y), round(place.z), CubeType::NONE);
			return;
		}
	}
}

void mouseMove(GLFWwindow* window, double xpos, double ypos)
{
	static float lastX = WIN_WIDTH / 2.0;
	static float lastY = WIN_HEIGHT / 2.0;

	static bool firstMouse = true;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	control->xz_rad += xoffset;
	control->xy_rad += yoffset;

	if (control->xy_rad > 80.0f)
		control->xy_rad = 80.0f;
	if (control->xy_rad < -80.0f)
		control->xy_rad = -80.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(control->xz_rad)) * cos(glm::radians(control->xy_rad));
	front.y = sin(glm::radians(control->xy_rad));
	front.z = sin(glm::radians(control->xz_rad)) * cos(glm::radians(control->xy_rad));
	control->cameraFront = glm::normalize(front);

	if (EYE_HEIGHT == 1)
	{
		float radius = 0.25f;
		float camX = cos(glm::radians(control->xz_rad)) * radius;
		float camZ = sin(glm::radians(control->xz_rad)) * radius;
		control->cameraPos.x = control->eyePos.x + camX;
		control->cameraPos.z = control->eyePos.z + camZ;
	}
	else
	{
		float radius = 5.0f;
		float camX = cos(glm::radians(control->xz_rad)) * radius;
		float camZ = sin(glm::radians(control->xz_rad)) * radius;
		control->cameraPos.x = control->eyePos.x - camX;
		control->cameraPos.z = control->eyePos.z - camZ;
	}
}

void Control::windowKey()
{
	if (glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window->getWindow(), true);
}

void Control::takeCubeKey()
{
	if (glfwGetKey(window->getWindow(), GLFW_KEY_1) == GLFW_PRESS)
		nowPlaceCube = CubeType::DIRT;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_2) == GLFW_PRESS)
		nowPlaceCube = CubeType::SAND;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_3) == GLFW_PRESS)
		nowPlaceCube = CubeType::STONE;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_4) == GLFW_PRESS)
		nowPlaceCube = CubeType::WOOD;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_5) == GLFW_PRESS)
		nowPlaceCube = CubeType::COBBLE;
}

void Control::cameraMoveKey()
{
	float cameraSpeed = moveSpeed * elapseTime;
	glm::vec3 oldPos = eyePos;

	if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		eyePos += cameraSpeed * cameraFront;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		eyePos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		eyePos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		eyePos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (eyePos != oldPos)
	{
		glm::vec3 newPos = eyePos + cameraPos - oldPos;

		if (isCollision(newPos.x, newPos.y, newPos.z))
		{
			cameraPos = newPos;
			eyePos.y = height;
			cameraPos.y = eyePos.y;
		}
		else
		{
			eyePos = oldPos;
		}
	}
}

void Control::cameraJumpKey()
{
	if (glfwGetKey(window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (ps == Status::GROUND)
		{
			ps = Status::UP;
			beginJumpAltitude = eyePos.y + float(EYE_HEIGHT) * 1.1;
		}
	}

	fall();
}

bool Control::isCollision(float x, float y, float z)
{
	if (world->judgeCube(ceil(x - 0.5), ceil(y - 1.5), ceil(z - 0.5), CubeType::NONE) &&
		world->judgeCube(ceil(x - 0.5), ceil(y - 0.5), ceil(z - 0.5), CubeType::NONE))
	{
		if (world->judgeCube(ceil(x - 0.5 + 0.2), ceil(y - 1.5), ceil(z - 0.5), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5 - 0.2), ceil(y - 1.5), ceil(z - 0.5), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5), ceil(y - 1.5), ceil(z - 0.5 + 0.2), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5), ceil(y - 1.5), ceil(z - 0.5 - 0.2), CubeType::NONE) &&

			world->judgeCube(ceil(x - 0.5 + 0.2), ceil(y - 0.5), ceil(z - 0.5), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5 - 0.2), ceil(y - 0.5), ceil(z - 0.5), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5), ceil(y - 0.5), ceil(z - 0.5 + 0.2), CubeType::NONE) &&
			world->judgeCube(ceil(x - 0.5), ceil(y - 0.5), ceil(z - 0.5 - 0.2), CubeType::NONE))
			return true;
	}
	return false;
}
