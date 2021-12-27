#include <iostream>
#include <cstdlib>
// IMPORTANTE: El include de GLEW debe estar siempre ANTES de el de GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

bool leftMouseButtonPressed = false;
double lastMouseX, lastMouseY;

// - Esta función callback será llamada cada vez que el área de dibujo
// OpenGL deba ser redibujada.
void window_refresh_callback(GLFWwindow* window)
{
	PAG::Renderer::Instance()->Refresh();
	// - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
	// intercambia el buffer back (que se ha estado dibujando) por el
	// que se mostraba hasta ahora front. Debe ser la última orden de
	// este callback
	glfwSwapBuffers(window);
	//std::cout << "Refresh callback called" << std::endl;
}
// - Esta función callback será llamada cada vez que se cambie el tamaño
// del área de dibujo OpenGL.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	PAG::Renderer::Instance()->Resize(width, height);
	std::cout << "Resize callback called" << std::endl;
}

// - Esta función callback será llamada cada vez que se pulse una tecla
// dirigida al área de dibujo OpenGL.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_W:
		// Move active model forward
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(0, 0, -0.02f));
		break;
	case GLFW_KEY_A:
		// Move active model left
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(-0.02, 0, 0));
		break;
	case GLFW_KEY_S:
		// Move active model backwards
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(0, 0, 0.02));
		break;
	case GLFW_KEY_D:
		// Move active model right
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(0.02, 0, 0));
		break;
	case GLFW_KEY_Q:
		// Move active model up
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(0, 0.02, 0));
		break;
	case GLFW_KEY_E:
		// Move active model down
		PAG::Renderer::Instance()->ApplyTransform(glm::vec3(0, -0.02, 0));
		break;
	case GLFW_KEY_TAB:
		std::cout << "Modelo activo cambiado: " << 
			PAG::Renderer::Instance()->SwitchActiveModel() << std::endl;
		break;
	case GLFW_KEY_LEFT_CONTROL:
		std::cout << "Modo de transformación cambiado: " <<
			PAG::Renderer::Instance()->SwitchTransformMode() << std::endl;
		break;
	case GLFW_KEY_1:
		std::cout << "Modelo añadido: TRIANGULO" << std::endl;
		PAG::Renderer::Instance()->AddModel(0);
		break;
	case GLFW_KEY_2:
		std::cout << "Modelo añadido: TETRAEDRO" << std::endl;
		PAG::Renderer::Instance()->AddModel(1);
		break;
	case GLFW_KEY_3:
		std::cout << "Modelo añadido: DADO" << std::endl;
		PAG::Renderer::Instance()->AddModel(2);
		break;
	case GLFW_KEY_4:
		std::cout << "Modelo añadido: VACA" << std::endl;
		PAG::Renderer::Instance()->AddModel(3);
		break;
	case GLFW_KEY_B:
		std::cout << "Modelo borrado" << std::endl;
		PAG::Renderer::Instance()->DeleteModel();
		break;
	case GLFW_KEY_SPACE:
		std::cout << "Cambiado modo de render" << std::endl;
		PAG::Renderer::Instance()->SwitchRenderMode();
		window_refresh_callback(window);
		break;
	case GLFW_KEY_ENTER: // Reset the camera
		std::cout << "Camara reseteada" << std::endl;
		PAG::Renderer::Instance()->ResetCamera();
		break;
	case GLFW_KEY_P:
		std::cout << "Movimiento actual: PAN" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::PAN);
		break;
	case GLFW_KEY_T:
		std::cout << "Movimiento actual: TILT" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::TILT);
		break;
	case GLFW_KEY_Y:
		std::cout << "Movimiento actual: DOLLY" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::DOLLY);
		break;
	case GLFW_KEY_C:
		std::cout << "Movimiento actual: CRANE" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::CRANE);
		break;
	case GLFW_KEY_O:
		std::cout << "Movimiento actual: ORBIT" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::ORBIT);
		break;
	case GLFW_KEY_Z:
		std::cout << "Movimiento actual: ZOOM" << std::endl;
		PAG::Renderer::Instance()->ChangeCameraMovement(PAG::CameraMovementType::ZOOM);
		break;
	case GLFW_KEY_LEFT:
		PAG::Renderer::Instance()->ApplyCameraMovement(-1, 0);
		break;
	case GLFW_KEY_RIGHT:
		PAG::Renderer::Instance()->ApplyCameraMovement(1, 0);
		break;
	case GLFW_KEY_UP:
		PAG::Renderer::Instance()->ApplyCameraMovement(0, 1);
		break;
	case GLFW_KEY_DOWN:
		PAG::Renderer::Instance()->ApplyCameraMovement(0, -1);
		break;
	default:
		std::cout << 	
			// Controles para mover el modelo
			"CONTROLES: \nWASD: Mover el modelo activo en el plano XY" << 
			"\nQE: Mover el modelo activo en el eje Z" << "\nLEFT CTRL: Cambiar modo de transformacion" <<

			// Crear, borrar, y modificar modelos
			"\n1, 2, 3, 4: Crear modelos\nB: Borrar modelo" <<
			"\nSPACE: Cambiar modo de render" << "\nTAB: Cambiar modelo activo"

			// Movimientos de la cámara
			"\nENTER: Resetear la cámara\nP : Seleccionar movimiento PAN" <<
			"\nT : Seleccionar movimiento TILT\nY : Seleccionar movimiento DOLLY" <<
			"\nC : Seleccionar movimiento CRANE\nO : Seleccionar movimiento ORBIT" <<
			"\nZ : Seleccionar movimiento ZOOM\nFlechas: Mover cámara con teclado" << std::endl;

		break;
	}

	window_refresh_callback(window);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (leftMouseButtonPressed) {

		double deltaX, deltaY;

		deltaX = xpos - lastMouseX;
		deltaY = ypos - lastMouseY;

		try {
			PAG::Renderer::Instance()->ApplyCameraMovement(deltaX, deltaY);

			lastMouseX = xpos;
			lastMouseY = ypos;
		}
		catch (std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}

		window_refresh_callback(window);
	}
}
// - Esta función callback será llamada cada vez que se pulse algún botón
// del ratón sobre el área de dibujo OpenGL.
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		leftMouseButtonPressed = true;
		glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
	}
	else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
	{
		leftMouseButtonPressed = false;
	}
}
// - Esta función callback será llamada cada vez que se mueva la rueda
// del ratón sobre el área de dibujo OpenGL.
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	double* color;
	color = PAG::Renderer::Instance()->GetClearColor();

	if (yoffset == -1 && color[0] > 0) {
		for (int i = 0; i < 3; i++) {
			color[i] -= 0.02f;
		}
	}
	else if (yoffset == 1 && color[0] < 1) {
		for (int i = 0; i < 3; i++) {
			color[i] += 0.02f;
		}
	}

	if (color[0] < 0) {
		color[0] = color[1] = color[2] = 0;
	}
	else if (color[0] > 1) {
		color[0] = color[1] = color[2] = 1;
	}

	std::cout << "Movida la rueda del raton " << yoffset << " unidades en vertical, color: "
		<< color[0] << std::endl;

	PAG::Renderer::Instance()->SetClearColor(color);
	window_refresh_callback(window);
}

