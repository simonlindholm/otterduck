#include "precomp.h"
#include "program.h"
#include "application.h"

// Only include the GL2 renderer (there is also support for GL1 and software,
// but we don't care about them right now)
#include <ClanLib/gl.h>

int Program::main(const std::vector<CL_String>& args) {
	try {
		// Initialize ClanLib base components
		CL_SetupCore setup_core;

		CL_SetupDisplay setup_display;

		CL_SetupGL setup_gl;

		CL_SetupSound setup_sound;
		CL_SetupMikMod setup_mikmod;

		// Start the Application
		Application app;
		return app.main(args);
	}
	catch (CL_Exception& exception) {
		// Create a console window for text-output if not available
		CL_ConsoleWindow console("Console", 80, 160);
		CL_Console::write_line("Exception caught: " + exception.get_message_and_stack_trace());
		console.display_close_message();

		return -1;
	}
}

// Instantiate CL_ClanApplication, informing it where the Program is located
CL_ClanApplication app(&Program::main);
