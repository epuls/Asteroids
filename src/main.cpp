#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <glad/glad.h>
#include <filesystem>

#include <Utilities/SDL_FrameTimer.hpp>
#include <Application.h>



// Statics/Constants
static SDL_Window*   g_window;
static SDL_GLContext g_glctx;

int Debug::LoggingLevel = Debug::LogLevel::DEBUG;
static void fail(const char* m) { SDL_Log("%s: %s", m, SDL_GetError()); }

const std::string ApplicationSettings::DIRECTORY = std::filesystem::current_path().string();

float IShader::camPos[2] = {0,0};
OffscreenFBO Application::particleFBO = {};

// Application context
FrameTimer ft;
Application asteroidApp{};



SDL_AppResult SDL_AppInit(void**, int, char**)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Request a safe context first
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


    // Create our Window
    g_window = SDL_CreateWindow("Asteroids",
                                asteroidApp.Context.applicationSettings->SCR_WIDTH, asteroidApp.Context.applicationSettings->SCR_HEIGHT,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!g_window) { fail("SDL_CreateWindow"); return SDL_APP_FAILURE; }

    g_glctx = SDL_GL_CreateContext(g_window);
    if (!g_glctx) { fail("SDL_GL_CreateContext"); return SDL_APP_FAILURE; }
    SDL_GL_MakeCurrent(g_window, g_glctx);
    // Check: is it already current?
    if (SDL_GL_GetCurrentContext() != g_glctx ||
        SDL_GL_GetCurrentWindow()  != g_window)
    {
        // Only call MakeCurrent if needed
        if (SDL_GL_MakeCurrent(g_window, g_glctx) != 0) {
            fail("SDL_GL_MakeCurrent");
            return SDL_APP_FAILURE;
        }
    }

    // Load GL w/ glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fail("gladLoadGLLoader");
        return SDL_APP_FAILURE;
    }

    SDL_Log("GL_VERSION:  %s", glGetString(GL_VERSION));
    SDL_Log("GL_VENDOR:   %s", glGetString(GL_VENDOR));
    SDL_Log("GL_RENDERER: %s", glGetString(GL_RENDERER));
    GLint maj=0,min=0;
    glGetIntegerv(GL_MAJOR_VERSION,&maj);
    glGetIntegerv(GL_MINOR_VERSION,&min);
    SDL_Log("GL reported: %d.%d", maj, min);
    SDL_Log("glGenVertexArrays ptr = %p", (void*)glGenVertexArrays);


    // Sanity: we actually have symbols
    if (!glViewport) { fail("glViewport not loaded"); return SDL_APP_FAILURE; }


    int w=0,h=0; SDL_GetWindowSizeInPixels(g_window,&w,&h);
    glViewport(0,0,w,h);

    if (!TTF_Init()) {
        fail("Couldn't initialize SDL_ttf!");
        return SDL_APP_FAILURE;
    }



    SDL_GL_SetSwapInterval(1);

    Application::particleFBO = CreateOffscreen(1000, 1000);
    asteroidApp.Init();
    ft.init();



    LOG_DEBUG("Main", "SDL application initialized");
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void*)
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double dt = ft.tick();

    asteroidApp.Loop(dt);

    SDL_GL_SwapWindow(g_window);
    ft.sample_and_set_title(g_window);
    return SDL_APP_CONTINUE;
}

// switch to switch statements
SDL_AppResult SDL_AppEvent(void*, SDL_Event* e)
{
    if (e->type == SDL_EVENT_WINDOW_RESIZED &&
        e->window.windowID == SDL_GetWindowID(g_window)) {
        glViewport(0,0,e->window.data1,e->window.data2);
        Application::particleFBO = CreateOffscreen(e->window.data1, e->window.data2);
    }
    if (e->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        e->window.windowID == SDL_GetWindowID(g_window)) {
        return SDL_APP_SUCCESS;
    }

    /**
    if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN){
        if (e->button.button == SDL_BUTTON_LEFT){
            float x=0, y=0;
            SDL_GetMouseState(&x, &y);
            x /= asteroidApp.settings.SCR_WIDTH; y /= asteroidApp.settings.SCR_HEIGHT;
            x*=2; y*= 2;
            x-=1; y-=1;
            std::cout << "clicked mouse at " << x << ", " << y << "\n";
        }
    }
     */

    if (e->type == SDL_EVENT_KEY_DOWN){
        if (e->key.key == SDLK_UP){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->UpKey, InputManager::EventType::KEY_DOWN);
        }

        if (e->key.key == SDLK_LEFT){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->LeftKey, InputManager::EventType::KEY_DOWN);
        }

        if (e->key.key == SDLK_RIGHT){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->RightKey, InputManager::EventType::KEY_DOWN);
        }

        if (e->key.key == SDLK_SPACE){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->SpaceKey, InputManager::EventType::KEY_DOWN);
        }

        if (e->key.key == SDLK_G){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->GKey, InputManager::EventType::KEY_DOWN);
        }


    }

    if (e->type == SDL_EVENT_KEY_UP){
        if (e->key.key == SDLK_UP){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->UpKey, InputManager::EventType::KEY_UP);
        }

        if (e->key.key == SDLK_LEFT){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->LeftKey, InputManager::EventType::KEY_UP);
        }

        if (e->key.key == SDLK_RIGHT){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->RightKey, InputManager::EventType::KEY_UP);
        }

        if (e->key.key == SDLK_SPACE){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->SpaceKey, InputManager::EventType::KEY_UP);
        }

        if (e->key.key == SDLK_G){
            InputManager::KeyEventHandler(asteroidApp.Context.inputManager->GKey, InputManager::EventType::KEY_UP);
        }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
    asteroidApp.Shutdown();

    if (g_glctx)  { SDL_GL_DestroyContext(g_glctx); g_glctx=nullptr; }
    if (g_window) { SDL_DestroyWindow(g_window);     g_window=nullptr; }

}