int main()
{
	std::cout << "Starting application pag prueba 01" << std::endl;
	// - Inicializa GLFW. Es un proceso que sólo debe realizarse una vez en la aplicación
	if (glfwInit() != GLFW_TRUE)
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// - Definimos las características que queremos que tenga el contexto gráfico
	// OpenGL de la ventana que vamos a crear. Por ejemplo, el número de muestras o el
	// modo Core Profile.
	glfwWindowHint(GLFW_SAMPLES, 4); // - Activa antialiasing con 4 muestras.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // - Esta y las 2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // siguientes activan un contexto
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL Core Profile 4.1.

	// - Definimos el puntero para guardar la dirección de la ventana de la aplicación y
	// la creamos
	GLFWwindow* window;

	// - Tamaño, título de la ventana, en ventana y no en pantalla completa,
	// sin compartir recursos con otras ventanas.
	window = glfwCreateWindow(1024, 576, "PAG P1", nullptr, nullptr);

	// - Comprobamos si la creación de la ventana ha tenido éxito.
	if (window == nullptr)
	{
		std::cout << "Failed to open GLFW window" << std::endl;
		glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
		return -2;
	}

	// - Hace que el contexto OpenGL asociado a la ventana que acabamos de crear pase a
	// ser el contexto actual de OpenGL para las siguientes llamadas a la biblioteca
	glfwMakeContextCurrent(window);

	// - Ahora inicializamos GLEW.
	// IMPORTANTE: GLEW debe inicializarse siempre DESPUÉS de que se haya
	// inicializado GLFW y creado la ventana
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
		return -3;
	}

	// - Interrogamos a OpenGL para que nos informe de las propiedades del contexto
	// 3D construido.
	try {
		PAG::Renderer::Instance()->ShoutInfo();
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;

		glfwDestroyWindow(window);
		glfwTerminate();
		return -4;
	}

	// - Registramos los callbacks que responderán a los eventos principales
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	PAG::Renderer::Instance()->InitializeOpenGL();
	PAG::Renderer::Instance()->Refresh();

	std::cout <<
		// Controles para mover el modelo
		"CONTROLES: \nWASD: Mover el modelo activo en el plano XY" <<
		"\nQE: Mover el modelo activo en el eje Z" << "\nLEFT CTRL: Cambiar modo de transformacion" <<

		// Crear, borrar, y modificar modelos
		"\n1, 2, 3, 4: Crear modelos\nB: Borrar modelo" <<
		"\nSPACE: Cambiar modo de render" << "\nTAB: Cambiar modelo activo"

		// Movimientos de la cámara
		"\nENTER: Resetear la cámara\nP : Seleccionar movimiento PAN" <<
		"\nT : Seleccionar movimiento TILT\nY : Seleccionar movimiento DOLLY" <<
		"\nC : Seleccionar movimiento CRANE\nO : Seleccionar movimiento ORBIT" <<
		"\nZ : Seleccionar movimiento ZOOM\nFlechas: Mover cámara con teclado" << std::endl;

	// - Ciclo de eventos de la aplicación. La condición de parada es que la
	// ventana principal deba cerrarse, por ejemplo, si el usuario pulsa el
	// botón de cerrar la ventana (la X).
	while (!glfwWindowShouldClose(window))
	{	// - Obtiene y organiza los eventos pendientes, tales como pulsaciones de
		// teclas o de ratón, etc. Siempre al final de cada iteración del ciclo
		// de eventos y después de glfwSwapBuffers ( window );
		glfwPollEvents();
	}

	std::cout << "Finishing application pag prueba" << std::endl;
	glfwDestroyWindow(window); // - Cerramos y destruimos la ventana de la aplicación.
	window = nullptr;
	glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
	// - Esta llamada es para impedir que la consola se cierre inmediatamente tras la
	// ejecución y poder leer los mensajes. Se puede usar también getChar();
	system("pause");
}

