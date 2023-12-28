#include "Sapphire.h"

#include "App.h"
#include "Sapphire/ImGui/ImGuiLayer.h"

Sandbox::Sandbox()
	: sph::Application()
{
	Init();
}

void Sandbox::Init()
{
	sph::Logger::Init();
	LogInfo("Hello Client!");

	m_window = sph::Window::Create(sph::WindowProperties(1280, 720, "MyApp"));

	m_window->SetEventCallback(BIND_EVENT_METHOD(Sandbox::OnEvent));

	m_layerStack.PushOverlay(new sph::ImGuiLayer(*m_window));
}

void Sandbox::OnEvent(sph::Event& _event)
{
	sph::EventDispatcher dispatcher(_event);
	dispatcher.Dispatch<sph::WindowCloseEvent>([&](sph::WindowCloseEvent& _event) { return m_isRunning = false; });

	for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
	{
		(*--it)->OnEvent(_event);
	}
}

sph::Application* sph::CreateApplication()
{
	return new Sandbox();
}