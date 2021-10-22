#include <iostream>
#include <cstdlib>
// IMPORTANTE: El include de GLEW debe estar siempre ANTES de el de GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

// - Esta funci�n callback ser� llamada cada vez que el �rea de dibujo
// OpenGL deba ser redibujada.
void window_refresh_callback(GLFWwindow* window)
{
	PAG::Renderer::Instance()->Refresh();
	// - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
	// intercambia el buffer back (que se ha estado dibujando) por el
	// que se mostraba hasta ahora front. Debe ser la �ltima orden de
	// este callback
	glfwSwapBuffers(window);
	std::cout << "Refresh callback called" << std::endl;
}
// - Esta funci�n callback ser� llamada cada vez que se cambie el tama�o
// del �rea de dibujo OpenGL.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	PAG::Renderer::Instance()->Resize(width, height);
	std::cout << "Resize callback called" << std::endl;
}

// - Esta funci�n callback ser� llamada cada vez que se pulse una tecla
// dirigida al �rea de dibujo OpenGL.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) 
	{
		PAG::Renderer::Instance()->AddModel();
		window_refresh_callback(window);
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		PAG::Renderer::Instance()->DeleteModel();
		window_refresh_callback(window);
	}
	std::cout << "Key callback called" << std::endl;
}
// - Esta funci�n callback ser� llamada cada vez que se pulse alg�n bot�n
// del rat�n sobre el �rea de dibujo OpenGL.
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		std::cout << "Pulsado el boton: " << button << std::endl;
	}
	else if (action == GLFW_RELEASE)
	{
		std::cout << "Soltado el boton: " << button << std::endl;
	}
}
// - Esta funci�n callback ser� llamada cada vez que se mueva la rueda
// del rat�n sobre el �rea de dibujo OpenGL.
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
	// - Inicializa GLFW. Es un proceso que s�lo debe realizarse una vez en la aplicaci�n
	if (glfwInit() != GLFW_TRUE)
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// - Definimos las caracter�sticas que queremos que tenga el contexto gr�fico
	// OpenGL de la ventana que vamos a crear. Por ejemplo, el n�mero de muestras o el
	// modo Core Profile.
	glfwWindowHint(GLFW_SAMPLES, 4); // - Activa antialiasing con 4 muestras.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // - Esta y las 2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // siguientes activan un contexto
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL Core Profile 4.1.

	// - Definimos el puntero para guardar la direcci�n de la ventana de la aplicaci�n y
	// la creamos
	GLFWwindow* window;

	// - Tama�o, t�tulo de la ventana, en ventana y no en pantalla completa,
	// sin compartir recursos con otras ventanas.
	window = glfwCreateWindow(1024, 576, "PAG P1", nullptr, nullptr);

	// - Comprobamos si la creaci�n de la ventana ha tenido �xito.
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
	// IMPORTANTE: GLEW debe inicializarse siempre DESPU�S de que se haya
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

	// - Registramos los callbacks que responder�n a los eventos principales
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	PAG::Renderer::Instance()->InitializeOpenGL();

	// - Ciclo de eventos de la aplicaci�n. La condici�n de parada es que la
	// ventana principal deba cerrarse, por ejemplo, si el usuario pulsa el
	// bot�n de cerrar la ventana (la X).
	while (!glfwWindowShouldClose(window))
	{	// - Obtiene y organiza los eventos pendientes, tales como pulsaciones de
		// teclas o de rat�n, etc. Siempre al final de cada iteraci�n del ciclo
		// de eventos y despu�s de glfwSwapBuffers ( window );
		glfwPollEvents();
	}

	std::cout << "Finishing application pag prueba" << std::endl;
	glfwDestroyWindow(window); // - Cerramos y destruimos la ventana de la aplicaci�n.
	window = nullptr;
	glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
	// - Esta llamada es para impedir que la consola se cierre inmediatamente tras la
	// ejecuci�n y poder leer los mensajes. Se puede usar tambi�n getChar();
	system("pause");
}

